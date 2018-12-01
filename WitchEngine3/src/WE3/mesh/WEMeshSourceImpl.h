#ifndef _WEMeshSourceImpl_h
#define _WEMeshSourceImpl_h

#include "WEMeshSource.h"
#include "WEMeshPool.h"

namespace WE {

	class MeshSourceImpl : public MeshSource {
	public:

		MeshSourceImpl(MeshPool* pPool = NULL);
		~MeshSourceImpl();

		bool setPool(MeshPool* pPool);
		MeshPool* getPool();

		virtual Mesh* get(MeshLoadInfo& loadInfo);

	protected:

		MeshPool* mpPool;
	};

}

#endif