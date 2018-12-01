#ifndef includeDXUT9_H
#define includeDXUT9_H

#include "tchar.h"
#include "../../../win/includeWindows.h"

#define DXUT_AUTOLIB
#include "../../dep/DXUT_9_0_c/dxstdafx.h"


//#define DX9_LIBNAME_dxut "dxut"
//#pragma comment(lib, DX9_LIBNAME_dxut)

#include "../../../../WEMath.h"
#include "../../../../coordSys/WECoordSys.h"
#include "../../../../render/WERenderer.h"

namespace WE {

	class DXUT {
	public:	

		static void setupCamera(CModelViewerCamera& cam, Renderer& renderer, const Sphere& volumeSphere);
		static void setupCamera(CFirstPersonCamera& cam, Renderer& renderer, float fov, float minDist, float maxDist, const Vector3& eye, const Vector3& lookAt);
	};

}

#endif