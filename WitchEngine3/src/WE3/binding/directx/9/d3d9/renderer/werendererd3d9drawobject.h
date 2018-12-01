#ifndef _WERendererD3D9DrawObject_h
#define _WERendererD3D9DrawObject_h

#include "includeRenderer.h"
#include "../includeD3D9.h"
#include "WERendererD3D9ResourceManager.h"

namespace WE {

	class RendererD3D9;

	class RendererD3D9_DrawObject : public RendererD3D9_VolatileResource {
	public:

			RendererD3D9* mpRenderer;

			D3DMATERIAL9 mat;

			IDirect3DVertexBuffer9* mpVB;
			IDirect3DIndexBuffer9*  mpIB_Box;
			IDirect3DIndexBuffer9*  mpIB_BoxNoDiag;
			IDirect3DIndexBuffer9*  mpIB_Sphere;
			IDirect3DIndexBuffer9*  mpIB_Matrix;

			DWORD mVertexCount;
			DWORD mIndexCount;

			DWORD mFVF;
			USHORT mVertexSizeBytes;

			HRESULT mTempResult;

			RendererD3D9_DrawObject();
			~RendererD3D9_DrawObject();
			void destroy();

			void setRenderer(RendererD3D9* pRenderer);
			void resetDevice(IDirect3DDevice9* pDevice);

			virtual void onDeviceState(IDirect3DDevice9* pDevice, RendererD3D9_DeviceState state);

			void _draw(IDirect3DDevice9* pDevice, 
							  const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, 
							  D3DPRIMITIVETYPE primType, UINT primCount, 
							  bool indexed = false, IDirect3DIndexBuffer9*  mpIB = NULL, UINT numVertices = 0, UINT startIndex = 0,
							  bool cullNone = false, bool alpha = false);

			void draw(IDirect3DDevice9* pDevice, const OBB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor);
			void draw(IDirect3DDevice9* pDevice, const AAB& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor, bool noDiag);
			void draw(IDirect3DDevice9* pDevice, const Sphere& object, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor);
			void draw(IDirect3DDevice9* pDevice, const Matrix3x3Base& matrix, const Vector& position, const float& size);
			void draw(IDirect3DDevice9* pDevice, const WE::RigidMatrix4x3& matrix, const float& size);
			void draw(IDirect3DDevice9* pDevice, const Point& p0, const Point& p1, const Point& p2, 
							const WE::Matrix4x3Base* pWorldMatrix, bool bumpDepthBias, bool wireFrame, const RenderColor* pColor);
			void draw(IDirect3DDevice9* pDevice, const Point& p0, const Point& p1, 
							const WE::Matrix4x3Base* pWorldMatrix, bool bumpDepthBias, const RenderColor* pColor);
			void draw(IDirect3DDevice9* pDevice, const AAPlane& plane, float size, 
							const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor);
			void draw(IDirect3DDevice9* pDevice, const Plane& plane, float size, 
							const WE::Matrix4x3Base* pWorldMatrix, const RenderColor* pColor);

			void draw2DQuad(IDirect3DDevice9* pDevice, int _x, int _y, int _x2, int _y2, const RenderColor* pColor, RenderTexture* pTexture);
		};

}


#endif