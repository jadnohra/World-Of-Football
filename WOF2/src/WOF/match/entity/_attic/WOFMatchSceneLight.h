#ifndef _WOFMatchSceneLight_h
#define _WOFMatchSceneLight_h

#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/WETime.h"
#include "WE3/data/WEDataSourceChunk.h"
using namespace WE;

#include "../ui/WOFMatchUIEvtQueue.h"
#include "../scene/WOFMatchSceneNode.h"

namespace WOF {

	class MatchSceneLight : public MatchSceneNode {
	public:

		static inline MatchSceneLight* lightFromObject(Object* pObject) {
			return (MatchSceneLight*) pObject;
		}

		static inline MatchSceneLight* lightFromNode(MatchSceneNode* pNode) {
			return (MatchSceneLight*) pNode;
		}

	public:

		MatchSceneLight();

		bool _init_settings(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool _init_lookAt(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool _init_camera();

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene();
		
		bool handle(const MatchUIEvtQueue::Event& evt);
		void frameMove(const ShortTime& dt);
		void signalLightChanged();
		
		void track(MatchSceneNode* pNode);
		MatchSceneNode* getTrackedNode();

		void clean();

		void setMatrices(Renderer& renderer, bool cleanIfNeeded, RenderPassEnum pass);

	public:

		Vector3 mDir;
		bool mDirIsLookAt;

		RenderLight mRenderLight;
	};

}

#endif