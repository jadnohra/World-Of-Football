#ifndef _WEMeshGeometryPart_h
#define _WEMeshGeometryPart_h

#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceArray.h"


namespace WE {


	class MeshGeometryPart {
	public:

		MeshGeometryVertexBuffer* mpVB;
		MeshGeometryIndexBuffer* mpIB;

		StaticMeshFaceArray* mpFB;

	public:

		MeshGeometryPart();
		virtual ~MeshGeometryPart();

		void destroy(bool geomBuffers, bool faceBuffer);

		bool hasVB();
		bool hasIB();
		bool hasFB();
		
		MeshGeometryVertexBuffer* createVB();
		MeshGeometryIndexBuffer* createIB();

		void* createVB(const RenderElementProfile& profile, RenderUINT elementCount);
		void* createIB(const RenderElementProfile& profile, RenderUINT elementCount);

		void* getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);
		void* getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);
		

		void createFB();
	};

}

#endif