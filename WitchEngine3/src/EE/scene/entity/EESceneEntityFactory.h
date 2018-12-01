#ifndef _EESceneEntityFactory_h
#define _EESceneEntityFactory_h

#include "../EEISceneNodeFactory.h"
#include "../EESceneNodeType.h"

#include "WE3/script/WEScriptVarTable.h"
using namespace WE;

namespace EE {

	

	class SceneEntityFactory : public ISceneNodeFactory {
	public:

		virtual ~SceneEntityFactory();

		virtual SceneNode* create(BufferString& tempStr, Scene& scene, DataChunk& chunk, ISceneGameNodeFactory* pGameFactory);

	protected:

		void setUnsetParams(BufferString& tempStr, ScriptVarTable* pVarTable, DataChunk& paramParentChunk, int& attrCount, bool set);
	};

}

#endif

