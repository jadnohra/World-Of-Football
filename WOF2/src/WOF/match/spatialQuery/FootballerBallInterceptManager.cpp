#include "FootballerBallInterceptManager.h"

#include "../inc/Scene.h"
#include "../Match.h"
#include "../DataTypes.h"

namespace WOF { namespace match {

FootballerBallInterceptManager::FootballerBallInterceptManager() {
}

void FootballerBallInterceptManager::init(Match& match) {

	if (mMatch.isValid()) {

		assrt(false);
	}

	mMatch = &match;

	{
		BallSimulation& simul = mSimul;

		simul.enableSyncMode(true);
		simul.enableSimul(true);

		simul.setupUsingMatch(match);
	}
}

void FootballerBallInterceptManager::load(Match& match, BufferString& tempStr, DataChunk& mainChunk, CoordSysConv* pConv) {

	SoftRef<DataChunk> chunk = mainChunk.getChild(L"ballPathIntercept");

	if (chunk.isValid()) {

		mSimul.loadConfig(mMatch, tempStr, chunk, pConv);
	}
}

void FootballerBallInterceptManager::frameMove(Match& match, const Time& time) {

	mSimul.updateSync(mMatch->getBall());
}

void FootballerBallInterceptManager::render(Renderer& renderer, const bool& flagExtraRenders, RenderPassEnum pass) {

	if (pass == RP_Normal) {

		/*
		if (mMatch->mTweakShowPitchBallBounceMarker && mBallPitchBounceMarker.isValid() && mSimul.isValidSync()) {

			const Vector3& ballPos = mMatch->getBall().getPos();

			if (ballPos.el[Scene_Up] >= mMatch->mTweakPitchBallBounceMarkerHeight) {

				Time time = mMatch->getClock().getTime();

				for (BallSimulation::Index i = mSimul.getSyncFlowCollSampleIndex(); i < mSimul.getCollSampleCount(); ++i) {

					const BallSimulation::CollSample& collSample = mSimul.getCollSample(i);

					if (collSample.time >= time && collSample.collider.isValid() && collSample.collider->objectType == Node_Pitch) {

						mBallPitchBounceMarker->mTransformLocal.setPosition(collSample.pos);
						mBallPitchBounceMarker->nodeMarkDirty();
						mBallPitchBounceMarker->render(renderer, true, flagExtraRenders, pass);

						break;
					}
				}
			}
		}
		*/
		if (mMatch->mTweakShowPitchBallBounceMarker && mSimul.isValidSync()) {

			RenderSprite& sprite = mMatch->getBallPitchMarkerSprite();
			const Vector3& ballPos = mMatch->getBall().getPos();

			if (ballPos.el[Scene_Up] >= mMatch->mTweakPitchBallBounceMarkerHeight) {

				Time time = mMatch->getClock().getTime();

				for (BallSimulation::Index i = mSimul.getSyncFlowCollSampleIndex(); i < mSimul.getCollSampleCount(); ++i) {

					const BallSimulation::CollSample& collSample = mSimul.getCollSample(i);

					if (collSample.time >= time && collSample.collider.isValid() && collSample.collider->objectType == Node_Pitch) {

						sprite.transform3D().setPosition(collSample.pos);
						sprite.render3D(renderer);

						break;
					}
				}
			}
		}

		if (flagExtraRenders)
			mSimul.render(renderer, true);
	}
}

/*
bool FootballerBallInterceptManager::requestUpdate(Match& match, Footballer& footballer, const Time& time) {

	//limit this and delay some requests for performance.
	update(match, footballer, time);

	return true;
}

bool FootballerBallInterceptManager::update(Match& match, Footballer& footballer, const Time& time) {

	PathIntercept& pathIntercept = footballer.getBallPathIntercept();
	SoftPtr<BallSimulation::Index> sampleIndex;

	if (pathIntercept.simulID == mAnalyzedSimul.getSimulID()) {

		if (pathIntercept.analysisTime == time) {

			return pathIntercept.type == PI_InterceptorWait;

		} else {

			if (pathIntercept.type == PI_InterceptorWait)
				sampleIndex = &pathIntercept.sampleIndex;
		}
	}

	return mAnalyzedSimul.updateShortestTimeInterceptState(match, footballer, mSimul, time, sampleIndex);
}

bool FootballerBallInterceptManager::isStillIntercepting(Match& match, Footballer& footballer, const Time& time) {

	PathIntercept& pathIntercept = footballer.getBallPathIntercept();
	
	if (pathIntercept.simulID == mAnalyzedSimul.getSimulID()) {

		if (pathIntercept.type == PI_InterceptorWait
			&& pathIntercept.expiryTime >= time) {

			return mAnalyzedSimul.getIntersectionTime(footballer, mAnalyzedSimul.getSimulID(), pathIntercept.sampleIndex, dref(mAnalyzedSimul.getSample(pathIntercept.sampleIndex)), pathIntercept, mAnalyzedSimul.getSimul()->getStartTime(), time, true);
		}
	} 

	return false;
}
*/

} }
