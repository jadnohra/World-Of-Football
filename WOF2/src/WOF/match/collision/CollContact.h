#ifndef _WOFMatchCollContact_h
#define _WOFMatchCollContact_h

#include "WE3/math/dynamicIntersect/WEDynamicIntersect.h"
using namespace WE;

namespace WOF { namespace match {

	class CollContact {
	public:

		static bool contactOBB(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0, const int& ignoreAxis);
		static bool checkContactAndPenetration(const OBB& b0, const TriangleEx1& tri, Vector3& normal, float& depth, const bool& normalToB0);
		static bool getPenetration(const OBB& b0, const TriangleEx1& tri, Vector3& normal, float& depth, const bool& normalToB0, const int& ignoreAxis, const bool& considerTriEdges, const Vector3* pAllowedDirection = NULL);
		static bool getPenetrationDepth(const OBB& b0, const TriangleEx1& tri, const Vector3& inNormal, float& depth);
	};
} }

#endif