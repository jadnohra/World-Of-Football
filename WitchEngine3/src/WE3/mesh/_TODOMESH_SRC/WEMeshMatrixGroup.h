#ifndef _WEMeshMatrixGroup_h
#define _WEMeshMatrixGroup_h

#include "WEMeshDataTypes.h"

#include "../WETL/WETLArray.h"
#include "../renderer/WERenderObjects.h"

namespace WE {
	
	class MeshMatrixGroup {
	public:

		typedef WETL::StaticArray<int, false, MeshMatrixIndex::Type> MatrixIndexArray;
		
		MatrixIndexArray mMatrixIndexArray;
		RenderBufferPart mPart;
	
	public:

		MeshMatrixGroup();
		~MeshMatrixGroup();

		void setMatrixCount(MeshMatrixIndex count);
		void setMatrixIndex(MeshMatrixIndex index, MeshMatrixIndex value);

		void setPart(RenderUINT startIndex, RenderUINT primCount);
	};

	
}

#endif