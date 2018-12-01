#ifndef _WEMeshMatrixGroup_h
#define _WEMeshMatrixGroup_h

#include "WEMeshDataTypes.h"

#include "../WETL/WETLArray.h"
#include "../render/WERenderer.h"

namespace WE {
	
	class MeshInstance;

	class MeshMatrixGroup {
	public:

		struct IndexedBlendingInfo {

			unsigned int vertexBlendMatrixCount;
		};

	public:

		MeshMatrixGroup();
		~MeshMatrixGroup();

		inline void setIsIdentityMatrixMapping(bool val) { mIsIdentityMatrixMapping = val; }
		void setMatrixCount(SceneTransformIndex count);
		void setMatrixIndex(SceneTransformIndex index, SceneTransformIndex value);
		SceneTransformIndex::Type extractMaxMatrixIndex();

		void setIndexedBlending(bool enable, unsigned int vertexBlendMatrixCount = 0);
		inline bool getIsIndexedBlending() { return mIndexedBlendingInfo.isValid(); }
		inline unsigned int getVertexBlendMatrixCount() { return mIndexedBlendingInfo.isValid() ? mIndexedBlendingInfo->vertexBlendMatrixCount : mMatrixIndexArray.size; }
		inline bool isIdentityMatrixMapping() { return mIsIdentityMatrixMapping; }

		//void set(Renderer& renderer, SceneTransformIndex::Type index, MeshInstance* pInstance);

	public:

		typedef WETL::Array<int, false, SceneTransformIndex::Type> MatrixIndexArray;
		
		MatrixIndexArray mMatrixIndexArray;
		RenderBufferPart mPartIB;
		RenderBufferPart mPartVB;

		bool mIsIdentityMatrixMapping;

		HardPtr<IndexedBlendingInfo> mIndexedBlendingInfo;
	};

	
}

#endif