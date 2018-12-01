#include "WEMeshMatrixGroupHelper.h"

namespace WE {

typedef MeshMatrixIndex BoneIndex;

struct pvb_Context {

	float ImplicitWeightEpsilon;

	MeshGeometryVertexBuffer* pVB;
	MeshGeometryIndexBuffer* pIB;

	MeshFaces* pFB;

	RenderUINT vertexCount;
	RenderUINT faceCount;

	const RenderElementProfile* iprofile;
	void* ib;

	const RenderElementProfile* vprofile;
	void* vb;

	RenderSubElIndex el_weights;
	RenderSubElIndex el_indices;

	RenderSubElIndex el_position;

	MeshMatrixIndex expWeightCount; //explicit weight count, total is expWeightCount + 1

	pvb_Context() {

		ImplicitWeightEpsilon = 0.0001f;

		pVB = NULL;
		pIB = NULL;
		pFB = NULL;

		vprofile = NULL;
		iprofile = NULL;

		vb = NULL;
		ib = NULL;
	
		el_weights = RenderElSubNull;
		el_indices = RenderElSubNull;

		el_position = RenderElSubNull;

		expWeightCount = 0;
	}

	void init(MeshGeometryVertexBuffer* _pVB, MeshGeometryIndexBuffer* _pIB, MeshFaces* _pFB) {

		pVB = _pVB;
		pIB = _pIB;
		pFB = _pFB;

		vprofile = &pVB->mProfile;
		iprofile = &pIB->mProfile;

		vb = pVB->bufferPtr();
		ib = pIB->bufferPtr();

		el_weights = vprofile->findSub(D3DDECLUSAGE_BLENDWEIGHT);
		el_indices = vprofile->findSub(D3DDECLUSAGE_BLENDINDICES);

		vertexCount = pVB->getElementCount();
		faceCount = pIB->getElementCount() / 3;
		
		if (el_weights != RenderElSubNull) {

			WORD sz = pVB->mProfile.sizeofSub(el_weights);
			expWeightCount = safeSizeCast<WORD, MeshMatrixIndex>(sz / sizeof(float));
		}
	}

	void setNewVB(MeshGeometryVertexBuffer* _pVB) {

		pVB = _pVB;

		vprofile = &pVB->mProfile;

		vb = pVB->bufferPtr();

		el_weights = vprofile->findSub(D3DDECLUSAGE_BLENDWEIGHT);
		el_indices = vprofile->findSub(D3DDECLUSAGE_BLENDINDICES);

		vertexCount = pVB->getElementCount();
			
		if (el_weights != RenderElSubNull) {

			WORD sz = pVB->mProfile.sizeofSub(el_weights);
			expWeightCount = safeSizeCast<WORD, MeshMatrixIndex>(sz / sizeof(float));
		}
	}
};


struct pvb_FaceBone {

	BoneIndex bone;
	float faceWeight;
};

int __cdecl pvb_FaceBone_qsort_compare_asc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((pvb_FaceBone*) e1)->faceWeight) < (((pvb_FaceBone*) e2)->faceWeight) ) {
		return -1;
	} else if ( (((pvb_FaceBone*) e1)->faceWeight) > (((pvb_FaceBone*) e2)->faceWeight) ) {
		return 1;
	}
	return 0;
}

int __cdecl pvb_FaceBone_qsort_compare_desc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((pvb_FaceBone*) e1)->faceWeight) < (((pvb_FaceBone*) e2)->faceWeight) ) {
		return 1;
	} else if ( (((pvb_FaceBone*) e1)->faceWeight) > (((pvb_FaceBone*) e2)->faceWeight) ) {
		return -1;
	}
	return 0;
}

struct pvb_FaceBoneGroup {

	typedef WETL::CountedArray<pvb_FaceBone, false, BoneIndex::Type> BoneArray;

	int memberCount;
	BoneIndex maxCount;
	BoneArray bones;

	pvb_FaceBoneGroup(BoneIndex _maxCount) {

		maxCount = _maxCount;
		reset();
		bones.reserve(maxCount);
	}


	void reset() {

		memberCount = 0;
		bones.count = 0;
	}

	void sort(bool asc = true) {

		if (asc) {

			qsort(bones.el, bones.count, sizeof(pvb_FaceBone), pvb_FaceBone_qsort_compare_asc);
		} else {

			qsort(bones.el, bones.count, sizeof(pvb_FaceBone), pvb_FaceBone_qsort_compare_desc);
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

			bones.el[index].faceWeight += weight;
			return;
		}

		pvb_FaceBone& newBone = bones.addOne();

		newBone.bone = bone;
		newBone.faceWeight = weight;

		assert(bones.count <= maxCount);
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

struct pvb_FaceBoneGroup12 : pvb_FaceBoneGroup {

	pvb_FaceBoneGroup12() : pvb_FaceBoneGroup(12) {
	}
};

struct pvb_FaceBoneGroup4 : pvb_FaceBoneGroup {

	pvb_FaceBoneGroup4() : pvb_FaceBoneGroup(4) {
	}
};

typedef WETL::CountedArray<pvb_FaceBoneGroup4, true, WETL::Index::Type> pvb_FaceBoneGroup4Array;



void optVB_processFace_countBones(pvb_Context& ct, UINT32 vertex, pvb_FaceBoneGroup& counter) {

	float weightCumul = 0.0f;
	float* weights;
	BYTE* indices;
	
	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.ib, vertex, ct.el_indices);


	for (MeshMatrixIndex i = 0; i < ct.expWeightCount; i++) {

		weightCumul += weights[i];
		counter.add(indices[i], weights[i]);

		if (indices[i] == 8 && weights[i] == 0.5f) {
			i = i;
		}
	}


	float lastWeight = 1.0f - weightCumul;
	if (lastWeight < ct.ImplicitWeightEpsilon) {

		lastWeight = 0.0f;
	}

	counter.add(indices[ct.expWeightCount], lastWeight);
}


void optVB_processFace_removeBone(pvb_Context& ct, float* weights, BYTE* indices, MeshMatrixIndex index, float weight) {

	MeshMatrixIndex nonZeroWeightCount = 0;
	float weightCumul = 0.0f;

	MeshMatrixIndex i;
	for (i = 0; i < ct.expWeightCount; i++) {
		
		weightCumul += weights[i];
		if (weights[i] != 0.0f) {
			nonZeroWeightCount++;
		}
	}

	float lastWeight = 1.0f - weightCumul;
	if (lastWeight < ct.ImplicitWeightEpsilon) {
		lastWeight = 0.0f;
	}
	if (lastWeight > 0.0f) {
		nonZeroWeightCount++;
	}

	float addedWeightPerIndex = (weight/* + ImplicitWeightEpsilon*/) / (nonZeroWeightCount - 1);
	
	for (i = 0; i < ct.expWeightCount; i++) {

		if (i != index) {
			if (weights[i] != 0.0f) {
				weights[i] += addedWeightPerIndex;
			}
		} else {
			weights[i] = 0.0f;
		}
	}
}

void optVB_processFace_removeBones(pvb_Context& ct, RenderUINT vertex, pvb_FaceBoneGroup& counter, MeshMatrixIndex bonesToRemove) {

	float weightCumul;
	float* weights;
	BYTE* indices;
	
	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.vb, vertex, ct.el_indices);

	bool removed;
	for (MeshMatrixIndex b = 0; b < bonesToRemove; b++) {

		removed = false;
		weightCumul = 0.0f;


		MeshMatrixIndex i;
		for (i = 0; i < ct.expWeightCount; i++) {

			weightCumul += weights[i];
			if ((weights[i] != 0.0f) && (indices[i] == counter.bones.el[b].bone)) {

				optVB_processFace_removeBone(ct, weights, indices, i, weights[i]);
				removed = true;
				break;
			}
		}

		if (removed == false) {

			i = ct.expWeightCount;
			float lastWeight = 1.0f - weightCumul;
			if (lastWeight < ct.ImplicitWeightEpsilon) {
				lastWeight = 0.0f;
			}
			if ((lastWeight > 0.0f) && indices[i] == counter.bones.el[b].bone) {
				optVB_processFace_removeBone(ct, weights, indices, i, lastWeight);
			}
		}

	}
}

void pvb_limitFaceWeights_processFace(pvb_Context& ct, UINT32 faceIndices[3], int max) {

	static pvb_FaceBoneGroup12 counter;
	counter.reset();

	for (short v = 0; v < 3; v++) {

		optVB_processFace_countBones(ct, faceIndices[v], counter);
	}

	/*
	if (counter.boneCount > ct.processingCosts.maxFaceBones) {

		ct.processingCosts.maxFaceBones = counter.boneCount;
	}

	if (counter.boneCount < ct.processingCosts.minFaceBones) {

		ct.processingCosts.minFaceBones = counter.boneCount;
	}
	*/

	if (counter.bones.count > max) {

		//ct.processingCosts.facesWithRemovedBones++;

		counter.sort();
		MeshMatrixIndex bonesToRemove = safeSizeCast<int, MeshMatrixIndex>((int)counter.bones.count - max);

		//ct.processingCosts.removedBoneWeights += bonesToRemove;

		//TODO improve algorithm remove bone with min weight not min face weight or sthg like that
		for (short v = 0; v < 3; v++) {

			optVB_processFace_removeBones(ct, faceIndices[v], counter, bonesToRemove);
		}
	
		/*
		//check
		counter.reset();
		for (short v = 0; v < 3; v++) {
			optVB_processFace_countBones(ct, faceIndices[v], counter);
		}

		if (counter.boneCount > max) {
			counter.boneCount = counter.boneCount;
		}
		*/
	}
}




bool pvb_group4_processFace_adapt(pvb_Context& ct, pvb_FaceBoneGroup& group, float* weights, BYTE* indices) {

	float weightCumul = 0.0f;
	float lastWeight;

	MeshMatrixIndex i;
	MeshMatrixIndex j;


	for (i = 0; i < ct.expWeightCount; i++) {
		weightCumul += weights[i];
	}

	lastWeight = 1.0f - weightCumul;
	if (lastWeight < ct.ImplicitWeightEpsilon) {
		lastWeight = 0.0f;
	}

	//if we got here we manage to add all bones now rearrange indices to fit group
	float origWeights[4];
	BYTE origIndices[4];

	memcpy(origWeights, weights, ct.expWeightCount * sizeof(float));
	origWeights[ct.expWeightCount] = lastWeight;

	memcpy(origIndices, indices, (ct.expWeightCount + 1) * sizeof(BYTE));

	for (j = 0; j < group.bones.count; j++) {
		indices[j] = group.bones.el[j].bone;

		if (j < ct.expWeightCount) {
			for (i = 0; i < ct.expWeightCount + 1; i++) {
				if (origIndices[i] == indices[j]) {
					weights[j] = origWeights[i];
					break;
				}
			}

			if (i == ct.expWeightCount + 1) {
				weights[j] = 0.0f;
			}
		}
	}

	//set remaining to 0
	for (; j < ct.expWeightCount; j++) {
		weights[j] = 0.0f;
	}


	return true;
}



bool pvb_group4_processFace_tryAdapt(pvb_Context& ct, pvb_FaceBoneGroup& group, 
											   pvb_FaceBoneGroup& groupToAdapt) {

	BYTE adaptedBoneCount = group.bones.count;

	MeshMatrixIndex i;
	MeshMatrixIndex j;
	bool boneFound;
	for (i = 0; i < groupToAdapt.bones.count; i++) {

		boneFound = false;
		for (j = 0; j < group.bones.count; j++) {
			if (group.bones.el[j].bone == groupToAdapt.bones.el[i].bone) {
				boneFound = true;
				break;
			}
		}

		if (boneFound == false) {
			if (adaptedBoneCount < 4) {
				group.bones.el[adaptedBoneCount].bone = groupToAdapt.bones.el[i].bone;
				adaptedBoneCount++;
			} else {
				return false;
			}
		} 
	}
	
	group.memberCount++;
	group.bones.count = adaptedBoneCount;

	return true;
}

WETL::Index::Type pvb_group4_processFace_findAdaptable(pvb_Context& ct, pvb_FaceBoneGroup4Array& groupArray, 
										 pvb_FaceBoneGroup& groupToAdapt) {

	bool adapted = false;
	WETL::Index g;
	for (g = 0; g < groupArray.count; g++) {
		if (pvb_group4_processFace_tryAdapt(ct, groupArray.el[g], groupToAdapt)) {
			return g;
		}
	}

	g = groupArray.count;
	groupArray.addOne();
	if (pvb_group4_processFace_tryAdapt(ct, groupArray.el[g], groupToAdapt) == false) {
		//ct.processingCosts.unadaptableFaceCount++;
	}

	return g;
}

void pvb_group4_mergeFace(pvb_Context& ct, UINT32 faceIndices[3], pvb_FaceBoneGroup& mergedNonZeroWeightGroup) {

	float* weights;
	BYTE* indices;
	
	for (short v = 0; v < 3; v++) {

		weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
		indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);

		float weightCumul = 0.0f;
		MeshMatrixIndex i;
		for (i = 0; i < ct.expWeightCount; i++) {
			weightCumul += weights[i];
			if (weights[i] != 0.0f) {
				mergedNonZeroWeightGroup.add(indices[i], weights[i]);
			}
		}

		float lastWeight = 1.0f - weightCumul;
		if (lastWeight > ct.ImplicitWeightEpsilon) {
			mergedNonZeroWeightGroup.add(indices[i], lastWeight);
		}
	}

	//make sure we have 4 bones max
	if (mergedNonZeroWeightGroup.bones.count > 4) {

		mergedNonZeroWeightGroup.sort(false);
		mergedNonZeroWeightGroup.bones.count = 4;
	}
}


struct pvb_AddedVertex4 {

	WETL::Index ref;
	float weights[3];
	BYTE indices[4];
};


typedef WETL::CountedArray<pvb_AddedVertex4, true, WETL::Index::Type> pvb_AddedVertex4Array;

struct pvb_VertexProcessInfo {

	int adaptedGroup;

	pvb_VertexProcessInfo() {
		adaptedGroup = -1;
	}
};


struct pvb_FaceGroupLink {

	WETL::Index faceIndex;
	int groupIndex;
	
};

struct pvb_FaceProcessInfo4 : pvb_FaceGroupLink {

	bool processed;
	bool merged;
	pvb_FaceBoneGroup mergedGroup;


	pvb_FaceProcessInfo4() : mergedGroup(4) {

		processed = false;
		merged = false;
		
		mergedGroup.bones.count = 0;
	}
};

bool pvb_group4_processFace(pvb_Context& ct, RenderUINT faceIndex, UINT32 faceIndices[3], pvb_FaceBoneGroup4Array& groupArray, 
								   pvb_FaceProcessInfo4& processInfo, pvb_VertexProcessInfo* vertexProcessInfos, 
								   pvb_AddedVertex4Array& addedVertices, int minBoneCount = -1) {

	if (processInfo.processed == true) {
		return false;
	}

	processInfo.faceIndex = faceIndex;
	processInfo.groupIndex = -1;
	
	if (processInfo.merged == false) {
		pvb_group4_mergeFace(ct, faceIndices, processInfo.mergedGroup);

		processInfo.merged = true;
	}


	
	if ((minBoneCount >= 0) && (processInfo.mergedGroup.bones.count < minBoneCount)) {
		return false;
	}

	processInfo.processed = true;

	processInfo.groupIndex = (int) pvb_group4_processFace_findAdaptable(ct, groupArray, processInfo.mergedGroup);


	float* weights;
	BYTE* indices;
	
	for (short v = 0; v < 3; v++) {

		if (vertexProcessInfos[faceIndices[v]].adaptedGroup == -1) {

			vertexProcessInfos[faceIndices[v]].adaptedGroup = processInfo.groupIndex;

			weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
			indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);
		
			pvb_group4_processFace_adapt(ct, groupArray.el[processInfo.groupIndex], weights, indices);

			

		} else {

			if (vertexProcessInfos[faceIndices[v]].adaptedGroup != processInfo.groupIndex) {
		
				pvb_AddedVertex4* pAdded = &(addedVertices.addOne());
				ct.iprofile->index_set(ct.ib, (faceIndex * 3) + v, ct.vertexCount + (addedVertices.count - 1));
				pAdded->ref = faceIndices[v];

				weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
				indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);

				memcpy(pAdded->weights, weights, ct.expWeightCount * sizeof(float));
				memcpy(pAdded->indices, indices, 1 * sizeof(DWORD));

				
				pvb_group4_processFace_adapt(ct, groupArray.el[processInfo.groupIndex], pAdded->weights, pAdded->indices);
				
			}
		}
	}


	return true;
}


int __cdecl pvb_FaceGroupLink_qsort_compare(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	return ( (((pvb_FaceGroupLink*) e1)->groupIndex) - (((pvb_FaceGroupLink*) e2)->groupIndex) );
}



/*
void pvb_group4_mergeAddedVerticesAndRemoveBlendIndices(pvb_Context& ct, pvb_AddedVertex4Array& addedVertices) {

	RenderUINT newVertexCount = ct.vertexCount + addedVertices.count;

	if (newVertexCount == 0) {
		return;
	}

	bool profileChanged = false;
	DWORD newFVF = ct.vprofile->mFVF;
	RenderElementProfile newProfile;

	if (ct.el_indices != RenderElSubNull) {

		profileChanged = true;

		newFVF &= ~D3DFVF_XYZB4;
		newFVF &= ~D3DFVF_LASTBETA_UBYTE4;
		newFVF |= D3DFVF_XYZB3;

		newProfile.vertex_set(newFVF, D3DFVF_XYZB3);
	} else {

		newProfile.set(*(ct.vprofile));
	}
	
	short newel_weights = newProfile.findSub(D3DDECLUSAGE_BLENDWEIGHT, 0);
	int newWeightOffset = newProfile.mSubElements.el[newel_weights].Offset;

	void* pNewVB = malloc(newProfile.bufferSizeBytes(newVertexCount));
	
	if (profileChanged) { 

		int indicesOffset = ct.vprofile->mSubElements.el[ct.el_indices].Offset;
		int indicesOffsetNext = indicesOffset + ct.vprofile->sizeofSub(ct.el_indices); //VertexProfile::_sizeof(ct.pVB->profile.mElements[ct.el_indices].Type);
		int afterIndicesCopySize = ct.vprofile->mElementSizeBytes - indicesOffsetNext;



		BYTE* pDest = (BYTE*) pNewVB;
		BYTE* pSrc = (BYTE*) ct.vb;

		for (RenderUINT i = 0; i < ct.vertexCount; i++) {

			memcpy(pDest, pSrc, indicesOffset);
			memcpy(pDest + indicesOffset, pSrc + indicesOffsetNext, afterIndicesCopySize);

			pSrc += ct.vprofile->mElementSizeBytes;
			pDest += newProfile.mElementSizeBytes;
		}

		

		for (WETL::Index a = 0; a < addedVertices.count; a++) {

			pSrc = ((BYTE*) ct.vb) + (addedVertices.el[a].ref * ct.vprofile->mElementSizeBytes);

			memcpy(pDest, pSrc, indicesOffset);
			memcpy(pDest + indicesOffset, pSrc + indicesOffsetNext, afterIndicesCopySize);

			memcpy(pDest + newWeightOffset, addedVertices.el[a].weights, ct.expWeightCount * sizeof(float));

			pDest += newProfile.mElementSizeBytes;
		
		}

		

	} else {

		memcpy(pNewVB, ct.vb, ct.vertexCount * newProfile.mElementSizeBytes);

		BYTE* pDest = (BYTE*) pNewVB;
		BYTE* pSrc = (BYTE*) ct.vb;

		for (WETL::Index a = 0; a < addedVertices.count; a++) {

			pSrc = ((BYTE*) ct.vb) + (addedVertices.el[a].ref * ct.vprofile->mElementSizeBytes);

			memcpy(pDest, pSrc, newProfile.mElementSizeBytes);

			memcpy(pDest + newWeightOffset, addedVertices.el[a].weights, ct.expWeightCount * sizeof(float));

			pDest += newProfile.mElementSizeBytes;
		
		}
	}

		
	MSafeFree(ct.pVB->pB);
	ct.pVB->pB = pNewVB;
	ct.pVB->vertexCount = (UINT) newVertexCount;

	if (profileChanged) {

		ct.pVB->profile.set(newProfile);
	}
}
*/


void pvb_group4_mergeAddedVerticesAndRemoveBlendIndices(pvb_Context& ct, pvb_AddedVertex4Array& addedVertices) {

	if ((ct.el_indices == RenderElSubNull) || (addedVertices.count == 0)) {

		return;
	}

	MeshGeometryVertexBuffer* pNewVB;
	MMemNew(pNewVB, MeshGeometryVertexBuffer());

	
	bool profileChanged = false;
	DWORD newFVF = ct.vprofile->mFVF;
	RenderElementProfile& newProfile = pNewVB->mProfile;

	if (ct.el_indices != RenderElSubNull) {

		profileChanged = true;

		newFVF &= ~D3DFVF_XYZB4;
		newFVF &= ~D3DFVF_LASTBETA_UBYTE4;
		newFVF |= D3DFVF_XYZB3;

		newProfile.vertex_set(newFVF, D3DFVF_XYZB3);
	} else {

		newProfile.set(*(ct.vprofile));
	}
	
	short newel_weights = newProfile.findSub(D3DDECLUSAGE_BLENDWEIGHT, 0);
	int newWeightOffset = newProfile.mSubElements.el[newel_weights].Offset;

	RenderUINT newVertexCount = ct.vertexCount + addedVertices.count;

	pNewVB->setElementCount(newVertexCount);
	
	if (profileChanged) { 

		int indicesOffset = ct.vprofile->mSubElements.el[ct.el_indices].Offset;
		int indicesOffsetNext = indicesOffset + ct.vprofile->sizeofSub(ct.el_indices); //VertexProfile::_sizeof(ct.pVB->profile.mElements[ct.el_indices].Type);
		int afterIndicesCopySize = ct.vprofile->mElementSizeBytes - indicesOffsetNext;



		BYTE* pDest = (BYTE*) pNewVB->bufferPtr();
		BYTE* pSrc = (BYTE*) ct.vb;

		for (RenderUINT i = 0; i < ct.vertexCount; i++) {

			memcpy(pDest, pSrc, indicesOffset);
			memcpy(pDest + indicesOffset, pSrc + indicesOffsetNext, afterIndicesCopySize);

			pSrc += ct.vprofile->mElementSizeBytes;
			pDest += newProfile.mElementSizeBytes;
		}

		

		for (WETL::Index a = 0; a < addedVertices.count; a++) {

			pSrc = ((BYTE*) ct.vb) + (addedVertices.el[a].ref * ct.vprofile->mElementSizeBytes);

			memcpy(pDest, pSrc, indicesOffset);
			memcpy(pDest + indicesOffset, pSrc + indicesOffsetNext, afterIndicesCopySize);

			memcpy(pDest + newWeightOffset, addedVertices.el[a].weights, ct.expWeightCount * sizeof(float));

			pDest += newProfile.mElementSizeBytes;
		
		}

		

	} else {

		BYTE* pDest = (BYTE*) pNewVB->bufferPtr();
		BYTE* pSrc = (BYTE*) ct.vb;

		memcpy(pDest, ct.vb, ct.vertexCount * newProfile.mElementSizeBytes);

		for (WETL::Index a = 0; a < addedVertices.count; a++) {

			pSrc = ((BYTE*) ct.vb) + (addedVertices.el[a].ref * ct.vprofile->mElementSizeBytes);

			memcpy(pDest, pSrc, newProfile.mElementSizeBytes);

			memcpy(pDest + newWeightOffset, addedVertices.el[a].weights, ct.expWeightCount * sizeof(float));

			pDest += newProfile.mElementSizeBytes;
		
		}
	}

	MMemDelete(ct.pVB);

	ct.setNewVB(pNewVB);
}

void pvb_group4(pvb_Context& ct, StaticMeshMatrixGroupArray& blendParts) {

	pvb_FaceBoneGroup4Array groupArray;
	UINT32 faceIndices[3];

	if (ct.faceCount > 0) {

		pvb_AddedVertex4Array addedVertices;
		addedVertices.reserve(5);

		RenderUINT faceCount = ct.faceCount;
		groupArray.reserve(5);

		RenderUINT processedFaceCount = 0;
		pvb_FaceProcessInfo4* facePorcessInfos;
		MMemNew(facePorcessInfos, pvb_FaceProcessInfo4[faceCount]);
		pvb_VertexProcessInfo* vertexProcessInfos;
		MMemNew(vertexProcessInfos, pvb_VertexProcessInfo[ct.vertexCount]);

		//for (int bc = ct.expWeightCount + 1; bc >= 0; bc--) {

			for (RenderUINT face = 0, i = 0; face < ct.faceCount; face++, i += 3) {

				ct.iprofile->face_get(ct.ib, i, faceIndices);
								
				if (pvb_group4_processFace(ct, face, faceIndices, groupArray, facePorcessInfos[face], 
						vertexProcessInfos, addedVertices, -1)) {

					processedFaceCount++;
					if (processedFaceCount == faceCount) {
						break;
					}
				}
			}

			//if (processedFaceCount == faceCount) {
			//	break;
			//}

		//}

		assert(processedFaceCount == faceCount);
	
		//ct.processingCosts.addedVertices = (int) addedVertices.count;
		//ct.processingCosts.stateCount = (int) groupArray.count;

		pvb_group4_mergeAddedVerticesAndRemoveBlendIndices(ct, addedVertices);

		addedVertices.destroy();
		MMemDelete(vertexProcessInfos);
		
		MeshFace* pOrigFace = ct.pFB->el;

		for (RenderUINT f = 0; f < faceCount; f++) {

			pOrigFace[facePorcessInfos[f].faceIndex].mMatrixGroupIndex = facePorcessInfos[f].groupIndex;
		}


		BoneIndex boneCount;
		MeshMatrixGroup* pBlendState;

		blendParts.setSize(groupArray.count);

		for (MeshMatrixIndex g = 0; g < groupArray.count; g++) {

			pBlendState = &(blendParts.el[g]);
			
			boneCount = groupArray.el[g].bones.size;
			pBlendState->setMatrixCount(boneCount);

			for (BYTE b = 0; b < boneCount; b++) {

				pBlendState->setMatrixIndex(b, groupArray.el[g].bones.el[b].bone);
			}
		}

		//We Dont rearrage indices by group anymore its being done eleswhere, now we just tell
		//a face which MatrixGroup it links to
		//We also dont set the bufferPart of a Matrix Group, this is done elswhere
		//as a result we can copy the groups to the groupArray withotu iterating over all the faces

		/*
		//rearrange indices
		qsort(facePorcessInfos, faceCount, sizeof(pvb_FaceProcessInfo4), pvb_FaceGroupLink_qsort_compare);
		
		void* origIB;
		MMemAllocVoid(origIB, ct.iprofile->bufferSizeBytes(3 * ct.faceCount));
		memcpy(origIB, ct.ib, ct.iprofile->bufferSizeBytes(3 * ct.faceCount));
		
		for (RenderUINT f = 0; f < faceCount; f++) {

			if (facePorcessInfos[f].faceIndex != f) {

				ct.iprofile->face_get(origIB, facePorcessInfos[f].faceIndex * 3, faceIndices);
				ct.iprofile->face_set(ct.ib, f * 3, faceIndices);
			}
		}
				

		BoneIndex boneCount;
		MeshMatrixGroupArray::Index stateIndex = 0;
		MeshMatrixGroup* pBlendState;

		blendParts.setSize(groupArray.count);
		pBlendState = &(blendParts.el[stateIndex]);
		
		pBlendState->setPart(0, 0);
		boneCount = groupArray.el[stateIndex].bones.size;
		pBlendState->setMatrixCount(boneCount);
		for (BYTE b = 0; b < boneCount; b++) {

			pBlendState->setMatrixIndex(b, groupArray.el[stateIndex].bones.el[b].bone);
		}

		for (RenderUINT i = 0; i < faceCount; i++) {

			if (facePorcessInfos[i].groupIndex != stateIndex) {

				stateIndex++;
				pBlendState = &(blendParts.el[stateIndex]);
				
				
				pBlendState->mPart.startIndex = i * 3;
				pBlendState->mPart.primCount = 1;
				boneCount = groupArray.el[stateIndex].bones.size;
				pBlendState->setMatrixCount(boneCount);
				for (BYTE b = 0; b < boneCount; b++) {

					pBlendState->setMatrixIndex(b, groupArray.el[stateIndex].bones.el[b].bone);
				}


			} else {

				pBlendState->mPart.primCount++;
			}
		}
		*/

		MMemDelete(facePorcessInfos);
		//MMemFreeVoid(origIB);
		
	
		return;
	}

}


void pvb_limitFaceWeights(pvb_Context& ct, int max) {

	UINT32 faceIndices[3];

	for (RenderUINT f = 0, i = 0; f < ct.faceCount; f++, i += 3) {

		ct.iprofile->face_get(ct.ib, i, faceIndices);
		pvb_limitFaceWeights_processFace(ct, faceIndices, max);
	}
}


void pvb_extractVertexBlendQueryInfo(MeshGeometryBuffer& VB, RenderVertexBlendingQuery& blendQuery) {

	RenderSubElIndex el_indices = VB.mProfile.findSub(D3DDECLUSAGE_BLENDINDICES);
	RenderSubElIndex el_weights = VB.mProfile.findSub(D3DDECLUSAGE_BLENDWEIGHT);

	blendQuery.inVertexBlend = (el_indices != RenderElSubNull) && (el_weights != RenderElSubNull); 

	if (blendQuery.inVertexBlend == false) {

		return;
	}

	WORD sz = VB.mProfile.sizeofSub(el_weights);
	MeshMatrixIndex expWeightCount = safeSizeCast<WORD, MeshMatrixIndex>(sz / sizeof(float));

	blendQuery.inMaxBlendMatrixCount = expWeightCount + 1;	

	BYTE* indices;
	blendQuery.inTotalMatrixCount = 0;
	
	RenderUINT vertexCount = VB.getElementCount();
	void* pVB = VB.bufferPtr();	

	for (RenderUINT v = 0; v < vertexCount; v++) {
		
		indices = VB.mProfile.getB(pVB, v, el_indices);

		for (MeshMatrixIndex i = 0; i <= expWeightCount; i++) {

			blendQuery.inTotalMatrixCount = MMax(indices[i], blendQuery.inTotalMatrixCount);
		}
	}

}

void pvb_createDestBuffers(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshGeometryVertexBuffer*& pDestVB, MeshGeometryIndexBuffer*& pDestIB) {

	MMemNew(pDestVB, MeshGeometryVertexBuffer());
	VB.cloneTo(*pDestVB, true);

	MMemNew(pDestIB, MeshGeometryIndexBuffer());
	IB.cloneTo(*pDestIB, true);

}


bool MeshMatrixGroupHelper::prepareMatrixGroupsForRenderer(
		Renderer& renderer, MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaces& FB,
		bool& hasBlending, bool& isIndexedBlending, bool& hasDestBuffers, 
		StaticMeshMatrixGroupArray& resultMatrixGroups, 
		MeshGeometryVertexBuffer*& pDestVB, MeshGeometryIndexBuffer*& pDestIB, 
		bool allowSoftwareBlending) {


	hasBlending = false;
	resultMatrixGroups.destroy();
	

	RenderVertexBlendingQuery blendQuery;
	
	pvb_extractVertexBlendQueryInfo(VB, blendQuery);

	if (blendQuery.inVertexBlend == false) {

		resultMatrixGroups.setSize(1);
		
		MeshMatrixGroup* pGroup = &(resultMatrixGroups.el[0]);

		pGroup->setMatrixCount(1);
		pGroup->setMatrixIndex(0, 0);

		return true;
	}

	renderer.processQuery(blendQuery);


	if (blendQuery.outVertexBlend == false) {

		assert(false);
		return false;
	}

	if (blendQuery.inMaxBlendMatrixCount != blendQuery.outMaxBlendMatrixCount) {

		//We dont support this kind of splitting for now

		assert(false);
		return (false);
	}
	
	hasBlending = true;
	hasDestBuffers = false;

	if (blendQuery.outUseIndexed) {

		isIndexedBlending = true;

		resultMatrixGroups.setSize(1);
		
		MeshMatrixGroup* pGroup = &(resultMatrixGroups.el[0]);

		pGroup->setMatrixCount(blendQuery.outMaxBlendMatrixCount);

		for (MeshMatrixIndex g = 0; g < blendQuery.outMaxBlendMatrixCount; g++) {

			pGroup->setMatrixIndex(g, g);
		}

	} else {

		isIndexedBlending = false;
		hasDestBuffers = true;

		pvb_createDestBuffers(VB, IB, pDestVB, pDestIB);

		pvb_Context ct;
	
		ct.init(pDestVB, pDestIB, &FB);

		pvb_limitFaceWeights(ct, blendQuery.outMaxBlendMatrixCount);
		pvb_group4(ct, resultMatrixGroups);

		//buffer ptrs might have changed
		pDestVB = ct.pVB;
		pDestIB = ct.pIB;
	}

	return true;
}

void initRenderReadyBuffers(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, 
								  Ptr<MeshGeometryVertexBuffer>& renderReadyVB, Ptr<MeshGeometryIndexBuffer>& renderReadyIB,
								  CoordSysConv* pConv) {


	MeshGeometryVertexBuffer* pRenderReadyVB;
	MMemNew(pRenderReadyVB, MeshGeometryVertexBuffer());
	VB.cloneTo(*pRenderReadyVB, true);
	pRenderReadyVB->applyConverter(pConv);

	renderReadyVB.set(pRenderReadyVB, Ptr_HardRef);

	MeshGeometryIndexBuffer* pRenderReadyIB;
	MMemNew(pRenderReadyIB, MeshGeometryIndexBuffer());
	IB.cloneTo(*pRenderReadyIB, true);
	pRenderReadyIB->applyConverter(pConv);

	renderReadyIB.set(pRenderReadyIB, Ptr_HardRef);
}

bool MeshMatrixGroupHelper::getRenderReadyBuffers(Renderer& renderer, CoordSysConvPool& convFactory, const CoordSysSemantics& srcSemantics,
				MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaces& FB,
				Ptr<MeshGeometryVertexBuffer>& renderReadyVB, Ptr<MeshGeometryIndexBuffer>& renderReadyIB, 
				StaticMeshMatrixGroupArray& renderMatrixGroups) {

	RefWrap<CoordSysConv> conv;

	conv.set(convFactory.getConverterFor(srcSemantics, renderer.getCoordSysSemantics()), false);

	initRenderReadyBuffers(VB, IB, renderReadyVB, renderReadyIB, conv.ptr());


	if (renderReadyVB.isNull() || renderReadyIB.isNull()) {

		//for now we dont know how to handle VB's without IB's

		return false;
	}


	bool hasBlending;
	bool isIndexedBlending;
	bool hasModifiedBuffers;

	MeshGeometryVertexBuffer* pModRenderReadyVB;
	MeshGeometryIndexBuffer* pModRenderReadyIB;


	MeshMatrixGroupHelper::prepareMatrixGroupsForRenderer(renderer, renderReadyVB.dref(), renderReadyIB.dref(), FB, 
					hasBlending, isIndexedBlending, hasModifiedBuffers, renderMatrixGroups, 
					pModRenderReadyVB, pModRenderReadyIB, false);

	if (hasModifiedBuffers) {

		renderReadyVB.set(pModRenderReadyVB, Ptr_HardRef);
		renderReadyIB.set(pModRenderReadyIB, Ptr_HardRef);
	}

	return true;
}

}