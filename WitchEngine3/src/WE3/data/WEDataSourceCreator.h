#ifndef _WEDataSourceCreator_h
#define _WEDataSourceCreator_h

#include "WEDataSource.h"

namespace WE {

	class DataSourceCreator {
	public:

		static DataSource* create(const TCHAR* pPath, DataTranslator* pTranslator = NULL);
	};
}

#endif