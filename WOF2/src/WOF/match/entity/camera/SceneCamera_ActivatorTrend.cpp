#include "SceneCamera_ActivatorTrend.h"

#include "WE3/helper/load/WEMathDataLoadHelper.h"
using namespace WE;

#include "SceneCamera.h"
#include "../../Match.h"

namespace WOF { namespace match { namespace sceneCamera {

Controller_ActivatorTrend::Controller_ActivatorTrend() 
	: mIsStarted(false) {

}

Controller_ActivatorTrend::~Controller_ActivatorTrend() {

	mFollowers.destroy();
}

void Controller_ActivatorTrend::init(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ControllerTable* pTable) {

	mConv = pConv;

	mTrendType = TT_Interval;
	mTrendInterval = 2.0f;

	if (chunk.scanAttribute(tempStr, L"trendDistance", L"%f", &mTrendDistance)) {

		mTrendType = TT_Distance;
	}

	if (pConv) 
		pConv->toTargetUnits(mTrendDistance);

	if (chunk.scanAttribute(tempStr, L"trendInterval", L"%f", &mTrendInterval)) {

		mTrendType = TT_Interval;
	}

	if (chunk.scanAttribute(tempStr, L"idleInterval", L"%f", &mIdleInterval) == false) {

		mIdleInterval = mTrendInterval * 0.5f;
	}

	if (chunk.scanAttribute(tempStr, L"sensitivity", L"%f", &mSensitivity) == false) {

		mSensitivity = 0.5f;
	}

	if (chunk.getAttribute(L"activationTarget", tempStr)) {

		if (pTable) {

			SoftRef<ControllerBase> target;
			
			pTable->find(tempStr.hash(), target.ptrRef());

			setActivationTarget(target);
		}
	}

	SoftRef<DataChunk> child = chunk.getFirstChild();

	while(child.isValid()) {

		if (child->getAttribute(L"name", tempStr)) {

			if (pTable) {

				SoftRef<ControllerBase> controller;

				pTable->find(tempStr.hash(), controller.ptrRef());

				if (controller.isValid()) {

					SoftPtr<Follower> follower;

					MMemNew(follower.ptrRef(), Follower());
					
					follower->target = controller.ptr();
					
					follower->directionCount = 0;
					child->scanAttribute(tempStr, L"directionCount", L"%d", &follower->directionCount);

					follower->distance = 0.0f;
					child->scanAttribute(tempStr, L"distance", L"%f", &follower->distance);

					follower->invDistance = 0.0f;
					child->scanAttribute(tempStr, L"invDistance", L"%f", &follower->invDistance);

					if (pConv) {

						pConv->toTargetUnits(follower->distance);
						pConv->toTargetUnits(follower->invDistance);
					}

					MMemNew(follower->provider.ptrRef(), ControllerProvider_Distance_Impl());

					if (!controller->setProvider(follower->provider.ptr())) {

						assrt(false);
					}

					follower->normalDir = match.getCoordAxis(Scene_Up);
					follower->refDir = match.getCoordAxis(Scene_Forward);

					MathDataLoadHelper::extract(tempStr, chunk, follower->refDir, true, false, true, L"refDir");
					MathDataLoadHelper::extract(tempStr, chunk, follower->normalDir, true, false, true, L"normalDir");

					
					if (pConv) {
					
						pConv->toTargetVector(follower->normalDir.el);
						pConv->toTargetVector(follower->refDir.el);
					}
					

					Vector3 test(0.0f, 0.0f, 5.0f);

					if (pConv)
						pConv->toTargetPoint(test.el);

					follower->provider->setDistance(test);

					mFollowers.addOne(follower.ptr());
				}
			}
		}

		toNextSibling(child);
	}
}

void Controller_ActivatorTrend::setActivationTarget(ControllerBase* pController) {

	mActivationTarget = pController;
}

void Controller_ActivatorTrend::start(CamState& state) {

	mIsStarted = true;
	mSamplingStarted = false;

	mTrend.zero();
	onTrendChanged(state);
}
 
void Controller_ActivatorTrend::stop(CamState& state) {

	mIsStarted = false;

	mTrend.zero();
	onTrendChanged(state);
}

void Controller_ActivatorTrend::forceStarted(CamState& state) {

	mIsStarted = true;
	mSamplingStarted = false;
	mIdleDetectStarted = false;

	if (mActivationTarget.isValid()) {

		mActivationTarget->forceStarted(state);
	}

	mTrend.zero();
	onTrendChanged(state);
}

void Controller_ActivatorTrend::updateFollowerProviders(CamState& state) {

	const float precision = k1DegreeInRad * 0.5f;

	Vector dist;
	Vector dir;

	for (Followers::Index i = 0; i < mFollowers.count; ++i) {

		Follower& follower = dref(mFollowers.el[i]);

		discretizeDirection(follower.refDir, follower.normalDir, mTrend, follower.directionCount, dir);

		bool useInvDist = (follower.invDistance != 0.0f) && ((follower.refDir.dot(dir) + precision) < 0.0f);

		dir.mul(useInvDist ? follower.invDistance : follower.distance, dist);

		follower.provider->setDistance(dist);
	}
}

void Controller_ActivatorTrend::onTrendChanged(CamState& state) {

	if (mActivationTarget.isValid()) {

		if (mTrend.isZero()) {

			mActivationTarget->stop(state);

		} else {

			String::debug(L"trend active\n");
			updateFollowerProviders(state);

			mActivationTarget->start(state);
		}
	}
}

void Controller_ActivatorTrend::update(CamState& state) {

	if (mIsStarted) {

		float distMag = state.track.lookAtBasePos.dist.mag();
		bool trendChanged = false;

		if (mSamplingStarted) {
			
			bool delay = false;

			if (distMag == 0.0f && mCumulDist == 0.0f && mTrend.isZero()) {

				delay = true;
			}

			if (!delay) {

				bool trendRestart;

				switch (mTrendType) {

					//TT_Interval
					default: {

						trendRestart = (state.t - mSamplingStartTime >= mTrendInterval);

					} break;

					case TT_Distance: {

						trendRestart = (mCumulDist >= mTrendDistance);

					} break;
				}

				if (!trendRestart) {

					mTrendDir.add(state.track.lookAtBasePos.dist);
					mCumulDist += distMag;

				} else {

					if (mCumulDist) {

						if (mTrendDir.mag() / mCumulDist >= mSensitivity) {

							mTrendDir.normalize(mTrend);

						} else {

							mTrend.zero();
						}

					} else {

						mTrend.zero();
					}

					trendChanged = true;
					
					mSamplingStarted = false; 
				}

			} else {

				mSamplingStartTime = state.t;
			}

		} else {

			mSamplingStartTime = state.t;

			mTrendDir.zero();
			mCumulDist = 0.0f;

			trendChanged = true;

			mSamplingStarted = true; 
		}

		if (mIdleDetectStarted) {

			if (distMag != 0.0f) {

				mIdleDetectStartTime = state.t;

			} else {

				if (state.t - mIdleDetectStartTime >= mIdleInterval) {

					//here we could maybe delay this if trend 'just started'
					//using some grace interval or so

					mTrend.zero();

					trendChanged = true;

					mIdleDetectStarted = false;
					mSamplingStarted = false;
				}
			}

		} else {

			mIdleDetectStartTime = state.t;

			mIdleDetectStarted = true;
		}

		if (trendChanged) {

			onTrendChanged(state);
		}
	}

	if (mActivationTarget.isValid()) {

		mActivationTarget->update(state);
	}
	
}

void Controller_ActivatorTrend::getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif) {

	if (mActivationTarget.isValid()) {

		mActivationTarget->getModifs(eyePosModif, targetPosModif, targetOffsetModif);
	}
	
}

void Controller_ActivatorTrend::getPostModifs(Vector3& postModif) {

	
	if (mActivationTarget.isValid()) {

		mActivationTarget->getPostModifs(postModif);
	}
	
}

} } }