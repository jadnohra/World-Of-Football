#ifndef _WEDataSource_TinyXML_h
#define _WEDataSource_TinyXML_h

#include "../../data/WEDataSource.h"
#include "../../WETL/WETLSizeAlloc.h"
#include "../../WEString.h"
#include "includeTinyXML.h"

namespace WE {

	class DataSource_TinyXML;

	class DataChunk_TinyXML : public DataChunk {
	public:

		bool mDeleteEl;
		TiXmlElement* mpEl;

		RefWrap<DataChunk> mParent;

	public:

		DataChunk_TinyXML(DataSource* pSource, TiXmlElement* pEl, bool deleteEl, DataChunk* pParent);
		virtual ~DataChunk_TinyXML();

		virtual DataChunk* getParent();
		
		virtual bool equals(const TCHAR* comp);
		virtual bool getValue(String& string);

		virtual bool getAttribute(const TCHAR* attribute, String& value);
		virtual bool hasAttribute(const TCHAR* attribute);

		virtual int getAttributeCount();
		virtual bool getAttribute(int attrIndex, String* pName, String* pValue);

		virtual DataChunk* _getFirstChild();
		virtual DataChunk* _getNextSibling();

		virtual bool hasChildren();
		virtual DataChunk* _getChild(const TCHAR* name, int len = 0);

	protected:

		typedef WETL::SizeAllocT<char, int> TempChars;
		TempChars mTempChars;

		char* getTempChar(const TCHAR* source, int len = 0);
		
		const char* findAttributeValue(const TCHAR* attribute);
	};

	class DataSource_TinyXML : public DataSource {
	public:

		TiXmlDocument* mpDocument;
	
	public:
	
		static TiXmlDocument* createDocument();
		static void destroy(TiXmlDocument*& pDocument);

		DataSource_TinyXML();
		virtual ~DataSource_TinyXML();

		void init(TiXmlDocument* pDocument);

		virtual DataChunk* getRoot();
	};
}

#endif