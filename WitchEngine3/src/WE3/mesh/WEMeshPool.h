#ifndef _WEMeshPool_h
#define _WEMeshPool_h

#include "WEMeshCreator.h"
#include "../pool/WERefObjectPool.h"

namespace WE {

	class MeshPool : public RefObjectPool {
	public:

		MeshPool();
		~MeshPool();

		Mesh* get(MeshLoadInfo& loadInfo);

	protected:

		virtual RefPoolObjectBase* create(RefPoolObjectLoadInfo& loadInfo);
	};

}

#endif