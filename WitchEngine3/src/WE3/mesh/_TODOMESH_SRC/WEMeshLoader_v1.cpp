#include "WEMeshLoader_v1.h"

#include "../geometry/WEGeometryBufferLoader_v1.h"

namespace WE {


bool mesh_geometry_loadPart(DataSourcePool& dataFactory, DataSourceChunk partChunk, MeshGeometryPart& part) {

	dataFactory.resolvePossiblyRemoteChunk(partChunk, true, NULL);

	if (partChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;
	
	child.set(partChunk->getFirstChild(), false);

	while (child.isValid()) {

		if (child->equals(L"indexBuffer")) {

			MeshGeometryIndexBuffer* pIB = part.createIB();

			if (pIB) {

				if (GeometryBufferLoader_v1::load(*pIB, DataSourceChunk(partChunk, child), true) == false) {

					assert(false);
					return false;
				}

			} else {

				assert(false);
				return false;
			}


		} else if (child->equals(L"vertexBuffer")) {

			MeshGeometryVertexBuffer* pVB = part.createVB();

			if (pVB) {

				if (GeometryBufferLoader_v1::load(*pVB, DataSourceChunk(partChunk, child), true) == false) {

					assert(false);
					return false;
				}

			} else {

				assert(false);
				return false;
			}
		}

		toNextSibling(child);
	}
		
	return true;
}

bool mesh_loadGeometry(DataSourcePool& dataFactory, DataSourceChunk geometryChunk) {

	dataFactory.resolvePossiblyRemoteChunk(geometryChunk, true, NULL);

	if (geometryChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;

	int partCount = geometryChunk->countChildren(L"part");

	if (partCount == 0) {

		return true;
	}

	StaticMeshGeometryPartArray parts;
	parts.setSize(safeSizeCast<int, MeshGeometryPartIndex::Type>(partCount));

	MeshGeometryPartIndex idx;
	MeshGeometryPart* pPart;
	
	child.set(geometryChunk->getFirstChild(), false);

	idx = 0;
	while (child.isValid()) {

		if (child->equals(L"part")) {

			MMemNew(pPart, MeshGeometryPart());
			parts.el[idx] = pPart;

			if (mesh_geometry_loadPart(dataFactory, DataSourceChunk(geometryChunk, child), *pPart) == false) {

				assert(false);
				return false;
			}
			idx++;
		}

		toNextSibling(child);
	}


	return true;

}

bool MeshLoader_v1::load(Mesh& mesh, DataSourcePool& dataFactory, DataSourceChunk meshChunk) {

	dataFactory.resolvePossiblyRemoteChunk(meshChunk, true, NULL);

	if (meshChunk.isNull()) {

		return false;
	}

	RefWrap<DataChunk> child;

	child.set(meshChunk->getFirstChild(), false);

	while (child.isValid()) {

		if (child->equals(L"geometry")) {

			if (mesh_loadGeometry(dataFactory, DataSourceChunk(meshChunk, child)) == false) {

				assert(false);
				return false;
			}
		}

		toNextSibling(child);
	}

	return true;
}

}