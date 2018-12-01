#ifndef _WOFMatchFootballerBody_ConfigurableAction_h
#define _WOFMatchFootballerBody_ConfigurableAction_h

#include "WE3/data/WEDataSource.h"
#include "WE3/coordSys/WECoordSysConv.h"
#include "WE3/WETime.h"
using namespace WE;


namespace WOF { namespace match {

	class Footballer_Body;

	class FootballerBody_ConfigurableAction {
	public:

		class CommandProcessor;
		struct TimedCommandIter;

		struct DynamicsState {

			Vector3 acc;
			Vector3 vel;
		};

		typedef WETL::Array<DynamicsState, false, unsigned int> DynamicsStates;

	public:

		bool load(Footballer_Body& body, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, const Time& animLength, const Time& tickLength, CommandProcessor* pCommandProcessor);
		float getSpeedFactor(Footballer_Body& body, const Vector3& bodyAcc, const Vector3& bodyVel, const float& defaultSpeedFactor, const Time& tickLength, DynamicsStates& dynamicsStates);
	
	public:

		static bool loadTime(BufferString& tempStr, DataChunk& chunk, Time& time, const Time& totalTime);

	public:

		class Command {
		public:

			Command() : mTime(0.0f) {}

			virtual ~Command() {}

			inline const Time& getTime() { return mTime; }
			virtual int getType() = 0;

		public:

			Time mTime;
		};

		class CommandProcessor {
		public:

			virtual Command* createCommand(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, const Time& totalTime) = 0;
			virtual void playSound(const Time& time) {}
		};

		enum StockCommandType {

			Command_None = -1, Command_PlaySound, Command_StartDynamics, Command_StopDynamics, CommandCount
		};

		class Dynamics;

		class StockCommand_PlaySound : public Command {
		public:

			StockCommand_PlaySound() {}

			virtual int getType() { return Command_PlaySound; }
		};

		

		class StockCommand_StartDynamics : public Command {
		public:

			StockCommand_StartDynamics() : linkPrevBody(false), adjustSpeedFactor(false), dynamicsStateIndex(0) {}

			virtual int getType() { return Command_StartDynamics; }

		public:

			SoftPtr<Dynamics> dynamics;
			SoftPtr<Dynamics> linkPrev;
			bool linkPrevBody;
			bool adjustSpeedFactor;
			DynamicsStates::Index dynamicsStateIndex;
		};

		class StockCommand_StopDynamics : public Command {
		public:

			StockCommand_StopDynamics() {}

			virtual int getType() { return Command_StopDynamics; }

		public:

			SoftPtr<StockCommand_StartDynamics> startCommand;
		};
	
	public:

		class Dynamics {
		public:

			virtual ~Dynamics() {}

			virtual bool load(Footballer_Body& body, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) { return true; }

			virtual bool isSimulable(Footballer_Body& body) { return false; }
			virtual bool hasRestingState(Footballer_Body& body, const Vector3& startAcc, const Vector3& startVel) { return false; }
			virtual bool simulate(Footballer_Body& body, const Time& tickLength, Vector3& inOutAcc, Vector3& inOutVel, Time& t) { return false; }

			virtual void start(Footballer_Body& body, const Time& t, Vector3& startAcc, Vector3& startVel) {}
			virtual void stop(Footballer_Body& body, const Time& t) {}

			virtual bool frameMove(Footballer_Body& body, const Time& t, const Time& dt, Vector3& currAcc, Vector3& currVel) { return false; }

		public:

			StringHash mNameHash;
		};

		class BodyDynamics : public Dynamics {
		public:

			Vector3 acc;
			Vector3 vel;
		};

	protected:

		friend class FootballerBody_ConfigurableActionInstance;	

		typedef WETL::PtrArrayEx<Dynamics, unsigned int> DynamicsArray;
		typedef WETL::CountedPtrArrayEx<Command, unsigned int, WETL::ResizeExact> Commands;
		
	public:

		struct TimedCommandIter {
	
			Commands::Index nextCommandIndex;

			TimedCommandIter() : nextCommandIndex(0) {}
		};

	protected:

		Dynamics* findDynamics(const String& str);
		StockCommand_StartDynamics* findStartDynamicsCommand(const String& str);

		static int compareCommandTime(const void *_pComp1, const void *_pComp2);
				
	protected:

		float mMinSpeedFactor;
		float mMaxSpeedFactor;

		DynamicsArray mDynamicsArray;
		Commands mCommands;
		bool mHashAdjustSpeedCommands;
		DynamicsStates::Index mNeededDynamicsStateCount;
	};

} }

#endif