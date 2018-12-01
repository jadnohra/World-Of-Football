#include "WESceneBaseLoader_v1.h"

#include "../data/WEDataSourcePool.h"
#include "../coordSys/WECoordSysConvPool.h"
#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../helper/load/WESceneDataLoadHelper.h"
#include "../helper/load/WEGeometryDataLoadHelper.h"
#include "../helper/load/WEVarTableHelper.h"
#include "../render/texture/WERenderTextureCreator.h"
#include "../mesh/WEMeshSource.h"

namespace WE {

void scene_doTransformMesh(BufferString& tempStr, DataChunk& meshChunk, Mesh& mesh, CoordSysConv* pFileToSceneConv) {

	//look at the reason in Skeleton.cpp at the Skeleton.transform function
	if (mesh.hasSkeleton() == false) {

		MeshGeometry& geom = mesh.getGeometry();

		bool apply;
		
		GeometryAnchorType anchor[3];
		Vector anchorOffset;
		apply = GeometryDataLoadHelper::extract(tempStr, meshChunk, anchor);

		if (apply && reanchorIsAnchoring(anchor)) {

			geom.anchorGetOffset(anchor, anchorOffset);
			geom.reanchor(anchorOffset.el);
		}

		GeometryVertexTransform transf;
		apply = GeometryDataLoadHelper::extract(tempStr, meshChunk, transf, pFileToSceneConv);

		if (apply) {
			
			geom.applyTransform(&transf);
		}
	}
}

bool scene_transformMesh(BufferString& tempStr, DataChunk& meshChunk, Mesh& mesh, CoordSysConv* pFileToSceneConv) {

	scene_doTransformMesh(tempStr, meshChunk, mesh, pFileToSceneConv);

	SoftRef<DataChunk> child = meshChunk.getFirstChild();

	while (child.isValid()) {

		scene_doTransformMesh(tempStr, child.dref(), mesh, pFileToSceneConv);

		toNextSibling(child);
	}

	return true;
}

bool SceneBaseLoader_v1::loadMesh(BufferString& tempStr, SceneMeshContainer& container, LoadContext& loadContext, DataChunk& meshChunk, const CoordSys& loadDestCoordSys, CoordSysConv* pFileToSceneConv, int& errCount) {

	String meshName;
	meshChunk.getValue(meshName);

	SoftRef<DataChunk> actualMeshChunk = loadContext.dataSourcePool().getRemoteChild(meshChunk, true);

	if (actualMeshChunk.isNull()) {

		++errCount;
		assrt(false);
		return false;
	}

	//dont pool meshes
	RefPoolObjectPoolInfo poolInfo(Pool_NonePoolable, 0);

	DataSourceChunk tempChunk(actualMeshChunk);

	int attrCount;
	ScriptVarTable* pVarTable = loadContext.varTablePtr();
	VarTableHelper::setUnsetParams(tempStr, pVarTable, meshChunk, attrCount, true);

	SoftRef<Mesh> actualMesh = loadContext.meshSource().getFromDataChunk(loadContext, tempChunk, loadDestCoordSys, &poolInfo);

	VarTableHelper::setUnsetParams(tempStr, pVarTable, meshChunk, attrCount, false);

	if (actualMesh.isNull()) {

		++errCount;
		return false;
	}

	scene_transformMesh(tempStr, meshChunk, actualMesh.dref(), pFileToSceneConv);
	

	if (!actualMesh->prepareForRendering(loadContext.render())) {

		log(L"Mesh [%s] cannot be rendered", meshName.c_tstr());

		++errCount;
		return false;
	}

	meshChunk.getValue(tempStr);
		
	return container.putMesh(tempStr.c_tstr(), actualMesh.ptr());
}


bool SceneBaseLoader_v1::loadMeshes(BufferString& tempStr, SceneMeshContainer& container, LoadContext& loadContext, DataChunk& _loopChunk, const CoordSys& loadDestCoordSys, CoordSysConv* pFileToSceneConv, int& errCount) {

	SoftRef<DataSource> pathResolver = _loopChunk.getSource();
	SoftRef<DataChunk> loopChunk =  loadContext.dataSourcePool().resolveRemoteChunk(_loopChunk, true, pathResolver);

	if (loopChunk.isValid()) {

		SoftRef<DataChunk> child = loopChunk->getFirstChild();

		while (child.isValid()) {

			loadMesh(tempStr, container, loadContext, child.dref(), loadDestCoordSys, pFileToSceneConv, errCount);

			toNextSibling(child);
		}

	} else {

		return false;
	}

	return true;
}

bool SceneBaseLoader_v1::loadSounds(BufferString& tempStr, AudioBufferContainer& container, LoadContext& loadContext, DataChunk& _loopChunk, CoordSysConv* pFileToSceneConv, int& errCount) {

	SoftRef<DataSource> pathResolver = _loopChunk.getSource();
	SoftRef<DataChunk> loopChunk =  loadContext.dataSourcePool().resolveRemoteChunk(_loopChunk, true, pathResolver);

	if (loopChunk.isValid()) {


		SoftRef<DataChunk> child = loopChunk->getFirstChild();
		BufferString path;

		while (child.isValid()) {

			if (child->getAttribute(L"remote", path)) {

				child->resolveFilePath(path);
				child->getValue(tempStr);

				if (container.loadBuffer(path.c_tstr(), tempStr.c_tstr()) == false) {

					log(LOG_ERROR, L"Failed To Load Sound [%s]", path.c_tstr());
					++errCount;
				} 

				SoftRef<AudioBuffer> buffer = container.getBuffer(tempStr.c_tstr()); 

				if (buffer.isValid()) {

					child->scanAttribute(tempStr, L"minPitch", L"%f", &buffer->defaultMinPitch);
					child->scanAttribute(tempStr, L"maxPitch", L"%f", &buffer->defaultMaxPitch);

					if (child->scanAttribute(tempStr, L"maxShockImpulse", L"%f", &buffer->maxShockImpulse)) {

						if(pFileToSceneConv)
							pFileToSceneConv->toTargetUnits(buffer->maxShockImpulse);

					}

					child->scanAttribute(tempStr, L"gainMultiplier", L"%f", &buffer->gainMultiplier);
					child->scanAttribute(tempStr, L"responseCurvature", L"%f", &buffer->responseCurvature);
				}
			}

			toNextSibling(child);
		}
	}

	return true;

}

void SceneBaseLoader_v1::loadDebug(BufferString& tempStr, DataChunk& dbgChunk, int& errCount) {

	if (dbgChunk.getAttribute(L"tex", tempStr)) {

		dbgChunk.resolveFilePath(tempStr);
		RenderTextureCreator::setDebugTexturePath(tempStr.c_tstr());
	}
}

bool SceneBaseLoader_v1::loadCoordSys(BufferString& tempStr, LoadContext& loadContext, 
										const CoordSys& loadDestCoordSys, DataChunk& sceneChunk, 
										CoordSys& fileSceneCoordSys, SoftRef<CoordSysConv>& fileToSceneConv, int& errCount) {

	SoftRef<DataChunk> child = sceneChunk.getFirstChild();


	if (CoordSysDataLoadHelper::extract(tempStr, &sceneChunk, fileSceneCoordSys, true) == false) {

		++errCount;
		return false;
	}

	//loadDestCoordSys.set(scene.mCoordSys); 

	if (loadDestCoordSys.isValid()) {

		if (loadDestCoordSys.hasUnitsPerMeter() == false) {

			++errCount;
			assrt(false);
			//warn(L"scene has no units per meter");
			//loadDestCoordSys.unitsPerMeter = fileSceneCoordSys.unitsPerMeter;
		} 

	} else {

		//loadDestCoordSys.set(fileSceneCoordSys);
		++errCount;
		assrt(false);
	}
	
	fileToSceneConv = loadContext.convPool().getConverterFor(fileSceneCoordSys, loadDestCoordSys, false);
		
	
	return true;
}


}