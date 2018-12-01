#include "WESkeleton.h"
#include "WESkeletonProcessingStructs.h"

namespace WE {



void skel_addFaceToBoneBoundingBox(skel_Context& ct, UINT32* faceIndices, 
										 skel_FaceBoneGroup12& counter, Skeleton& skeleton, short maxInfluence, float minWeight) {

	if (counter.bones.count == 0) {

		return;
	}

	counter.sort(false);

	Point pos;
	Bone* pBone;
	
	//loop for max number of bones that we want a face can be assigned to
	for (short i = 0; i < maxInfluence && i < counter.bones.count; i++) {
	 
		if (counter.bones[i].faceWeight < minWeight) {
			break;
		}

		pBone = skeleton.getBoneByBlendIndex(counter.bones.el[i].bone);

		if (pBone) {
			for (short j = 0; j < 3; j++) {

				pos.set(MExpand3(ct.vprofile->getF(ct.vb, faceIndices[j], ct.el_position)));
				pBone->addToBoundingBox(pos);
			}
		}
	}
}




void skel_addVertexToBoneBoundingBox(skel_Context& ct, UINT32 vertex, 
										 skel_FaceBoneGroup12& counter, Skeleton& skeleton, short maxInfluence, float minWeight) {

	if (counter.bones.count == 0) {

		return;
	}

	counter.sort(false);

	Point pos;
	Bone* pBone;
	
	//loop for max number of bones that we want a vertex can be assigned to
	for (short i = 0; i < maxInfluence && i < counter.bones.count; i++) {
	 
		if (counter.bones[i].faceWeight < minWeight) {
			break;
		}

		pBone = skeleton.getBoneByBlendIndex(counter.bones.el[i].bone);

		if (pBone) {

			pos.set(MExpand3(ct.vprofile->getF(ct.vb, vertex, ct.el_position)));
			pBone->addToBoundingBox(pos);
		}
	}
}


void skel_addToBoneAABs(Skeleton& skeleton, GeometryVertexBuffer& VB, GeometryIndexBuffer& IB, bool byFace, 
										short maxInfluence, float minWeight) {

	
	if (byFace) {

		if (IB.getElementCount() == 0) {

			byFace = false;
		}
	} 

	if (VB.getElementCount() == 0) {

		return;
	}

	skel_Context ct;

	ct.init(&VB, &IB);


	if (ct.el_weights < 0 || ct.el_indices < 0 || ct.el_position < 0) {

		return;
	}

	
	if (maxInfluence <= 0) {
		maxInfluence = 1;
	}

	skel_FaceBoneGroup12 counter;
	counter.reset();

	if (byFace) {

		UINT32 faceIndices[3];

		for (RenderUINT f = 0, i = 0; f < ct.faceCount; f++, i += 3) {

			ct.iprofile->face_get(ct.ib, i, faceIndices);
			
			for (short j = 0; j < 3; j++) {

				skel_processFace_countBones(ct, faceIndices[j], counter); 
			}

			skel_addFaceToBoneBoundingBox(ct, faceIndices, counter, skeleton, maxInfluence, minWeight);
			counter.reset();
		}

	} else {

		for (RenderUINT i = 0; i < ct.vertexCount; i++) {

			skel_processFace_countBones(ct, i, counter); 

			skel_addVertexToBoneBoundingBox(ct, i, counter, skeleton, maxInfluence, minWeight);
			counter.reset();
		}
	}
}


void Skeleton::addToBoneBoundingBoxes(GeometryVertexBuffer& VB, GeometryIndexBuffer& IB, bool byFace, short maxInfluence, float minWeight) {

	skel_addToBoneAABs(*this, VB, IB, byFace, maxInfluence, minWeight);
}

}