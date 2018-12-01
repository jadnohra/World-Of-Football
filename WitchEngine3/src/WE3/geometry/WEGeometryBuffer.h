#ifndef _WEGeometryBuffer_h
#define _WEGeometryBuffer_h

#include "../WETL/WETLArray.h"
#include "../render/WERenderDataTypes.h"
#include "../render/WERenderElementProfile.h"
#include "../coordSys/WECoordSys.h"


namespace WE {


	class GeometryBuffer {
	public:

		//typedef WETL::SizeAllocT<BYTE, RenderUINT, true> Buffer;
		typedef WETL::MemAllocT<RenderUINT, RenderUINT, true> Buffer;

		CoordSys mCoordSys;
		RenderElementProfile mProfile;
		Buffer mBuffer;

	public:

		GeometryBuffer();
		GeometryBuffer(const RenderElementProfile* pProfile, RenderUINT elementCount);
		GeometryBuffer(const GeometryBuffer& ref, bool copyData);

		~GeometryBuffer();
		void destroy();

		bool init(const RenderElementProfile* pProfile = NULL, RenderUINT elementCount = 0);
		void* setElementCount(RenderUINT elementCount);

		RenderUINT getStride();
		const CoordSys& getCoordSys();
		
		const RenderElementProfile& getProfile();
		bool signalProfileChanged(bool resizeBuffer);

		RenderUINT getBufferSizeBytes() const;
		RenderUINT getElementCount() const;
		void* bufferPtr();
		//void* bufferPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);

		void cloneTo(GeometryBuffer& buffer, bool copyData) const;
		void copyDataTo(void* pBuff) const;
	};

}

#endif