#ifndef _WEPropertyManagerT_h
#define _WEPropertyManagerT_h

#pragma warning( push )
#pragma warning( disable : 4311 )

#include "../WEMem.h"
#include "../WETL/WETLHashMap.h"

namespace WE {

	template<class ObjectT, class PropertyT>
	class PropertyManagerT {
	public:

		bool hasProperty(const ObjectT& obj) {

			return (mMap.hasKey(&obj));
		}

		PropertyT* createProperty(const ObjectT& obj) {

			PropertyT* pRet = NULL;

			if (mMap.find(&obj, pRet) == false) {

				MMemNew(pRet, PropertyT());
				mMap.insert(&obj, pRet);

			} else {

				//already has prop.
				assrt(false);
			}

			return pRet;
		}

		PropertyT* getProperty(const ObjectT& obj) {

			PropertyT* pRet = NULL;

			mMap.find(&obj, pRet);
			return pRet;
		}
	
	protected:

		typedef int Index;
		typedef WETL::PtrHashMap<const ObjectT*, PropertyT*, Index> Map;


		Map mMap;
	};
}

#pragma warning( pop )

#endif