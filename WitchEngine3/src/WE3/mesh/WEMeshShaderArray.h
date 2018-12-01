#ifndef _WEMeshShaderArray_h
#define _WEMeshShaderArray_h

#include "WEMeshShader.h"

#include "../WETL/WETLArray.h"

namespace WE {

	typedef WETL::ArrayBase<MeshShader*, MeshShaderIndex::Type> MeshShaderArrayBase;
	
	typedef MeshShaderArrayBase MeshShaders;
}

#endif