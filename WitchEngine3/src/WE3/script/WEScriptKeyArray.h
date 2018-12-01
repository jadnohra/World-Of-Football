#ifndef _WEScriptKeyArray_h
#define _WEScriptKeyArray_h

#include "../WEString.h"
#include "../WETL/WETLArray.h"
#include "../data/WEDataSource.h"
#include "WEScriptVarTable.h"

namespace WE {

	class ScriptKeyArray {
	public:

		struct Record {

			String key;
			String value;
		};

	public:

		void addFromAttributes(DataChunk* pChunk);
		void addTo(ScriptVarTable& table);

	protected:

		typedef WETL::CountedPtrArrayEx<Record, unsigned int, WETL::ResizeExact> Records;

		Records mRecords;
	};

}

#endif