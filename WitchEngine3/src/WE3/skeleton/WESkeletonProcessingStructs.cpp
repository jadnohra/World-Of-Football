#include "WESkeletonProcessingStructs.h"

namespace WE {

int __cdecl skel_FaceBone_qsort_compare_asc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((skel_FaceBone*) e1)->faceWeight) < (((skel_FaceBone*) e2)->faceWeight) ) {
		return -1;
	} else if ( (((skel_FaceBone*) e1)->faceWeight) > (((skel_FaceBone*) e2)->faceWeight) ) {
		return 1;
	}
	return 0;
}

int __cdecl skel_FaceBone_qsort_compare_desc(const void* e1, const void* e2) {
	
	return -skel_FaceBone_qsort_compare_asc(e1, e2);
}

int __cdecl skel_FaceBone_qsort_compareMax_asc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((skel_FaceBone*) e1)->maxWeight) < (((skel_FaceBone*) e2)->maxWeight) ) {
		return -1;
	} else if ( (((skel_FaceBone*) e1)->maxWeight) > (((skel_FaceBone*) e2)->maxWeight) ) {
		return 1;
	}
	return 0;
}

int __cdecl skel_FaceBone_qsort_compareMax_desc(const void* e1, const void* e2) {
	
	return -skel_FaceBone_qsort_compareMax_asc(e1, e2);
}

int __cdecl skel_FaceBone_qsort_compareSmart_asc(const void* e1, const void* e2) {

	//< 0 if elem1 less than elem2

	if ( (((skel_FaceBone*) e1)->maxWeight) >= 1.0f) {

		return 1;
	}

	if ( (((skel_FaceBone*) e2)->maxWeight) >= 1.0f) {

		return -1;
	}

	if ( (((skel_FaceBone*) e1)->faceWeight) < (((skel_FaceBone*) e2)->faceWeight) ) {
		return -1;
	} else if ( (((skel_FaceBone*) e1)->faceWeight) > (((skel_FaceBone*) e2)->faceWeight) ) {
		return 1;
	}

	return 0;
}

int __cdecl skel_FaceBone_qsort_compareSmart_desc(const void* e1, const void* e2) {
	
	return -skel_FaceBone_qsort_compareSmart_asc(e1, e2);
}

void skel_processFace_countBones(skel_Context& ct, UINT32 vertex, skel_FaceBoneGroup& counter) {

	float weightCumul = 0.0f;
	float* weights;
	BYTE* indices;

	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.vb, vertex, ct.el_indices);

	for (SceneTransformIndex i = 0; i < ct.expWeightCount; i++) {

		weightCumul += weights[i];
		counter.add(indices[i], weights[i]);
	}


	float lastWeight = 1.0f - weightCumul;
	if (lastWeight < ct.ImplicitWeightEpsilon) {

		lastWeight = 0.0f;
	}

	counter.add(indices[ct.expWeightCount], lastWeight);
}

}

