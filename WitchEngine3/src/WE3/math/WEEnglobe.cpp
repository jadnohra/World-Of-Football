#include "WEEnglobe.h"

#include "../WEAssert.h"

namespace WE {

bool englobes(AAB& mainVolume, const AAB& testVolume) {

	return mainVolume.contains(testVolume.min) && mainVolume.contains(testVolume.max);
}

bool englobes(AAB& mainVolume, const Point* points, const int& pointCount) {

	for (int i = 0; i < pointCount; ++i) {

		if (!mainVolume.contains(points[i])) {

			return false;
		}
	}

	return true;
}

bool englobes(AAB& mainVolume, const OBB& testVolume) {

	const int pointCount = 8;
	Point points[pointCount];

	testVolume.toPoints(points);
	return englobes(mainVolume, points, pointCount);
}

bool englobes(AAB& mainVolume, const TriangleEx1& testVolume) {

	return englobes(mainVolume, testVolume.vertices, 3);
}

bool englobes(AAB& mainVolume, const Sphere& testVolume) {

	AAB englobingBox;

	englobingBox.initFromSphere(testVolume.center, testVolume.radius);

	return englobes(mainVolume, englobingBox);
}

bool englobes(AAB& mainVolume, const VolumeStruct& testVolume) {

	switch (testVolume.type) {

		case V_AAB: return englobes(mainVolume, testVolume.ctToAAB()); break;
		case V_OBB: return englobes(mainVolume, testVolume.ctToOBB()); break;
		case V_Triangle: return englobes(mainVolume, testVolume.ctToTriangle()); break;
		case V_Sphere: return englobes(mainVolume, testVolume.ctToSphere()); break;

		default: assrt(false); break;
	}

	return false;
}

bool englobes(VolumeStruct& mainVolume, const VolumeStruct& testVolume) {

	switch (mainVolume.type) {

		case V_AAB: return englobes(mainVolume.toAAB(), testVolume); break;

		default: assrt(false); break;
	}

	return false;
}

}