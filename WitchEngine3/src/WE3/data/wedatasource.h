#ifndef _WEDataSource_h
#define _WEDataSource_h

#include "WEDataPath.h"
#include "WEDataTranslator.h"
#include "../WERef.h"
#include "../WEFilePath.h"
#include "../string/WEBufferString.h"

namespace WE {

	class DataSource;


	class DataChunk : public Ref {
	protected:

		RefWrap<DataSource> mSource;

	public:

		virtual ~DataChunk();

		DataSource* getSource(); 
		virtual DataChunk* getParent() = 0;

		virtual bool equals(const TCHAR* comp) = 0;
		virtual bool getValue(String& string) = 0;

		virtual bool getAttribute(const TCHAR* attribute, String& value) = 0;
		virtual bool hasAttribute(const TCHAR* attribute) = 0;
		
		virtual int getAttributeCount() = 0;
		virtual bool getAttribute(int attrIndex, String* pName, String* pValue) = 0;

		virtual DataChunk* getFirstChild();
		virtual DataChunk* getNextSibling();

		virtual bool hasChildren() = 0;
		virtual DataChunk* getChild(const TCHAR* name, int len = 0);

		bool scanAttribute(BufferString& tempStr, const TCHAR* attribute, bool& value);
		bool scanAttribute(BufferString& tempStr, const TCHAR* attribute, const TCHAR* format, void* pValue);
		StringHash scanValueHash(BufferString& tempStr);
		StringHash scanAttributeHash(BufferString& tempStr, const TCHAR* attribute);

		DataChunk* getChildByPath(const TCHAR* pPath);
		DataChunk* getChild(DataPath* pPath);
		//DataChunk* toNextSibling();
		DataChunk* getSibling(const TCHAR* name);

		void buildPath(String& path);

		int countChildren(BufferString& tempStr, const TCHAR* filterValue = NULL, const TCHAR* filterAttrName = NULL, const TCHAR* filterAttrVal = NULL);
		
		bool sourceEnableTranslation(bool enable);

		void translate(const TCHAR* source, BufferString& dest);
		void translate(BufferString& inPlace);

		StringHash getSourcePathId();
		StringHash generatePathId(bool includeSourcePath, bool includeChunkPath);

		StringHash extractId(bool autoRevertToSourcePathId, const TCHAR* attribute = L"id");
		bool extractId(String& id, const TCHAR* attribute = L"id") ;

		bool resolveFilePath(String& filePath);

	protected:

		bool isEnabled();

		virtual DataChunk* _getFirstChild() = 0;
		virtual DataChunk* _getNextSibling() = 0;

		virtual DataChunk* _getChild(const TCHAR* name, int len = 0) = 0;
	};

	void toNextSibling(DataChunk*& pChunk);
	void toNextSibling(RefWrap<DataChunk>& chunkRef);

	void toFirstChild(DataChunk*& pChunk);
	void toFirstChild(RefWrap<DataChunk>& chunkRef);

	void toParent(DataChunk*& pChunk);
	void toParent(RefWrap<DataChunk>& chunkRef);

	
	class DataSource : public Ref, public PathResolver {
	protected:

		String mSourcePath;

		bool mTranslatorEnabled;
		RefWrap<DataTranslator> mTranslator;

	public:
	
		DataSource();

		virtual ~DataSource();

		void setSourcePath(const TCHAR* pSourcePath);
		const TCHAR* getSourcePath();
		StringHash getSourcePathId();

		void setTranslator(DataTranslator* pTranslator);
		bool hasTranslator(); 
		DataTranslator* getTranslator(); 
		bool enableTranslation(bool enable); //returns last state
		void translate(const TCHAR* source, BufferString& dest);
		void translate(BufferString& inPlace);


		virtual void resolvePath(const TCHAR* path, String& absPath) const;

		virtual DataChunk* getRoot() = 0;

		
		DataChunk* getChild(DataPath* pPath);
		DataChunk* getChild(const TCHAR* name);
		DataChunk* getChildByPath(const TCHAR* pPath);
		DataChunk* getRootFirstChild();
	};
	
	class DataSourceTranslationContext {
	public:
		
		DataSourceTranslationContext();
		~DataSourceTranslationContext();

		void start(DataChunk* pChunk, DataTranslator* pTrans, bool enableTrans);
		void start(DataSource* pSource, DataTranslator* pTrans, bool enableTrans);

		void start(DataChunk* pChunk, bool enableTrans);
		void start(DataSource* pSource, bool enableTrans);

		void end();

	protected:
	
		bool mStarted;
		bool mEnableFlagOnly;

		RefWrap<DataSource> mSource;
		RefWrap<DataTranslator> mSavedTrans;
		bool mSavedEnabled;
	};

}


#endif