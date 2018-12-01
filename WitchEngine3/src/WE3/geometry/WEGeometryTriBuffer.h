#ifndef _WEGeometryTriBuffer_h
#define _WEGeometryTriBuffer_h

#include "../WETL/WETLSizeAlloc.h"
#include "WEGeometryIndexBuffer.h"
#include "WEGeometryVertexBuffer.h"
#include "../render/WERenderer.h"
#include "../math/WETriangle.h"

namespace WE {

	/*
		Here we have to make a compromise between 
		memory size vs. processing to access one TriangleEx1, 
		we could also support flexible tri format (with/out normals, indexed/none-indexed ...)
		which would mean more processing as well ...

		but thats why this is isolated here so that it can be changed without 
		affecting the rest of the code.
	*/

	class GeometryTriBufferContainer;

	class GeometryTriBuffer {
	public:

		typedef RenderUINT Index;

		struct Simplification {

			enum Mode {

				M_Planar = 0
			};

			enum LoopMode {

				LM_Iterate = 0, LM_ClosestToCenter
			};

			int stepLimit;

			Mode mode;
			LoopMode loopMode;
			bool considerAesthetics;
			bool useIslands;

			float planarAngle;

			Simplification();
		};
	

	public:

		void destroy();

		void append(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, const Matrix4x3Base* pTransf);
		void append(GeometryTriBuffer& container);
		void transform(const Matrix4x3Base& transf);
		void render(Renderer& renderer, const RenderColor* pColor);
		
		void addToVol(AAB& vol);

		inline Index getCount() { return mTris.size; }
		inline TriangleEx1& get(const Index& index) { return mTris.el[index]; }

		void simplify(const Simplification* pSettings);

	protected:

		typedef WETL::SizeAllocT<TriangleEx1, Index> TriBuffer;

		TriBuffer mTris;
	};
}

#endif