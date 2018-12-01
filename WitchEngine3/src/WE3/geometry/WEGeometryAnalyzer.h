#ifndef _WEGeometryAnalyzer_h
#define _WEGeometryAnalyzer_h

#include "WEGeometryVertexBuffer.h"
#include "WEGeometryIndexBuffer.h"

#include "../math/WEAAB.h"
#include "../coll/narrowPhase/WECollFaceArray.h"

namespace WE {


	class GeometryAnalyzer {
	public:

		static void addToVolume(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, AAB& volume);
		static void toCollFaceArray(GeometryVertexBuffer& VB, GeometryIndexBuffer* pIB, StaticCollFaceArray& faceArray, StaticCollVertexArray* pVertexArray, const Matrix4x3Base* pTransf = NULL);
	};

}

#endif