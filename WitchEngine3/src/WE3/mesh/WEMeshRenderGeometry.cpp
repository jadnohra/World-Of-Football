#include "WEMeshRenderGeometry.h"

#include "WEMeshMatrixGroupHelper.h"

namespace WE {

MeshRenderGeometry::MeshRenderGeometry() {

	mMatrixCount = 0;
}

MeshRenderGeometry::~MeshRenderGeometry() {
}

bool MeshRenderGeometry::isCreated() {

	return (mParts.size != 0);
}


bool MeshRenderGeometry::RenderGeometryPart::create(RenderLoadContext& renderLoadContext, 
													MeshGeometryPart& part, MeshGeometryPartBindings& bind, MeshMaterialGroup* pMatGroup) {

	if (mVB.isValid() || mIB.isValid()) {

		assrt(false);
		return false;
	}


	FlexiblePtr<MeshGeometryVertexBuffer> renderReadyVB;
	FlexiblePtr<MeshGeometryIndexBuffer> renderReadyIB;

	if (part.hasFB() == false) {

		part.createFB();
	}

	Renderer& renderer = renderLoadContext.renderer();

	if (MeshMatrixGroupHelper::getRenderReadyBuffers(renderer, renderLoadContext.convPool(), 
						part.getVB(), part.getIB(), part.getFB(), renderReadyVB, renderReadyIB, mRenderMatrixGroups) == false) {

		assrt(false);
		return false;
	}

	MeshMatrixGroupHelper::groupByMatrixGroup(renderReadyVB.dref(), renderReadyIB.dref(), part.getFB(), mRenderMatrixGroups, true, false);


	createBuffers(renderer, renderReadyVB.dref(), renderReadyIB.dref());
	fillBuffers(renderer, renderReadyVB.dref(), renderReadyIB.dref());

	mMaterialIndex = bind.mMaterialIndex;

	/*
	if (part.hasFB()) {

		MeshFaceBuffer& FB = part.getFB();

		if (FB.getFaceCount()) {

			mMaterialIndex = FB.arrayBase().el[0].mMaterialIndex;
		} else {

			mMaterialIndex = MeshMaterialIndex::Null;
		}

	} else {

		mMaterialIndex = MeshMaterialIndex::Null;
		assrt(false);
	}
	*/

	if (mMaterialIndex != MeshMaterialIndex::Null) {

		pMatGroup->prepareForRenderer(renderLoadContext, mMaterialIndex);
	}

	return true;
}

SceneTransformIndex::Type MeshRenderGeometry::RenderGeometryPart::extractMaxMatrixIndex() {

	return MeshMatrixGroupArrayHelper::extractMaxMatrixIndex(mRenderMatrixGroups);
}

bool MeshRenderGeometry::RenderGeometryPart::createBuffers(Renderer& renderer, MeshGeometryVertexBuffer& renderReadyVB, MeshGeometryIndexBuffer& renderReadyIB) {

	if (mVB.isValid() || mIB.isValid()) {

		assrt(false);
		return false;
	}

	RenderBufferUsage usage(false, true);

	renderReadyIB.optimizeIndexData(renderReadyIB.extractMaxIndex());

	mRenderPrimCount = renderReadyIB.getElementCount() / 3;

	if (renderer.createIB(renderReadyIB.isBit32(), usage, renderReadyIB.getBufferSizeBytes(), mIB.pptr()) == false) {

		assrt(false);
		return false;
	}


	mStride = renderReadyVB.getStride();
	mVertexCount = renderReadyVB.getElementCount();
	mShader = renderReadyVB.getProfile().mFVF;

	if (renderer.createVB(renderReadyVB.getProfile(), usage, renderReadyVB.getElementCount(), mVB.pptr()) == false) {

		assrt(false);
		return false;
	}

	return true;
}

bool MeshRenderGeometry::RenderGeometryPart::fillBuffers(Renderer& renderer, MeshGeometryVertexBuffer& sourceVB, MeshGeometryIndexBuffer& sourceIB) {

	void* pData;
	
	if (mIB.isValid()) {

		renderer.lockIB(mIB.ptr(), 0, sourceIB.getBufferSizeBytes(), &pData);
		sourceIB.copyDataTo(pData);
		renderer.unlockIB(mIB.ptr());
	}

	if (mVB.isValid()) {

		renderer.lockVB(mVB.ptr(), 0, sourceVB.getBufferSizeBytes(), &pData);
		sourceVB.copyDataTo(pData);
		renderer.unlockVB(mVB.ptr());
	}

	return true;
}

bool MeshRenderGeometry::create(RenderLoadContext& renderLoadContext, MeshGeometry& geometry, MeshMaterialGroup* pMatGroup) {

	if (isCreated() ) {

		assrt(false);
		return false;
	}

	mParts.setSize(geometry.getPartCount());

	if (mParts.size == 0) {

		return true;
	}


	for (PartIndex i = 0; i < mParts.size; i++) {

		MMemNew(mParts.el[i], RenderGeometryPart());
		
		if (!mParts.el[i]->create(renderLoadContext, geometry.getPart(i), geometry.getPartBindings(i), pMatGroup)) {

			mParts.destroy();
			return false;
		}
	}
	
	mMatrixCount = extractMaxMatrixIndex() + 1;

	return true;
}

SceneTransformIndex::Type MeshRenderGeometry::extractMaxMatrixIndex() {

	SceneTransformIndex::Type maxMatrixIndex = 0;

	for (PartIndex i = 0; i < mParts.size; i++) {

		maxMatrixIndex = MMax(mParts.el[i]->extractMaxMatrixIndex(), maxMatrixIndex);
	}
		
	return maxMatrixIndex;	
}

void MeshRenderGeometry::render(Renderer& renderer, MeshMaterialGroup* pMatGroup, MeshMaterialInstances* pMatInstances, const Matrix4x3Base* pTransforms, RenderPassEnum pass) {

	MeshMatrixGroupIndex g;
	SceneTransformIndex m;
	
	bool isIdentityMapMode = true;
	SceneTransformIndex reachedIdentityMatrixCount = 0;

	for (PartIndex i = 0; i < mParts.size; i++) {

		bool unset = false;
		RenderGeometryPart& part = *(mParts.el[i]);

		if ((pass == RP_Normal) && (part.mMaterialIndex.isValid())) {

			unset = pMatGroup->set(renderer, part.mMaterialIndex, pMatInstances->el[part.mMaterialIndex]);
		}

		renderer.setShader(part.mShader);

		renderer.setSourceVB(0, part.mVB.ptr(), part.mStride);
		renderer.setSourceIB(part.mIB.ptr());

		//if (pTransforms) {

			
			for (g = 0; g < part.mRenderMatrixGroups.size; g++) {

				MeshMatrixGroup& grp = part.mRenderMatrixGroups.el[g];
				
				renderer.setRenderState(RS_VertexBlendMatrixCount, grp.getVertexBlendMatrixCount());
				renderer.setRenderState(RS_VertexBlendIndexed, grp.getIsIndexedBlending() ? 1 : 0);

				if (!grp.isIdentityMatrixMapping())
					isIdentityMapMode = false;

				if (isIdentityMapMode) {

					if (grp.mMatrixIndexArray.size > reachedIdentityMatrixCount) {

						for (m = reachedIdentityMatrixCount; m < grp.mMatrixIndexArray.size; m++) {

							const Matrix4x3Base* pTransform = pTransforms + m;

							renderer.setTransform(dref(pTransform), RT_World, m);
						}

						reachedIdentityMatrixCount = grp.mMatrixIndexArray.size;
					}

				} else {

					for (m = 0; m < grp.mMatrixIndexArray.size; m++) {

						const Matrix4x3Base* pTransform = pTransforms + grp.mMatrixIndexArray.el[m];

						renderer.setTransform(dref(pTransform), RT_World, m);
					}
				}

				renderer.drawIndexed(RP_TriangleList, grp.mPartIB.startIndex, grp.mPartIB.primCount, 
										grp.mPartVB.startIndex, grp.mPartVB.primCount);
			}

			/*
		} else {

			renderer.setRenderState(RS_VertexBlendMatrixCount, 0);
			renderer.setTransform(pTransforms[0], RT_World, 0);

			renderer.drawIndexed(RP_TriangleList, 0, part.mRenderPrimCount, 0, part.mVertexCount);
		}
		*/
	
		if (unset) 
			pMatGroup->unset(renderer, part.mMaterialIndex, pMatInstances->el[part.mMaterialIndex]);

	}
}


}