#ifndef _WECollFaceArray_h
#define _WECollFaceArray_h

#include "WECollFace.h"
#include "../WECollDataTypes.h"
#include "../../WETL/WETLSizeAlloc.h"

namespace WE {

	typedef WETL::ArrayBase<CollFace, CollPrimIndex> CollFaceArrayBase;
	typedef WETL::SizeAllocT<CollFace, CollPrimIndex, true> StaticCollFaceArray;

	typedef CollFaceArrayBase CollFaces;

	typedef WETL::ArrayBase<CollVertex, CollPrimIndex> CollVertexArrayBase;
	typedef WETL::SizeAllocT<CollVertex, CollPrimIndex, true> StaticCollVertexArray;

	typedef CollVertexArrayBase CollVertices;

}

#endif