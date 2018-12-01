#include "EESceneEntityFactory.h"


#include "../EEScene.h"
#include "EESceneEntityMesh.h"
//#include "EESceneEntitySkeletalMesh.h"


namespace EE {

SceneEntityFactory::~SceneEntityFactory() {
}

void SceneEntityFactory::setUnsetParams(BufferString& tempStr, ScriptVarTable* pVarTable, DataChunk& paramParentChunk, int& attrCount, bool set) {

	SoftRef<DataChunk> paramChunk = paramParentChunk.getChild(L"params");

	if (paramChunk.isValid()) {

		if (set) {
			
			attrCount = paramChunk->getAttributeCount();
		}

		if (attrCount != 0) {

			if (pVarTable == NULL) {

				assrt(false);
				return;
			}
		}

		int i;
		String* pVar;
		if (set) {

			for (i = 0; i < attrCount; i++) {

				if (paramChunk->getAttribute(i, &tempStr, NULL)) {
				
					pVar = pVarTable->createValue(tempStr.c_tstr());
					paramChunk->getAttribute(i, NULL, pVar);
				}
			}

		} else {

			for (i = 0; i < attrCount; i++) {

				if (paramChunk->getAttribute(i, &tempStr, NULL)) {
				
					pVarTable->unsetValue(tempStr.c_tstr());
				}
			}
		}
	}

}

SceneNode* SceneEntityFactory::create(BufferString& tempStr, Scene& scene, DataChunk& chunk, ISceneGameNodeFactory* pGameFactory) {

	SoftPtr<SceneNode> ret;
	SoftPtr<SceneNode> gameCreatedNode;

	CollMask collMask;
	collMask.setFull();

	int attrCount;
	ScriptVarTable* pVarTable = scene.mLoadContext->varTablePtr();

	bool coll = false;
	chunk.scanAttribute(tempStr, L"coll", coll);

	bool collNP = false;
	chunk.scanAttribute(tempStr, L"collNP", collNP);

	BufferString collMaterial;
	chunk.getAttribute(L"collMaterial", collMaterial);

	bool overrideMeshMaterials = false;
	chunk.scanAttribute(tempStr, L"overrideMeshMaterials", overrideMeshMaterials);

	ObjectType baseType;
	ObjectType gameType;

	if (pGameFactory && pGameFactory->getGameType(tempStr, scene, chunk, baseType, gameType, collMask)) {

		gameCreatedNode = pGameFactory->create(tempStr, scene, chunk, baseType, gameType);

		if (gameCreatedNode.isNull()) {

			return NULL;
		}

	} else {

		chunk.getAttribute(L"type", tempStr);

		if (tempStr.equals(L"mesh")) {

			baseType = SNT_EntMesh;
			gameType = SNT_EntMesh;

		} else {

			return NULL;
		}
	}

	switch (baseType) {

		case SNT_EntMesh:
		{
			if (chunk.scanAttributeHash(tempStr, L"source")) {

				SoftRef<Mesh> source = scene.mMeshes->getMesh(tempStr.c_tstr());

				if (source.isValid()) {

					SceneEntityMesh* pEnt;

					if (gameCreatedNode.isValid()) {

						pEnt = SceneEntityMesh::fromNode(gameCreatedNode.ptr());

					} else {

						MMemNew(pEnt, SceneEntityMesh());
					}

					if (pEnt) {

						chunk.getValue(tempStr);
						pEnt->nodeInit(scene, gameType, scene.genNodeId(), tempStr.c_tstr());

						setUnsetParams(tempStr, pVarTable, chunk, attrCount, true);
						pEnt->entInit(scene, source.dref(), coll, &collMask, collNP, &collMaterial, overrideMeshMaterials);
						setUnsetParams(tempStr, pVarTable, chunk, attrCount, false);

						if (gameCreatedNode.isValid()) {

							if (pGameFactory->init(tempStr, scene, chunk, gameCreatedNode.ptr()) == false) {

								pGameFactory->destroy(tempStr, scene, chunk, gameCreatedNode.ptr());
								pEnt = NULL;
							}
						}

						ret = pEnt;
					}

				}
			}
		}
		break;
	}


	return ret.ptr();
}

}