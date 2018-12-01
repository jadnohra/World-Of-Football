#include "WE_MeshLoader_v1_internal.h"

namespace WE {


bool mesh_loadPhysicalProxy(BufferString& tempStr, Mesh& mesh, DataSourcePool& dataSrcPool, DataSourceChunk proxyChunk, RenderLoadContext& renderLoadContext) {

	dataSrcPool.resolvePossiblyRemoteChunk(proxyChunk, true, NULL);

	if (proxyChunk.isNull()) {

		return false;
	}

	DataSourceTranslationContext ctx;
	ctx.start(proxyChunk.pChunk(), true);

	mesh.createPhysicalProxy();
	MeshPhysicalProxy& proxy = mesh.getPhysicalProxy();

	if (proxyChunk->getAttribute(L"mapping", tempStr)) {

		if (tempStr.equals(L"clone")) {

			proxy.setMappingType(MeshPhysicalProxy::M_Clone);

			
		} else if (tempStr.equals(L"merge")) {

			proxy.setMappingType(MeshPhysicalProxy::M_Merge);

			if (proxyChunk->getAttribute(L"material", tempStr)) {

				proxy.setMaterial(tempStr.hash());

			} else {

				assrt(false);
				return false;
			}

			
		} else if (tempStr.equals(L"complex")) {

			proxy.setMappingType(MeshPhysicalProxy::M_Complex);

		} else {

			proxy.setMappingType(MeshPhysicalProxy::M_None);

		}
	}

	SoftRef<DataChunk> child;

	child = proxyChunk->getFirstChild();

	while (child.isValid()) {

		if (child->equals(L"part") && (proxy.getMappingType() == MeshPhysicalProxy::M_Complex)) {

			SoftPtr<MeshPhysicalProxy::Part> part = proxy.createPart();
			
			if (child->getAttribute(L"material", tempStr)) {

				part->material = tempStr.hash();

			} else {

				assrt(false);

				mesh.destroy(false, false, false, false, true);
				return false;
			}
			
			bool partProcessed = false;

			if (child->getAttribute(L"src", tempStr)) {

				if (tempStr.equals(L"all")) {

					MeshGeometry& geom = mesh.getGeometry();
					MeshGeometryPartIndex count = geom.getPartCount();
					
					for (MeshGeometryPartIndex i = 0; i < count; i++) {

						MeshGeometryPart& geomPart = geom.getPart(i);
						MeshGeometryPartBindings& bind = geom.getPartBindings(i);

						if (bind.mMaterialIndex.isValid() && geomPart.hasVB()) {

							part->srcMaterials.addOne(bind.mMaterialIndex);
						}
					}

					partProcessed = true;

				} else {

					assrt(false);
				}
			}

			if (!partProcessed) {

				SoftRef<DataChunk> srcMat = child->getFirstChild();
				SoftPtr<MeshMaterialGroup> matGroup = mesh.getMaterialGroupPtr();

				while (srcMat.isValid()) {

					srcMat->getValue(tempStr);

					MeshMaterialIndex::Type foundMat = matGroup->findMaterialIndex(tempStr.c_tstr());

					if (foundMat != MeshMaterialIndex::Null) {

						part->srcMaterials.addOne(foundMat);
					}

					toNextSibling(srcMat);
				}
			}

		} else if (child->equals(L"simplification")) {

			bool enabled = true;
			child->scanAttribute(tempStr, L"enabled", enabled);

			if (enabled) {

				MeshPhysicalProxy::Simplification& simpl = proxy.createSimplification();

				simpl.mode = MeshPhysicalProxy::Simplification::M_Planar;
				if (child->getAttribute(L"mode", tempStr)) {
					
				}

				simpl.loopMode = MeshPhysicalProxy::Simplification::LM_Iterate;
				if (child->getAttribute(L"loopMode", tempStr)) {

					if (tempStr.equals(L"closestToCenter")) {

						simpl.loopMode = MeshPhysicalProxy::Simplification::LM_ClosestToCenter;
					} 
				}

				simpl.planarAngle = degToRad(2.5f);
				
				if (child->scanAttribute(tempStr, L"planarAnlge", L"%f", &simpl.planarAngle)) {

					simpl.planarAngle = degToRad(simpl.planarAngle);
				}

				simpl.stepLimit = -1;
				child->scanAttribute(tempStr, L"stepLimit", L"%d", &simpl.stepLimit);

				simpl.considerAesthetics = true;
				child->scanAttribute(tempStr, L"considerAesthetics", simpl.considerAesthetics);

				simpl.useIslands = true;
				child->scanAttribute(tempStr, L"useIslands", simpl.useIslands);

			} else {

				proxy.destroySimplification();
			}
		}

		toNextSibling(child);
	}

	return true;
}


}