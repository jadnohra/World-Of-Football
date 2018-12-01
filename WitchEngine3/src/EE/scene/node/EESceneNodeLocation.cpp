#include "EESceneNodeLocation.h"
#include "../EEScene.h"


namespace EE {

SceneNodeLocation::SceneNodeLocation() {
}

void SceneNodeLocation::nodeProcessSceneEvt(SceneBaseEvent& sceneBaseEvt, const bool& nodeWasDirty) {

	if (sceneBaseEvt.isGameEvt == false) {

		SceneEvent& sceneEvt = (SceneEvent&) sceneBaseEvt;

		if (sceneEvt.type == SE_Render 
				&& mNodeScene->mFlagExtraRenders
				&& mNodeScene->mFlagRenderLocationNodes) {

			Renderer& renderer = dref<Renderer>(sceneEvt.pRenderer);
			renderer.draw(mTransformWorld, mNodeScene->mFlagRenderLocationNodeSize);
		}
	}
}


}