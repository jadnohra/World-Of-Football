#ifndef _WEMeshFaceBuffer_h
#define _WEMeshFaceBuffer_h

#include "WEMeshFaceArray.h"

namespace WE {

	class MeshFaceBuffer {
	public:

		MeshFaceBuffer();
		~MeshFaceBuffer();

		bool create(RenderUINT faceCount);

		RenderUINT getFaceCount();
		MeshFaceArrayBase& arrayBase();

		void batchSetMaterialIndex(MeshMaterialIndex::Type matIndex);
		void batchSetMatrixGroupIndex(MeshMatrixGroupIndex::Type matGroupIndex);

	protected:

		StaticMeshFaceArray mFaces;
	};

}

#endif