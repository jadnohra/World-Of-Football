#ifndef _WEMerge_h
#define _WEMerge_h

#include "WEVolume.h"

namespace WE {

	//merges newVolume into main volume
	void mergeInto(VolumeStruct& mainVolume, const VolumeStruct& newVolume);

	void mergeInto(AAB& mainVolume, const VolumeStruct& newVolume);
	void mergeInto(AAB& mainVolume, const AAB& newVolume);
	void mergeInto(AAB& mainVolume, const OBB& newVolume);
	void mergeInto(AAB& mainVolume, const TriangleEx1& newVolume);
	void mergeInto(AAB& mainVolume, const Point* points, const int& pointCount);
}

#endif