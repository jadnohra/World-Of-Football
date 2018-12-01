#include "WOFMatchBallSimulation.h"

#include "WE3/scene/WESceneDirection.h"
using namespace WE;

#include "../../../WOFMatch.h"
#include "../WOFMatchBall.h"

namespace WOF { namespace match {

SimulationProcessingRate::SimulationProcessingRate() {

	logPerformance = false;

	frameCount = 1;
	FPS = 30;
	durationPerFrame = 2.0f;
	processingFrameCount = 1;
	maxDuration = 10.0f;
}

void SimulationProcessingRate::load(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	{
		chunk.scanAttribute(tempStr, L"logPerformance", logPerformance);

		chunk.scanAttribute(tempStr, L"durationPerFrame", L"%f", &durationPerFrame);
		chunk.scanAttribute(tempStr, L"maxDuration", L"%f", &maxDuration);
		chunk.scanAttribute(tempStr, L"frameCount", L"%d", &frameCount);
		
		Time matchTicksPerSec = match.getTime().mTime.ticksPerSec;

		if (!chunk.scanAttribute(tempStr, L"FPS", L"%u", &FPS)) {

			FPS = (TickCount) matchTicksPerSec;
		}

		assrt(((int) matchTicksPerSec) > 0);

		processingFrameCount = frameCount * (int) ((float) FPS / (float) matchTicksPerSec);

		assrt(processingFrameCount > 0);

		if (processingFrameCount > (int) FPS) {

			assrt(false);
			processingFrameCount = FPS;
		}
	}
}

BallSimulation::BallSimulation() 
	: mIsValid(false), mID(0) {

	mEventDiscontinuityEnabled = false;
	setDiscontinuityEventAngle(20.0f);
}

bool BallSimulation::analyzeDiscontinuity(const Index& sampleIndex, const Index& relSampleIndex, Vector3& dir1, Vector3& dir2, bool& isDiscontinuity) {

	if (sampleIndex > relSampleIndex && relSampleIndex >= 1) {

		//analyze
		const Sample* pDirSample1 = getSample(relSampleIndex - 1);
		const Sample* pDirSample2 = getSample(relSampleIndex);
		const Sample* pCurrSample = getSample(sampleIndex);

		pDirSample2->pos.subtract(pDirSample1->pos, dir1);
		pCurrSample->pos.subtract(pDirSample2->pos, dir2);

		dir1.normalize();
		dir2.normalize();
		
		if (dir1.dot(dir2) <= 1.0f - mDiscontinuityEventDeviation) {

			isDiscontinuity = true;

		} else {

			isDiscontinuity = false;
		}

		return true;
	} 
	
	return false;
}

void BallSimulation::signalBallState(const Point& pos, const Vector3& v, const Vector3& w, const Time& time) {

	if (mEventDiscontinuityEnabled) {

		Index lastCount = getSampleCount();

		BallSimulation_Filtered_Trajectory::signalBallState(pos, v, w, time);

		Index newCount = getSampleCount();

		if (lastCount < newCount && lastCount >= 2) {

			Vector3 dir1;
			Vector3 dir2;
			bool isDiscontinuity;

			Index relSampleIndex = lastCount - 1;

			if (analyzeDiscontinuity(newCount - 1, relSampleIndex, dir1, dir2, isDiscontinuity)) {

				if (isDiscontinuity) {

					addEvent(ET_Discontinuity, newCount - 1, &relSampleIndex);
				}
			}
		}

	} else {

		BallSimulation_Filtered_Trajectory::signalBallState(pos, v, w, time);
	}
}

void BallSimulation::resetNew(const Time& time) {

	mEventCount = 0;

	reset(time, true, true, false, false);
}

void BallSimulation::resetContinue() {

	BallSimulation_Filtered::reset(true, false, false);
}

void BallSimulation::enableEvent(EventType evt, bool enable) {

	if (evt == ET_Discontinuity) {

		mEventDiscontinuityEnabled = enable;
	}
}

void BallSimulation::setDiscontinuityEventAngle(float degAngle) {

	mDiscontinuityEventDeviation = cosf(degToRad(degAngle));
}

BallSimulation::Event* BallSimulation::addEvent() {

	++mEventCount;

	mEvents.resize(mEventCount);

	return &(mEvents.el[mEventCount - 1]);
}

BallSimulation::Event* BallSimulation::addEvent(const EventType& type, const Index& sampleIndex, const Index* pRelSampleIndex) {

	Event* pEvent = addEvent();

	pEvent->set(type, sampleIndex);

	if (pRelSampleIndex)
		pEvent->relSampleIndex = *pRelSampleIndex;

	return pEvent;
}

void BallSimulation::render(Renderer& renderer, const float& ballRadiusStart, const float* pBallRadiusEnd, const Index* pStartIndex, const Index* pEndIndex) {

	mRenderEvtIndex = 0;

	BallTrajectory::render(renderer, ballRadiusStart, this, pBallRadiusEnd
									, &RenderColor::kWhite, &RenderColor::kRed
									, &RenderColor::kGreen, &RenderColor::kRed, pStartIndex, pEndIndex);
}

void BallSimulation::onRenderedSample(Renderer& renderer, const BallTrajectory::Index& index, const BallTrajectory::Sample& sample, const float& radius, const RenderColor& color) {
	
	AAPlane evtDrawPlane;
	Vector3 evtDrawPlaneNormal(0.0f);
	evtDrawPlaneNormal.el[Scene_Up] = 1.0f;

	if (mRenderEvtIndex < mEventCount) {

		if (mEvents.el[mRenderEvtIndex].sampleIndex == index) {

			RenderColor col = color;

			col.toColorComplement();
			evtDrawPlane.init(sample.pos, evtDrawPlaneNormal, true);

			renderer.draw(evtDrawPlane, radius * 1.1f, &RigidMatrix4x3::kIdentity, &col);

			while (mEvents.el[mRenderEvtIndex].sampleIndex == index) {

				++mRenderEvtIndex;
			}
		}
	}
}

void BallSimulation::setEmptySimul(Match& match, bool valid) {

	mIsValid = valid;

	if (getSampleCount() != 0) {

		resetNew(match.getTime().getPhysTime());

		mIsPartial = false;
		mID++;
	}
}

void BallSimulation::reSimul(Ball& ball, bool newSimul, const SimulationProcessingRate& rate) {

	BallControllerPhysImpl* pPhysImpl = ball.getControllerPhys();
	Match& match = ball.mNodeMatch;

	mIsValid = true;

	if (pPhysImpl) {

		static int ct = 0;
		DWORD time;

		if (rate.logPerformance) {

			time = timeGetTime();
		}

		const Timing& timing = match.getTime();

		if (newSimul) {

			if (ball.isResting()) {

				setEmptySimul(match, true);

			} else {

				resetNew(timing.getPhysTime());

				if (rate.processingFrameCount <= 1) {
					
					setDuration(rate.maxDuration);

				} else {

					mPartID = 0;
					mIsPartial = true;
					setDuration(rate.durationPerFrame);
				}

				const PhysRigidBody& ballPhys = ball.getPhysBody();

				pPhysImpl->simulate(match, timing, *this, timing.getPhysTime(), ballPhys.pos, ballPhys.v, ballPhys.w);

				++mID;
			}

		} else {

			if (mIsPartial == true) {

				BallSimulation_Filtered::EndReason endReason = getEndReason();

				resetContinue();

				const BallSimulation_Filtered::BallState& ballState = getEndState();

				switch (endReason) {
					case BallSimulation_Filtered::ER_Distance: 
					case BallSimulation_Filtered::ER_Duration: {

						if ((int) mPartID + 1 >= rate.processingFrameCount) {

							setDuration(rate.maxDuration);
							
						} else {
							
							setDuration(rate.durationPerFrame);
						}

						pPhysImpl->simulate(match, timing, *this, ballState.time, ballState.pos, ballState.vel, ballState.w);

					} break;

					default: {
					} break;
				}

				mPartID++;
			}

			if (rate.processingFrameCount <= 1) {

				mIsPartial = false;

			} else {

				if ((int) mPartID >= rate.processingFrameCount) {

					mIsPartial = false;

				} else {

					mIsPartial = true;
				}
			}

			if (!mIsPartial)
				addEndStateAsSample();
					

			if (rate.logPerformance) {

				time = timeGetTime() - time;

				if (mIsPartial) {

					log(L"ballsim%d: part%d %d ms. %u samples", ct, mPartID, (int) time, getSampleCount());

					#ifdef _DEBUG
					String::debug(L"ballsim%d: part%d %d ms. %u samples\n", ct, mPartID, (int) time, getSampleCount());
					#endif

				} else {

					//String dbgStr;
					//dbgStr.assignEx(L"ballsim%d: %d ms. %u samples", ct, (int) time, getSampleCount());
					//ball.mNodeMatch->getConsole().print(dbgStr.c_tstr());

					log(L"ballsim%d: %d ms. %u samples", ct, (int) time, getSampleCount());

					#ifdef _DEBUG
					String::debug(L"ballsim%d: %d ms. %u samples\n", ct, (int) time, getSampleCount());
					#endif
				}

				ct++;
			}
		}
	}
}

bool BallSimulation::simulIsDesynced(Ball& ball) {

	Time time = ball.mNodeMatch->getTime().getTime();

	//make a better estimate comparing ball pos to trajectory

	if (mIsValid) {

		if (ball.isResting()) 
			return true;
		
		const BallSimulation_Filtered::BallState& endState = getEndState();

		Time startTime = getStartTime();
		time -= startTime;

		if (time - endState.time > 0.0f)
			return true;

		Index preIndex = -1;
		Index postIndex = -1;

		for (Index i = mTimeValidSampleIndex; i < getSampleCount(); ++i) {

			const Sample& sample = dref(getSample(i));

			if (time == sample.time) {

				preIndex = i;
				postIndex = i;

				break;

			} else if (time > sample.time) {

				preIndex = i;

			} else {

				postIndex = i;
				break;
			}
		}

		if (preIndex != -1 && postIndex != -1) {

			mTimeValidSampleIndex = preIndex;

			/*
			if (preIndex == postIndex) {
				
				const Vector3& currPos = ball.mTransformWorld.getPosition();

				float distSq = distanceSq(getSample(preIndex)->pos, currPos);
				float maxDist = ball.getRadius(false);

				if (distSq > maxDist * maxDist)
					return true;

				return false;

			} else {

				const Vector3& currPos = ball.mTransformWorld.getPosition();

				float distSq = distanceSq(getSample(preIndex)->pos, currPos);
				float maxDistSq = distanceSq(getSample(postIndex)->pos, getSample(preIndex)->pos);

				if (distSq > maxDistSq)
					return true;

				return false;
			}
			*/
		}

		for (; mTimeValidCollSampleIndex < getCollSampleCount(); ++mTimeValidCollSampleIndex) {

			const CollSample& sample = getCollSample(mTimeValidCollSampleIndex);

			if (sample.prevSampleIndex < mTimeValidSampleIndex)
				break;
		}
		
		return false;
	} 

	return true;
}

void BallSimulation::reSyncToBall(Ball& ball, bool ballWasManipulated, const SimulationProcessingRate& rate) {

	mTestSimul.updateSync(ball);

	if (ball.isSimulable()) {

		enum SimulAction {

			SA_None = 0, SA_New, SA_Continue
		};

		SimulAction action = SA_None;
		bool ballSimulIsDirty = ballWasManipulated;

		if (mIsValid) {

			if (ballSimulIsDirty) {

				action = SA_New;

			} else if (mIsPartial) {

				action = SA_Continue;

			} else if (simulIsDesynced(ball)) {

				action = SA_New;
			}

		} else {

			action = SA_New;
		}

		if (action != SA_None) {

			ballSimulIsDirty = false;
		}

		switch (action) {

			case SA_New: {

				mTimeValidSampleIndex = 0;
				mTimeValidCollSampleIndex = 0;
				reSimul(ball, true, rate);

			} break;

			case SA_Continue: {

				reSimul(ball, false, rate);

			} break;

			default: {
			} break;
		}

	} else {

		setEmptySimul(ball.mNodeMatch, false);
	}
}

void BallSimulation::render(Ball& ball, Renderer& renderer, const Index* pStartIndex, const Index* pEndIndex) {

	/*
	if (mIsValid) {

		float r1 = ball.getRadius(false) * 0.25f;
		float r2 = r1 * 3.0f;

		Index startIndex = pStartIndex ? MMax(*pStartIndex, mTimeValidSampleIndex) : mTimeValidSampleIndex;

		render(renderer, r1, &r2, &startIndex, pEndIndex);
	}
	*/

	if (mTestSimul.isValid()) {

		float r1 = ball.getRadius(false) * 0.25f;
		float r2 = r1 * 3.0f;

		mTestSimul.render(renderer, true);

	}
}

bool BallSimulation::searchForTime(const Time& _time, Index& sample1, Index& sample2) {

	if (!isValid())
		return false;

	Time time = _time - getStartTime();
	Index searchIndex = getTimeValidSampleIndex();

	sample1 = -1;

	while (searchIndex != -1 && getSample(searchIndex)->time > time) {

		--searchIndex;
	}

	if (searchIndex == -1)
		return false;

	sample1 = searchIndex;
	
	if (searchIndex + 1 >= getSampleCount()) {

		if (getEndReason() == ER_Resting) {

			sample1 = sample2;
			return true;
		}

		return false;
	}

	sample2 = searchIndex + 1;

	return true;
}

} }