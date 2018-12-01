#include "WEMeshRenderTreeOptimizer.h"

#include "../WEMacros.h"

namespace WE {

enum rto_RSInfoType {

	RSIT_Normal = 0, RSIT_ExpandSingleMaterial, RSIT_RenderLevel
};

/*
struct rto_RSInfo {

	rto_RSInfoType type;

	MeshMaterialIndex level; //root level is level with MeshMaterialIndexNull

	CountedMaterialIndices materials; //the index of the render state in a material is the same as level
	MeshMaterialIndex RSIndexInMaterials;
	
};
*/

struct rto_RSLocator {

	MeshMaterialIndex matIndex;
	MeshRSIndex RSIndex;
};

typedef MeshRSIndex rto_RSLocatorIndex;
typedef WETL::CountedArray<rto_RSLocator, false, rto_RSLocatorIndex::Type> rto_RSLocators;


struct rto_RSSimilInfo : rto_RSLocator {

	MeshRSIndex simCount;
};

struct rto_MaterialInfo {

	MeshRSIndex processedLevelIndex;
};

typedef WETL::StaticArray<rto_MaterialInfo, false, MeshMaterialIndex::Type> rto_MaterialInfos;


struct rto_RSNode {

	MeshRSIndex treeLevel;
	rto_RSLocator renderStateLocator;
};

typedef IndexT<unsigned int> rto_NodeIndex;
typedef WETL::StaticArray<rto_RSNode, false, rto_NodeIndex::Type> rto_Nodes;


inline MeshMaterial& mat_el(MeshMaterials& materials, const MeshMaterialIndex::Type& index) {

	return dref<MeshMaterial>(materials.el[index]);
}

inline MeshMaterial& mat_pel(MeshMaterials& materials, const MeshMaterialIndex::Type& index) {

	return dref<MeshMaterial>(materials.el[index]);
}


void rto_zeroInfluences(MeshMatrixGroups& matrixGroups, MeshMaterials& materials) {

	for(MeshMaterialIndex m = 0; m < materials.size; m++) {

		mat_el(materials, m).mPart.primCount = 0;
	}
	
	for(MeshMatrixGroupIndex m = 0; m < matrixGroups.size; m++) {

		matrixGroups.el[m].mPart.primCount = 0;
	}
}

void rto_countInfluences(MeshFaces& FB, MeshMatrixGroups& matrixGroups, MeshMaterials& materials) {

	
	for(MeshFaceIndex f = 0; f < FB.size; f++) {

		if (FB.el[f].mMaterialIndex != MeshMaterialIndex::Null) {

			mat_el(materials, FB.el[f].mMaterialIndex).mPart.primCount++;
		}

		if (FB.el[f].mMatrixGroupIndex != MeshMatrixGroupIndex::Null) {

			matrixGroups.el[FB.el[f].mMatrixGroupIndex].mPart.primCount++;
		}
	}
}

MeshMaterialIndex::Type rto_material_countSimilNodes(const MeshRenderState& state, MeshMaterials& materials, rto_MaterialInfos& matInfos, rto_RSLocators* pLocators, bool resetLocatorCount) {

	if (pLocators && resetLocatorCount) {

		pLocators->count = 0;
	}

	MeshMaterialIndex simCount = 0;

	for(MeshMaterialIndex m = 0; m < materials.size; m++) {

		MeshMaterial& mat = mat_el(materials, m);
		MeshRSIndex stateCount = mat.getStateCount();


		for (MeshRSIndex i = (matInfos.el[m].processedLevelIndex + 1); i < stateCount; i++) {

			if (mat.getState(i)->compare(state) == RSC_Equal) {

				simCount++;

				if (pLocators) {

					rto_RSLocator& loc = pLocators->addOne();
					loc.matIndex = m;
					loc.RSIndex = i;
				}
			}
		}	
	}

	return simCount;
}

void rto_material_findMaxSimilNode(MeshMaterials& materials, rto_MaterialInfos& matInfos, MeshRSIndex maxPossibleSimilCount, rto_RSSimilInfo& info) {

	info.simCount = 0;

	if (maxPossibleSimilCount == 0) {

		return;
	}

	for(MeshMaterialIndex m = 0; m < materials.size; m++) {

		MeshMaterial& mat = mat_el(materials, m);
		MeshRSIndex stateCount = mat.getStateCount();
		MeshRSIndex simCount;

		for (MeshRSIndex i = (matInfos.el[m].processedLevelIndex + 1); i < stateCount; i++) {

			simCount = rto_material_countSimilNodes(dref<const MeshRenderState>(mat.getState(i)), materials, matInfos, NULL, true);

			if (simCount > info.simCount) {

				info.simCount = simCount;
				info.RSIndex = i;
				info.matIndex = m;

				if (simCount == maxPossibleSimilCount) {

					return;
				}
			}
		}
	}

}

void rto_material_bumpLocated(rto_MaterialInfos& matInfos, rto_RSLocators& locators) {

	for (rto_RSLocatorIndex i = 0; i < locators.count; i++) {

		matInfos.el[locators.el[i].matIndex].processedLevelIndex++;
	}
}

void rto_material_moveLocatedUp(MeshMaterials& materials, rto_MaterialInfos& matInfos, rto_RSLocators& locators) {

	MeshRSIndex targetIndex;

	for (rto_RSLocatorIndex i = 0; i < locators.count; i++) {

		targetIndex = matInfos.el[locators.el[i].matIndex].processedLevelIndex++;

		mat_el(materials, locators.el[i].matIndex).moveStateToIndex(locators.el[i].RSIndex, targetIndex);
	}
}

bool rto_material_reorderNextUnprocessedRenderState(MeshMaterials& materials, rto_MaterialInfos& matInfos, MeshRSIndex maxPossibleSimilCount, MeshRSIndex& foundSimCount, rto_RSLocators& tempLocators) {

	rto_RSSimilInfo similInfo;
	
	rto_material_findMaxSimilNode(materials, matInfos, maxPossibleSimilCount, similInfo);
	
	if (similInfo.simCount == 0) {

		return false;
	}


	rto_material_countSimilNodes(dref<const MeshRenderState>(mat_el(materials, similInfo.matIndex).getState(similInfo.RSIndex)), 
									materials, matInfos, &tempLocators, true);

	rto_material_moveLocatedUp(materials, matInfos, tempLocators);

	foundSimCount = similInfo.simCount;

	return true;
}

void rto_material_initMaterialInfos(MeshMaterials& materials, rto_MaterialInfos& matInfos) {

	matInfos.setSize(materials.size);
}


void rto_material_resetMaterialInfos(rto_MaterialInfos& matInfos) {

	for (MeshMaterialIndex m = 0; m < matInfos.size; m++) {

		matInfos.el[m].processedLevelIndex = MeshRSIndex::Null;
	}
}

void rto_material_optimizeRenderStateOrders(MeshMaterials& materials, rto_MaterialInfos& matInfos) {

	rto_material_resetMaterialInfos(matInfos);

	MeshRSIndex maxPossibleSimilCount;
	MeshRSIndex foundSimilCount;

	rto_RSLocators tempLocators;

	maxPossibleSimilCount = materials.size;

	while (rto_material_reorderNextUnprocessedRenderState(materials, matInfos, maxPossibleSimilCount, foundSimilCount, tempLocators)) {

		maxPossibleSimilCount = foundSimilCount;
	}

}

void rto_material_createRenderTree(MeshMaterials& materials, rto_MaterialInfos& matInfos) {

	//we expect render states have been ordered in an 'optimal'
	//way with RenderState indexes descending in order of similarity
	//as done by rto_material_optimizeRenderStateOrders

	rto_material_resetMaterialInfos(matInfos);

	
	MeshRSIndex treeLevel = 0;
	rto_RSLocators tempLocators;
	rto_RSLocatorIndex nodeCount = 0;

	//1st pass count nodes
	for(MeshMaterialIndex m = 0; m < materials.size; m++) {

		if ((matInfos.el[m].processedLevelIndex + 1 <= treeLevel)) {

			MeshMaterial& mat = mat_el(materials, m);

			if (mat.getStateCount() >= (treeLevel + 1)) {

				if (rto_material_countSimilNodes(*mat.getState(treeLevel), materials, matInfos, &tempLocators, true) != 0) {

					rto_material_bumpLocated(matInfos, tempLocators);
					nodeCount++;
				}
			}
		}
	}

	

	rto_Nodes tree;

	tree.setSize(nodeCount);

	nodeCount = 0;
	//2nd pass create nodes
	for(MeshMaterialIndex m = 0; m < materials.size; m++) {

		if ((matInfos.el[m].processedLevelIndex + 1 <= treeLevel)) {

			MeshMaterial& mat = mat_el(materials, m);

			if (mat.getStateCount() >= (treeLevel + 1)) {

				if (rto_material_countSimilNodes(*mat.getState(treeLevel), materials, matInfos, &tempLocators, true) != 0) {

					tree.el[nodeCount].treeLevel = treeLevel;
					tree.el[nodeCount].renderStateLocator = tempLocators.el[0]; //take RS from 1st material

					rto_material_bumpLocated(matInfos, tempLocators);
					nodeCount++;
				}
			}
		}
	}

}

int __cdecl rto_material_sortMaterialsByRSCount_qsort_compare_asc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((MeshMaterial*) e1)->getStateCount()) < (((MeshMaterial*) e2)->getStateCount()) ) {
		return -1;
	} else if ( (((MeshMaterial*) e1)->getStateCount()) > (((MeshMaterial*) e2)->getStateCount()) ) {
		return 1;
	}
	return 0;
}

int __cdecl rto_material_sortMaterialsByRSCount_qsort_compare_desc(const void* e1, const void* e2) {
	//< 0 if elem1 less than elem2
	if ( (((MeshMaterial*) e1)->getStateCount()) < (((MeshMaterial*) e2)->getStateCount()) ) {
		return 1;
	} else if ( (((MeshMaterial*) e1)->getStateCount()) > (((MeshMaterial*) e2)->getStateCount()) ) {
		return -1;
	}
	return 0;
}

void rto_material_sortMaterialsByRSCount(MeshMaterials& materials, bool asc = true) {

	if (asc) {

		qsort(materials.el, materials.size, sizeof(MeshMaterial), rto_material_sortMaterialsByRSCount_qsort_compare_asc);
	} else {

		qsort(materials.el, materials.size, sizeof(MeshMaterial), rto_material_sortMaterialsByRSCount_qsort_compare_desc);
	}
}

bool MeshRenderTreeOptimizer::createRenderTree(Renderer& renderer, 
							MeshGeometryBuffer& VB, MeshGeometryBuffer& IB, MeshFaces& FB,
							MeshMatrixGroups& matrixGroups, MeshMaterials& materials) {
	
	//we misuse mPart.primCount to count number of face influences
	//in the end it must contain a valid index buffer part 

	//rto_zeroInfluences(matrixGroups, materials);
	//rto_countInfluences(FB, matrixGroups, materials);

	if (materials.size) {

		
		rto_material_sortMaterialsByRSCount(materials, true);

		rto_MaterialInfos matInfos;
			
		rto_material_initMaterialInfos(materials, matInfos);
		rto_material_optimizeRenderStateOrders(materials, matInfos);
		rto_material_createRenderTree(materials, matInfos);
	}

	return false;
}

}