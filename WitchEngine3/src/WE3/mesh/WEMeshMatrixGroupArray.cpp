#include "WEMeshMatrixGroupArray.h"

namespace WE {

SceneTransformIndex::Type MeshMatrixGroupArrayHelper::extractMaxMatrixIndex(MeshMatrixGroups& groups) {

	SceneTransformIndex::Type max = 0;

	for (MeshMatrixGroupIndex i = 0; i < groups.size; i++) {

		max = MMax(groups.el[i].extractMaxMatrixIndex(), max);
	}

	return max;
}


}