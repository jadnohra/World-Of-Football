#include "includeTinyXML.h"

//#include "WEString.h"

namespace WE {


bool load(TiXmlDocument& document, const TCHAR* pPath) {

	FlexiblePtr<char> pathCharStr;
	
	tcharToChar(pPath, pathCharStr);
	
	return document.LoadFile(pathCharStr.ptr());
}

/*
void countNodes(TiXmlNode* pNode, int& count, int depth, const char* filter) {

	if (filter) {

		TiXmlElement* pEl = pNode->ToElement();
		if (pEl && (strcmp(pEl->Value(), filter) == 0)) {
			++count;
		}
	} else {
		++count;
	}

	if (depth || (depth <= -1)) {

		TiXmlNode* pChild = pNode->IterateChildren(NULL);
		
		while (pChild) {

			countNodes(pChild, count, depth - 1, filter);
			pChild = pNode->IterateChildren(pChild);
		}
	}
}

bool hasChildren(TiXmlNode* pNode) {

	return (pNode->IterateChildren(NULL) != NULL);
}

bool fromString(TiXmlElement* pEl, const char* attribute, WE::String& convert) {

	if (pEl == NULL) {
		return false;
	}
	
	convert.assign(pEl->Attribute(attribute));

	return (convert.length() != 0);
}

bool fromString(TiXmlElement* pEl, const char* attribute, bool& convert) {
	if (pEl == NULL) {
		return false;
	}

	const char* val = pEl->Attribute(attribute);
	if(val == NULL) {
		return false;
	}

	return WE::String::fromString(val, convert);
}

bool fromString(TiXmlElement* pEl, const char* attribute, unsigned int& convert) {
	if (pEl == NULL) {
		return false;
	}

	const char* val = pEl->Attribute(attribute);
	if(val == NULL) {
		return false;
	}

	return (sscanf(val, "%u", &convert) == 1);
}


bool fromString(TiXmlElement* pEl, const char* attribute, float& convert) {
	if (pEl == NULL) {
		return false;
	}

	const char* val = pEl->Attribute(attribute);
	if(val == NULL) {
		return false;
	}

	return (sscanf(val, "%f", &convert) == 1);
}

//hex 
bool fromStringHex(TiXmlElement* pEl, const char* attribute, unsigned long& convert) {

	if (pEl == NULL) {
		return false;
	}

	const char* val = pEl->Attribute(attribute);
	if(val == NULL) {
		return false;
	}

	return (sscanf(val, "%X", &convert) == 1);
}

bool fromString(TiXmlElement* pEl, const char* attribute, WE::Vector3& convert) {

	if (pEl == NULL) {
		return false;
	}

	const char* val = pEl->Attribute(attribute);
	if(val == NULL) {
		return false;
	}

	return (sscanf(val, "%f,%f,%f", &convert.x, &convert.y, &convert.z) == 3);
}

bool fromStringBitField(TiXmlElement* pEl, const char* attribute, unsigned char& bits, short bitCount) {

	if (pEl == NULL) {

		return false;
	}

	const char* val = pEl->Attribute(attribute);

	if(val == NULL) {

		return false;
	}

	bits = 0;
	int len = (int) strlen(val);
	int b = 0;
	bool set;
	
	bits = 0;

	for (int i = len - 1; i >= 0; i--) {

		set = (val[i] != 48);

		if (set) {

			bits |= (1 << b);
		}

		++b

		if (b == bitCount) {

			break;
		}
	}

	return true;
}
*/

/*
bool fromString(TiXmlElement* pEl, const char* attribute, WE::AnchorType* anchor) {

	WE::String anchorStr;

	if (fromString(pEl, attribute, anchorStr) == false) {

		return false;
	}

	long findIndex = 0;
	WE::StringPart part;
	short dimIndex = 0;

	findIndex = anchorStr.findNext(L',', findIndex, part, false);
	while (findIndex >= 0) {

		if (dimIndex >= 3) {

			break;
		}

		if (part.equals(L"min")) {
			
			anchor[dimIndex] = WE::AT_Min;

		} else if (part.equals(L"max")) {

			anchor[dimIndex] = WE::AT_Max;

		} else {

			anchor[dimIndex] = WE::AT_Center;
		}

		++dimIndex;
		findIndex = anchorStr.findNext(L',', ++findIndex, part, false);
	}

	return true;
}
*/

}