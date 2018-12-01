#ifndef _WESkeletonProcessingStructs_h
#define _WESkeletonProcessingStructs_h

#include "WESkeletonDataTypes.h"
#include "../WETL/WETLArray.h"

#include "../geometry/WEGeometryVertexBuffer.h"
#include "../geometry/WEGeometryIndexBuffer.h"

namespace WE {


struct skel_Context {

	float ImplicitWeightEpsilon;

	GeometryVertexBuffer* pVB;
	GeometryIndexBuffer* pIB;

	RenderUINT vertexCount;
	RenderUINT faceCount;

	const RenderElementProfile* iprofile;
	void* ib;

	const RenderElementProfile* vprofile;
	void* vb;

	RenderSubElIndex el_weights;
	RenderSubElIndex el_indices;

	RenderSubElIndex el_position;

	SceneTransformIndex expWeightCount; //explicit weight count, total is expWeightCount + 1

	skel_Context() {

		ImplicitWeightEpsilon = 0.0001f;

		pVB = NULL;
		pIB = NULL;

		vprofile = NULL;
		iprofile = NULL;

		vb = NULL;
		ib = NULL;
	
		el_weights = RenderElSubNull;
		el_indices = RenderElSubNull;

		el_position = RenderElSubNull;

		expWeightCount = 0;
	}

	void init(GeometryVertexBuffer* _pVB, GeometryIndexBuffer* _pIB) {

		pVB = _pVB;
		pIB = _pIB;

		vprofile = &pVB->mProfile;
		iprofile = &pIB->mProfile;

		vb = pVB->bufferPtr();
		ib = pIB->bufferPtr();

		el_weights = vprofile->findSub(D3DDECLUSAGE_BLENDWEIGHT);
		el_indices = vprofile->findSub(D3DDECLUSAGE_BLENDINDICES);
		el_position = vprofile->findSub(D3DDECLUSAGE_POSITION);

		vertexCount = pVB->getElementCount();
		faceCount = pIB->getElementCount() / 3;
		
		if (el_weights != RenderElSubNull) {

			WORD sz = pVB->mProfile.sizeofSub(el_weights);
			expWeightCount = safeSizeCast<WORD, SceneTransformIndex>(sz / sizeof(float));
		}
	}

	void setNewVB(GeometryVertexBuffer* _pVB) {

		pVB = _pVB;

		vprofile = &pVB->mProfile;

		vb = pVB->bufferPtr();

		el_weights = vprofile->findSub(D3DDECLUSAGE_BLENDWEIGHT);
		el_indices = vprofile->findSub(D3DDECLUSAGE_BLENDINDICES);
		el_position = vprofile->findSub(D3DDECLUSAGE_POSITION);

		vertexCount = pVB->getElementCount();
			
		if (el_weights != RenderElSubNull) {

			WORD sz = pVB->mProfile.sizeofSub(el_weights);
			expWeightCount = safeSizeCast<WORD, SceneTransformIndex>(sz / sizeof(float));
		}
	}
};

struct skel_FaceBone {

	BoneIndex bone;
	float faceWeight;
	float minWeight;
	float maxWeight;
};

int __cdecl skel_FaceBone_qsort_compare_asc(const void* e1, const void* e2);
int __cdecl skel_FaceBone_qsort_compare_desc(const void* e1, const void* e2);

int __cdecl skel_FaceBone_qsort_compareMax_asc(const void* e1, const void* e2);
int __cdecl skel_FaceBone_qsort_compareMax_desc(const void* e1, const void* e2);

int __cdecl skel_FaceBone_qsort_compareSmart_asc(const void* e1, const void* e2);
int __cdecl skel_FaceBone_qsort_compareSmart_desc(const void* e1, const void* e2);

struct skel_FaceBoneGroup {

	typedef WETL::CountedArray<skel_FaceBone, false, BoneIndex::Type> BoneArray;

	int memberCount;
	BoneIndex maxCount;
	BoneArray bones;

	skel_FaceBoneGroup(BoneIndex _maxCount) {

		maxCount = _maxCount;
		reset();
		bones.reserve(maxCount);
	}

	void init(BoneIndex _maxCount) {

		maxCount = _maxCount;
		reset();
		bones.reserve(maxCount);
	}

	~skel_FaceBoneGroup() {
	}

	void reset() {

		memberCount = 0;
		bones.count = 0;
	}

	void sort(bool asc = true) {

		if (asc) {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compare_asc);
		} else {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compare_desc);
		}
	}

	void sortMax(bool asc = true) {

		if (asc) {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compareMax_asc);
		} else {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compareMax_desc);
		}
	}

	void sortSmart(bool asc = true) {

		if (asc) {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compareSmart_asc);
		} else {

			qsort(bones.el, bones.count, sizeof(skel_FaceBone), skel_FaceBone_qsort_compareSmart_desc);
		}
	}

	void add(BoneIndex bone, float weight) {

		if (weight <= 0.0f) {

			return;
		}

		//if (boneCount == maxCount) {

		//	return;
		//}

		BoneIndex index;
		if (find(bone, index)) {

			if (weight > bones.el[index].maxWeight)
				bones.el[index].maxWeight = weight;

			if (weight < bones.el[index].minWeight)
				bones.el[index].minWeight = weight;

			bones.el[index].faceWeight += weight;
			return;
		}

		skel_FaceBone& newBone = bones.addOne();

		newBone.bone = bone;
		newBone.faceWeight = weight;
		newBone.minWeight = weight;
		newBone.maxWeight = weight;

		assrt(bones.count <= maxCount);
	}

	bool find(BoneIndex bone, BoneIndex& index) {

		for (BoneIndex i = 0; i < bones.count; i++) {

			if (bones.el[i].bone == bone) {

				index = i;
				return true;
			}
		}

		return false;
	}
};

struct skel_FaceBoneGroup12 : skel_FaceBoneGroup {

	skel_FaceBoneGroup12() : skel_FaceBoneGroup(12) {
	}
};

struct skel_FaceBoneGroup4 : skel_FaceBoneGroup {

	skel_FaceBoneGroup4() : skel_FaceBoneGroup(4) {
	}
};

typedef WETL::CountedPtrArray<skel_FaceBoneGroup4*, false, WETL::Index::Type> skel_FaceBoneGroup4Array;
typedef WETL::CountedPtrArray<skel_FaceBoneGroup*, false, WETL::Index::Type> skel_FaceBoneGroupArray;


void skel_processFace_countBones(skel_Context& ct, UINT32 vertex, skel_FaceBoneGroup& counter);


}

#endif