#include "WEWorldPolygonalCollider.h"

namespace WE {

WorldPolygonalCollider::WorldPolygonalCollider() {
}

WorldMaterialID WorldPolygonalCollider::getMaterial(const ObjectBinding* pBinding) {

	if (mBindings.size == 1)
		return mBindings[0].materialID;


	for (Bindings::Index i = 0; i < mBindings.size; ++i) {

		if (&(mBindings[i]) == pBinding)
			return mBindings[i].materialID;
	}

	assrt(false);

	WorldMaterialID ID;

	WorldMaterialManager::invalidateMaterialID(ID);

	return ID;
}

void WorldPolygonalCollider::destroyNotBound() {

	mBindings.destroy();
	mBuffers->destroy();
}

bool WorldPolygonalCollider::isAddedToManager() {

	if (mBindings.size) {

		return (mBindings[0].isBoundData());
	}

	return false;
}

bool WorldPolygonalCollider::add(SpatialManagerBuildPhase& buildPhase) {

	assrt(!isAddedToManager());

	for (Bindings::Index i = 0; i < mBindings.size; i++) {

		if (!buildPhase.add(mBuffers->el[i], &(mBindings[i]), true)) {

			assrt(false);
			return false;
		}

		//ownership transferred to buildPhase
		mBuffers->el[i] = NULL;
	}

	mBuffers.destroy();

	return true;
}

void WorldPolygonalCollider::remove(SpatialManager& manager) {

	for (Bindings::Index i = 0; i < mBindings.size; i++) {

		manager.removeTris(&(mBindings[i]));
	}
}

void WorldPolygonalCollider::destroy(SpatialManager* pManager) {

	if (pManager) {

		remove(*pManager);

	} else {

		assrt(!isAddedToManager());
	}

	mBindings.destroy();
	mBuffers.destroy();
}

bool WorldPolygonalCollider::createFromMesh(Object* pOwner, Mesh& mesh, WorldMaterialManager& matManager, const Matrix4x3Base* pTransform, bool simplifiyIfEnabled, bool* pResultIsEmpty) {

	if (mBindings.size) {

		assrt(false);
		return false;
	}

	MeshPhysicalProxy::MappingType mapping = MeshPhysicalProxy::M_None;
	SoftPtr<MeshPhysicalProxy> proxy;

	if (mesh.hasPhysicalProxy()) {

		proxy = &mesh.getPhysicalProxy();
		mapping = proxy->getMappingType();
	} 

	if (pResultIsEmpty)
		*pResultIsEmpty = true;

	switch (mapping) {

		case MeshPhysicalProxy::M_Clone: {

			MeshGeometry& geom = mesh.getGeometry();
			MeshGeometryPartIndex count = geom.getPartCount();
			SoftPtr<MeshMaterialGroup> matGroup = mesh.getMaterialGroupPtr();

			if (count) {

				WE_MMemNew(mBuffers.ptrRef(), Buffers());
			}

			mBindings.setSize(count);
			mBuffers->setSize(count);

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				WE_MMemNew(mBuffers->el[i], Buffer());
				Binding& binding = mBindings.el[i];

				binding.bindObject(pOwner);
			}

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				MeshGeometryPart& part = geom.getPart(i);
				MeshGeometryPartBindings& bind = geom.getPartBindings(i);

				if (bind.mMaterialIndex.isValid() && part.hasVB()) {

					WorldMaterialID matID;

					if (matManager.getMaterialID(matGroup->getMaterial(bind.mMaterialIndex).getMaterialName(), matID)) {

						Binding& binding = mBindings.el[i];
						Buffer& buff = dref(mBuffers->el[i]);

						binding.materialID = matID;
						buff.append(part.getVB(), part.getIBPtr(), pTransform);

					} else {

						assrt(false);

						destroyNotBound();

						return false;
					}
				}
			}
		
		} break;

		case MeshPhysicalProxy::M_Merge: {

			MeshGeometry& geom = mesh.getGeometry();
			MeshGeometryPartIndex count = geom.getPartCount();
			SoftPtr<MeshMaterialGroup> matGroup = mesh.getMaterialGroupPtr();

			WE_MMemNew(mBuffers.ptrRef(), Buffers());

			mBindings.setSize(1);
			mBuffers->setSize(1);

			WE_MMemNew(mBuffers->el[0], Buffer());
			Binding& binding = mBindings.el[0];

			binding.bindObject(pOwner);

			Buffer& buff = dref(mBuffers->el[0]);

			if (!matManager.getMaterialID(proxy->getMaterial(), binding.materialID)) {

				assrt(false);

				destroyNotBound();

				return false;
			}

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				MeshGeometryPart& part = geom.getPart(i);
				MeshGeometryPartBindings& bind = geom.getPartBindings(i);

				if (bind.mMaterialIndex.isValid() && part.hasVB()) {

					buff.append(part.getVB(), part.getIBPtr(), pTransform);
				}
			}

		} break;

		case MeshPhysicalProxy::M_Complex: {

			MeshGeometry& geom = mesh.getGeometry();
			MeshGeometryPartIndex count = proxy->getPartCount();
			SoftPtr<MeshMaterialGroup> matGroup = mesh.getMaterialGroupPtr();

			if (count) {

				WE_MMemNew(mBuffers.ptrRef(), Buffers());
			}

			mBindings.setSize(count);
			mBuffers->setSize(count);

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				WE_MMemNew(mBuffers->el[i], Buffer());
				Binding& binding = mBindings.el[i];

				binding.bindObject(pOwner);
			}

			for (MeshGeometryPartIndex i = 0; i < count; i++) {

				MeshPhysicalProxy::Part proxyPart = dref(proxy->getPart(i));

				Binding& binding = mBindings.el[i];
				Buffer& buff = dref(mBuffers->el[i]);

				if (!matManager.getMaterialID(proxyPart.material, binding.materialID)) {

					assrt(false);

					destroyNotBound();
					
					return false;
				}

				MeshGeometryPartIndex geomCount = geom.getPartCount();

				for (MeshPhysicalProxy::Index j = 0; j < proxyPart.srcMaterials.count; j++) {

					MeshMaterialIndex::Type searchMatIndex = proxyPart.srcMaterials.el[j];

					for (MeshGeometryPartIndex k = 0; k < count; k++) {

						MeshGeometryPart& geomPart = geom.getPart(k);
						MeshGeometryPartBindings& bind = geom.getPartBindings(k);

						if (bind.mMaterialIndex == searchMatIndex) {

							buff.append(geomPart.getVB(), geomPart.getIBPtr(), pTransform);
						}
					}
				}
			}

		} break;

		default: {

		} break;
	}

	if (simplifiyIfEnabled) {

		const MeshPhysicalProxy::Simplification* pSimpl = NULL;

		if (proxy.isValid()) {

			pSimpl = proxy->getSimplification();
		} 

		if (pSimpl) {

			for (Buffers::Index i = 0; i < mBuffers->size; i++) {

				mBuffers->el[i]->simplify(pSimpl);
			}
		}
	}

	if (pResultIsEmpty)
		*pResultIsEmpty = (mBindings.size == 0);

	if (mBindings.size > 0) {

		mOwner = mBindings[0].owner;
	}


	return true;
}

void WorldPolygonalCollider::transform(const Matrix4x3Base& trans) {

	for (Buffers::Index i = 0; i < mBuffers->size; i++) {

		mBuffers->el[i]->transform(trans);
	}
}

void WorldPolygonalCollider::render(Renderer& renderer, const RenderColor* pColor) {

	if (mBuffers.isValid()) {

		for (Buffer::Index i = 0; i < mBuffers->size; i++) {

			mBuffers->el[i]->render(renderer, pColor);
		}
	}
}


}