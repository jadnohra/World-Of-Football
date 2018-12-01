#include "WEDataPath.h"

#include "../WEMacros.h"

namespace WE {


DataPath::Part::Part() : pString(NULL)/*, hash(0)*/ {
}

DataPath::Part::~Part() {

	MMemDelete(pString);
}

void DataPath::Part::assign(const TCHAR* buffer, StringPartBase& part) {

	MMemDelete(pString);
	
	MMemNew(pString, String());

	part.assignTo(buffer, *pString);
	//hash = pString->hash();
}


DataPath::DataPath(const TCHAR* pPath) {

	append(pPath);
}

DataPath::~DataPath() {

	destroy();
}

void DataPath::destroy() {

	mParts.destroy();
}

DataPath::PartIndex DataPath::getPartCount() const {

	return mParts.count;
}

String* DataPath::getPart(PartIndex index) const {

	return mParts.el[index]->pString;
}

void DataPath::append(const TCHAR* pPath) {

	destroy();

	String completePath(pPath);

	if (completePath.length() == 0) {

		return;
	}

	const TCHAR* buff = completePath.c_tstr();

	int index = 0;
	bool found;

	int partCount = completePath.count(L'/') + 1;
	StringPartBase part;

	mParts.reserve(partCount);
	
	for (int i = 0; i < partCount; ++i) {
		
		index = completePath.findNext(L'/', index, found, part.startIndex, part.count, false);
		++index;

		if (found) {

			Part* pPart;
			MMemNew(pPart, Part());

			pPart->assign(completePath.c_tstr(), part);

			mParts.addOne(pPart);

		} else {

			break;
		}
	} 
}

/*
void DataPath::appendElement(const TCHAR* pElement) {

	int len = String::length(pElement);

	if (len == 0) {

		return;
	}

	Part& part = mParts.addOne();

	part.startIndex = mPath.length() + 1;
	part.count = len;

	mPath.append(L"/");
	mPath.append(pElement);

	part.updateHash(mPath.c_tstr());
}
*/

}