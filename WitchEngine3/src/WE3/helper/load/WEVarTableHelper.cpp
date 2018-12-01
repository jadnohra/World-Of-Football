#include "WEVarTableHelper.h"

namespace WE {


void VarTableHelper::setUnsetParams(BufferString& tempStr, ScriptVarTable* pVarTable, DataChunk& paramParentChunk, int& attrCount, bool set) {

	SoftRef<DataChunk> paramChunk = paramParentChunk.getChild(L"params");

	if (paramChunk.isValid()) {

		if (set) {
			
			attrCount = paramChunk->getAttributeCount();
		}

		if (attrCount != 0) {

			if (pVarTable == NULL) {

				assrt(false);
				return;
			}
		}

		int i;
		if (set) {

			for (i = 0; i < attrCount; i++) {

				if (paramChunk->getAttribute(i, &tempStr, NULL)) {
				
					const TCHAR* currValue = pVarTable->getValue(tempStr.c_tstr());

					if (currValue != NULL) {

						assrt(false);
						log(L"VarTable Overwritten key[%s] value[%s]", tempStr.c_tstr(), currValue);
					}

					BufferString value;
					paramChunk->getAttribute(i, NULL, &value);

					pVarTable->translate(value);

					pVarTable->setValue(tempStr.c_tstr(), value.c_tstr());
				}
			}

		} else {

			for (i = 0; i < attrCount; i++) {

				if (paramChunk->getAttribute(i, &tempStr, NULL)) {
				
					pVarTable->unsetValue(tempStr.c_tstr());
				}
			}
		}
	}

}


}