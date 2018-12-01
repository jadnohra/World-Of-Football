#ifndef _WEScriptVarTable_h
#define _WEScriptVarTable_h

#include "../WEString.h"
#include "../WETL/WETLHashMap.h"
#include "../data/WEDataTranslator.h"
#include "WEScriptEngine.h"

namespace WE {

	class ScriptVarTable : public DataTranslator {
	public:

		typedef WETL::IndexShort VariableIndex;

	public:

		ScriptVarTable();
		~ScriptVarTable();

		inline void setScriptEngine(ScriptEngine* pEngine) { mScriptEngine = pEngine; }

		void destroy();
		void removeAll();

		void reserve(VariableIndex count);
		
		const TCHAR* getValue(const StringHash& variableHash);
		const TCHAR* getValue(const TCHAR* variable);
		const TCHAR* getValue(const StringPart& variable);

		String* createValue(const TCHAR* variable);

		String* getValueRef(const StringHash& variableHash);
		String* getValueRef(const TCHAR* variable);
		String* getValueRef(const StringPart& variable);

		void setValue(const TCHAR* variable, const TCHAR* value);
		void unsetValue(const TCHAR* variable);
		
		bool isSet(const TCHAR* variable);
		bool isSet(const StringHash& variableHash);

		bool isParametrized(const TCHAR* source, TCHAR left = L'[', TCHAR right = L']');
		bool isScripted(const TCHAR* source, TCHAR left = L'{', TCHAR right = L'}');
		void replaceInString(const TCHAR* in, String& out, TCHAR left = L'[', TCHAR right = L']', TCHAR scriptLeft = L'{', TCHAR scriptRight = L'}');
	
		virtual bool inject(const TCHAR* variable, const TCHAR* value) {

			setValue(variable, value);
			return true;
		}

		virtual void translate(const TCHAR* source, BufferString& dest);
		virtual void translate(BufferString& inPlace);

	protected:
		
		typedef WETL::PtrHashMap<StringHash, String*, VariableIndex::Type> VariablesByHash;

		VariablesByHash mVariablesByHash;
		SoftPtr<ScriptEngine> mScriptEngine;
	};

}


#endif