#include "BallCommandManager.h"

#include "../../Match.h"
#include "Ball.h"

#include "WE3/coordSys/script/WEScriptCoordSysConv.h"
#include "WE3/script/WEScriptDataChunk.h"
using namespace WE;

DECLARE_INSTANCE_TYPE_NAME(WOF::match::BallCommandManager, CBallCommandManager);

namespace WOF { namespace match {

const TCHAR* BallCommandManager::ScriptClassName = L"CBallCommandManager";

void BallCommandManager::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<BallCommandManager>(ScriptClassName).
		func(&BallCommandManager::script_getCommandCount, L"getCommandCount").
		func(&BallCommandManager::script_getCommand, L"getCommand").
		func(&BallCommandManager::script_getFinalCommand, L"getFinalCommand");
}

bool BallCommandManager::init(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (chunk.getAttribute(L"scriptBallCommandManagerClass", tempStr)) {

		ScriptEngine& scriptEngine = match.getScriptEngine();

		if (scriptEngine.createInstance(tempStr.c_tstr(), mScriptObject)) {

			ScriptFunctionCall<bool> func(scriptEngine, mScriptObject, L"load");

			if (!func(this, scriptArg(chunk), scriptArg(pConv))) {

				assrt(false);
				return false;
			}
	
		} else {

			assrt(false);
			return false;
		}

	} else {

		assrt(false);
		return false;
	}

	return true;
}

void BallCommandManager::process(Match& match, Ball& ball) {

	ScriptEngine& scriptEngine = ball.mNodeMatch->getScriptEngine();

	mFinalCommand.invalidate();

	for (Commands::Index i = 0; i < mCommands.count; ++i) {

		if (!mCommands[i].isValid()) {

			mCommands.removeSwapWithLast(i, true);
			--i;
		}
	}

	if (!mScriptObject.IsNull()) {

		ScriptFunctionCall<void> func(scriptEngine, mScriptObject, L"process");
		func(scriptArg(this), scriptArg(ball), match.getClock().getTime());
	}

	/*
	for (Commands::Index i = 0; i < mCommands.count; ++i) {

		BallCommand& command = mCommands[i];

		if (!command.isValid())
			command.notifyRejected();
	}
	*/

	if (mFinalCommand.isValid())
		mLastCommand = mFinalCommand;
	
	mCommands.count = 0;
}

} }