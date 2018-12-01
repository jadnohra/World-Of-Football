#ifndef _WOFMatchSceneNodeFactory_h
#define _WOFMatchSceneNodeFactory_h

#include "WE3/data/WEDataSourceChunk.h"
#include "WE3/coordSys/WECoordSysConv.h"
using namespace WE;

namespace WOF { namespace match {

	class Match;
	class SceneNode;

	class SceneNodeFactory {
	public:

		virtual SceneNode* createNode(BufferString& tempStr, Match& match, DataChunk& chunk, CoordSysConv* pConv, bool& success) = 0;
		virtual void signalNodeAttached(BufferString& tempStr, Match& match, DataChunk& chunk, SceneNode* pNode, CoordSysConv* pConv) = 0;
		virtual SceneNode* getNode(const TCHAR* nodeName) = 0;
	};

} }

#endif