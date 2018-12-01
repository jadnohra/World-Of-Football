#include "WEScriptKeyArray.h"

namespace WE {

void ScriptKeyArray::addFromAttributes(DataChunk* pChunk) {

	if (pChunk != NULL) {

		int attrCount = pChunk->getAttributeCount();

		for (int i = 0; i < attrCount; ++i) {

			SoftPtr<Record> record = mRecords.addNewOne();

			if (!pChunk->getAttribute(i, &record->key, &record->value)) {

				mRecords.removeOne();
			}
		}
	}
}

void ScriptKeyArray::addTo(ScriptVarTable& table) {

	for (Records::Index i = 0; i < mRecords.count; ++i) {

		table.setValue(mRecords[i]->key.c_tstr(), mRecords[i]->value.c_tstr());
	}
}

}