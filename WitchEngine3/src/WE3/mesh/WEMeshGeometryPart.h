#ifndef _WEMeshGeometryPart_h
#define _WEMeshGeometryPart_h

#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceBuffer.h"

#include "../coordSys/WECoordSysConvPool.h"

namespace WE {


	class MeshGeometryPart {
	protected:

		HardPtr<MeshGeometryVertexBuffer> mVB;
		HardPtr<MeshGeometryIndexBuffer> mIB;

		HardPtr<MeshFaceBuffer> mFB;

	public:

		MeshGeometryPart();
		virtual ~MeshGeometryPart();

		void destroy(bool geomBuffers, bool faceBuffer);

		bool hasVB();
		bool hasIB();
		bool hasFB();
		
		bool createVB();
		bool createIB();
		bool createFB();

		//void* createVB(const RenderElementProfile& profile, RenderUINT elementCount);
		//void* createIB(const RenderElementProfile& profile, RenderUINT elementCount);

		//void* getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);
		//void* getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);

		
		MeshGeometryVertexBuffer& getVB();
		MeshGeometryIndexBuffer& getIB();
		MeshFaceBuffer& getFB();

		MeshGeometryVertexBuffer* getVBPtr();
		MeshGeometryIndexBuffer* getIBPtr();
		MeshFaceBuffer* getFBPtr();


		void applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys);
		void applyTransform(GeometryVertexTransform* pTrans);
		void reanchor(float* anchoringOffset);
	};

}

#endif