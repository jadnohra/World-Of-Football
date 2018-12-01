#include "WEGeometryAnchor.h"

namespace WE {

bool reanchorIsAnchoring(GeometryAnchorType* anchor) {

	for (short dim = 0; dim < 3; dim++) {

		if (anchor[dim] != GAT_None) {

			return true;
		}
	}

	return false;
}

void reanchorGetOffset(AAB& box, const GeometryAnchorType* anchor, float* anchoringOffset) {

	for (short dim = 0; dim < 3; dim++) {

		switch (anchor[dim]) {

			case GAT_Min:
				anchoringOffset[dim] = box.min[dim];
				break;
			case GAT_Max:
				anchoringOffset[dim] = box.max[dim];
				break;
			case GAT_Center:
				anchoringOffset[dim] = (box.min[dim] + box.max[dim]) * 0.5f;
				break;
			default:
				anchoringOffset[dim] = 0.0f;
				break;
		}
		
	}
}

void reanchor(float* anchoringOffset, float* pos) {

	pos[0] -= anchoringOffset[0];
	pos[1] -= anchoringOffset[1];
	pos[2] -= anchoringOffset[2];
}

void reanchor(float* anchoringOffset, float* src, float* dest) {

	dest[0] = src[0] - anchoringOffset[0];
	dest[1] = src[1] - anchoringOffset[1];
	dest[2] = src[2] - anchoringOffset[2];
}

void reanchor(float* anchoringOffset, AAB& box) {

	reanchor(anchoringOffset, box.min.el);
	reanchor(anchoringOffset, box.max.el);
}

}