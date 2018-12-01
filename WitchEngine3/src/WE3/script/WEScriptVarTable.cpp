#include "WEScriptVarTable.h"

#include "../WEMem.h"

namespace WE {

ScriptVarTable::ScriptVarTable() {
}

ScriptVarTable::~ScriptVarTable() {

	destroy();
}

void ScriptVarTable::destroy() {

	mVariablesByHash.destroy();
}

void ScriptVarTable::reserve(VariableIndex count) {

	mVariablesByHash.reserve(count);
}

void ScriptVarTable::removeAll() {

	mVariablesByHash.removeAll();
}

void ScriptVarTable::unsetValue(const TCHAR* variable) {

	String* pVal;
	StringHash hash = String::hash(variable);

	if (mVariablesByHash.find(hash, pVal)) {

		MMemDelete(pVal);
	}

	mVariablesByHash.remove(hash);
}

bool ScriptVarTable::isSet(const TCHAR* variable) {

	return mVariablesByHash.hasKey(String::hash(variable));
}

bool ScriptVarTable::isSet(const StringHash& variableHash) {

	return mVariablesByHash.hasKey(variableHash);
}

String* ScriptVarTable::createValue(const TCHAR* variable) {

	String* pVal;
	StringHash hash = String::hash(variable);

	if (mVariablesByHash.find(hash, pVal)) {

		return pVal;
	}

	MMemNew(pVal, String());
	if (mVariablesByHash.insert(hash, pVal) == false) {

		return NULL;
	}

	return pVal;
}

String* ScriptVarTable::getValueRef(const StringHash& hash) {

	String* pVal;

	if (mVariablesByHash.find(hash, pVal)) {

		return pVal;
	}

	return NULL;
}

String* ScriptVarTable::getValueRef(const TCHAR* variable) {

	return getValueRef(String::hash(variable));
}

String* ScriptVarTable::getValueRef(const StringPart& variable) {

	return getValueRef(variable.hash());
}

const TCHAR* ScriptVarTable::getValue(const StringHash& hash) {

	String* pVal;

	if (mVariablesByHash.find(hash, pVal)) {

		return pVal ? pVal->c_tstr() : NULL;
	}

	return NULL;
}

const TCHAR* ScriptVarTable::getValue(const TCHAR* variable) {

	return getValue(String::hash(variable));
}

const TCHAR* ScriptVarTable::getValue(const StringPart& variable) {

	return getValue(variable.hash());
}

void ScriptVarTable::setValue(const TCHAR* variable, const TCHAR* value) {

	String* pVal;
	StringHash hash = String::hash(variable);

	if (mVariablesByHash.find(hash, pVal)) {

		pVal->assign(value);

	} else {

		MMemNew(pVal, String(value));

		mVariablesByHash.insert(hash, pVal);
	}
}


/*
void ScriptVarTable::replaceInString(const String& in, String& out, TCHAR left, TCHAR right) {

	int indexLeft;
	int indexRight;
	
	out.destroy();

	if (in.length() == 0) {

		return;
	}

	StringPart value(&in);
	bool found;
	indexRight = -1;
	
	indexLeft = in.findNext(left, indexRight + 1, found, value.startIndex, value.count, false);

	while(found) {
		
		if (value.length()) {

			value.appendTo(out);
		}

		indexRight = in.findNext(right, indexLeft + 1, found, value.startIndex, value.count, false);

		if (found == false) {

			break;
		}

		if (value.length()) {

			out.append(getValue(value.hash()));
		}

		indexLeft = in.findNext(left, indexRight + 1, found, value.startIndex, value.count, false);
	}
}
*/

void ScriptVarTable::replaceInString(const TCHAR* _in, String& out, TCHAR paramLeft, TCHAR paramRight, TCHAR scriptLeft, TCHAR scriptRight) {


	int indexLeft;
	int indexRight;
	const TCHAR* in;
	int len;
	String tempIn;
	String temp;
	
	out.destroy();

	for (int phase = 0; phase < 2; ++phase) {

		TCHAR left;
		TCHAR right;

		if (phase == 0) {

			in = _in;
			
			left = paramLeft;
			right = paramRight;

		} else {

			if (!mScriptEngine.isValid() || !isScripted(out.c_tstr(), scriptLeft, scriptRight))
				return;

			tempIn.assign(out);
			out.destroy();

			in = tempIn.c_tstr();
			
			left = scriptLeft;
			right = scriptRight;
		}

		len = String::length(in, true);

		if (len == 0) {

			return;
		}

		StringHash hash;
		StringPartBase value;
		bool found;
		indexRight = -1;
		
		indexLeft = String::findNext(in, left, indexRight + 1, found, value.startIndex, value.count, false, len);

		while(found) {
			
			if (value.length()) {

				value.appendTo(in, out);
			}

			indexRight = String::findNext(in, right, indexLeft + 1, found, value.startIndex, value.count, false, len);

			if (found == false) {

				break;
			}

			if (value.length()) {

				if (phase == 0) {

					hash = value.hash(in);

					if (isSet(hash)) {

						out.append(getValue(hash));

					} else {

						//if value not found only remove left and right chars
						value.appendTo(in, out);
					}

				} else {

					temp.assign(L"print(");
					value.appendTo(in, temp);
					temp.append(L")");
					mScriptEngine->run(temp.c_tstr(), ScriptEngine::Out_Current, ScriptEngine::Out_Silent, &out);
				}
			}

			indexLeft = String::findNext(in, left, indexRight + 1, found, value.startIndex, value.count, false, len);
		}
	}
}

bool ScriptVarTable::isParametrized(const TCHAR* source, TCHAR left, TCHAR right) {

	return (String::count(source, left) != 0);
}

bool ScriptVarTable::isScripted(const TCHAR* source, TCHAR left, TCHAR right) {

	return (String::count(source, left) != 0);
}

void ScriptVarTable::translate(const TCHAR* source, BufferString& dest) {

	if (isParametrized(source)) {

		replaceInString(source, dest);

	} else {

		dummyTranslate(source, dest);
	}

}

void ScriptVarTable::translate(BufferString& inPlace) {

	if (isParametrized(inPlace.c_tstr())) {

		BufferString temp(inPlace);

		replaceInString(temp.c_tstr(), inPlace);

	}
}

}