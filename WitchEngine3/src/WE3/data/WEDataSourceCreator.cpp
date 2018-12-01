#include "WEDataSourceCreator.h"

#include "../binding/tinyXML/WEDataSource_TinyXML.h"
#include "../WEAssert.h"

namespace WE {

DataSource* DataSourceCreator::create(const TCHAR* pPath, DataTranslator* pTranslator) {

	TiXmlDocument* pDocument = DataSource_TinyXML::createDocument();
	
	if (load(*pDocument, pPath) == false) {

		assrt(false);

		
		if (pDocument) {

			BufferString errorDesc;

			charToString(pDocument->ErrorDesc(), errorDesc);
			
			int row = pDocument->ErrorRow();
			int col = pDocument->ErrorCol();

			log(L"Parsing file failed [%s] [%s] [row %d, col %d]", pPath, errorDesc.c_tstr(), row, col);
		}
	

		DataSource_TinyXML::destroy(pDocument);
		return NULL;
	}

	DataSource* pRet;

	MMemNew(pRet, DataSource_TinyXML());
	((DataSource_TinyXML*) pRet)->init(pDocument);
	pRet->setSourcePath(pPath);
	pRet->setTranslator(pTranslator);

	return pRet;
}

}