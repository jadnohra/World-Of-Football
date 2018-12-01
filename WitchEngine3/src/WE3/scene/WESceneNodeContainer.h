#ifndef _WESceneNodeContainer_h
#define _WESceneNodeContainer_h

#include "../string/WEBufferString.h"
#include "../WETL/WETLHashMap.h"
#include "../WETL/WETLResize.h"

namespace WE {

	typedef StringHash SceneNodeContainerNodeKey;

	template<class NodeT, class IndexT, class MapClassT >
	class SceneNodeContainerT {
	public:

		bool putNode(const TCHAR* sceneName, NodeT* pNode){

			return mNodeMap.insert(nodeKey(sceneName), pNode);
		}

		NodeT* getNode(const BufferString& sceneName) {
	
			NodeT* pRet = NULL;

			mNodeMap.find(nodeKey(sceneName), pRet);
			return pRet;
		}

		NodeT* getNode(const TCHAR* sceneName) {
	
			NodeT* pRet = NULL;

			mNodeMap.find(nodeKey(sceneName), pRet);
			return pRet;
		}

		bool removeNode(const TCHAR* sceneName) {

			return mNodeMap.remove(nodeKey(sceneName));
		}

		//void releaseUnused();
		IndexT getNodeCount(){

			return mNodeMap.getCount();
		}

		void destroy() {

			mNodeMap.destroy();
		}

	public:

		typedef SceneNodeContainerNodeKey NodeKey;
		typedef MapClassT NodeMap;

		NodeMap mNodeMap;

		inline NodeKey nodeKey(const BufferString& sceneName) {

			return sceneName.hash();
		}

		inline NodeKey nodeKey(const TCHAR* sceneName) {

			return String::hash(sceneName);
		}
	};


	template<class NodeT, class IndexT, class ResizeT = WETL::ResizeDouble, long InitSizeT = 16>
	class SceneNodeContainer : 
		public SceneNodeContainerT<NodeT, IndexT, WETL::PtrHashMap<SceneNodeContainerNodeKey, NodeT*, IndexT, ResizeT, InitSizeT> > {
	};

	template<class NodeT, class IndexT, class ResizeT = WETL::ResizeDouble, long InitSizeT = 16>
	class SceneNodeContainerSoft : 
		public SceneNodeContainerT<NodeT, IndexT, WETL::HashMap<SceneNodeContainerNodeKey, NodeT*, IndexT, ResizeT, InitSizeT> > {
	};
}


#endif