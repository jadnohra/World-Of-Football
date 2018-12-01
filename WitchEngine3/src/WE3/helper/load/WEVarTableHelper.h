#ifndef _WEVarTableHelper_h
#define _WEVarTableHelper_h

#include "../../script/WEScriptVarTable.h"
#include "../../data/WEDataSource.h"

namespace WE {


	class VarTableHelper {
	public:

		static void setUnsetParams(BufferString& tempStr, ScriptVarTable* pVarTable, DataChunk& paramParentChunk, int& attrCount, bool set);
	};

}

#endif