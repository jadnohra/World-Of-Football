#ifndef _WEMesh_h
#define _WEMesh_h

#include "../WERenderer.h"
#include "../WERef.h"
#include "../WECoordSys.h"

#include "WEMeshGeometryBuffer.h"
#include "WEMeshFaceArray.h"
#include "WEMeshMatrixGroupArray.h"
#include "WEMeshMaterialArray.h"

namespace WE {

	class Mesh : public Ref {
	protected:

		CoordSysSemantics mGeomSemantics;
		MeshGeometryVertexBuffer* mpVB;
		MeshGeometryIndexBuffer* mpIB;
		
		StaticMeshFaceArray* mpFB;

		StaticMeshMatrixGroupArray mRenderMatrixGroups;

	public:

		Mesh();
		virtual ~Mesh();

		void destroy(bool sourceBuffers, bool faceBuffer, bool renderBuffers);

		bool hasVB();
		bool hasIB();
		bool hasFB();
		bool isRenderReady();

		
		void* createVB(const RenderElementProfile& profile, RenderUINT elementCount);
		void* createIB(const RenderElementProfile& profile, RenderUINT elementCount);

		void* getVBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);
		void* getIBPtr(const RenderElementProfile*& pProfile, RenderUINT& elementCount);

		void createFB();

		bool prepareForRendering(Renderer& renderer, CoordSysConvPool& convFactory);

	protected:
		
	};
}

#endif