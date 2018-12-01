#include "WEDataTranslator.h"

namespace WE {

void DataTranslator::dummyTranslate(const TCHAR* source, BufferString& dest) {

	dest.assign(source);
}

void DataTranslator::dummyTranslate(BufferString& inPlace) {
}

}