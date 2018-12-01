#include "WERenderElementProfile.h"

#include "../WEMacros.h"

namespace WE {


WORD RenderElementProfile::_sizeof(BYTE type) {

	switch(type) {
		case D3DDECLTYPE_FLOAT1:
			return 1 * sizeof(float);
		case D3DDECLTYPE_FLOAT2:
			return 2 * sizeof(float);
		case D3DDECLTYPE_FLOAT3:
			return 3 * sizeof(float);
		case D3DDECLTYPE_FLOAT4:
			return 4 * sizeof(float);
		case D3DDECLTYPE_UBYTE4:
			return sizeof(DWORD);
	}

	return 0;
}

WORD RenderElementProfile::sizeofSub(short sub) const {

	return _sizeof(mSubElements.el[sub].Type);
}


RenderElementProfile::RenderElementProfile() {

	mElementSizeBytes = 0;
	mExtra = 0;
}

bool RenderElementProfile::isVertexProfile() const {

	return !(isIndexProfile());
}

bool RenderElementProfile::isIndexProfile() const {

	return (mSubElements.size == 1) && (mSubElements.el[0].Usage == _WE_D3DDECLUSAGE_INDEX);
}

RenderElementProfile::~RenderElementProfile() {

	destroy();
}

void RenderElementProfile::destroy() {

	mElementSizeBytes = 0;
	mExtra = 0;
	mSubElements.destroy();
}

RenderUINT RenderElementProfile::elementSizeBytes() const {

	return mElementSizeBytes;
}

RenderUINT RenderElementProfile::bufferSizeBytes(RenderUINT elementCount) const {

	return elementCount * mElementSizeBytes;
}

RenderUINT RenderElementProfile::bufferElementCount(RenderUINT bufferSizeBytes) const {

	if (mElementSizeBytes == 0) {

		assert (bufferSizeBytes == 0);
		return 0;

	}

	return bufferSizeBytes / mElementSizeBytes;
}

void RenderElementProfile::set(const RenderElementProfile& ref) {

	mExtra = ref.mExtra;
	mElementSizeBytes = ref.mElementSizeBytes;
	ref.mSubElements.memCloneTo(mSubElements);
}

RenderElementProfile::SubElementIndex RenderElementProfile::findSubByFVF(DWORD fvf) const {

	switch (fvf) {
		case D3DFVF_XYZ:
			return findSub(D3DDECLUSAGE_POSITION, 0);
		case D3DFVF_XYZB1:
		case D3DFVF_XYZB2:
		case D3DFVF_XYZB3:
		case D3DFVF_XYZB4:
			return findSub(D3DDECLUSAGE_BLENDWEIGHT, 0);
		case D3DFVF_LASTBETA_UBYTE4:
			return findSub(D3DDECLUSAGE_BLENDINDICES, 0);
		case D3DFVF_NORMAL:
			return findSub(D3DDECLUSAGE_NORMAL, 0);
		case D3DFVF_TEX1:
			return findSub(D3DDECLUSAGE_TEXCOORD, 0);
	}

	return (SubElementIndex) -1;
}


RenderElementProfile::SubElementIndex RenderElementProfile::findSub(D3DDECLUSAGE usage, BYTE usageIndex) const {

	for (SubElementIndex i = 0; i < mSubElements.size; i++) {

		if ((mSubElements.el[i].Usage == usage) /*&& (mSubElements.el[i].UsageIndex == usageIndex)*/) {

			return i;
		}
	}

	return -1;
}

void* RenderElementProfile::getPtr(void* buffer, UINT32 element, SubElementIndex sub) const {

	BYTE* comp = (BYTE*) buffer;
	comp += element * (UINT32) mElementSizeBytes;
	comp += (mSubElements.el[sub].Offset);

	return (void*) comp;
}


float* RenderElementProfile::getF(void* buffer, UINT32 element, SubElementIndex sub) const {

	return (float*) getPtr(buffer, element, sub);
}

BYTE* RenderElementProfile::getB(void* buffer, UINT32 element, SubElementIndex sub) const {

	return (BYTE*) getPtr(buffer, element, sub);
}


void* RenderElementProfile::getRaw(void* buffer, UINT32 element) const {

	BYTE* comp = (BYTE*) buffer;
	comp += element * (UINT32) mElementSizeBytes;

	return (void*) comp;
}

void RenderElementProfile::setRaw(void* buffer, UINT32 element, void* raw) {

	BYTE* comp = (BYTE*) buffer;
	comp += element * (UINT32) mElementSizeBytes;

	memcpy(comp, raw, mElementSizeBytes);
}

void RenderElementProfile::setRaw(void* buffer, UINT32 element, void* raw, RenderUINT sizeBytes) const {

	BYTE* comp = (BYTE*) buffer;
	comp += element * (UINT32) mElementSizeBytes;

	memcpy(comp, raw, sizeBytes);
}

void RenderElementProfile::setF(void* buffer, UINT32 element, SubElementIndex sub, float* values, unsigned short valueCount) const {

	memcpy(getPtr(buffer, element, sub), values, valueCount * sizeof(float));
}



void RenderElementProfile::setB4(void* buffer, UINT32 element, SubElementIndex sub, BYTE values[4]) const {

	//BYTE temp[4];
	//temp[0] = values[3];
	//temp[1] = values[2];
	//temp[2] = values[1];
	//temp[3] = values[0];
	

	//A packed DWORD contains index3, index2, index1, and index0, 
	//where index0 is located in the lowest byte of the DWORD.

	memcpy(getPtr(buffer, element, sub), values, 4 * sizeof(BYTE));
}


bool RenderElementProfile::vertex_set(DWORD fvf, DWORD fvf_XYZ) {

	mFVF = fvf;

	/*

	DWORD dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE |
              D3DFVF_SPECULAR | D3DFVF_TEX2 |
              D3DFVF_TEXCOORDSIZE1(0) |  // Uses 1-D texture coordinates for
                                         // texture coordinate set 1 (index 0).
              D3DFVF_TEXCOORDSIZE2(1);   // And 2-D texture coordinates for 
                                         // texture coordinate set 2 (index 1).
										 */
	
	SubElementIndex elCount = 0;

	 if (fvf_XYZ == D3DFVF_XYZB4) {

		elCount += 2;
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) elCount++;

	} else if (fvf_XYZ == D3DFVF_XYZB3) {

		elCount += 2;
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) elCount++;

	} else if (fvf_XYZ == D3DFVF_XYZB2) {

		elCount += 2;
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) elCount++;

	} else if (fvf_XYZ == D3DFVF_XYZB1) {

		elCount += 2;

	} else if (fvf_XYZ == D3DFVF_XYZ) {

		elCount++;
	}

	if (mFVF & D3DFVF_NORMAL) {
		elCount++;
	}
	if (mFVF & D3DFVF_TEX1) {
		elCount++;
	}
	
	if (elCount == 0) {
		
		destroy();
		return false;
	}

	//Direct3D compliance
	//mSubElements.setSize(elCount + 1);
	mSubElements.resize(elCount);
		

	SubElementIndex eli = 0;
	WORD offset = 0;
	
	 if (fvf_XYZ == D3DFVF_XYZB4) {

		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_POSITION;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;

		short bcount = 4;
		bcount -= mFVF & D3DFVF_LASTBETA_UBYTE4 ? 1 : 0;
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;

		switch(bcount) {
			case 1:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT1;
				break;
			case 2:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
				break;
			case 3:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
				break;
			case 4:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT4;
				break;
		}
		
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_BLENDWEIGHT;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
		
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) {

			//mSubElements.el[eli].Stream = 0;
			mSubElements.el[eli].Offset = offset;
			mSubElements.el[eli].Type = D3DDECLTYPE_UBYTE4;
			//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
			mSubElements.el[eli].Usage =  D3DDECLUSAGE_BLENDINDICES;
			//mSubElements.el[eli].UsageIndex = 0;

			offset += _sizeof(mSubElements.el[eli].Type);
			eli++;
		}

	} else if (fvf_XYZ == D3DFVF_XYZB3) {

		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_POSITION;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;

		short bcount = 3;
		bcount -= mFVF & D3DFVF_LASTBETA_UBYTE4 ? 1 : 0;
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;

		switch(bcount) {
			case 1:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT1;
				break;
			case 2:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
				break;
			case 3:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
				break;
			case 4:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT4;
				break;
		}
		
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_BLENDWEIGHT;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
		
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) {

			//mSubElements.el[eli].Stream = 0;
			mSubElements.el[eli].Offset = offset;
			mSubElements.el[eli].Type = D3DDECLTYPE_UBYTE4;
			//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
			mSubElements.el[eli].Usage =  D3DDECLUSAGE_BLENDINDICES;
			//mSubElements.el[eli].UsageIndex = 0;

			offset += _sizeof(mSubElements.el[eli].Type);
			eli++;
		}
	} else if (fvf_XYZ == D3DFVF_XYZB2) {

		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_POSITION;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;

		short bcount = 2;
		bcount -= mFVF & D3DFVF_LASTBETA_UBYTE4 ? 1 : 0;
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;

		switch(bcount) {
			case 1:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT1;
				break;
			case 2:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
				break;
			case 3:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
				break;
			case 4:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT4;
				break;
		}
		
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_BLENDWEIGHT;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
		
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) {

			//mSubElements.el[eli].Stream = 0;
			mSubElements.el[eli].Offset = offset;
			mSubElements.el[eli].Type = D3DDECLTYPE_UBYTE4;
			//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
			mSubElements.el[eli].Usage =  D3DDECLUSAGE_BLENDINDICES;
			//mSubElements.el[eli].UsageIndex = 0;

			offset += _sizeof(mSubElements.el[eli].Type);
			eli++;
		}
	} else if (fvf_XYZ == D3DFVF_XYZB1) {
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_POSITION;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;

		short bcount = 1;
		bcount -= mFVF & D3DFVF_LASTBETA_UBYTE4 ? 1 : 0;
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		switch(bcount) {
			case 1:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT1;
				break;
			case 2:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
				break;
			case 3:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
				break;
			case 4:
				mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT4;
				break;
		}
		
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_BLENDWEIGHT;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
		
		if (mFVF & D3DFVF_LASTBETA_UBYTE4) {
			//mSubElements.el[eli].Stream = 0;
			mSubElements.el[eli].Offset = offset;
			mSubElements.el[eli].Type = D3DDECLTYPE_UBYTE4;
			//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
			mSubElements.el[eli].Usage =  D3DDECLUSAGE_BLENDINDICES;
			//mSubElements.el[eli].UsageIndex = 0;

			offset += _sizeof(mSubElements.el[eli].Type);
			eli++;
		}
	} else if (fvf_XYZ == D3DFVF_XYZ) {
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_POSITION;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
	}

	if (mFVF & D3DFVF_NORMAL) {
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_NORMAL;
		//mSubElements.el[eli].UsageIndex = 0;

		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
	}

	if (mFVF & D3DFVF_TEX1) {
		//mSubElements.el[eli].Stream = 0;
		mSubElements.el[eli].Offset = offset;
		mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
		//mSubElements.el[eli].Method = D3DDECLMETHOD_DEFAULT;
		mSubElements.el[eli].Usage = D3DDECLUSAGE_TEXCOORD;
		//mSubElements.el[eli].UsageIndex = 0;

		if (mFVF & D3DFVF_TEXCOORDSIZE1(0)) {
			mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT1;
		} else if (mFVF & D3DFVF_TEXCOORDSIZE2(0)) {
			mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT2;
		} else if (mFVF & D3DFVF_TEXCOORDSIZE3(0)) {
			mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT3;
		} else if (mFVF & D3DFVF_TEXCOORDSIZE4(0)) {
			mSubElements.el[eli].Type = D3DDECLTYPE_FLOAT4;
		} 
		
		
		offset += _sizeof(mSubElements.el[eli].Type);
		eli++;
	}

	mElementSizeBytes = offset;

	//Direct3D compliance

	if (mElementSizeBytes % 4) {

		//require DWORD alignment for now
		assrt(false);
	}

	//add last element to comply with Direct3D
	/*
	if (mSubElements.size) {

		mSubElements.el[eli].Stream = 0xFF;
		mSubElements.el[eli].Offset = 0;
		mSubElements.el[eli].Type = D3DDECLTYPE_UNUSED;
		mSubElements.el[eli].Method = 0;
		mSubElements.el[eli].Usage = 0;
		mSubElements.el[eli].UsageIndex = 0;
	}
	*/

	return true;
}



void RenderElementProfile::vertex_simpleConvert(void* src, void* dest, UINT32 element, CoordSysConv* pConv) const {

	float* srcF;
	float* destF;

	for (SubElementIndex i = 0; i < mSubElements.size; i++) {

		switch (mSubElements.el[i].Usage) {
		case D3DDECLUSAGE_POSITION:
			{
				srcF = getF(src, element, i);
				destF = getF(dest, element, i);

				if (srcF == destF) {
					pConv->toTargetPoint(destF);
				} else {
					pConv->toTargetPoint(srcF, destF);
				}
			}
			break;
		case D3DDECLUSAGE_NORMAL:
			{
				srcF = getF(src, element, i);
				destF = getF(dest, element, i);

				if (srcF == destF) {
					pConv->toTargetVector(destF);
				} else {
					pConv->toTargetVector(srcF, destF);
				}
			}
			break;
		}
	}
}

void RenderElementProfile::vertex_simpleConvert(void* src, void* dest, UINT32 element, Matrix4x3Base* pConv) const {

	float* srcF;
	Vector3 temp;

	for (SubElementIndex i = 0; i < mSubElements.size; i++) {

		switch (mSubElements.el[i].Usage) {
		case D3DDECLUSAGE_POSITION:
			{
				srcF = getF(src, element, i);
				temp.set(MExpand3(srcF));
				pConv->transformPoint(temp);
				setF(dest, element, i, temp.el, 3);
			}
			break;
		/*
		Normals might be messed up after this!
		a better way is to check matrix and see what it does ...
		case D3DDECLUSAGE_NORMAL:
			{
				srcF = getF(src, element, i);
				temp.set(srcF);
				pConv->transformVector(temp);
				temp.normalize();
				setF(dest, element, i, temp.el, 3);
			}
			break;
		*/
		}
	}
}

void RenderElementProfile::vertex_reanchor(void* src, void* dest, UINT32 element, float* anchoringOffset) const {

	float* srcF;
	float* destF;
	Vector3 temp;

	for (SubElementIndex i = 0; i < mSubElements.size; i++) {

		switch (mSubElements.el[i].Usage) {
		case D3DDECLUSAGE_POSITION:
			{
				srcF = getF(src, element, i);
				destF = getF(dest, element, i);

				reanchor(anchoringOffset, srcF, destF);
			}
			break;
		}
	}

}

void RenderElementProfile::vertex_simpleConvertArray(void* src, void* dest, UINT32 elementCount, CoordSysConv* pConv) const {

	for (UINT32 i = 0; i < elementCount; i++) {

		vertex_simpleConvert(src, dest, i, pConv);
	}
}

void RenderElementProfile::vertex_simpleConvertArray(void* src, void* dest, UINT32 elementCount, Matrix4x3Base* pConv) const {

	for (UINT32 i = 0; i < elementCount; i++) {

		vertex_simpleConvert(src, dest, i, pConv);
	}
}

void RenderElementProfile::vertex_reanchorArray(void* src, void* dest, UINT32 elementCount, float* anchoringOffset) const {

	for (UINT32 i = 0; i < elementCount; i++) {

		vertex_reanchor(src, dest, i, anchoringOffset);
	}
}

void RenderElementProfile::face_simpleConvert(void* src, void* dest, UINT32 firstIndex, CoordSysConv* pConv) const {

	UINT32 face[3];

	for (SubElementIndex i = 0; i < mSubElements.size; i++) {

		switch (mSubElements.el[i].Usage) {
		case _WE_D3DDECLUSAGE_INDEX:
			{
				face_get(src, firstIndex, face);
				pConv->toTargetFaceIndices(face);
				face_set(dest, firstIndex, face);
			}
			break;
		}
	}
}

void RenderElementProfile::face_simpleConvertArray(void* src, void* dest, UINT32 indexCount, CoordSysConv* pConv) const {

	for (UINT32 i = 0; i < indexCount; i += 3) {

		face_simpleConvert(src, dest, i, pConv);
	}
}

void RenderElementProfile::vertex_load(DataChunk& el_vb, DataChunk* pEl_weights, bool scanVBSiblings) {

	DWORD fvf = 0;
	DWORD fvf_XYZ = 0;

	BufferString tempStr;
	bool test;
	
	test = false;
	el_vb.scanAttribute(tempStr, L"positions", test);

	if (scanVBSiblings && !test) {

		SoftRef<DataChunk> sib = el_vb.getNextSibling();

		while (!test && sib.isValid()) {

			sib->scanAttribute(tempStr, L"positions", test);
			toNextSibling(sib);
		}
	}

	if (test) {
	
		if (pEl_weights == NULL) {

			fvf |= D3DFVF_XYZ;
			fvf_XYZ = D3DFVF_XYZ;
		} else {

			fvf |= D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4;
			fvf_XYZ = D3DFVF_XYZB4;
		}
	}

	test = false;
	el_vb.scanAttribute(tempStr, L"normals", test);

	if (scanVBSiblings && !test) {

		SoftRef<DataChunk> sib = el_vb.getNextSibling();

		while (!test && sib.isValid()) {

			sib->scanAttribute(tempStr, L"normals", test);
			toNextSibling(sib);
		}
	}

	if (test) {

		fvf |= D3DFVF_NORMAL;
	}

	unsigned int test2 = 0;
	HardRef<DataChunk> texCoordChunk = &el_vb;

	texCoordChunk->scanAttribute(tempStr, L"texture_coords", L"%u", &test2);

	if (scanVBSiblings && (test2 == 0)) {

		SoftRef<DataChunk> sib = el_vb.getNextSibling();

		while ((test2 == 0) && sib.isValid()) {

			texCoordChunk = sib;
			sib->scanAttribute(tempStr, L"texture_coords", L"%u", &test2);
			toNextSibling(sib);
		}
	}

	if (test2 != 0) {

		fvf |= D3DFVF_TEX1;

		test2 = 2;
		texCoordChunk->scanAttribute(tempStr, L"texture_coords_dimensions_0", L"%u", &test2);
		
		switch (test2) {
			case 1:
				fvf |= D3DFVF_TEXCOORDSIZE1(0);
				break;
			case 2:
				fvf |= D3DFVF_TEXCOORDSIZE2(0);
				break;
			case 3:
				fvf |= D3DFVF_TEXCOORDSIZE3(0);
				break;
			case 4:
				fvf |= D3DFVF_TEXCOORDSIZE4(0);
				break;
		}

	}

	vertex_set(fvf, fvf_XYZ);
}

RenderVertexProfile_CachedSubs::RenderVertexProfile_CachedSubs() {

	el_XYZ = RenderElSubNull;
	el_NORMAL = RenderElSubNull;
	el_TEX1 = RenderElSubNull;
	el_XYZB4 = RenderElSubNull;
	el_LASTBETA_UBYTE4 = RenderElSubNull;
}

void RenderVertexProfile_CachedSubs::set(RenderElementProfile& profile) {
		
	el_XYZ = profile.findSubByFVF(D3DFVF_XYZ);
	el_NORMAL = profile.findSubByFVF(D3DFVF_NORMAL);
	el_TEX1 = profile.findSubByFVF(D3DFVF_TEX1);
	el_XYZB4 = profile.findSubByFVF(D3DFVF_XYZB4); 
	el_LASTBETA_UBYTE4 = profile.findSubByFVF(D3DFVF_LASTBETA_UBYTE4);
}


RenderSubElement& RenderElementProfile::index_element() const {

	return mSubElements.el[0];
}


void RenderElementProfile::index_set(bool is32Bit) {

	mSubElements.resize(1);

	RenderSubElement& el = index_element();

	//mFormat = is32Bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16;

	//el.Stream = 0xFF;
	el.Offset = 0;
	//el.Method = 0;
	//el.UsageIndex = 0;

	el.Type = is32Bit ? _WE_D3DDECLTYPE_INDEX32 : _WE_D3DDECLTYPE_INDEX16;
	el.Usage = _WE_D3DDECLUSAGE_INDEX;

	mElementSizeBytes = is32Bit ? sizeof(UINT32) : sizeof(UINT16);
}

/*
bool RenderElementProfile::face_isStrip() const {

	return (index_element().Usage == _WE_D3DDECLUSAGE_FACE_STRIP);
}
*/

bool RenderElementProfile::index_is32bit() const {

	return (index_element().Type == _WE_D3DDECLTYPE_INDEX32);
}

/*
UINT32 RenderElementProfile::face_faceBase(UINT32 face) const {

	return face_isStrip() ? face : face * 3;
}

UINT32 RenderElementProfile::face_indexFace(UINT32 index) const {

	return face_isStrip() ? index : index / 3;
}

UINT32 RenderElementProfile::face_indexCount(UINT32 faceCount) const {

	return face_isStrip() ? faceCount + 2 : faceCount * 3;
}

UINT32 RenderElementProfile::face_faceCount(UINT32 indexCount) const {

	return face_isStrip() ? indexCount - 2 : indexCount / 3;
}

UINT32 RenderElementProfile::face_bufferSizeBytes(UINT32 faceCount) const {

	return face_indexCount(faceCount) * mElementSizeBytes;
}
*/

UINT32 RenderElementProfile::index_get(void* buffer, UINT32 element) const {

	if (index_is32bit()) {

		return ((UINT32*) buffer)[element];
	} else {

		return (UINT32) (((UINT16*) buffer)[element]);
	}
}

void RenderElementProfile::index_get(void* buffer, UINT32 startElement, UINT32* values, UINT32 count) const {

	bool is32bit = index_is32bit();

	for (UINT32 i = 0; i < count; i++) {

		if (is32bit) {

			values[i] = ((UINT32*) buffer)[startElement++];
		} else {

			values[i] = (UINT32) (((UINT16*) buffer)[startElement++]);
		}
	}
}

void RenderElementProfile::index_set(void* buffer, UINT32 element, UINT32 value) const {

	if (index_is32bit()) {

		((UINT32*) buffer)[element] = value;
	} else {

		((UINT16*) buffer)[element] = safeSizeCast<UINT32, UINT16>(value);
	}
}

void RenderElementProfile::index_set(void* buffer, UINT32 startElement, UINT32* values, UINT32 count) const {

	bool is32bit = index_is32bit();

	for (UINT32 i = 0; i < count; i++) {

		if (is32bit) {

			((UINT32*) buffer)[startElement++] = values[i];
		} else {

			((UINT16*) buffer)[startElement++] = safeSizeCast<UINT32, UINT16>(values[i]);
		}
	}
}

bool RenderElementProfile::index_isOptimizable(UINT32 maxIndex, RenderUINT& newElementSize, void*& reserved) {

	newElementSize = sizeof(UINT16);

	return (index_is32bit() && (maxIndex < (UINT32) UINT16_MAX) );
}

void RenderElementProfile::index_setOptimized(void* reserved) {

	index_set(false);
}

void RenderElementProfile::index_optimizeIndexDataType(UINT32 indexCount, UINT32 maxIndex, void* pCurrIB, void* pNewIB, void* reserved) {

	if (indexCount && /*index_is32bit() &&*/ (maxIndex < (UINT32) UINT16_MAX) ) {

		UINT16* pTemp = (UINT16*) pNewIB;
		//MMemAlloc(pTemp, indexCount * sizeof(UINT16), UINT16*);

		for(UINT32 i = 0; i < indexCount; i++) {

			pTemp[i] = safeSizeCast<UINT32, UINT16>( (((UINT32*) pCurrIB)[i]) );
		}

		//MMemFreeVoid(pIB);
		//pIB = pTemp;
		//index_set(false/*, face_isStrip()*/);
	}
}


void RenderElementProfile::face_get(void* buffer, UINT32 firstIndex, UINT32* indices) const {

	index_get(buffer, firstIndex/*face_faceBase(face)*/, indices, 3);
}

void RenderElementProfile::face_set(void* buffer, UINT32 firstIndex, UINT32* indices) const {

	index_set(buffer, firstIndex/*face_faceBase(face)*/, indices, 3);
}

/*
void RenderElementProfile::face_optimizeIndexDataType(UINT32 faceCount, UINT32 maxIndex, void*& pIB) {

	index_optimizeIndexDataType(face_indexCount(faceCount), maxIndex, pIB);
}
*/

}