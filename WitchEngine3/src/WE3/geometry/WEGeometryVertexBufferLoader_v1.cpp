#include "WEGeometryBufferLoader_v1.h"

#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../WEAssert.h"

namespace WE {

void vb_loadVertex(BufferString& tempStr, RenderVertexProfile_CachedSubs& elements, RenderElementProfile& vp, void* pVB, DataChunk& el_vert, UINT32 idx) {

	RefWrap<DataChunk> part;
	part.set(el_vert.getFirstChild(), false);

	float temp[4]; 

	while(part.isValid()) {

		
		if (part->equals(L"position")) {

			if (elements.el_XYZ >= 0) {

				if (part->scanAttribute(tempStr, L"x", L"%f", &(temp[0])) == false) assrt(false);
				if (part->scanAttribute(tempStr, L"y", L"%f", &(temp[1])) == false) assrt(false);
				if (part->scanAttribute(tempStr, L"z", L"%f", &(temp[2])) == false) assrt(false);
								
				//if (pConverter) pConverter->toTargetPoint(temp);

				vp.setF(pVB, idx, elements.el_XYZ, temp, 3);
			}
			

		} else if (part->equals(L"normal")) {

			if (elements.el_NORMAL >= 0) {

				if (part->scanAttribute(tempStr, L"x", L"%f", &(temp[0])) == false) assrt(false);
				if (part->scanAttribute(tempStr, L"y", L"%f", &(temp[1])) == false) assrt(false);
				if (part->scanAttribute(tempStr, L"z", L"%f", &(temp[2])) == false) assrt(false);

				//if (pConverter) pConverter->toTargetVector(temp);
				
				vp.setF(pVB, idx, elements.el_NORMAL, temp, 3);
			}

		} else if (part->equals(L"texcoord")) {

			if (elements.el_TEX1 >= 0) {

				if (part->scanAttribute(tempStr, L"u", L"%f", &(temp[0])) == false) assrt(false);
				if (part->scanAttribute(tempStr, L"v", L"%f", &(temp[1])) == false) assrt(false);

				vp.setF(pVB, idx, elements.el_TEX1, temp, 2);
			}
		}

		part.set(part->getNextSibling(), false);
	}
}


void vb_extractVertexWeights(BufferString& tempStr, RefWrap<DataChunk>& chunk_weight_browse, UINT32 idx, BYTE blendIndices[4], float blendWeights[4]) {

	if (chunk_weight_browse.isNull()) {

		assrt(false);
		//return NULL;
		return;
	}

	//DataChunk* pChunk_weight_search = pChunk_weight;
	UINT vertexindex = UINT_MAX;
	UINT boneindex;
	float weight;
	float minWeight = 2.0f;
	short minWeightIndex = -1;

	
	short i;
	short setCount = 0;
	memset(blendIndices, 0, 4 * sizeof(BYTE));
	memset(blendWeights, 0, 4 * sizeof(float));
	

	
	while(
			(chunk_weight_browse.isValid())
		&&	(chunk_weight_browse->scanAttribute(tempStr, L"vertexindex", L"%u", &vertexindex))
		) {

		if (vertexindex == idx) {

			chunk_weight_browse->scanAttribute(tempStr, L"boneindex", L"%u", &boneindex);
			chunk_weight_browse->scanAttribute(tempStr, L"weight", L"%f", &weight);
			
			if (setCount < 4) {

				blendIndices[setCount] = boneindex;
				blendWeights[setCount] = weight;
				
				if (weight <= minWeight) {

					minWeight = weight;
					minWeightIndex = setCount;
				}
				
				++setCount;

			} else {

				if (weight > minWeight) {

					blendIndices[minWeightIndex] = boneindex;
					blendWeights[minWeightIndex] = weight;

					minWeight = 2.0f;
					minWeightIndex = -1;

					for (i = 0; i < 4; ++i) {

						if (blendWeights[i] <= minWeight) {

							minWeight = blendWeights[i];
							minWeightIndex = i;
						}
					}
					
				}
			}
			

			toNextSibling(chunk_weight_browse);

		} else {
			break;
		}
	}
}

void vb_setVertexWeights(RenderVertexProfile_CachedSubs& elements, RenderElementProfile& vp, void* pVB, const UINT32& idx, BYTE blendIndices[4], float blendWeights[4]) {
	
	vp.setF(pVB, idx, elements.el_XYZB4, blendWeights, 3);
	vp.setB4(pVB, idx, elements.el_LASTBETA_UBYTE4, blendIndices);
}

/*
bool GeometryBufferLoader_v1::load(GeometryVertexBuffer& buffer, DataSourceChunk data, bool loadCoordSys) {

	if (data.isNull()) {

		return false;
	}

	if (buffer.getElementCount() != 0) {

		assrt(false);
		return false;
	}

	BufferString tempStr;

	if (loadCoordSys && (CoordSysDataLoadHelper::extract(tempStr, data.pChunk(), buffer.mCoordSys, true) == false)) {

		//no coordSys found
		assrt(false);
		return false;
	}

	RefWrap<DataChunk> chunk_vertexbuffer(data.pChunk(), true);

	RenderUINT vertexCount = 0;

	RefWrap<DataChunk> chunk_boneassignments(chunk_vertexbuffer->getSibling(L"boneassignments"), false);

	if (chunk_vertexbuffer->scanAttribute(tempStr, L"vertexCount", L"%u", &vertexCount) == false) {

		if (chunk_vertexbuffer->scanAttribute(tempStr, L"vertexcount", L"%u", &vertexCount) == false) {

			assrt(false);
			return false;
		}
	}
	
	if (vertexCount == 0) {

		return true;
	}

	if (chunk_vertexbuffer.isValid() && chunk_vertexbuffer->equals(L"geometry")) {

		toFirstChild(chunk_vertexbuffer.ptrRef());
	}

	RenderElementProfile& profile = buffer.mProfile;

	profile.vertex_load(chunk_vertexbuffer.dref(), chunk_boneassignments.ptr());

	if (profile.elementSizeBytes() == 0) {

		assrt(false);
		return false;
	}

	if (buffer.init(NULL, vertexCount) == false) {

		assrt(false);
		return false;
	}

	void* pVB = buffer.bufferPtr();
	
	RenderVertexProfile_CachedSubs elements;
	elements.set(profile);

	bool setWeights = (chunk_boneassignments.isValid()) && (elements.el_XYZB4 >= 0) && (elements.el_LASTBETA_UBYTE4 >= 0);

	RefWrap<DataChunk> chunk_vert;
	RefWrap<DataChunk> chunk_weight;

	chunk_vert.set(chunk_vertexbuffer->getFirstChild(), false);
	
	if (setWeights) {

		chunk_weight.set(chunk_boneassignments->getFirstChild(), false);
	}
	
	BYTE blendIndices[4];
	float blendWeights[4];
	

	UINT32 idx = 0;
	while(chunk_vert.isValid()) {

		if (idx >= vertexCount) {

			assrt(false);
			break;
		}

		vb_loadVertex(tempStr, elements, profile, pVB, chunk_vert.dref(), idx
			//, pConverter
			);
	
		if (setWeights) {

			vb_extractVertexWeights(tempStr, chunk_weight, idx, blendIndices, blendWeights);
			vb_setVertexWeights(elements, profile, pVB, idx, blendIndices, blendWeights);
		}
		
		++idx;
		toNextSibling(chunk_vert);
	}

	if (idx != vertexCount) {

		assrt(false);
	}
	

	return true;
}
*/


bool GeometryBufferLoader_v1::load(GeometryVertexBuffer& buffer, DataSourceChunk data, bool loadCoordSys) {

	if (data.isNull()) {

		return false;
	}

	if (buffer.getElementCount() != 0) {

		assrt(false);
		return false;
	}

	BufferString tempStr;

	if (loadCoordSys && (CoordSysDataLoadHelper::extract(tempStr, data.pChunk(), buffer.mCoordSys, true) == false)) {

		//no coordSys found
		assrt(false);
		return false;
	}

	SoftPtr<DataChunk> geomtryChunk = data.pChunk();
	SoftRef<DataChunk> vertexBufferChunk0 = geomtryChunk->getFirstChild();
	SoftRef<DataChunk> boneAssignChunk= geomtryChunk->getNextSibling();

	if (geomtryChunk->equals(L"geometry")) {

		vertexBufferChunk0 = geomtryChunk->getFirstChild();

	} else {

		vertexBufferChunk0 = geomtryChunk.ptr();
		vertexBufferChunk0->AddRef();
	}

	RenderUINT vertexCount = 0;

	if (vertexBufferChunk0->scanAttribute(tempStr, L"vertexCount", L"%u", &vertexCount) == false) {

		if (geomtryChunk->scanAttribute(tempStr, L"vertexcount", L"%u", &vertexCount) == false) {

			assrt(false);
			return false;
		}
	}
	
	if (vertexCount == 0) {

		return true;
	}

	RenderElementProfile& profile = buffer.mProfile;

	profile.vertex_load(vertexBufferChunk0, boneAssignChunk, true);

	if (profile.elementSizeBytes() == 0) {

		assrt(false);
		return false;
	}

	if (buffer.init(NULL, vertexCount) == false) {

		assrt(false);
		return false;
	}

	void* pVB = buffer.bufferPtr();
	
	RenderVertexProfile_CachedSubs elements;
	elements.set(profile);

	bool setWeights = (boneAssignChunk.isValid()) && (elements.el_XYZB4 >= 0) && (elements.el_LASTBETA_UBYTE4 >= 0);

	SoftRef<DataChunk> chunk_vert = vertexBufferChunk0->getFirstChild();
	SoftRef<DataChunk> chunk_weight;

	if (setWeights) {

		chunk_weight = boneAssignChunk->getFirstChild();
	}
	
	BYTE blendIndices[4];
	float blendWeights[4];

	{
		UINT32 idx = 0;
		while(chunk_vert.isValid()) {

			if (idx >= vertexCount) {

				assrt(false);
				break;
			}

			vb_loadVertex(tempStr, elements, profile, pVB, chunk_vert.dref(), idx/*, pConverter*/);
		
			if (setWeights) {

				vb_extractVertexWeights(tempStr, chunk_weight, idx, blendIndices, blendWeights);
				vb_setVertexWeights(elements, profile, pVB, idx, blendIndices, blendWeights);
			}
			
			++idx;
			toNextSibling(chunk_vert);
		}

		if (idx != vertexCount) {

			assrt(false);
		}
	}

	{
		SoftRef<DataChunk> vertexBufferChunkSib = vertexBufferChunk0->getNextSibling();
		
		while (vertexBufferChunkSib.isValid()) {

			chunk_vert = vertexBufferChunkSib->getFirstChild();

			UINT32 idx = 0;
			while(chunk_vert.isValid()) {

				if (idx >= vertexCount) {

					assrt(false);
					break;
				}

				vb_loadVertex(tempStr, elements, profile, pVB, chunk_vert.dref(), idx/*, pConverter*/);
			
				++idx;
				toNextSibling(chunk_vert);
			}

			if (idx != vertexCount) {

				assrt(false);
			}

			toNextSibling(vertexBufferChunkSib);
		}
	}
	

	return true;
}


}