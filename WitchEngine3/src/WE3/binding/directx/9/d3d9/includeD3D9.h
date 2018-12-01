#ifndef _includeD3D9_h
#define _includeD3D9_h


#define DX9_LIBNAME_d3d9 "d3d9"
#define DX9_LIBNAME_d3dx9 "d3dx9"
#define DX9_LIBNAME_dxguid "dxguid"
#define DX9_LIBNAME_dinput8 "dinput8"

//#define WIN_LIBNAME_winmm "winmm"

#pragma comment(lib, DX9_LIBNAME_d3d9)
//#pragma comment(lib, DX9_LIBNAME_dxguid)

//#pragma comment(lib, WIN_LIBNAME_winmm)

#include "../../../win/includeWindows.h"

#include "d3d9.h"

/*
#ifndef D3D9_NO_D3DI
#define DIRECTINPUT_VERSION  0x0800
#include "dinput.h"
#pragma comment(lib, DX9_LIBNAME_dinput8)
#endif
*/

#define MDXExpandVect(v) (v).x, (v).y, (v).z
#define MDXVectFromPtr(v, ptr) (v).x = (ptr)[0]; (v).y = (ptr)[1]; (v).z = (ptr)[2];
#define MDXPtrFromVect(v, ptr) (ptr)[0] = (v).x; (ptr)[1] = (v).y; (ptr)[2] = (v).z;
#define MDXZeroVect(v) (v).x = 0.0f; (v).y = 0.0f; (v).z = 0.0f;
#define MDXZeroQuat(q) MDXZeroVect(q); (q).w = 0.0f; 


	
#endif