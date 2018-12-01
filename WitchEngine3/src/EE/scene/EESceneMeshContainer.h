#ifndef _EESceneMeshContainer_h
#define _EESceneMeshContainer_h

#include "WE3/mesh/WEMesh.h"
#include "WE3/WETL/WETLHashMap.h"
using namespace WE;

namespace EE {

	class Scene;

	class SceneMeshContainer {
	public:

		SceneMeshContainer(Scene& scene);
		~SceneMeshContainer();

		bool putMesh(const BufferString& sceneName, Mesh* pMesh);
		Mesh* getMesh(const BufferString& sceneName);

		bool removeMesh(const BufferString& sceneName);

		void releaseUnused();

		SceneMeshIndex::Type getObjectCount();

	protected:

		typedef StringHash MeshKey;
		typedef WETL::RefHashMap<MeshKey, Mesh*, SceneMeshIndex::Type, WETL::ResizeDouble, 16> MeshMap;

		SoftPtr<Scene> mScene;
		MeshMap mMeshMap;

		inline MeshKey meshKey(const BufferString& sceneName);
	};


}

#endif