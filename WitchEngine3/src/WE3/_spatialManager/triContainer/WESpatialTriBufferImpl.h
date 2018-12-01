#ifndef _WESpatialTriBufferImpl_h
#define _WESpatialTriBufferImpl_h

#include "../../WETL/WETLSizeAlloc.h"
#include "../WESpatialManagerObjects.h"
#include "../../geometry/WEGeometryIndexBuffer.h"
#include "../../geometry/WEGeometryVertexBuffer.h"
#include "../../render/WERenderer.h"

namespace WE {


	/*
		Here we have to make a compromise between 
		memory size vs. processing to access one TriangleEx1, 
		we could also support flexible tri format (with/out normals, indexed/none-indexed ...)
		which would mean more processing as well ...

		but thats why this is isolated here so that it can be changed without 
		affecting the rest of the code.
	*/

	class SpatialTriBufferImpl : public SpatialManagerTypeDefs {
	public:

		typedef AbstractContainer::Index Index;

		typedef SpatialAbstractContainer::Simplification Simplification;

	public:

		void destroy();

		void append(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, const Matrix4x3Base* pTransf);
		void transform(const Matrix4x3Base& transf);
		void render(Renderer& renderer, const RenderColor* pColor);
		
		inline Index getCount() { return mTris.size; }
		inline Tri1& get(const Index& index, Tri1& holder) { return mTris.el[index]; }

		void simplify(const Simplification* pSettings);

	protected:

		typedef WETL::SizeAllocT<TriangleEx1, Index> TriBuffer;

		TriBuffer mTris;
	};
}

#endif