#ifndef _WOFMatchFootballer_Brain_AI_h
#define _WOFMatchFootballer_Brain_AI_h

#include "WE3/string/WEBufferString.h"
#include "WE3/data/WEDataSourceChunk.h"
#include "WE3/coordSys/WECoordSysConv.h"
#include "WE3/render/WERenderer.h"
using namespace WE;

#include "../../WOFMatchTime.h"

namespace WOF { namespace match {

	class Match;
	class Footballer_Brain;
	class Footballer;
	class Team;

	class Footballer_Brain_AI {
	public:

		Footballer_Brain_AI();

		void cancel_init_create();
		bool init_create(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(Match& match, Footballer_Brain& brain, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene(Match& match, Team& team, Footballer& footballer);

		void frameMove(Match& match, Team& team, Footballer_Brain& brain, const DiscreeteTime& time, const TickCount& tickIndex, const float& tickLength);
		void render(Match& match, Team& team, Footballer_Brain& brain, Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);

		void attach(Match& match, Team& team, Footballer_Brain& brain);
		void detach(Match& match, Team& team, Footballer_Brain& brain);

	protected:

		enum  NextUpdateCriterium {

			NUC_None = 0, NUC_Time, NUC_Idle
		};
			
		NextUpdateCriterium mNextUpdateCriterium;
		Time mNextUpdateTime;
		int mState;

		Vector3 mStartPos;
		Vector3 mRunDir;
	};
	
} }

#endif