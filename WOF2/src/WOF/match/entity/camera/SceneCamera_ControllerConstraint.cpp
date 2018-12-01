#include "SceneCamera_ControllerConstraint.h"

#include "../../scene/SceneNode.h"

namespace WOF { namespace match { namespace sceneCamera {


Controller_Constraint::Controller_Constraint() 
 : mEnable(false) {

	 mOffset.zero();
}

void Controller_Constraint::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	if (!mEnable)
		return;

}

void Controller_Constraint::getPostModifs(Vector3& postModif) {

	if (!mEnable)
		return;

	postModif.add(mOffset);
}


void Controller_Constraint::init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (chunk.scanAttribute(tempStr, L"enable", mEnable) == false) {

		mEnable = false;
	}

	SoftRef<DataChunk> childChunk = chunk.getFirstChild();

	while(childChunk.isValid()) {

		if (childChunk->getAttribute(L"axis", tempStr)) {

			SoftPtr<Constraint_Axis> constraint;

			if (tempStr.equals(L"forward")) {

				MMemNew(constraint.ptrRef(), Constraint_Axis());

				constraint->axis = Scene_Forward;

			} else if (tempStr.equals(L"right")) {

				MMemNew(constraint.ptrRef(), Constraint_Axis());

				constraint->axis = Scene_Right;

			} else if (tempStr.equals(L"up")) {

				MMemNew(constraint.ptrRef(), Constraint_Axis());

				constraint->axis = Scene_Up;

			} 

			if (constraint.isValid()) {

				mConstraints.addOne(constraint);

				if (childChunk->scanAttribute(tempStr, L"min", L"%f", &constraint->min.val)) {

					constraint->min.enable = true;

					if (pConv) {

						pConv->toTargetUnits(constraint->min.val);
					}
				}

				if (childChunk->scanAttribute(tempStr, L"max", L"%f", &constraint->max.val)) {

					constraint->max.enable = true;

					if (pConv) {

						pConv->toTargetUnits(constraint->max.val);
					}
				}
			}

		} else {

			assrt(false);
		}

		toNextSibling(childChunk);
	}

	mState = S_Stopped;
	mPreparingState = S_None;
}

void Controller_Constraint::forceStarted(CamState& state) {

	if (!mEnable)
		return;

	mPreparingState = S_None;

	mState = S_Started;
	mStateStartTime = state.t;
	
	mOffset.zero();
}

void Controller_Constraint::start(CamState& state) {

	if (!mEnable)
		return;

	if (mState == S_None || mState == S_Stopped) {

		stateSwitch(state.t, S_Started, NULL);
	}
}

void Controller_Constraint::stop(CamState& state) {

	if (!mEnable)
		return;

	if (mState != S_Stopped) {

		stateSwitch(state.t, S_Stopped, NULL);
	}
}


void Controller_Constraint::stateSwitch(const Time& t, const State& newState, bool* pRecurse) {

	mState = newState;
	mStateStartTime = t;

	if (pRecurse)
		*pRecurse = true;
}

bool Controller_Constraint::statePrepare() {

	bool ret = (mState != mPreparingState);

	mPreparingState = mState;

	return ret;
}

void Controller_Constraint::Constraint_Axis::addConstraintOffset(const Vector3& ref, Vector3& offset) {

	if (min.enable) {

		if (ref.el[axis] < min.val) {

			offset.el[axis] += min.val - ref.el[axis];
		}
	}

	if (max.enable) {

		if (ref.el[axis] > max.val) {

			offset.el[axis] += max.val - ref.el[axis];
		}
	}
}

void Controller_Constraint::update(CamState& state) {

	if (!mEnable)
		return;

	bool recurse = true;
	
	while (recurse) {

		recurse = false;

		bool isNewState = statePrepare();

		switch (mState) {

			//S_Stopped
			default: {

				if (isNewState)
					mOffset.zero();

			} break;

			case S_Started: {

				if (isNewState) {

					//mOffset.zero();
				} 

				mOffset.zero();

				const Vector3& ref = state.camera.target.currPos;

				for (Constraints::Index i = 0; i < mConstraints.count; ++i) {

					mConstraints.el[i]->addConstraintOffset(ref, mOffset);
				}

				/*
				if (state.camera.target.currPos.el[Scene_Forward] > 5000.0f) {

					mOffset.el[Scene_Forward] = 5000.0f - state.camera.target.currPos.el[Scene_Forward];
				}
				*/
				

			} break;
		}
	}
}


} } }