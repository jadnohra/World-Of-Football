#include "SceneScriptedCharacter.h"

#include "../../Match.h"
#include "WE3/math/WEContactNormalSolver.h"

namespace WOF { namespace match {


ScriptObject SceneScriptedCharacter::script_collFindDynVolColliderInt(ScriptInt* searchStartIndex, int type) {

	if (mCollProcessing.isValid()) {

		CollRegistry& registry = mCollProcessing->collRegistry;
		CollRegistry::Index dynRecordCount = registry.getDynVolRecordCount();

		CollRegistry::Index& i = dref(searchStartIndex).value();

		while (i < dynRecordCount) {

			SoftPtr<CollRecordVol> record =  registry.getDynVolRecord(i);
			++i;

			if (typeIsEntMesh(record->getObjectType())) {

				SoftPtr<SceneScriptedCharacter> scripted = entMeshScriptedFromObject(record->getOwner());

				if (scripted->scriptedGetTypeInt() == type) {

					return scripted->getScriptObject();
				}
			}
		}
	}

	return ScriptObject();
}

ScriptObject SceneScriptedCharacter::script_collFindDynVolCollider(ScriptInt* searchStartIndex, const TCHAR* type) {

	if (mCollProcessing.isValid()) {

		CollRegistry& registry = mCollProcessing->collRegistry;
		CollRegistry::Index dynRecordCount = registry.getDynVolRecordCount();

		String str(type);

		CollRegistry::Index i = searchStartIndex ? searchStartIndex->value() : 0;

		for (; i < dynRecordCount; ++i) {

			SoftPtr<CollRecordVol> record =  registry.getDynVolRecord(i);

			if (typeIsScriptedCharacter(record->getObjectType())) {

				SoftPtr<SceneScriptedCharacter> scripted = entMeshScriptedFromObject(record->getOwner());

				if (str.equals(scripted->scriptedGetType())) {

					return scripted->getScriptObject();
				}
			}
		}
	}

	return ScriptObject();
}

bool SceneScriptedCharacter::script_collEnablePorcessing(bool enable) {

	if (enable) {

		if (mVolumeColliders.isValid() && mVolumeColliders->dynamicColl.isValid()) {

			if (!mCollProcessing.isValid()) {

				WE_MMemNew(mCollProcessing.ptrRef(), CollisionProcessing());
			}

			return true;
		}

		return false;
	} 
	
	mCollProcessing.destroy();
	return true;
}

void SceneScriptedCharacter::script_collUpdate() {

	scriptClean();
}

bool SceneScriptedCharacter::collResolve_Trackback(CollisionProcessing& state) {

	if (state.loadTransform(mTransformLocal)) {

		nodeMarkDirty();
		return true;
	} 

	return false;
}

bool SceneScriptedCharacter::collResolve_Slide(CollisionProcessing& state, bool considerDynamicColls, bool considerStaticColls) {

	static ContactNormalSolver solver;

	if (!state.savedTransformValid)
		return false;

	Vector3 posDiff;

	mTransformLocal.getPosition().subtract(state.savedTransform.getPosition(), posDiff);

	posDiff.el[Scene_Up] = 0.0f;

	if (posDiff.isZero())
		return false;

	CollRegistry& collRegistry = state.getPosCollRegistry(this);
	
	/*
		Here we are really guessing and hoping ...

		It is best to KISS and use sthg that works in the obvious cases
		which is gathering motion opposing triangle normals,

		anythign more complicated is probably not worth it
		and when it gets complicated it usually means this is not a good place for sliding

		having the trackback as a robust alternative saves the day in any case

		another not tested way would be to have swept information (which needs a whole diff. coll system)
		to determin	the first collider and try to slide along its contactResolve normal.
		that would be a better guess.
	*/

	solver.reset();

	//const OBB& thisVol = mVolumeColliders->dynamicColl->volume.toOBB();
	//const AAB& thisVol = mVolumeColliders->dynamicColl->volume.toAAB();
		
	//use contact resolve normals, with/without edges
	//collRegistry.gatherTriContactNormals(thisVol, solver, false);
	
	//use simple triangle normals
	//collRegistry.gatherTriNormals(solver);

	if (considerStaticColls) {

		//use simple direction opposing triangle normals
		Vector3 filterNormal;
		posDiff.negate(filterNormal);
		collRegistry.gatherTriNormals(solver, &filterNormal);
	}
	
	{
		DynamicVolumeCollider& thisCollider = state.getPosCollider(this);

		Vector3 moveDir;
		float moveDist;

		posDiff.normalize(moveDir, moveDist);

		/*
			if AAB/OBB are used, they might block at corners because 
			both corner axis's are valid ...

			the real solution is to figure out with sweeping which axis collision 
			occured 1st
			a somehow hacked solution would be to use the move dir to select 
			the correct axis even if it is the second best axis 
			(also checking if depth is in line with pos diff)
			in that way we are basically not blindly serching for min depth axis 
			but for the axis which most probably blocked the given posDiff
			basically a 'SAT' that use time coherence.
		*/	

		if (considerDynamicColls) 
			collRegistry.gatherDynVolContactNormalsSwept(moveDir, moveDist, thisCollider.volume, solver, true);

		if (considerStaticColls) 
			collRegistry.gatherVolContactNormalsSwept(moveDir, moveDist, thisCollider.volume, solver, true);
	}


	const Vector3* pGlobalResolveNormal = solver.solve();

	if (pGlobalResolveNormal) {

		Vector3 globalResolveNormal = dref(pGlobalResolveNormal);
		
		globalResolveNormal.el[Scene_Up] = 0.0;
		if (globalResolveNormal.isZero())
			return false;

		globalResolveNormal.normalize();

		if (globalResolveNormal.dot(posDiff) > 0.0f)
			return false;

		bool slideAtFullSpeed = true;

		if (true) {

			//use globalResolveNormal to correct posDiff

			Vector3 correction;
		
			project(posDiff, globalResolveNormal, correction);
			
			if (correction.equals(posDiff))
				return false; //actually a simple block

			correction.negate();

			
			if (slideAtFullSpeed) {

				Vector3 newPosDiff;

				mTransformLocal.getPosition().add(correction, newPosDiff);
				newPosDiff.subtract(state.savedTransform.getPosition());

				newPosDiff.normalize();
				newPosDiff.mul(posDiff.mag());

				nodeMarkDirty();
				mTransformLocal.setPosition(state.savedTransform.getPosition());
				mTransformLocal.movePosition(newPosDiff);

			} else {

				nodeMarkDirty();
				mTransformLocal.movePosition(correction);
			}

			

		} else {

			//another and more expensive way would be to iterate 
			//over static objects and correct, 
			//also ensure that the corrected position still is in move radius, 
			//we need this because of the way we are constraining the normal
			//this might lead to erroneous corrections			
		}
	
		return true;
	}

	return false;
}

/*
int SceneScriptedCharacter::script_collResolve(bool ghostMode) {

	bool wasChanged = false;
	bool didResolve = true;

	if (mCollProcessing.isValid()) {

		CollisionProcessing& state = mCollProcessing.dref();
		CollRegistry& collRegistry = state.getPosCollRegistry(this);

		if (ghostMode) {

			mCollState = Coll_Ghosted;

		} else {

			script_collUpdate();
			bool done = collRegistry.getTotalRecordCount() == 0;

			if (done) {

				mCollState = Coll_NoColls;

			} else {

				int maxIterCount = 1;
				int iter = 0;

				while (!done && iter < maxIterCount) {

					if (collResolve_Slide(state)) {

						mCollState = Coll_Constrained;

						script_collUpdate();
						done = collRegistry.getTotalRecordCount() == 0;
					}

					++iter;
				}
			}

			if (!done) {

				if (collResolve_Trackback(state)) {

					mCollState = Coll_Blocked;
					didResolve = false;
					//script_collUpdate();
					//done = collRegistry.getTotalRecordCount();

				} else {

					mCollState = Coll_Unresolved;
				}
			}
		}
		
		state.saveTransform(mTransformLocal);
	}

	return mCollState;
}
*/

int SceneScriptedCharacter::script_collResolve(bool enableGhostDynamic, bool enableGhostStatic) {

	if (mCollProcessing.isValid()) {

		CollisionProcessing& state = mCollProcessing.dref();
		CollRegistry& collRegistry = state.getPosCollRegistry(this);

		script_collUpdate();
		unsigned int dynRecordCount = collRegistry.getDynVolRecordCount();
		unsigned int staticRecordCount = collRegistry.getVolRecordCount() + collRegistry.getTriRecordCount();
		unsigned int totalRecordCount = dynRecordCount + staticRecordCount;

		
		if (dynRecordCount == 0) {

			state.mCollGhostedDynamic = false;

		} else {

			if (!state.mCollGhostedDynamic && enableGhostDynamic) {

				state.mCollGhostedDynamic = true;
			} 
		}

		if (staticRecordCount == 0) {

			state.mCollGhostedStatic = false;

		} else {

			if (!state.mCollGhostedStatic && enableGhostStatic) {

				state.mCollGhostedStatic = true;
			} 
		}

		bool hasUnresolvedColls;

		hasUnresolvedColls = (((dynRecordCount != 0) && !state.mCollGhostedDynamic)
								|| ((staticRecordCount != 0) && !state.mCollGhostedStatic));
		 
		if (hasUnresolvedColls) {

			bool resolveTryFailed;

			if (collResolve_Slide(state, !state.mCollGhostedDynamic, !state.mCollGhostedStatic)) {

				resolveTryFailed = false;
				mCollState = Coll_Constrained;

			} else {

				resolveTryFailed = true;
			}

			bool tryTrackBack;

			if (resolveTryFailed) {

				tryTrackBack = true;

			} else {

				script_collUpdate();
				dynRecordCount = collRegistry.getDynVolRecordCount();
				staticRecordCount = collRegistry.getVolRecordCount() + collRegistry.getTriRecordCount();

				hasUnresolvedColls = (((dynRecordCount != 0) && !state.mCollGhostedDynamic)
										|| ((staticRecordCount != 0) && !state.mCollGhostedStatic));

				if (hasUnresolvedColls) {

					//slided but some collisions remain unresvoled or new collisions were produced
					tryTrackBack = true;

				} else {

					//succesfully slided
					tryTrackBack = false;
				}
			} 

			if (tryTrackBack) {

				if (collResolve_Trackback(state)) {

					script_collUpdate();

					mCollState = Coll_Blocked;

				} else {

					mCollState = Coll_Unresolved;
				}
			} 

		} else {

			mCollState = totalRecordCount == 0 ? Coll_NoColls : Coll_Ghosted;
		}

		state.saveTransform(mTransformLocal);
	}

		
	return mCollState;
}

} }