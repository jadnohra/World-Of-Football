#ifndef _WESceneMeshContainer_h
#define _WESceneMeshContainer_h

#include "../mesh/WEMeshSource.h"
#include "../WETL/WETLHashMap.h"

namespace WE {

	class SceneBase;

	class SceneMeshContainer {
	public:

		SceneMeshContainer();
		~SceneMeshContainer();

		bool putMesh(const TCHAR* sceneName, Mesh* pMesh);
		Mesh* getMesh(const TCHAR* sceneName); //ref bumped

		bool removeMesh(const TCHAR* sceneName);

		void releaseUnused();

		SceneMeshIndex::Type getObjectCount();

	protected:

		typedef StringHash MeshKey;
		typedef WETL::RefHashMap<MeshKey, Mesh*, SceneMeshIndex::Type, WETL::ResizeDouble, 16> MeshMap;

		MeshMap mMeshMap;

		inline MeshKey meshKey(const TCHAR* sceneName);
	};


}

#endif