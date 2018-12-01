#ifndef _WEDataPath_h
#define _WEDataPath_h

#include "../WEString.h"
#include "../WETL/WETLArray.h"

namespace WE {

	class DataPath {
	public:

		struct Part {

			String* pString;
			//StringHash hash;

			Part();
			~Part();

			void assign(const TCHAR* buffer, StringPartBase& part);
		};

		typedef WETL::IndexShort PartIndex;
		typedef WETL::CountedPtrArray<Part*, false, PartIndex::Type, WETL::ResizeExact> Parts;
		Parts mParts;

		//String mPath;

	public:

		DataPath(const TCHAR* pPath = NULL);
		~DataPath();

		void destroy();

		void append(const TCHAR* pPath);
		
		PartIndex getPartCount() const;
		String* getPart(PartIndex partIndex) const;
	};

}

#endif