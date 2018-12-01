#include "../Match.h"
#include "SpatialMatchState.h"

#include "../inc/Entities.h"


namespace WOF { namespace match {


SpatialMatchState::SpatialMatchState() {

	m90DegRotMat.setupRotation(kPiOver2, true);
}

void SpatialMatchState::init(Match& match) {


	mMatch = &match;

	mBallIntercept.init(match);
	mBallDist.init(match);

	onActiveFootballersChange();
}

void SpatialMatchState::onActiveFootballersChange() {

	mBallIntercept.resetFootballerIterator(&mMatch->getActiveFootballerIterator());
	mBallDist.resetFootballerIterator(&mMatch->getActiveFootballerIterator());
}

void SpatialMatchState::frameMove(Match& match, const Clock& clock) {

	{
		bool consideredFootballersUpdated;
		mBallIntercept.update(match, clock.getTime(), match.getFootballerBallInterceptManager().getValidSimul(), consideredFootballersUpdated);
	}

	{
		mBallDist.update(match, clock.getTime());
	}

	mBallState.update(match, clock);
}

void findFootballerFreeSpace_updateSafetyTime(const CtVector3& footballerPos, const CtVector3& oppPos, float& safetyTime, const float& fbRadius, const float& safetyDistSq, const float& footballerSpeed) {

	float distSq = distanceSq(footballerPos, oppPos);

	if (distSq <= safetyDistSq) {

		safetyTime = 0.0f;

	} else {

		float dist = sqrtf(distSq);

		dist -= fbRadius;
		float arrivalTime = dist / footballerSpeed;

		safetyTime = tmin(safetyTime, arrivalTime);
	}
}

float findFootballerFreeSpace_calcSafetyTime(Footballer* pFootballer, const CtVector3& footballerPos, FootballerIterator& oppIter, const float& fbRadius, const float& safetyDistSq, const float& footballerSpeed, const float& minSafetyTime) {

	float safetyTime = FLT_MAX;

	SoftPtr<Footballer> fb;
	FootballerIndex i;

	while (safetyTime >= minSafetyTime && oppIter.iterateFootballers(fb.ptrRef(), i)) {

		if (pFootballer != fb.ptr())
			findFootballerFreeSpace_updateSafetyTime(footballerPos, fb->getLocalPos(), safetyTime, fbRadius, safetyDistSq, footballerSpeed);
	}
	
	return safetyTime;
}

bool SpatialMatchState::findFootballerFreeSpace(Footballer& footballer, const CtVector3* pSearchDir, bool tryCurrPos, 
													float footballerRadius, float footballerSpeed, float searchDistance, float minSafetyTime, int maxIterCount, 
													PitchRegionShape* pRegion, Vector3& outPos, float& outSafetyTime) {

	SoftPtr<FootballerIterator> fbIter;
	float safetyDistSq = footballerRadius * footballerRadius;

	//if (footballerTeam == Team_Invalid) {
	//
	//	fbIter = &(mMatch->getActiveFootballerIterator());
	//
	//} else {

		fbIter = &(mMatch->getTeam(opponentTeam(footballer.mTeam)));
	//}

	const CtVector3& footballerPos = footballer.getLocalPos();
	Vector2 pos2D;
	bool origPosInRegion = true;

	if (pRegion) {

		origPosInRegion = pRegion->contains(mMatch, footballerPos);
	}


	outSafetyTime = -1.0f;

	if (tryCurrPos) {
		
		outSafetyTime = findFootballerFreeSpace_calcSafetyTime(&footballer, footballerPos, fbIter, footballerRadius, safetyDistSq, footballerSpeed, minSafetyTime);
		outPos = footballerPos;
	}
	
	{

		Vector3 refDir;
		Vector3 testDir;
		Vector3 testPos;

		if (pSearchDir) {

			refDir = dref(pSearchDir);
			refDir.el[Scene_Up] = 0.0f;

			refDir.normalize();

		} else {

			refDir.el[Scene_Right] = trand2(-1.0f, 1.0f);
			refDir.el[Scene_Up] = 0.0f;
			refDir.el[Scene_Forward] = trand2(-1.0f, 1.0f);

			refDir.normalize();
		}

		
		testDir = refDir;
		
		for (int iter = 0; iter < maxIterCount && outSafetyTime < minSafetyTime; ++iter) {

			footballerPos.addMultiplication(testDir, searchDistance, testPos);
			bool skip = false;

			if (pRegion) {

				if (origPosInRegion) {

					skip = true;

					{
					
						Vector2 testDir2D;
						mMatch->toScene2D(testDir, testDir2D);

						//search for a point in region
						for (int i = 0; skip && i < 4; ++i) {

							if (pRegion->contains(mMatch, testPos)) {

								skip = false;
							} 

							m90DegRotMat.transform(Vector2(testDir2D), testDir2D);
							mMatch->toScene3D(testDir2D, testDir, footballerPos.el[Scene_Up]);
							footballerPos.addMultiplication(testDir, searchDistance, testPos);
						}
					}

					//clip last test point
					if (skip) {

						pRegion->clip(mMatch, Vector3(testPos), testPos, footballerPos.el[Scene_Up]);
						skip = false;
					}

				} else {

					skip = !pRegion->contains(mMatch, testPos);
				}
			}

			if (!skip) {

				float safetyTime = findFootballerFreeSpace_calcSafetyTime(&footballer, testPos, fbIter, footballerRadius, safetyDistSq, footballerSpeed, minSafetyTime);

				if (safetyTime > outSafetyTime) {

					outSafetyTime = safetyTime;
					outPos = testPos;
				}
			}

			if (iter % 4 != 0) {

				Vector2 testDir2D;

				mMatch->toScene2D(refDir, testDir2D);
				m90DegRotMat.transform(Vector2(testDir2D), testDir2D);
				mMatch->toScene3D(testDir2D, refDir, 0.0f);

			} else {

				refDir.el[Scene_Right] = trand2(-1.0f, 1.0f);
				refDir.el[Scene_Forward] = trand2(-1.0f, 1.0f);

				refDir.normalize();
			}
		}
	}

	return outSafetyTime >= minSafetyTime;
}

Footballer* SpatialMatchState::iterNextRegionFootballer(PitchRegionShape* pRegion, TeamEnum teamFilter, FootballerIndex& startIndex) {

	if (pRegion == NULL)
		return NULL;

	if (teamFilter == Team_Invalid) {

		assrt(false);
		return NULL;

	} else {

		PitchRegionShape& region = dref(pRegion);
		Team& team = mMatch->getTeam(teamFilter);
				
		for (FootballerIndex i = startIndex; i < team.getRawFootballerCount(); ++i) {

			Footballer& footballer = team.getRawFootballer(i);

			if (region.contains(mMatch, footballer.getLocalPos())) {

				startIndex = i + 1;
				return &footballer;	
			}
		}
	}

	return NULL;
}

} }