#ifndef _WERendererD3D9Buffers_h
#define _WERendererD3D9Buffers_h

#include "includeRenderer.h"
#include "../includeD3D9.h"
#include "WERendererD3D9ResourceManager.h"


namespace WE {

	class RendererD3D9;
	class RendererD3D9_FaceSorter;
	
	class RendererD3D9_VertexBuffer : public RenderBuffer {
	public:

		IDirect3DVertexBuffer9* pB;
		
		RendererD3D9_VertexBuffer();
		virtual ~RendererD3D9_VertexBuffer();
	};

	class RendererD3D9_IndexBuffer : public RenderBuffer {
	public:

		IDirect3DIndexBuffer9* pB;

		RendererD3D9_IndexBuffer();
		virtual ~RendererD3D9_IndexBuffer();
	};
	
	inline void RendererD3D9_convert(const RenderBuffer* from, RendererD3D9_IndexBuffer*& to) {

		to = (RendererD3D9_IndexBuffer*) from;
	}

	
	inline RendererD3D9_IndexBuffer& RendererD3D9_convertIB(const RenderBuffer* from) {

		return dref<RendererD3D9_IndexBuffer>((RendererD3D9_IndexBuffer*) from);
	}
	
	inline void RendererD3D9_convert(const RenderBuffer* from, RendererD3D9_VertexBuffer*& to) {

		to = (RendererD3D9_VertexBuffer*) from;
	}

	inline RendererD3D9_VertexBuffer& RendererD3D9_convertVB(const RenderBuffer* from) {

		return dref<RendererD3D9_VertexBuffer>((RendererD3D9_VertexBuffer*) from);
	}
	
	DWORD extractFVF(const RenderElementProfile& profile);
	D3DFORMAT extractIndexFormat(const RenderElementProfile& profile);
}

#endif