#include "WEMeshMaterialInstanceArray.h"


namespace WE {

	void cloneMaterialInstanceArray(StaticMeshMaterialInstanceArray& from, StaticMeshMaterialInstanceArray& to) {

		to.setSize(from.size);

		for (MeshMaterialIndex i = 0; i < from.size; i++) {

			if (from.el[i]) {

				to.el[i] = from.el[i]->createClone();

			} else {

				to.el[i] = NULL;
			}
		}
	}
}

