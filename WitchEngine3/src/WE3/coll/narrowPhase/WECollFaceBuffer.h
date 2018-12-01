#ifndef _WECollFaceBuffer_h
#define _WECollFaceBuffer_h

#include "WECollFaceArray.h"

namespace WE {

	class CollFaceBuffer {
	public:

		CollFaceBuffer();
		~CollFaceBuffer();

		void createFB(CollPrimIndex faceCount);
		void createVB(CollPrimIndex vertexCount);

		CollPrimIndex getFaceCount();
		CollFace* faceArrayPtr();

		CollPrimIndex getVertexCount();
		CollVertex* vertexArrayPtr();

		void transform(const CollFaceTransform& transform);

	public:

		StaticCollFaceArray mFB;
		StaticCollVertexArray mVB;
	};

	class MaterialCollFaceBuffer : public CollFaceBuffer {
	public:

		CollMaterial mMaterial;

	};
}

#endif