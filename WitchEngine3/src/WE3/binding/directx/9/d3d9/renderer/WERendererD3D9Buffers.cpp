#include "WERendererD3D9Buffers.h"

#include "WERendererD3D9.h"

namespace WE {


DWORD extractFVF(const RenderElementProfile& profile) {

	return profile.mFVF;
}

D3DFORMAT extractIndexFormat(const RenderElementProfile& profile) {

	return profile.index_is32bit() ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
}

RendererD3D9_VertexBuffer::RendererD3D9_VertexBuffer() : pB(NULL) {
}

RendererD3D9_VertexBuffer::~RendererD3D9_VertexBuffer() {

	MSafeRelease(pB);
}

bool RendererD3D9::createVB(const RenderElementProfile& profile, RenderBufferUsage& usage, RenderUINT elementCount, RenderBuffer** ppBuffer) {
	
	*ppBuffer = NULL;

	assrt(profile.isVertexProfile());

	DWORD d3d_usage;
	D3DPOOL d3d_pool;
	bool needsSoftwareProcessing = false;
	bool needsVertexBlending = false;
	bool illegal = false;
	
	if (elementCount == 0) {

		return true;
	}

	//RendererD3D9_convert(creationStruct->pVertexBlending, illegal, needsSoftwareProcessing, needsVertexBlending);

	if (illegal) {
		
		assrt(false);
		return false;
	}
	

	RendererD3D9_convert(usage, needsSoftwareProcessing, d3d_usage);
	d3d_pool = D3DPOOL_MANAGED;

	RendererD3D9_VertexBuffer* pRet;
	MMemNew(pRet, RendererD3D9_VertexBuffer());

	if (FAILED(mpDevice->CreateVertexBuffer(profile.bufferSizeBytes(elementCount), 
											d3d_usage, extractFVF(profile), 
											d3d_pool, &pRet->pB, NULL))) {
		
		assrt(false);

		MMemDelete(pRet);
		return false;
	}

	*ppBuffer = pRet;

	return true;
}

void RendererD3D9::lockVB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convertVB(pBuffer);

	//(d3d_usage & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0)
	MCheckD3DStmt(vb.pB->Lock(offset, sizeBytes, ppData, 0));
}

void RendererD3D9::unlockVB(RenderBuffer* pBuffer) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convertVB(pBuffer);

	MCheckD3DStmt(vb.pB->Unlock());
}

void RendererD3D9::setSourceVB(RenderUINT stream, RenderBuffer* pBuffer, RenderUINT stride) {

	RendererD3D9_VertexBuffer& vb = RendererD3D9_convertVB(pBuffer);

	MCheckD3DStmt(mpDevice->SetStreamSource(stream, vb.pB, 0, stride));
}



RendererD3D9_IndexBuffer::RendererD3D9_IndexBuffer() : pB(NULL) {
}

RendererD3D9_IndexBuffer::~RendererD3D9_IndexBuffer() {

	MSafeRelease(pB);
}

void RendererD3D9::setSourceIB(RenderBuffer* pBuffer) {

	RendererD3D9_IndexBuffer& ib = RendererD3D9_convertIB(pBuffer);

	MCheckD3DStmt(mpDevice->SetIndices(ib.pB));
}


void RendererD3D9::drawIndexed(RenderPrimitiveEnum prim, RenderUINT indexBufferOffset, RenderUINT primCount, RenderUINT vertexBufferOffset, RenderUINT vertexCount) {

	MCheckD3DStmt(mpDevice->DrawIndexedPrimitive(RendererD3D9_convert(prim), 0, vertexBufferOffset, vertexCount, indexBufferOffset, primCount));
}

void RendererD3D9::draw(RenderPrimitiveEnum prim, RenderUINT vertexBufferOffset, RenderUINT vertexCount) {

	MCheckD3DStmt(mpDevice->DrawPrimitive(RendererD3D9_convert(prim), vertexBufferOffset, vertexCount));
}

bool RendererD3D9::createIB(bool bit32, RenderBufferUsage& usage, RenderUINT bufferSizeBytes, RenderBuffer** ppBuffer) {

	*ppBuffer = NULL;

	//assrt(profile.isIndexProfile());

	DWORD d3d_usage;
	D3DPOOL d3d_pool;
	bool needsSoftwareProcessing = false;
	
	if (bufferSizeBytes == 0) {

		return false;
	}

	/*
	RendererD3D9_convert(creationStruct->pVertexBlending, illegal, needsSoftwareProcessing, needsVertexBlending);

	if (illegal) {

		assrt(false);

		return false;
	}
	*/

	RendererD3D9_convert(usage, needsSoftwareProcessing, d3d_usage);
	//RendererD3D9_convert(creationStruct->pool, d3d_pool);
	d3d_pool = D3DPOOL_MANAGED;

	RendererD3D9_IndexBuffer* pRet;
	MMemNew(pRet, RendererD3D9_IndexBuffer());


	if (mpDevice->CreateIndexBuffer(//profile.bufferSizeBytes(elementCount), 
									bufferSizeBytes,
									d3d_usage, 
									//extractIndexFormat(profile), 
									bit32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16, 
									d3d_pool, &pRet->pB, 
									NULL) != D3D_OK) {
		
		assrt(false);
										
		MMemDelete(pRet);	
		return false;
	}

	*ppBuffer = pRet;

	return true;
}


void RendererD3D9::lockIB(RenderBuffer* pBuffer, RenderUINT offset, RenderUINT sizeBytes, void** ppData) {

	RendererD3D9_IndexBuffer& ib = RendererD3D9_convertIB(pBuffer);

	//(d3d_usage & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0)
	MCheckD3DStmt(ib.pB->Lock(offset, sizeBytes, ppData, 0));
}

void RendererD3D9::unlockIB(RenderBuffer* pBuffer) {

	RendererD3D9_IndexBuffer& ib = RendererD3D9_convertIB(pBuffer);

	MCheckD3DStmt(ib.pB->Unlock());
}

}