#include "WEDataSource_TinyXML.h"

#include "../../WEMem.h"
#include "../../WEMacros.h"

#pragma warning( disable : 4995 4996)

#include "tchar.h"
#include "string.h"

namespace WE {

bool strequal(const TCHAR* str1, int str1l, const char* str2, int str2l) {

	if (str1l != str2l) {

		return false;
	}

	for (int i = 0; i < str1l; ++i) {

		if (((char) str1[i]) != str2[i]) {

			return false;
		}
	}

	return true;
}


DataChunk_TinyXML::DataChunk_TinyXML(DataSource* pSource, TiXmlElement* pEl, bool deleteEl, DataChunk* pParent) 
	: mDeleteEl(deleteEl) , mpEl(pEl), mParent(pParent, true) {

	mSource.set(pSource, true);
}

DataChunk_TinyXML::~DataChunk_TinyXML() {

	if (mDeleteEl) {

		MMemDelete(mpEl);
	}
}

DataChunk* DataChunk_TinyXML::getParent() {

	return mParent.bumpedPtr();
}

char* DataChunk_TinyXML::getTempChar(const TCHAR* source, int len) {

	if (len == 0) {

		len = String::length(source, false);
	}

	if (len == 0) {

		return NULL;
	}

	++len;

	mTempChars.resize(len);

	tcharToChar(source, mTempChars.el, len);

	return mTempChars.el;
}


bool DataChunk_TinyXML::equals(const TCHAR* comp) {

	if (mpEl == NULL) {

		return false;
	}

	const char* valStr = mpEl->Value();

	return (strequal(comp, safeSizeCast<size_t, int>(_tcslen(comp)), valStr, safeSizeCast<size_t, int>(strlen(valStr))));
}

bool DataChunk_TinyXML::getValue(String& value) {

	if (mpEl == NULL) {

		return false;
	}

	charToString(mpEl->Value(), value);
			
	return true;
}


const char* DataChunk_TinyXML::findAttributeValue(const TCHAR* attribute) {

	if (mpEl == NULL) {

		return NULL;
	}

	return mpEl->Attribute(getTempChar(attribute));

	/*
	TiXmlAttribute* pAttr = mpEl->FirstAttribute();
	int attrlen = safeSizeCast<size_t, int>(_tcslen(attribute));
	const char* valStr;

	while (pAttr) {
		
		valStr  = pAttr->Name();
		if (strequal(attribute, attrlen, valStr, safeSizeCast<size_t, int>(strlen(valStr)))) {

			return pAttr;
		}

		pAttr = pAttr->Next();
	}

	return NULL;
	*/
}

int DataChunk_TinyXML::getAttributeCount() {

	if (mpEl == NULL) {

		return 0;
	}

	int attrCount = 0;

	TiXmlAttribute* pAttr = mpEl->FirstAttribute();

	while (pAttr != NULL) {

		++attrCount;
		pAttr = pAttr->Next();
	}

	return attrCount;
}

bool DataChunk_TinyXML::getAttribute(int attrIndex, String* pName, String* pValue) {

	if (mpEl == NULL) {

		return 0;
	}

	int attrCount = 0;

	TiXmlAttribute* pAttr = mpEl->FirstAttribute();

	while (pAttr != NULL) {

		if (attrCount == attrIndex) {

			if (pName) {
				charToString(pAttr->Name(), *pName);
			}
			if (pValue) {
				charToString(pAttr->Value(), *pValue);
			}
			return true;
		}

		++attrCount;
		pAttr = pAttr->Next();
	}

	return false;
}


bool DataChunk_TinyXML::getAttribute(const TCHAR* attribute, String& value) {

	
	const char* pAttrVal = findAttributeValue(attribute);
	
	if (pAttrVal == NULL) {

		return false;
	}
	
	charToString(pAttrVal, value);
	
	translate(value);

	return true;
}

bool DataChunk_TinyXML::hasAttribute(const TCHAR* attribute) {

	
	const char* pAttrVal = findAttributeValue(attribute);
	
	return (pAttrVal != NULL);
}

DataChunk* DataChunk_TinyXML::_getFirstChild() {

	if (mpEl == NULL) {

		return NULL;
	}

	TiXmlElement* pEl = mpEl->FirstChildElement();

	if (pEl == NULL) {

		return NULL;
	}

	DataChunk_TinyXML* pChunk;
	MMemNew(pChunk, DataChunk_TinyXML(mSource.ptr(), pEl, false, this));
		

	return pChunk;
}

DataChunk* DataChunk_TinyXML::_getNextSibling() {

	if (mpEl == NULL) {

		return NULL;
	}

	TiXmlElement* pEl = mpEl->NextSiblingElement();

	if (pEl == NULL) {

		return NULL;
	}

	DataChunk_TinyXML* pChunk;
	MMemNew(pChunk, DataChunk_TinyXML(mSource.ptr(), pEl, false, mParent.ptr()));
		

	return pChunk;
}

bool DataChunk_TinyXML::hasChildren() {

	if (mpEl == NULL) {

		return false;
	}

	return (mpEl->NoChildren() == false);
}

DataChunk* DataChunk_TinyXML::_getChild(const TCHAR* name, int len) {

	if ((mpEl == NULL) || (name == NULL)) {

		return NULL;
	}

	if (len == 0) {

		len = String::length(name, false);
	}

	const TCHAR* pathStr = name;
	int pathLen = len;
	const char* valStr;

	TiXmlElement* pEl = mpEl->FirstChildElement();

	while (pEl) {

		valStr = pEl->Value();

		if (strequal(pathStr, pathLen, valStr, safeSizeCast<size_t, int>(strlen(valStr)))) {

			DataChunk_TinyXML* pChunk;
			MMemNew(pChunk, DataChunk_TinyXML(mSource.ptr(), pEl, false, this));
		
			return pChunk;
		}

		pEl = pEl->NextSiblingElement();
	}

	
	return NULL;
}


TiXmlDocument* DataSource_TinyXML::createDocument() {

	TiXmlDocument* pRet;

	MMemNew(pRet, TiXmlDocument());

	return pRet;
}

void DataSource_TinyXML::destroy(TiXmlDocument*& pDocument) {

	MMemDelete(pDocument);
}


DataSource_TinyXML::DataSource_TinyXML() : mpDocument(NULL) {
}

DataSource_TinyXML::~DataSource_TinyXML() {

	destroy(mpDocument);
}

void DataSource_TinyXML::init(TiXmlDocument* pDocument) {

	mpDocument = pDocument;
}


DataChunk* DataSource_TinyXML::getRoot() {

	if (mpDocument == NULL) {

		return NULL;
	}

	DataChunk_TinyXML* pChunk;
	MMemNew(pChunk, DataChunk_TinyXML(this, mpDocument->RootElement(), false, NULL));
		
	return pChunk;
}

}