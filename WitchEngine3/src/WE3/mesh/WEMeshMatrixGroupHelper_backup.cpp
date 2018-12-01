#include "WEMeshMatrixGroupHelper.h"

#include "../skeleton/WESkeletonProcessingStructs.h"
#include "../WETL/WETLArray.h"

namespace WE {

struct FaceProcessingLimitWeights {

	skel_FaceBoneGroup12 counter;
};

struct skel_Context2 : skel_Context {

	MeshFaceBuffer* pFB;

	typedef WETL::Array<FaceProcessingLimitWeights, false, unsigned int> FaceProcessingLimitWeightsArr;
	FaceProcessingLimitWeightsArr mFaceProcessingLimitWeightsArr;

	skel_Context2() : skel_Context() {

		pFB = NULL;
	}

	void setFB(MeshFaceBuffer* _pFB) {

		pFB = _pFB;
	}
};


void skel_processFace_removeBone(skel_Context& ct, float* weights, BYTE* indices, SceneTransformIndex index, float weight) {

	SceneTransformIndex nonZeroWeightCount = 0;
	float weightCumul = 0.0f;

	SceneTransformIndex i;
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
	float addedWeight = weight;
	
	if (index < ct.expWeightCount)
		weights[index] = 0.0f;

	while (addedWeight != 0.0f) {

		int maxi = 0;
		float max = 0.0f;

		float cumulWeight = 0.0f;
		float lastWeight = 0.0f;

		for (i = 0; i < ct.expWeightCount; i++) {

			cumulWeight += weights[i];

			if (weights[i] > max) {

				maxi = i;
				max = weights[i];
			}
		}

		lastWeight = 1.0f - cumulWeight;

		if ((index < ct.expWeightCount) && lastWeight > max) {

			//do nothing, weight will automatically go to last weight
			addedWeight = 0.0f;

		} else {

			weights[maxi] += addedWeight;

			if (weights[maxi] > 1.0f) {

				weights[maxi] = 1.0f;
				addedWeight = weights[maxi] - 1.0f;

			} else {

				addedWeight = 0.0f;
			}

			if (fabs(addedWeight) < 0.0001f)
				addedWeight = 0.0f;
		}
	}

	/*
	for (i = 0; i < ct.expWeightCount; i++) {

		if (i != index) {

			if (weights[i] != 0.0f) {

				weights[i] += addedWeightPerIndex;
			}

		} else {
			weights[i] = 0.0f;
		}
	}

	for (i = 0; i < ct.expWeightCount; i++) {

		if (i != index) {
			if (weights[i] != 0.0f) {
				weights[i] += addedWeightPerIndex;
			}
		} else {
			weights[i] = 0.0f;
		}
	}
	*/
}

void skel_processFace_removeBone(skel_Context& ct, RenderUINT vertex, BoneIndex indexedBoneIndex) {

	float weightCumul;
	float* weights;
	BYTE* indices;
	
	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.vb, vertex, ct.el_indices);

	bool removed = false;
	weightCumul = 0.0f;

	int i;

	for (i = 0; i < ct.expWeightCount; i++) {

		weightCumul += weights[i];
		if ((weights[i] != 0.0f) && (indices[i] == indexedBoneIndex)) {

			if (weights[i] >= 1.0f) {

				assrt(false);
			}

			skel_processFace_removeBone(ct, weights, indices, i, weights[i]);
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
		if ((lastWeight > 0.0f) && indices[i] == indexedBoneIndex) {

			if (lastWeight >= 1.0f) {

				assrt(false);
			}

			skel_processFace_removeBone(ct, weights, indices, i, lastWeight);
		}
	}
}

void skel_processFace_removeBoneFromFaceVertices(skel_Context& ct, RenderUINT fi, BoneIndex indexedBoneIndex) {

	UINT32 faceIndices[3];

	ct.iprofile->face_get(ct.ib, fi, faceIndices);

	for (int v = 0; v < 3; ++v) {

		skel_processFace_removeBone(ct, faceIndices[v], indexedBoneIndex);
	}
}

void skel_processFace_removeBones(skel_Context& ct, RenderUINT vertex, skel_FaceBoneGroup12& counter, SceneTransformIndex bonesToRemove) {

	float weightCumul;
	float* weights;
	BYTE* indices;
	
	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.vb, vertex, ct.el_indices);

	bool removed;
	for (SceneTransformIndex b = 0; b < bonesToRemove; b++) {

		removed = false;
		weightCumul = 0.0f;


		SceneTransformIndex i;
		for (i = 0; i < ct.expWeightCount; i++) {

			weightCumul += weights[i];
			if ((weights[i] != 0.0f) && (indices[i] == counter.bones.el[b].bone)) {

				skel_processFace_removeBone(ct, weights, indices, i, weights[i]);
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
				skel_processFace_removeBone(ct, weights, indices, i, lastWeight);
			}
		}

	}
}


bool skel_group4_processFace_adapt(skel_Context& ct, skel_FaceBoneGroup4& group, float* weights, BYTE* indices) {

	float weightCumul = 0.0f;
	float lastWeight;

	SceneTransformIndex i;
	SceneTransformIndex j;


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



bool skel_group4_processFace_tryAdapt(skel_Context& ct, skel_FaceBoneGroup4& group, 
											   skel_FaceBoneGroup4& groupToAdapt) {

	BYTE adaptedBoneCount = group.bones.count;

	SceneTransformIndex i;
	SceneTransformIndex j;
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

WETL::Index::Type skel_group4_processFace_findAdaptable(skel_Context& ct, skel_FaceBoneGroup4Array& groupArray, 
										 skel_FaceBoneGroup4& groupToAdapt) {

	bool adapted = false;
	WETL::Index g;
	for (g = 0; g < groupArray.count; g++) {
		if (skel_group4_processFace_tryAdapt(ct, *groupArray.el[g], groupToAdapt)) {
			return g;
		}
	}

	g = groupArray.count;
	groupArray.addOne(new skel_FaceBoneGroup4());
	if (skel_group4_processFace_tryAdapt(ct, *groupArray.el[g], groupToAdapt) == false) {
		//ct.processingCosts.unadaptableFaceCount++;
	}

	return g;
}

void skel_group4_mergeFace(skel_Context& ct, UINT32 faceIndices[3], skel_FaceBoneGroup4& mergedNonZeroWeightGroup) {

	float* weights;
	BYTE* indices;
	
	for (short v = 0; v < 3; v++) {

		weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
		indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);

		float weightCumul = 0.0f;
		SceneTransformIndex i;
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

		mergedNonZeroWeightGroup.sortSmart(false);
		mergedNonZeroWeightGroup.bones.count = 4;
	}
}


struct skel_AddedVertex4 {

	WETL::Index ref;
	float weights[3];
	BYTE indices[4];
};


typedef WETL::CountedArray<skel_AddedVertex4, true, WETL::Index::Type> skel_AddedVertex4Array;

struct skel_VertexProcessInfo {

	int adaptedGroup;

	skel_VertexProcessInfo() {
		adaptedGroup = -1;
	}
};


struct skel_FaceGroupLink {

	WETL::Index faceIndex;
	int groupIndex;
	
};

struct skel_FaceProcessInfo4 : skel_FaceGroupLink {

	bool processed;
	bool merged;
	skel_FaceBoneGroup4 mergedGroup;


	skel_FaceProcessInfo4() {

		processed = false;
		merged = false;
		
		mergedGroup.bones.count = 0;
	}
};

bool skel_group4_processFace(skel_Context& ct, RenderUINT faceIndex, UINT32 faceIndices[3], skel_FaceBoneGroup4Array& groupArray, 
								   skel_FaceProcessInfo4& processInfo, skel_VertexProcessInfo* vertexProcessInfos, 
								   skel_AddedVertex4Array& addedVertices, int minBoneCount = -1) {

	if (processInfo.processed == true) {
		return false;
	}

	processInfo.faceIndex = faceIndex;
	processInfo.groupIndex = -1;
	
	if (processInfo.merged == false) {
		skel_group4_mergeFace(ct, faceIndices, processInfo.mergedGroup);

		processInfo.merged = true;
	}


	
	if ((minBoneCount >= 0) && (processInfo.mergedGroup.bones.count < minBoneCount)) {
		return false;
	}

	processInfo.processed = true;

	processInfo.groupIndex = (int) skel_group4_processFace_findAdaptable(ct, groupArray, processInfo.mergedGroup);


	float* weights;
	BYTE* indices;
	
	for (int v = 0; v < 3; v++) {

		if (vertexProcessInfos[faceIndices[v]].adaptedGroup == -1) {

			vertexProcessInfos[faceIndices[v]].adaptedGroup = processInfo.groupIndex;

			weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
			indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);
		
			skel_group4_processFace_adapt(ct, *groupArray.el[processInfo.groupIndex], weights, indices);

			

		} else {

			if (vertexProcessInfos[faceIndices[v]].adaptedGroup != processInfo.groupIndex) {
		
				skel_AddedVertex4* pAdded = &(addedVertices.addOne());
				ct.iprofile->index_set(ct.ib, (faceIndex * 3) + v, ct.vertexCount + (addedVertices.count - 1));
				pAdded->ref = faceIndices[v];

				weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
				indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);

				memcpy(pAdded->weights, weights, ct.expWeightCount * sizeof(float));
				memcpy(pAdded->indices, indices, 1 * sizeof(DWORD));

				
				skel_group4_processFace_adapt(ct, *groupArray.el[processInfo.groupIndex], pAdded->weights, pAdded->indices);
				
			}
		}
	}


	return true;
}


int __cdecl skel_FaceGroupLink_qsort_compare(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	return ( (((skel_FaceGroupLink*) e1)->groupIndex) - (((skel_FaceGroupLink*) e2)->groupIndex) );
}



/*
void skel_group4_mergeAddedVerticesAndRemoveBlendIndices(skel_Context& ct, skel_AddedVertex4Array& addedVertices) {

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


void skel_group4_mergeAddedVerticesAndRemoveBlendIndices(skel_Context& ct, skel_AddedVertex4Array& addedVertices) {

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

		RenderUINT sizeDiff = 0;

		newFVF &= ~D3DFVF_XYZB4;
		newFVF &= ~D3DFVF_LASTBETA_UBYTE4;
		newFVF |= D3DFVF_XYZB3;

		newProfile.vertex_set(newFVF, D3DFVF_XYZB3);

	} else {

		newProfile.set(*(ct.vprofile));
	}
	pNewVB->signalProfileChanged(false);
	
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

void skel_group4(skel_Context2& ct, StaticMeshMatrixGroupArray& blendParts) {

	skel_FaceBoneGroup4Array groupArray;
	UINT32 faceIndices[3];

	if (ct.faceCount > 0) {

		skel_AddedVertex4Array addedVertices;
		addedVertices.reserve(5);

		RenderUINT faceCount = ct.faceCount;
		groupArray.reserve(5);

		RenderUINT processedFaceCount = 0;
		skel_FaceProcessInfo4* facePorcessInfos;
		MMemNew(facePorcessInfos, skel_FaceProcessInfo4[faceCount]);
		skel_VertexProcessInfo* vertexProcessInfos;
		MMemNew(vertexProcessInfos, skel_VertexProcessInfo[ct.vertexCount]);

		//init groups by starting with vertices with most weights
		/*
		{
			skel_FaceBoneGroup4 vertexGroup;

			for (int vwc = 4; vwc >= 1; --vwc) {

				for (RenderUINT v = 0; v < ct.vertexCount; ++v) {

					vertexGroup.reset();
					skel_processFace_countBones(ct, v, vertexGroup);

					if (vertexGroup.bones.count == vwc) {

						skel_group4_processFace_findAdaptable(ct, groupArray, vertexGroup);
					}
				}
			}
		}
		*/

		//for (int bc = ct.expWeightCount + 1; bc >= 0; bc--) {

			for (RenderUINT face = 0, i = 0; face < ct.faceCount; face++, i += 3) {

				ct.iprofile->face_get(ct.ib, i, faceIndices);
								
				if (skel_group4_processFace(ct, face, faceIndices, groupArray, facePorcessInfos[face], 
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

		assrt(processedFaceCount == faceCount);
	
		//ct.processingCosts.addedVertices = (int) addedVertices.count;
		//ct.processingCosts.stateCount = (int) groupArray.count;

		skel_group4_mergeAddedVerticesAndRemoveBlendIndices(ct, addedVertices);

		addedVertices.destroy();
		MMemDeleteArray(vertexProcessInfos);
		
		MeshFace* pOrigFace = ct.pFB->arrayBase().el;

		for (RenderUINT f = 0; f < faceCount; f++) {

			pOrigFace[facePorcessInfos[f].faceIndex].mMatrixGroupIndex = facePorcessInfos[f].groupIndex;
		}


		BoneIndex boneCount;
		MeshMatrixGroup* pBlendState;

		blendParts.setSize(groupArray.count);

		for (SceneTransformIndex g = 0; g < groupArray.count; g++) {

			pBlendState = &(blendParts.el[g]);
			
			boneCount = groupArray.el[g]->bones.count;
			pBlendState->setMatrixCount(boneCount);

			for (BYTE b = 0; b < boneCount; b++) {

				pBlendState->setMatrixIndex(b, groupArray.el[g]->bones.el[b].bone);
			}
		}

		//We Dont rearrage indices by group anymore its being done eleswhere, now we just tell
		//a face which MatrixGroup it links to
		//We also dont set the bufferPart of a Matrix Group, this is done elswhere
		//as a result we can copy the groups to the groupArray withotu iterating over all the faces

		/*
		//rearrange indices
		qsort(facePorcessInfos, faceCount, sizeof(skel_FaceProcessInfo4), skel_FaceGroupLink_qsort_compare);
		
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
		boneCount = groupArray.el[stateIndex].bones.count;
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
				boneCount = groupArray.el[stateIndex].bones.count;
				pBlendState->setMatrixCount(boneCount);
				for (BYTE b = 0; b < boneCount; b++) {

					pBlendState->setMatrixIndex(b, groupArray.el[stateIndex].bones.el[b].bone);
				}


			} else {

				pBlendState->mPart.primCount++;
			}
		}
		*/

		MMemDeleteArray(facePorcessInfos);
		//MMemFreeVoid(origIB);
		
	
		return;
	}

}

bool skel_limitFaceWeights_processFace(skel_Context& ct, UINT32 faceIndices[3], int max) {

	skel_FaceBoneGroup12 counter;
	
	for (short v = 0; v < 3; v++) {

		skel_processFace_countBones(ct, faceIndices[v], counter);
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

		counter.sortSmart(true);
		SceneTransformIndex bonesToRemove = safeSizeCast<int, SceneTransformIndex>((int)counter.bones.count - max);

		//ct.processingCosts.removedBoneWeights += bonesToRemove;

		//TODO improve algorithm remove bone with min weight not min face weight or sthg like that
		for (short v = 0; v < 3; v++) {

			skel_processFace_removeBones(ct, faceIndices[v], counter, bonesToRemove);
		}
	
		/*
		//check
		counter.reset();
		for (short v = 0; v < 3; v++) {
			skel_processFace_countBones(ct, faceIndices[v], counter);
		}

		if (counter.boneCount > max) {
			counter.boneCount = counter.boneCount;
		}
		*/

		return true;
	}

	return false;
}

bool skel_limitFaceWeights_removeVertexEpsilons(skel_Context2& ct, RenderUINT vertex, float epsilon) {

	bool ret = false;

	float* weights;
	BYTE* indices;

	weights = ct.vprofile->getF(ct.vb, vertex, ct.el_weights);
	indices = ct.vprofile->getB(ct.vb, vertex, ct.el_indices);

	for (SceneTransformIndex i = 0; i < ct.expWeightCount; i++) {

		if (weights[i] != 0.0f && weights[i] <= epsilon) {

			skel_processFace_removeBone(ct, vertex, indices[i]);
			ret = true;
		}
	}

	float weightCumul = 0.0f;

	for (SceneTransformIndex i = 0; i < ct.expWeightCount; i++) {

		weightCumul += weights[i];
	}

	float lastWeight = 1.0f - weightCumul;

	if (lastWeight > 0.0f && lastWeight <= epsilon) {

		ret = true;
		skel_processFace_removeBone(ct, vertex, indices[ct.expWeightCount]);
	}

	return ret;
}

bool skel_limitFaceWeights_removeFaceEpsilons(skel_Context2& ct, RenderUINT face, RenderUINT facei, float epsilon) {

	bool ret = false;

	UINT32 faceIndices[3];

	ct.iprofile->face_get(ct.ib, face, faceIndices);
	for (short v = 0; v < 3; v++) {

		ret = skel_limitFaceWeights_removeVertexEpsilons(ct, faceIndices[v], epsilon) || ret;
	}

	return ret;
}


void skel_limitFaceWeights_old(skel_Context2& ct, int max) {

	float epsilon = 0.75f * (1.0f / (float) max);

	for (RenderUINT v = 0; v < ct.vertexCount; ++v) {

		skel_limitFaceWeights_removeVertexEpsilons(ct, v, epsilon);
	}

	UINT32 faceIndices[3];

	for (RenderUINT f = 0, i = 0; f < ct.faceCount; f++, i += 3) {

		ct.iprofile->face_get(ct.ib, i, faceIndices);
		skel_limitFaceWeights_processFace(ct, faceIndices, max);
	}
}

void skel_updateFaceCounter(skel_Context& ct, RenderUINT facei, skel_FaceBoneGroup12& counter) {

	UINT32 faceIndices[3];

	counter.reset();
	ct.iprofile->face_get(ct.ib, facei, faceIndices);
	for (short v = 0; v < 3; v++) {

		skel_processFace_countBones(ct, faceIndices[v], counter);
	}
}

bool skel_limitFaceWeights_processSharedVertex(skel_Context2& ct, RenderUINT vi, RenderUINT face1, RenderUINT facei1, RenderUINT face2, RenderUINT facei2, int max, bool& hadModifs1, bool& hadModifs2) {

	bool wasModified = false;
	hadModifs1 = false;
	hadModifs2 = false;

	skel_FaceBoneGroup12 sharedCounter;

	FaceProcessingLimitWeights& proc1 = ct.mFaceProcessingLimitWeightsArr[face1];
	FaceProcessingLimitWeights& proc2 = ct.mFaceProcessingLimitWeightsArr[face2];


	int max1 = proc1.counter.bones.count;
	int max2 = proc2.counter.bones.count;

	for (int i1 = 0; i1 < proc1.counter.bones.count; ++i1) {

		skel_FaceBone& bone1 = proc1.counter.bones.el[i1];
		sharedCounter.add(bone1.bone, bone1.faceWeight);
	}

	for (int i2 = 0; i2 < proc2.counter.bones.count; ++i2) {

		skel_FaceBone& bone2 = proc2.counter.bones.el[i2];
		sharedCounter.add(bone2.bone, bone2.faceWeight);
	}

	if (sharedCounter.bones.count > max) {

		float epsilon = 0.75f * (1.0f / (float) max);

		if (skel_limitFaceWeights_removeFaceEpsilons(ct, face1, facei1, epsilon)) {

			hadModifs1 = true;
			skel_updateFaceCounter(ct, facei1, proc1.counter);
		}

		if (skel_limitFaceWeights_removeFaceEpsilons(ct, face2, facei2, epsilon)) {

			hadModifs2 = true;
			skel_updateFaceCounter(ct, facei2, proc2.counter);
		}
	}

	sharedCounter.reset();

	float stageWeights[3] = { 1.0f, 0.501f, 0.5f };

	for (int wi = 0; wi < 3; ++wi) {

		if (sharedCounter.bones.count < max
			&& (max1 > sharedCounter.bones.count || max2 > sharedCounter.bones.count)) {

			for (int i1 = 0; i1 < proc1.counter.bones.count; ++i1) {

				skel_FaceBone& bone1 = proc1.counter.bones.el[i1];

				if (bone1.maxWeight >= stageWeights[wi]) {

					sharedCounter.add(bone1.bone, bone1.faceWeight);

					if (sharedCounter.bones.count == max)
						break;
				}
			}
		}

		if (sharedCounter.bones.count < max
			&& (max1 > sharedCounter.bones.count || max2 > sharedCounter.bones.count)) {

			for (int i2 = 0; i2 < proc2.counter.bones.count; ++i2) {

				skel_FaceBone& bone2 = proc2.counter.bones.el[i2];

				if (bone2.maxWeight >= stageWeights[wi]) {

					sharedCounter.add(bone2.bone, bone2.faceWeight);

					if (sharedCounter.bones.count == max)
						break;
				}
			}
		}
	}

	if (sharedCounter.bones.count < max
		&& (max1 > sharedCounter.bones.count || max2 > sharedCounter.bones.count)) {

			//gather shared bones
		for (int i1 = 0; i1 < proc1.counter.bones.count; ++i1) {

			skel_FaceBone& bone1 = proc1.counter.bones.el[i1];

			if (bone1.faceWeight > 0.0f) {

				for (int i2 = 0; i2 < proc2.counter.bones.count; ++i2) {

					skel_FaceBone& bone2 = proc2.counter.bones.el[i2];

					if (bone2.faceWeight > 0.0f) {

						bool gather = (bone2.bone == bone1.bone);
						float gatherWeight = 0.5f * (bone1.faceWeight + bone2.faceWeight);
					
						if (gather) {

							sharedCounter.add(bone1.bone, gatherWeight);

							if (sharedCounter.bones.count == max)
								break;
						}
					}
				}
			}

			if (sharedCounter.bones.count == max)
				break;
		}
	}

	bool hasMoreToAdd1 = true;
	bool hasMoreToAdd2 = true;

	int addStage = 0;

	while (sharedCounter.bones.count < max && (hasMoreToAdd1 || hasMoreToAdd2)) {

		FaceProcessingLimitWeights& proc = addStage % 2 == 0 ? proc1 : proc2;
		bool& hasMoreToAdd = addStage % 2 == 0 ? hasMoreToAdd1 : hasMoreToAdd2;
		++addStage;

		if (hasMoreToAdd) {

			bool added = false;

			for (int i1 = 0; i1 < proc.counter.bones.count; ++i1) {

				skel_FaceBone& bone = proc.counter.bones.el[i1];
				BoneIndex bi;

				if (!sharedCounter.find(bone.bone, bi)) {

					sharedCounter.add(bone.bone, bone.faceWeight);
					added = true;
					break;
				}
			}

			if (!added)
				hasMoreToAdd = false;
		}
	}

	int stage = 0;

	while (stage <= 1) {

		FaceProcessingLimitWeights& proc = stage % 2 == 0 ? proc1 : proc2;
		RenderUINT& facei = stage % 2 == 0 ? facei1 : facei2;
		bool& hadModifs = stage % 2 == 0 ? hadModifs1 : hadModifs2;

		++stage;

		for (int i1 = 0; i1 < proc.counter.bones.count; ++i1) {

			bool found = false;

			for (int i = 0; i < sharedCounter.bones.count; ++i) {

				if (sharedCounter.bones.el[i].bone == proc.counter.bones.el[i1].bone) {

					found = true;
					break;
				}
			}

			if (!found) {

				hadModifs = true;
				wasModified = true;
				skel_processFace_removeBoneFromFaceVertices(ct, facei, proc.counter.bones.el[i1].bone);
			}
		}

		
		skel_updateFaceCounter(ct, facei, proc.counter);
	}

	return wasModified;
}

void skel_limitFaceWeights_new(skel_Context2& ct, int max) {

	ct.mFaceProcessingLimitWeightsArr.reserve(ct.faceCount);

	for (RenderUINT f = 0, i = 0; f < ct.faceCount; f++, i += 3) {

		FaceProcessingLimitWeights& proc = ct.mFaceProcessingLimitWeightsArr[f];

		skel_updateFaceCounter(ct, i, proc.counter);
	}

	const unsigned int maxIterCount = ct.faceCount / 2;
	unsigned int iterCount = 0;
	bool hadModifs = true;
	RenderUINT startModifFace = 0;
	UINT32 faceIndices[3];
	UINT32 faceIndices2[3];

	while (hadModifs) {

		hadModifs = false;
		++iterCount;

		for (RenderUINT vi = 0; vi < ct.vertexCount; ++vi) {

			for (RenderUINT f1 = 0, i1 = 0; f1 < ct.faceCount; ++f1, i1 += 3) {

				ct.iprofile->face_get(ct.ib, i1, faceIndices);

				for (int fi1 = 0; fi1 < 3; ++fi1) {

					if (faceIndices[fi1] == vi) {

						//face f1 containts vertex vi at fi1

						for (RenderUINT f2 = f1 + 1, i2 = i1 + 3; f2 < ct.faceCount; ++f2, i2 += 3) {

							ct.iprofile->face_get(ct.ib, i2, faceIndices2);

							for (int fi2 = 0; fi2 < 3; ++fi2) {

								if (faceIndices2[fi2] == vi) {

									bool hadModifs1;
									bool hadModifs2;

									//face f2 containts vertex vi at fi2
									hadModifs = skel_limitFaceWeights_processSharedVertex(ct, vi, f1, i1, f2, i2, max, hadModifs1, hadModifs2) || hadModifs;

									if (hadModifs) {

										if (startModifFace == 0)
											startModifFace = f1;
									}

									if (hadModifs1) {

										f2 = f1 + 1;
										i2 = i1 + 3;
									}
								}
							}
						}
					}
				}
			}
		}

		if (iterCount == maxIterCount && hadModifs) {

			log(L"Skeletal Mesh Vertex Blend Adapt failed (%u iters)", iterCount);
			assrt(false);
			break;
		}

		startModifFace = 0; 
	}

	iterCount = iterCount;

	/*
	{
		UINT32 faceIndices[3];

		for (RenderUINT f = 0, i = 0; f < ct.faceCount; f++, i += 3) {

			ct.iprofile->face_get(ct.ib, i, faceIndices);
			skel_limitFaceWeights_processFace(ct, faceIndices, max);
		}
	}
	*/
}

#define skel_limitFaceWeights skel_limitFaceWeights_old

void skel_extractVertexBlendQueryInfo(MeshGeometryVertexBuffer& VB, RenderVertexBlendingQuery& blendQuery) {

	RenderSubElIndex el_indices = VB.mProfile.findSub(D3DDECLUSAGE_BLENDINDICES);
	RenderSubElIndex el_weights = VB.mProfile.findSub(D3DDECLUSAGE_BLENDWEIGHT);

	blendQuery.inVertexBlend = (el_indices != RenderElSubNull) && (el_weights != RenderElSubNull); 

	if (blendQuery.inVertexBlend == false) {

		return;
	}

	WORD sz = VB.mProfile.sizeofSub(el_weights);
	SceneTransformIndex expWeightCount = safeSizeCast<WORD, SceneTransformIndex>(sz / sizeof(float));

	blendQuery.inMaxBlendMatrixCount = expWeightCount + 1;	
	blendQuery.inHasNormals = (VB.mProfile.findSub(D3DDECLUSAGE_NORMAL) != RenderElSubNull);
	
	BYTE* indices;
	blendQuery.inTotalMatrixCount = 0;
	
	RenderUINT vertexCount = VB.getElementCount();
	void* pVB = VB.bufferPtr();	

	for (RenderUINT v = 0; v < vertexCount; v++) {
		
		indices = VB.mProfile.getB(pVB, v, el_indices);

		for (SceneTransformIndex i = 0; i <= expWeightCount; i++) {

			blendQuery.inTotalMatrixCount = MMax(indices[i] + 1, blendQuery.inTotalMatrixCount);
		}
	}

	blendQuery.inAllowIndexedSplit = true;
	blendQuery.inIndexedSplitMinMatrixCount = 12;
}

void skel_createDestBuffers(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshGeometryVertexBuffer*& pDestVB, MeshGeometryIndexBuffer*& pDestIB) {

	MMemNew(pDestVB, MeshGeometryVertexBuffer());
	VB.cloneTo(*pDestVB, true);

	MMemNew(pDestIB, MeshGeometryIndexBuffer());
	IB.cloneTo(*pDestIB, true);

}


bool MeshMatrixGroupHelper::prepareMatrixGroupsForRenderer(
		Renderer& renderer, MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB,
		bool& hasBlending, bool& isIndexedBlending, bool& hasDestBuffers, 
		StaticMeshMatrixGroupArray& resultMatrixGroups, 
		MeshGeometryVertexBuffer*& pDestVB, MeshGeometryIndexBuffer*& pDestIB, 
		bool allowSoftwareBlending) {


	hasDestBuffers = false;
	hasBlending = false;
	resultMatrixGroups.destroy();
	

	RenderVertexBlendingQuery blendQuery;
	
	skel_extractVertexBlendQueryInfo(VB, blendQuery);

	blendQuery.inAllowIndexedSplit = false;

	if (blendQuery.inVertexBlend == false) {

		resultMatrixGroups.setSize(1);
		
		MeshMatrixGroup* pGroup = &(resultMatrixGroups.el[0]);

		pGroup->setMatrixCount(1);
		pGroup->setMatrixIndex(0, 0);

		FB.batchSetMatrixGroupIndex(0);

		return true;
	}

	renderer.processQuery(blendQuery);

	if (blendQuery.outVertexBlend == false) {

		assrt(false);
		return false;
	}

	if (blendQuery.inMaxBlendMatrixCount != blendQuery.outMaxBlendMatrixCount) {

		//We dont support this kind of splitting for now

		assrt(false);
		return (false);
	}
	
	
	if (blendQuery.outUseIndexed) {
	
		if (blendQuery.outIndexedSplit) {

			assrt(false);

			/*
			skel_createDestBuffers(VB, IB, pDestVB, pDestIB);

			isIndexedBlending = true;
			createMatrixGroupsByMatrixCountLimit(dref(pDestVB), dref(pDestIB), FB, blendQuery.inTotalMatrixCount, blendQuery.outIndexedSplitMaxMatrixCount, resultMatrixGroups);
			*/

		} else {

			//can be optimized by setting identity (1 to 1 flag) and used to minimize
			//setting similar groups more than once
			
			isIndexedBlending = true;

			resultMatrixGroups.setSize(1);
			
			MeshMatrixGroup* pGroup = &(resultMatrixGroups.el[0]);

			pGroup->setMatrixCount(blendQuery.inTotalMatrixCount);

			for (SceneTransformIndex g = 0; g < blendQuery.inTotalMatrixCount; g++) {

				pGroup->setMatrixIndex(g, g);
				pGroup->setIndexedBlending(true, blendQuery.outMaxBlendMatrixCount);
			}
		

			{
				MeshFaces& faces = FB.arrayBase();

				for (RenderUINT i = 0; i < faces.size; i++) {

					faces.el[i].mMatrixGroupIndex = 0;
				}
			}
		}

	} else {

		isIndexedBlending = false;
		hasDestBuffers = true;

		skel_createDestBuffers(VB, IB, pDestVB, pDestIB);

		skel_Context2 ct;
	
		ct.init(pDestVB, pDestIB);
		ct.setFB(&FB);

		skel_limitFaceWeights(ct, blendQuery.outMaxBlendMatrixCount);
		skel_group4(ct, resultMatrixGroups);

		//buffer ptrs might have changed
		pDestVB = ct.pVB;
		pDestIB = ct.pIB;
	}

	return true;
}

void initRenderReadyBuffers(Renderer& renderer, MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, 
								  FlexiblePtr<MeshGeometryVertexBuffer>& renderReadyVB, FlexiblePtr<MeshGeometryIndexBuffer>& renderReadyIB,
								  CoordSysConvPool& convPool) {

	RefWrap<CoordSysConv> conv;

	const CoordSys& targetSem = renderer.getCoordSys();
	conv.set(convPool.getConverterFor(VB.getCoordSys(), targetSem), false);

	MeshGeometryVertexBuffer* pRenderReadyVB;
	MMemNew(pRenderReadyVB, MeshGeometryVertexBuffer());
	VB.cloneTo(*pRenderReadyVB, true);
	pRenderReadyVB->applyConverter(conv.ptr(), &targetSem);

	renderReadyVB.set(pRenderReadyVB, Ptr_HardRef);

	conv.set(convPool.getConverterFor(IB.getCoordSys(), renderer.getCoordSys()), false);

	MeshGeometryIndexBuffer* pRenderReadyIB;
	MMemNew(pRenderReadyIB, MeshGeometryIndexBuffer());
	IB.cloneTo(*pRenderReadyIB, true);
	pRenderReadyIB->applyConverter(conv.ptr(), &targetSem);

	renderReadyIB.set(pRenderReadyIB, Ptr_HardRef);
}

bool MeshMatrixGroupHelper::getRenderReadyBuffers(Renderer& renderer, CoordSysConvPool& convPool,
				MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB,
				FlexiblePtr<MeshGeometryVertexBuffer>& renderReadyVB, FlexiblePtr<MeshGeometryIndexBuffer>& renderReadyIB, 
				StaticMeshMatrixGroupArray& renderMatrixGroups) {

	initRenderReadyBuffers(renderer, VB, IB, renderReadyVB, renderReadyIB, convPool);


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




void MeshMatrixGroupHelper::groupByMatrixGroup(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB, MeshMatrixGroups& matrixGroups, bool updateMatrixGroupPart, bool updateFaceBufferBaseIndex) {

	if (matrixGroups.size == 0) {

		return;
	}

	MeshGeometryIndexBuffer origIB;
	IB.cloneTo(origIB, true);

	void* origIBPtr = origIB.bufferPtr();
	void* IBPtr = IB.bufferPtr();
	
	const RenderElementProfile& iprofile = origIB.getProfile();

	
	UINT32 minVertexIndex;
	UINT32 maxVertexIndex;	
	UINT32 indices[3];
	RenderUINT indexIB = 0;
	
	for (RenderUINT g = 0; g < matrixGroups.size; g++) {

		if (updateMatrixGroupPart) {

			matrixGroups.el[g].mPartIB.startIndex = indexIB;

			GeometryIndexBuffer::initMinMaxIndex(minVertexIndex, maxVertexIndex);
		}

		MeshFaces& faces = FB.arrayBase();

		for (RenderUINT i = 0; i < faces.size; i++) {

			assrt(faces.el[i].mMatrixGroupIndex != (MeshMatrixGroupIndex::Type) -1);

			if (faces.el[i].mMatrixGroupIndex == g) {
				
				iprofile.face_get(origIBPtr, faces.el[i].mBaseIndex, indices);
				iprofile.face_set(IBPtr, indexIB, indices);

				if (updateFaceBufferBaseIndex) {

					faces.el[i].mBaseIndex = indexIB;
				}

				if (updateMatrixGroupPart) {

					GeometryIndexBuffer::updateMinMaxIndex(indices, minVertexIndex, maxVertexIndex);
				}

				indexIB += 3;
			}
		}

		if (updateMatrixGroupPart) {

			matrixGroups.el[g].mPartIB.primCount = (indexIB - matrixGroups.el[g].mPartIB.startIndex) / 3;
			matrixGroups.el[g].mPartVB.startIndex = minVertexIndex;
			matrixGroups.el[g].mPartVB.primCount = (maxVertexIndex - minVertexIndex) + 1;

		}
	}


	origIB.destroy();
}

/*
void MeshMatrixGroupHelper::createMatrixGroupsByMatrixCountLimit(MeshGeometryVertexBuffer& VB, MeshGeometryIndexBuffer& IB, MeshFaceBuffer& FB, int totalMatrixCount, int maxMatrixCount, StaticMeshMatrixGroupArray& matrixGroups) {

	{
		MeshMatrixGroupArray processMatrixGroups;

		MeshFaces& faces = FB.arrayBase();

		skel_Context2 ct;
	
		ct.init(&VB, &IB);
		ct.setFB(&FB);

		UINT32 faceIndices[3];

		MeshMatrixGroup* pMatrixGroup = NULL;
		int currMatrixCount = 0;

		int weightCount = ct.expWeightCount + 1;

		if (weightCount > 4) {

			assrt(false);
			return;
		}

		struct OutIndicies {

			BYTE outIndices[4];
		};

		SizeAllocT<OutIndices, SceneTransformIndex> outIndiciesArray;

		outIndiciesArray.resize(faces.size);
		outIndiciesArray.memSet(0);

		for (RenderUINT fi = 0; fi < faces.size; fi++) {

			ct.iprofile->face_get(ct.ib, faces.el[fi].mBaseIndex, faceIndices);

			float* weights;
			BYTE* indices;
			
			for (int v = 0; v < 3; v++) {

				weights = ct.vprofile->getF(ct.vb, faceIndices[v], ct.el_weights);
				indices = ct.vprofile->getB(ct.vb, faceIndices[v], ct.el_indices);

				float weightCumul = 0.0f;
				float lastWeight;

				BYTE outIndices[4] = outIndiciesArray.el[fi];
				bool found[4] = {true,true,true,true};
				int unFoundCount = weightCount;

				for (int wi = 0; wi < ct.expWeightCount; wi++) {

					if (weights[wi] != 0.0f) {

						if (pMatrixGroup == NULL) {

							pMatrixGroup = &processMatrixGroups.addOne();
							currMatrixCount = 0;
						}

						found[wi] = false;

						for (int mi = 0; mi < pMatrixGroup->mMatrixIndexArray.size; ++mi) {

							if (pMatrixGroup->mMatrixIndexArray[mi] == indices[wi]) {

								outIndices[wi] = mi;
								
								found[wi] = true;
								--unFoundCount;

								break;
							}
						}

					} else {

						--unFoundCount;
					}

					weightCumul += weights[wi];
				}

				lastWeight = 1.0f - weightCumul;
				
				if (lastWeight > 0.0f) {

					int wi = weightCount - 1;

					if (pMatrixGroup == NULL) {

						pMatrixGroup = &processMatrixGroups.addOne();
						currMatrixCount = 0;
					}

					found[wi] = false;

					for (int mi = 0; mi < pMatrixGroup->mMatrixIndexArray.size; ++mi) {

						if (pMatrixGroup->mMatrixIndexArray[mi] == indices[wi]) {

							outIndices[wi] = mi;
							
							--unFoundCount;
							found[wi] = true;
							break;
						}
					}

				} else {

					--unFoundCount;
				}

				if (unFoundCount > 0) {

					if (currMatrixCount + unFoundCount <= maxMatrixCount) {

						if (pMatrixGroup == NULL) {

							pMatrixGroup = &processMatrixGroups.addOne();
							currMatrixCount = 0;
						}

						faces.el[fi].mMatrixGroupIndex = processMatrixGroups.count - 1;

						for (int ui = 0; ui < weightCount; ++ui) {

							if (!found[ui]) {

								outIndices[ui] = currMatrixCount;

								pMatrixGroup->setMatrixCount(currMatrixCount + 1);
								pMatrixGroup->mMatrixIndexArray[currMatrixCount] = indices[ui];
								++currMatrixCount;
							}
						}

					} else {

						pMatrixGroup = &processMatrixGroups.addOne();
						currMatrixCount = 0;

						faces.el[fi].mMatrixGroupIndex = processMatrixGroups.count - 1;

						for (int ui = 0; ui < weightCount; ++ui) {

							if (weights[ui] != 0.0f) {

								outIndices[ui] = currMatrixCount;

								pMatrixGroup->setMatrixCount(currMatrixCount + 1);
								pMatrixGroup->mMatrixIndexArray[currMatrixCount] = indices[ui];
								++currMatrixCount;
							}
						}

						if (lastWeight > 0.0f) {

							int ui = weightCount - 1;

							outIndices[ui] = currMatrixCount;

							pMatrixGroup->setMatrixCount(currMatrixCount + 1);
							pMatrixGroup->mMatrixIndexArray[currMatrixCount] = indices[ui];
							++currMatrixCount;
						}
					}
				}
			}
		}

		for (RenderUINT fi = 0; fi < faces.size; fi++) {

			no, here we might overwrite vertex data from faces which share the vertex!
		}

		matrixGroups.setSize(processMatrixGroups.count);

		for (int i = 0; i < processMatrixGroups.count; ++i) {

			MeshMatrixGroup& from = processMatrixGroups[i];
			MeshMatrixGroup& to = matrixGroups[i];

			to.setMatrixCount(from.mMatrixIndexArray.size);
			to.setIndexedBlending(true, weightCount);

			for (int j = 0; j < from.mMatrixIndexArray.size; ++j) {

				to.setMatrixIndex(i, from.mMatrixIndexArray[j]);
			}
		}

	}
}
*/


}