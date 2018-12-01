#ifndef h_WOF_match_SceneScriptedCharacter_TImpl
#define h_WOF_match_SceneScriptedCharacter_TImpl

namespace WOF { namespace match {

	template<typename OverloadingClassT>
	bool SceneScriptedCharacter::init_create(OverloadingClassT* pThis, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv, ObjectType objType) {

		if (!SceneEntityMesh::init_create(match, tempStr, chunk, pConv, objType)) {

			return false;
		}

		if (chunk.getAttribute(L"scriptClass", tempStr)) {

			ScriptEngine& scriptEngine = mNodeMatch->getScriptEngine();

			if (scriptEngine.createInstance(tempStr.c_tstr(), mScriptObject)) {

				ScriptFunctionCall<bool> func(scriptEngine, mScriptObject, L"load");

				if (!func(pThis, scriptArg(chunk), scriptArg(pConv))) {

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

} }

#endif