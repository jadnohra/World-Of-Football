#ifndef h_WOF_match_BallCommandManager
#define h_WOF_match_BallCommandManager

#include "WE3/data/WEDataSource.h"
#include "WE3/coordSys/WECoordSysConv.h"
using namespace WE;

#include "BallCommand.h"
#include "../../../script/ScriptEngine.h"

namespace WOF { namespace match {

	class Match;

	class BallCommandManager {
	public:

		BallCommandManager() {}

		bool init(Match& match, BufferString& tempStr, DataChunk& ballChunk, CoordSysConv* pConv);
		void process(Match& match, Ball& ball);

		BallCommand* getValidFinalCommand() { return ((mFinalCommand.type != BallCommand_None) ? &mFinalCommand : NULL); }
		inline BallCommand& addCommand() { return mCommands.addOne(); }; 

		inline BallCommand& getLastCommand() { return mLastCommand; }
		BallCommand* getValidLastCommand() { return ((mLastCommand.type != BallCommand_None) ? &mLastCommand : NULL); }
		
		//only use this if you know what you are doing ... commands could get lost
		//without notifications
		inline void clearCommands() { mCommands.count = 0; }

	public:

		inline unsigned int script_getCommandCount() { return mCommands.count; }
		inline BallCommand* script_getCommand(unsigned int index) { return scriptArg(mCommands[index]); }

		inline BallCommand* script_getFinalCommand() { return scriptArg(mFinalCommand); }

	public:

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);

	protected:

		typedef WETL::CountedArray<BallCommand, false, unsigned int> Commands;

		Commands mCommands;
		BallCommand mFinalCommand;
		BallCommand mLastCommand;
		

		ScriptObject mScriptObject;
	};

	inline BallCommandManager* scriptArg(BallCommandManager& mgr) { return &mgr; }
	inline BallCommandManager* scriptArg(BallCommandManager* mgr) { return mgr; }

} }

#endif