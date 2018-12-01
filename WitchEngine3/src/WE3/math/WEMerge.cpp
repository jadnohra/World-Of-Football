#include "WEMerge.h"

#include "../WEAssert.h"

namespace WE {

void mergeInto(AAB& mainVolume, const AAB& newVolume) {

	mainVolume.add(newVolume);
}

void mergeInto(AAB& mainVolume, const Point* points, const int& pointCount) {

	for (int i = 0; i < pointCount; ++i) {

		mainVolume.add(points[i]);
	}
}

void mergeInto(AAB& mainVolume, const OBB& newVolume) {

	const int pointCount = 8;
	Point points[pointCount];

	newVolume.toPoints(points);
	mergeInto(mainVolume, points, pointCount);
}

void mergeInto(AAB& mainVolume, const TriangleEx1& newVolume) {

	mergeInto(mainVolume, newVolume.vertices, 3);
}

void mergeInto(AAB& mainVolume, const VolumeStruct& newVolume) {

	switch (newVolume.type) {

		case V_AAB: mergeInto(mainVolume, newVolume.ctToAAB()); break;
		case V_OBB: mergeInto(mainVolume, newVolume.ctToOBB()); break;
		case V_Triangle: mergeInto(mainVolume, newVolume.ctToTriangle()); break;

		default: assrt(false); break;
	}

}

void mergeInto(VolumeStruct& mainVolume, const VolumeStruct& newVolume) {

	switch (mainVolume.type) {

		case V_AAB: mergeInto(mainVolume.toAAB(), newVolume); break;

		default: assrt(false); break;
	}
}

}