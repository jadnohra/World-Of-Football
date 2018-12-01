#include "WOFMatchFootballer_Brain_AI.h"

#include "WE3/WEMacros.h"
using namespace WE;

#include "../../WOFMatch.h"
#include "WOFMatchFootballer.h"
#include "WOFMatchFootballer_Brain.h"
#include "../../WOFMatchTeam.h"

namespace WOF { namespace match {

Footballer_Brain_AI::Footballer_Brain_AI() {

	mNextUpdateTime = 0.0f;
}

void Footballer_Brain_AI::cancel_init_create() {
}

bool Footballer_Brain_AI::init_create(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

bool Footballer_Brain_AI::init_nodeAttached(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	return true;
}

void Footballer_Brain_AI::init_prepareScene(Match& match, Team& team, Footballer& footballer) {

	mStartPos = footballer.mTransformLocal.getPosition();
}

void Footballer_Brain_AI::attach(Match& match, Team& team, Footballer_Brain& brain) {

#ifdef FOOTBALLER_BODY1

	switch(brain.body_getCurrState()) {

		case Footballer_Body::BS_Tackling:
		case Footballer_Body::BS_Shooting: {

			mNextUpdateCriterium = NUC_Idle;
			mNextUpdateTime	= 0;

			mState = brain.body_getCurrState();

		} break;

		case Footballer_Body::BS_Running: {

			mNextUpdateCriterium = NUC_Time;
			mNextUpdateTime	= 0;

			mState = Footballer_Body::BS_Idle;
			brain.body_setIdle();

		} break;
		
		case Footballer_Body::BS_Idle:
		default: {

			mNextUpdateCriterium = NUC_Time;
			mNextUpdateTime	= 0;

			mState = Footballer_Body::BS_Idle;

		} break;
	}
#endif
}

void Footballer_Brain_AI::detach(Match& match, Team& team, Footballer_Brain& brain) {
}


void Footballer_Brain_AI::frameMove(Match& match, Team& team, Footballer_Brain& brain, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength) {

#ifdef FOOTBALLER_BODY1

	if (match.mTweakEnableAI == false) {

		brain.body_setIdle();
		return;
	}

	if (mNextUpdateCriterium == NUC_Idle) {

		if (brain.body_getCurrState() == Footballer_Body::BS_Idle) {

			mNextUpdateCriterium = NUC_Time;
			mNextUpdateTime	= 0;
		}
	}

	Footballer_Body::EventIndex eventCount = brain.body_getEventCount();

	for (Footballer_Body::EventIndex i = 0; i < eventCount; ++i) {

		Footballer_Body::Event& evt = brain.body_getEvent(i);

		if (evt.type == Footballer_Body::ET_MoveBlockEnded) {

			brain.body_setRunning_dir(mRunDir, true);
		}
	}

	if (mNextUpdateTime <= time.t_discreet) {

		mNextUpdateTime = time.t_discreet + MRand2f(1.0f, 5.0f);

		mState = ((int) mNextUpdateTime) % 3;

		switch (mState) {
			case Footballer_Body::BS_Running:
				{
					const FastUnitCoordSys& coordSys = match.getCoordSys();

					int runType = ((int) MRandf(10.0f)) % 3;
					bool randomRunDir = true;

					int runBack = ((int) MRandf(10.0f)) % 2;

					if (runBack) {

						mStartPos.subtract(brain.mTransformLocal.getPosition(), mRunDir);
						
						if (mRunDir.isZero() == false) {

							randomRunDir = false;

							float vel;
							float runDist;

							mRunDir.normalize(runDist);
							brain.body_toVelocity_Running(coordSys, (Footballer::RunType) runType, vel);

							mNextUpdateTime = time.t_discreet + MMax(1.0f, runDist / vel) + MRandf(2.0f);
						}

					} 

					if (randomRunDir) {

						coordSys.setRUF(MRand2f(-1.0f, 1.0f), 0.0f, MRand2f(-1.0f, 1.0f), mRunDir.el);
						mRunDir.normalize();
					}

					brain.body_setRunning_type((Footballer_Body::RunType) runType);
					brain.body_setRunning_dir(mRunDir);
					brain.body_setRunning();

					
				}
				break;
			case Footballer_Body::BS_Tackling:
				{
					brain.body_setTackling(Footballer_Body::ST_ShotPass);
				}
				break;
			default:
				{
					brain.body_setIdle();
				}
				break;
		}
	
	}
	
	brain.body_clearEvents();

#endif
}

void Footballer_Brain_AI::render(Match& match, Team& team, Footballer_Brain& brain, Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass) {

#ifdef FOOTBALLER_BODY1

	int dbgX = (brain.mTeam == _TeamA) ? 100 : 600;
	int dbgY = 200 + brain.mNumber * 25;
	
	String str;

	Footballer::BallOwnership& own = brain.getBallOwnership();

	if (own.isBallOwner) {

		

		str.assignEx(L"[%d-%d] Body State [%s] Ball [%g - %g] [%f]", 
					(int) brain.mTeam, (int) brain.mNumber, 
					Footballer_Body::toString(brain.body_getCurrState()),
					match.getCoordSys().invConvUnit(brain.getRelBallDist()),
					radToDeg(brain.getRelBallAngle()),
					own.ballControl
					);

	} else {

		str.assignEx(L"[%d-%d] Body State [%s] Ball [%g]", 
					(int) brain.mTeam, (int) brain.mNumber, 
					Footballer_Body::toString(brain.body_getCurrState()),
					match.getCoordSys().invConvUnit(brain.getRelBallDist())
					);
	}

	{
		String switchStr;

		match.getControlSwitcher().getFootballerDebugText(match, brain, switchStr);

		if (switchStr.isValid()) {
			
			str.append(L" Switch ");
			str.append(switchStr);
		}
	}

	renderer.drawText(str.c_tstr(), dbgX, dbgY);

#endif
}


} }

