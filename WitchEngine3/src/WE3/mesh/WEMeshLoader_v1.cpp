#include "WEMeshLoader_v1.h"

#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../geometry/WEGeometryBufferLoader_v1.h"
#include "../skeleton/WESkeletonLoader_v1.h"

#include "WEMeshLoader_Ogre.h"
#include "WE_MeshLoader_v1_internal.h"

namespace WE {


bool mesh_geometry_loadPart(BufferString& tempStr, DataSourcePool& dataSrcPool, DataSourceChunk partChunk, 
							MeshGeometryPart& part, MeshGeometryPartBindings& bind, MeshMaterialGroup* pMatGroup) {

	dataSrcPool.resolvePossiblyRemoteChunk(partChunk, true, NULL);

	if (partChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;
	
	child.set(partChunk->getFirstChild(), false);

	while (child.isValid()) {

		if (child->equals(L"faceBuffer")) {

			if (part.createIB()) {

				if (GeometryBufferLoader_v1::load(part.getIB(), DataSourceChunk(partChunk, child), true) == false) {

					assrt(false);
					return false;
				}

			} 


		} else if (child->equals(L"vertexBuffer")) {

			if (part.createVB()) {

				if (GeometryBufferLoader_v1::load(part.getVB(), DataSourceChunk(partChunk, child), true) == false) {

					assrt(false);
					return false;
				}

			}
		}

		toNextSibling(child);
	}

	if (part.hasIB()) {

		MeshMaterialIndex::Type matIndex = MeshMaterialIndex::Null;

		if (partChunk->getAttribute(L"materialName", tempStr)) {

			if (pMatGroup != NULL) {

				matIndex = pMatGroup->findMaterialIndex(tempStr.c_tstr());
				assrt(MeshMaterialIndex::isValid(matIndex));
			}
		}

		bind.mMaterialIndex = matIndex;

		if (part.createFB()) {

			part.getFB().batchSetMaterialIndex(matIndex);
		}
	}

		
	return true;
}

bool mesh_loadGeometry(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk geometryChunk) {

	dataSrcPool.resolvePossiblyRemoteChunk(geometryChunk, true, NULL);

	if (geometryChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;

	int partCount = geometryChunk->countChildren(tempStr, NULL, L"type", L"geometryPart");

	if (partCount == 0) {

		return true;
	}

	mesh.createGeometry();
	MeshGeometry& geom = mesh.getGeometry();
	MeshMaterialGroup* pMatGroup = mesh.getMaterialGroupPtr();
	
	if (mesh.hasMaterialGroup()) {
	}

	geom.setPartCount(safeSizeCast<int, MeshGeometryPartIndex::Type>(partCount));

	MeshGeometryPartIndex idx;
	MeshGeometryPart* pPart;
		
	child.set(geometryChunk->getFirstChild(), false);

	idx = 0;
	while (child.isValid()) {

		if (child->getAttribute(L"type", tempStr)) {

			if (tempStr.equals(L"geometryPart")) {

				geom.createPart(idx, &pPart);

				if (mesh_geometry_loadPart(tempStr, dataSrcPool, DataSourceChunk(geometryChunk, child), 
						*pPart, geom.getPartBindings(idx), pMatGroup) == false) {

					assrt(false);
					return false;
				}
				idx++;
			}
		}

		toNextSibling(child);
	}

	return true;
}

bool mesh_matGroup_loadMaterial(BufferString& tempStr, DataSourcePool& dataSrcPool, DataSourceChunk matChunk, MeshMaterial& mat, RenderLoadContext& renderLoadContext) {

	dataSrcPool.resolvePossiblyRemoteChunk(matChunk, true, NULL);

	if (matChunk.isNull()) {

		return false;
	}

	if (matChunk->getAttribute(L"name", tempStr) == false) {

		return false;
	}

	mat.mNameHash = tempStr.hash();

	RenderMaterialSource& matSource = renderLoadContext.materialSource();
	mat.mRenderMaterial.set(matSource.getFromDataChunk(dataSrcPool, matChunk, renderLoadContext), false);
	assrt(mat.mRenderMaterial.isValid());

	return true;
}

bool mesh_loadMaterialGroup(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk materialsChunk, RenderLoadContext& renderLoadContext) {

	dataSrcPool.resolvePossiblyRemoteChunk(materialsChunk, true, NULL);

	if (materialsChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;

	int partCount = materialsChunk->countChildren(tempStr, NULL, L"type", L"material");

	if (partCount == 0) {

		return true;
	}

	mesh.createMaterialGroup();
	MeshMaterialGroup& matGroup = mesh.getMaterialGroup();

	matGroup.setMaterialCount(safeSizeCast<int, MeshMaterialIndex::Type>(partCount));

	
	MeshMaterialIndex idx;
	MeshMaterial* pMat;
		
	child.set(materialsChunk->getFirstChild(), false);

	idx = 0;
	while (child.isValid()) {

		if (child->getAttribute(L"type", tempStr)) {

			if (tempStr.equals(L"material")) {

				matGroup.createMaterial(idx, &pMat);

				if (mesh_matGroup_loadMaterial(tempStr, dataSrcPool, DataSourceChunk(materialsChunk, child), *pMat, renderLoadContext) == false) {

					assrt(false);
					return false;
				}
				idx++;
			}
		}

		toNextSibling(child);
	}
		
	return true;
}

bool mesh_loadSkeleton(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk& skelChunk, const CoordSys* pGeomCoordSys, RenderLoadContext& renderLoadContext) {

	mesh.createSkeleton();
	Skeleton& skel = mesh.getSkeleton();

	//We need to convert skeleton to mesh coordSys 
	//in order for addToBoneBoundingBoxes to work
	if (SkeletonLoader_v1::load(skel, dataSrcPool, skelChunk, pGeomCoordSys, &(renderLoadContext.convPool())) == false) {

		mesh.destroy(false, false, false, true, false);
		assrt(false);
		return false;
	}

	if (mesh.hasGeometry()) {

		if (pGeomCoordSys == false) {

			assrt(false);
			log(LOG_WARN, L"Mesh has no coordSys, skeleton bounding boxes might be wrong");
		}

		MeshGeometry& geom = mesh.getGeometry();

		for (MeshGeometryPartIndex::Type i = 0; i < geom.getPartCount(); i++) {

			MeshGeometryPart& part = geom.getPart(i);
			assert(part.hasVB() && part.hasIB());

			skel.addToBoneBoundingBoxes(part.getVB(), part.getIB());
		}
	}

	return true;
}


bool MeshLoader_v1::load(Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk meshChunk, RenderLoadContext& renderLoadContext, const CoordSys& geometryCoordSys) {

	if (meshChunk.isValid() && meshChunk.chunk().equals(L"mesh") && (meshChunk.chunk().getAttributeCount() == 0)) {

		SoftRef<DataChunk> childCunk = meshChunk.chunk().getChild(L"submeshes");

		if (childCunk.isValid()) {

			//looks like an Ogre mesh file

			return MeshLoader_Ogre::load(mesh, dataSrcPool, meshChunk, renderLoadContext, geometryCoordSys);
		}
	}

	assrt(geometryCoordSys.isValid());

	if (mesh.isLoaded() == true) {

		assrt(false);
		return false;
	}

	dataSrcPool.resolvePossiblyRemoteChunk(meshChunk, true, NULL);

	if (meshChunk.isNull()) {

		return false;
	}

	BufferString tempStr;
	SoftRef<DataChunk> child = meshChunk->getFirstChild();


	while (child.isValid()) {

		if (child->equals(L"materials")) {

			if (mesh_loadMaterialGroup(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), renderLoadContext) == false) {

				assrt(false);
				return false;
			}

		} else if (child->equals(L"geometry")) {

			if (mesh_loadGeometry(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child)) == false) {

				assrt(false);
				return false;
			}

			mesh.getGeometry().unifyCoordSys(renderLoadContext.convPool(), &geometryCoordSys);
			

		} else if (child->equals(L"skeleton")) {

			if (mesh_loadSkeleton(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), &geometryCoordSys, renderLoadContext) == false) {

				assrt(false);
				return false;
			}

		} else if (child->equals(L"physicalProxy")) {

			if (mesh_loadPhysicalProxy(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), renderLoadContext) == false) {

				assrt(false);
				return false;
			}
		}

		toNextSibling(child);
	}

	return true;
}


}