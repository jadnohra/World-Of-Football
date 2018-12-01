#ifndef _WECoordSysConvPool_h
#define _WECoordSysConvPool_h

#include "WECoordSysConv.h"
#include "../WETL/WETLHashMap.h"

namespace WE {


	class CoordSysConvPool : public Ref {
	protected:

		typedef UINT32 ConvHashType;
		typedef WETL::IndexShort ConvIndex;
		typedef WETL::RefHashMap<ConvHashType, CoordSysConv*, ConvIndex::Type> ConvMap;

		ConvMap mConvMap;


		void getHashFor(const CoordSys& src, const CoordSys& dest, ConvHashType& hash);
		void createConverterFor(const CoordSys& src, const CoordSys& dest, const ConvHashType& hash, CoordSysConv*& pConv, bool createIfNotNeeded);
		
	public:

		CoordSysConvPool();
		virtual ~CoordSysConvPool();

		static CoordSysConv* createConverterFor(const CoordSys& src, const CoordSys& dest, bool createIfNotNeeded = false);
		CoordSysConv* getConverterFor(const CoordSys& src, const CoordSys& dest, bool getIfNotNeeded = false);
	};
}

#endif