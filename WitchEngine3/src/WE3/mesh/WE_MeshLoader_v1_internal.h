#ifndef _WE_MeshLoader_v1_internal_h
#define _WE_MeshLoader_v1_internal_h

#include "WEMeshLoader_v1.h"

namespace WE {

	bool mesh_loadPhysicalProxy(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk proxyChunk, RenderLoadContext& renderLoadContext);

}

#endif