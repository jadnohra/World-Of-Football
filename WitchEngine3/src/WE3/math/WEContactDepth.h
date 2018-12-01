#ifndef _WEContactDepth_h
#define _WEContactDepth_h

#include "WESpan.h"
#include "WEVolume.h"

namespace WE {

	const float defGuideAngleSinSpread = sinf(degToRad(45.0f));

	bool getContactDepth(const OBB& dynObj, const TriangleEx1& staticObj, Vector3& normal, float& depth, const int& filterOutAxis, bool considerTriEdges = true, const Vector3* pNormalGuide = NULL, float guideAngleSinSpread = defGuideAngleSinSpread);

	//optimized for the case that an intersection really exists
	//normal points to the direction dynObj should be moved in to resolve intersection and depth is positive
	bool getContactDepth(const OBB& dynObj, const TriangleEx1& staticObj, Vector3& normal, float* pDepth = NULL, bool considerTriEdges = true);
	bool getContactDepth(const AAB& dynObj, const TriangleEx1& staticObj, Vector3& normal, float* pDepth = NULL, bool considerTriEdges = true);
	bool getContactDepth(const AAB& dynObj, const AAB& staticObj, Vector3& normal, float* pDepth = NULL);
	bool getContactDepth(const AAB& dynObj, const OBB& staticObj, Vector3& minNormal, float* pDepth = NULL);

	//dynObj contains the position after the sweep
	bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, const AAB& staticObj, Vector3& minNormal, float* pDepth);
	bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, VolumeStruct& dynObj, VolumeStruct& staticObj, Vector3& normal, float* pDepth = NULL, bool useSimplifiedTest = false);
	bool getContactDepthSwept(const Vector3& dynMoveDir, const float& moveDist, const AAB& dynObj, VolumeStruct& staticObj, Vector3& normal, float* pDepth, bool useSimplifiedTest = false);
}

#endif