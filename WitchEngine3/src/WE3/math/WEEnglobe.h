#ifndef _WEEnglobe_h
#define _WEEnglobe_h

#include "WEVolume.h"

namespace WE {

	bool englobes(VolumeStruct& mainVolume, const VolumeStruct& testVolume);

	bool englobes(AAB& mainVolume, const VolumeStruct& testVolume);
	bool englobes(AAB& mainVolume, const AAB& testVolume);
	bool englobes(AAB& mainVolume, const OBB& testVolume);
	bool englobes(AAB& mainVolume, const TriangleEx1& testVolume);
	bool englobes(AAB& mainVolume, const Sphere& testVolume);
	bool englobes(AAB& mainVolume, const Point* points, const int& pointCount);

}

#endif