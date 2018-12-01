#include "WEMeshLoader_Ogre.h"

#include "../helper/load/WECoordSysDataLoadHelper.h"
#include "../geometry/WEGeometryBufferLoader_v1.h"
#include "../skeleton/WESkeletonLoader_v1.h"
#include "../binding/ogre/WEOgre.h"
#include "WE_MeshLoader_v1_internal.h"

namespace WE {

bool mesh_load_submesh(BufferString& tempStr, DataSourcePool& dataSrcPool, DataSourceChunk partChunk, 
							MeshGeometryPart& part, MeshGeometryPartBindings& bind, MeshMaterialGroup* pMatGroup, CoordSys& ogreCoordSys) {

	dataSrcPool.resolvePossiblyRemoteChunk(partChunk, true, NULL);

	if (partChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;
	
	child.set(partChunk->getFirstChild(), false);

	while (child.isValid()) {

		if (child->equals(L"faces")) {

			if (part.createIB()) {

				part.getIB().mCoordSys.set(ogreCoordSys);

				if (GeometryBufferLoader_v1::load(part.getIB(), DataSourceChunk(partChunk, child), false) == false) {

					assrt(false);
					return false;
				}

			} 


		} else if (child->equals(L"geometry")) {

			if (part.createVB()) {

				part.getVB().mCoordSys.set(ogreCoordSys);

				if (GeometryBufferLoader_v1::load(part.getVB(), DataSourceChunk(partChunk, child), false) == false) {

					assrt(false);
					return false;
				}

			}
		}

		toNextSibling(child);
	}

	if (part.hasIB()) {

		MeshMaterialIndex::Type matIndex = MeshMaterialIndex::Null;

		if (partChunk->getAttribute(L"material", tempStr)) {

			if (pMatGroup != NULL) {

				matIndex = pMatGroup->findMaterialIndex(tempStr.c_tstr());

				if (!MeshMaterialIndex::isValid(matIndex)) {

					log(L"Fatal Error: Material not found [%s]", tempStr.c_tstr());
					assrt(false);
				}
			}
		}

		bind.mMaterialIndex = matIndex;

		if (part.createFB()) {

			part.getFB().batchSetMaterialIndex(matIndex);
		}
	}

		
	return true;
}

bool mesh_load_submeshes(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk geometryChunk, CoordSys& ogreCoordSys) {

	dataSrcPool.resolvePossiblyRemoteChunk(geometryChunk, true, NULL);

	if (geometryChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;

	int partCount = geometryChunk->countChildren(tempStr, NULL, L"type", L"submesh");

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

		if (child->equals(L"submesh")) {

			geom.createPart(idx, &pPart);

			if (mesh_load_submesh(tempStr, dataSrcPool, DataSourceChunk(geometryChunk, child), 
					*pPart, geom.getPartBindings(idx), pMatGroup, ogreCoordSys) == false) {

				assrt(false);
				return false;
			}
			idx++;
		}

		toNextSibling(child);
	}

	return true;
}

bool mesh_scan_load_material(BufferString& tempStr, DataSourcePool& dataSrcPool, DataSourceChunk matChunk, MeshMaterial& mat, RenderLoadContext& renderLoadContext) {

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

bool mesh_scan_physicalProxy(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk geometryChunk, RenderLoadContext& renderLoadContext) {

	dataSrcPool.resolvePossiblyRemoteChunk(geometryChunk, true, NULL);

	if (geometryChunk.isNull()) {

		return true;
	}

	SoftPtr<DataSource> dataSource = geometryChunk.pSource(); 
	SoftRef<DataChunk> testChunk = dataSrcPool.getRelativeRemoteChunk(geometryChunk.chunk(), true, dataSource, L"materials.xml/physicalProxy");
		
	if (!testChunk.isValid()) {

		return true;
	}

	DataSourceChunk proxyChunk(testChunk.dref());

	return mesh_loadPhysicalProxy(tempStr, mesh, dataSrcPool, proxyChunk, renderLoadContext);
}

bool mesh_scan_submeshes_materials(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk geometryChunk, RenderLoadContext& renderLoadContext) {

	dataSrcPool.resolvePossiblyRemoteChunk(geometryChunk, true, NULL);

	if (geometryChunk.isNull()) {

		return false;
	}

	SoftPtr<DataSource> dataSource = geometryChunk.pSource(); 
	SoftRef<DataChunk> testChunk = dataSrcPool.getRelativeRemoteChunk(geometryChunk.chunk(), true, dataSource, L"materials.xml/materials");
		
	if (!testChunk.isValid()) {

		log(LOG_WARN, L"Ogre Mesh Materials file not found");

		assrt(false);
		return false;
	}

	DataSourceChunk materialsChunk(testChunk.dref());

	int partCount = materialsChunk.chunk().countChildren(tempStr);

	if (partCount == 0) {

		return true;
	}
	
	mesh.createMaterialGroup();
	MeshMaterialGroup& matGroup = mesh.getMaterialGroup();

	matGroup.setMaterialCount(safeSizeCast<int, MeshMaterialIndex::Type>(partCount));

	
	MeshMaterialIndex idx;
	MeshMaterial* pMat;
	
	RefWrap<DataChunk> child;

	child.set(materialsChunk->getFirstChild(), false);

	idx = 0;
	while (child.isValid()) {

		if (child->getAttribute(L"type", tempStr)) {

			if (tempStr.equals(L"material")) {

				if (matGroup.createMaterial(idx, &pMat)) {

					if (mesh_scan_load_material(tempStr, dataSrcPool, DataSourceChunk(materialsChunk, child), *pMat, renderLoadContext) == false) {

						assrt(false);
						return false;
					}
					idx++;

				} else {

					assrt(false);
				}
			}
		}

		toNextSibling(child);
	}

	return true;
}


bool mesh_load_skeleton(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk& skelChunk, const CoordSys* pGeomCoordSys, RenderLoadContext& renderLoadContext, CoordSys& ogreCoordSys) {

	mesh.createSkeleton();
	Skeleton& skel = mesh.getSkeleton();

	skel.mCoordSys.set(ogreCoordSys);

	//We need to convert skeleton to mesh coordSys 
	//in order for addToBoneBoundingBoxes to work
	if (SkeletonLoader_v1::load(skel, dataSrcPool, skelChunk, pGeomCoordSys, &(renderLoadContext.convPool()), false) == false) {

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

bool mesh_load_skeletonlink(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk& skelChunk, const CoordSys* pGeomCoordSys, RenderLoadContext& renderLoadContext, CoordSys& ogreCoordSys) {

	if (skelChunk.chunk().getAttribute(L"name", tempStr)) {

		String relPath(tempStr);
		relPath.append(L".xml");

		SoftPtr<DataSource> dataSource = skelChunk.pSource(); 
		SoftRef<DataChunk> testChunk = dataSrcPool.getRelativeRemoteChunk(skelChunk.chunk(), true, dataSource, relPath.c_tstr());

		if (!testChunk.isValid()) {

			log(LOG_WARN, L"Skeleton Link not found");

			assrt(false);
			return false;
		}

		DataSourceChunk realSkelChunk(testChunk);

		return mesh_load_skeleton(tempStr, mesh, dataSrcPool, realSkelChunk, pGeomCoordSys, renderLoadContext, ogreCoordSys);
	}

	return true;
}

bool MeshLoader_Ogre::load(Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk meshChunk, RenderLoadContext& renderLoadContext, const CoordSys& geometryCoordSys) {

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

	CoordSys ogreCoordSys;
	Ogre::getCoordSys(ogreCoordSys);

	ogreCoordSys.setFloatUnitsPerMeter(100.0f);

	
	while (child.isValid()) {

		if (child->equals(L"submeshes")) {

			
			if (mesh_scan_submeshes_materials(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), renderLoadContext) == false) {

				assrt(false);
				return false;
			}
			

			if (mesh_load_submeshes(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), ogreCoordSys) == false) {

				assrt(false);
				return false;
			}

			mesh.getGeometry().unifyCoordSys(renderLoadContext.convPool(), &geometryCoordSys);

			if (mesh_scan_physicalProxy(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), renderLoadContext) == false) {

				assrt(false);
				return false;
			}

		} else if (child->equals(L"skeletonlink")) {

			if (mesh_load_skeletonlink(tempStr, mesh, dataSrcPool, DataSourceChunk(meshChunk, child), &geometryCoordSys, renderLoadContext, ogreCoordSys) == false) {

				assrt(false);
				return false;
			}
	
		}

		toNextSibling(child);
	}
	
	return true;
}

}
