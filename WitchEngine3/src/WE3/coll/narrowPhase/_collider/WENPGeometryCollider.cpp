#include "WENPGeometryCollider.h"

#include "../../../geometry/WEGeometryAnalyzer.h"

namespace WE {

bool NPGeometryCollider::create(Mesh& mesh, CollMaterialManager* pMatManager, const CollMaterial* pCollMaterialOverride) {

	if (mesh.hasGeometry() == false) {

		assrt(false);
		return false;
	}

	MeshGeometry& geom = mesh.getGeometry();
	MeshGeometryPartIndex count = geom.getPartCount();
	MeshMaterialGroup* pMatGroup = mesh.getMaterialGroupPtr();

	bool assignMats = (pMatManager != NULL) && (pMatGroup != NULL);

	MeshGeometryPartIndex collPartCount;

	if (assignMats) {

		collPartCount = 0;

		for (MeshGeometryPartIndex i = 0; i < count; i++) {

			MeshGeometryPart& part = geom.getPart(i);
			MeshGeometryPartBindings& bind = geom.getPartBindings(i);

			if (pCollMaterialOverride) {

				++collPartCount;

			} else {

				if (bind.mMaterialIndex.isValid() && part.hasVB()) {

					if (pMatManager->isCollMaterial(pMatGroup->getMaterial(bind.mMaterialIndex).getMaterialName())) {

						++collPartCount;
					}
				}
			}
		}


		if (pCollMaterialOverride == NULL) {

			mBindArray.setSize(collPartCount);
			MeshGeometryPartIndex::Type collPartIndex = 0;	

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				MeshGeometryPart& part = geom.getPart(i);
				MeshGeometryPartBindings& bind = geom.getPartBindings(i);

				if (bind.mMaterialIndex.isValid() && part.hasVB()) {

					if (pMatManager->isCollMaterial(pMatGroup->getMaterial(bind.mMaterialIndex).getMaterialName())) {

						mBindArray.el[collPartIndex++] = i;
					}
				}
			}
		}
			

	} else {

		collPartCount = count;

		mBindArray.destroy();
	}

	mGroup.mBufferArray.setSize(collPartCount);
	

	CollMaterial collMat;
	MeshGeometryPartIndex collPartIndex = 0;

	for (MeshGeometryPartIndex i = 0; i < count; i++) {

		MeshGeometryPart& part = geom.getPart(i);
		MeshGeometryPartBindings& bind = geom.getPartBindings(i);
		
		if (pCollMaterialOverride) {

			mGroup.mBufferArray.el[collPartIndex].mMaterial = *pCollMaterialOverride;

		} else {

			if (assignMats && bind.mMaterialIndex.isValid() && part.hasVB()) {

				if (pMatManager->getCollMaterial(pMatGroup->getMaterial(bind.mMaterialIndex).getMaterialName(),
													collMat) == false) {
					continue;
				}

				if (collPartIndex >= collPartCount) {

					assrt(false);
					break;
				}

				mGroup.mBufferArray.el[collPartIndex].mMaterial = collMat;
			}
		}

		++collPartIndex;
	}

	if (collPartIndex != collPartCount) {

		assrt(false);
	}

	return true;
}

void NPGeometryCollider::init(Mesh& mesh, const CollFaceTransform* pTransf) {

	MeshGeometry& geom = mesh.getGeometry();
	MeshGeometryPartIndex count = geom.getPartCount();
	MeshMaterialGroup* pMatGroup = mesh.getMaterialGroupPtr();
	MeshGeometryPartIndex::Type refPartIndex;

	MeshGeometryPartIndex collPartCount = mGroup.mBufferArray.size;

	for (MeshGeometryPartIndex i = 0; i < collPartCount; i++) {

		if (mBindArray.size) {

			refPartIndex = mBindArray.el[i];

		} else {

			refPartIndex = i;
		}

		MeshGeometryPart& part = geom.getPart(refPartIndex);
		MeshGeometryPartBindings& bind = geom.getPartBindings(refPartIndex);
		
		GeometryAnalyzer::toCollFaceArray(part.getVB(), part.getIBPtr(), 
			mGroup.mBufferArray.el[i].mFB, &(mGroup.mBufferArray.el[i].mVB), pTransf);

	}
}

void NPGeometryCollider::transform(const CollFaceTransform& trans) {

	for (MeshGeometryPartIndex i = 0; i < mGroup.mBufferArray.size; i++) {

		mGroup.mBufferArray.el[i].transform(trans);
	}
}

bool NPGeometryCollider::collIterStart(CollIterator& iter) {

	iter.partIndex = 0;
	iter.faceIndex = 0;

	return true;
}

bool NPGeometryCollider::collIterNext_sweptContact(CollIterator& iter, const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, SweptContact& _sweptContact, CollMaterial& mat) {


	while (iter.partIndex < mGroup.mBufferArray.size) {

		MaterialCollFaceBuffer& collBuff = mGroup.mBufferArray.el[iter.partIndex];
		CollPrimIndex faceCount = collBuff.mFB.size;
		CollFace* faces = collBuff.mFB.el;
		CollVertex* vertices = collBuff.mVB.el;


		while (iter.faceIndex < faceCount) {

			faces[iter.faceIndex++].resolve(vertices, _sweptContact.vertices, _sweptContact.normal);

			if (CollContact::sweptContactTri(sphereRadius, startPos, posDiff, _sweptContact)) {

				mat = collBuff.mMaterial;
				return true;
			}
		}

		++iter.partIndex;
		iter.faceIndex = 0;
	}

	return false;
}

void NPGeometryCollider::collIterEnd(CollIterator& iter) {
}

void NPGeometryCollider::render(Renderer& renderer, const RenderColor* pColor) {

	CollIterator iter;

	iter.partIndex = 0;
	iter.faceIndex = 0;

	renderer.setTransform(Matrix4x3Base::kIdentity, RT_World);

	while (iter.partIndex < mGroup.mBufferArray.size) {

		CollFaceBuffer& collBuff = mGroup.mBufferArray.el[iter.partIndex];
		CollPrimIndex faceCount = collBuff.mFB.size;
		CollFace* faces = collBuff.mFB.el;
		CollVertex* vertices = collBuff.mVB.el;
		TriangleEx1 resolved;

		while (iter.faceIndex < faceCount) {

			faces[iter.faceIndex++].resolve(vertices, resolved.vertices, resolved.normal);
			renderer.draw(resolved, NULL, pColor, false, true);
		}

		++iter.partIndex;
		iter.faceIndex = 0;
	}
}

}