#ifndef _includeTinyXML_h
#define _includeTinyXML_h


#include "../../../../../dep/tinyxml/src/tinyxml.h"
#include "../../WEString.h"
//#include "../../math/WEVector.h"

namespace WE {

	bool load(TiXmlDocument& document, const TCHAR* pPath);

	/*
	void countNodes(TiXmlNode* pNode, int& count, int depth = 1, const char* filter = NULL);
	bool hasChildren(TiXmlNode* pNode);
	bool fromString(TiXmlElement* pEl, const char* attribute, WE::String& convert);
	bool fromString(TiXmlElement* pEl, const char* attribute, bool& convert);
	bool fromString(TiXmlElement* pEl, const char* attribute, unsigned int& convert);
	bool fromString(TiXmlElement* pEl, const char* attribute, float& convert);
	bool fromStringHex(TiXmlElement* pEl, const char* attribute, unsigned long& convert); 
	bool fromString(TiXmlElement* pEl, const char* attribute, WE::Vector3& convert); 
	//bool fromString(TiXmlElement* pEl, const char* attribute, WE::AnchorType* anchor);
	bool fromStringBitField(TiXmlElement* pEl, const char* attribute, unsigned char& bits, short bitCount);
	*/
	
}

#endif

