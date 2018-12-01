#ifndef _WEGeometryAnchor_h
#define _WEGeometryAnchor_h

#include "../math/WEAAB.h"

namespace WE {

	enum GeometryAnchorType {

		GAT_None = 0, GAT_Min, GAT_Center, GAT_Max
	};

	bool reanchorIsAnchoring(GeometryAnchorType* anchor);
	void reanchorGetOffset(AAB& box, const GeometryAnchorType* anchor, float* anchoringOffset);
	void reanchor(float* anchoringOffset, float* pos);
	void reanchor(float* anchoringOffset, float* src, float* dest);
	void reanchor(float* anchoringOffset, AAB& box);
}

#endif