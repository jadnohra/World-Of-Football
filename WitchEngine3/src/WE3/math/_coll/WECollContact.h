#ifndef _WECollContact_h
#define _WECollContact_h

#include "../WEVector.h"
#include "../WETriangle.h"
#include "../WEOBB.h"
#include "WECollContactType.h"

#include "../../WELog.h"

#include "sweptEllipsoid/SweptEllipsoid.h"

namespace WE {

	
	class CollContact {
	public:


		static bool sweptContactTri(const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, SweptContact& contact);
		static bool sweptContact(const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, OBB& staticVol, SweptContact& contact);

		class Vector2NoInit : public WE_3rdp_SweptEllipsoid::Vector2 {
		public:

			Vector2NoInit() : WE_3rdp_SweptEllipsoid::Vector2(true) {};
		};
		
		struct CollIteratorOBB {

			WE_3rdp_SweptEllipsoid::Plane inPlane;
			Vector2NoInit inVertices[4];

			WE_3rdp_SweptEllipsoid::Vector3 inBegin;
			WE_3rdp_SweptEllipsoid::Vector3 inDelta;
			
			Vector obbPoints[8];

			float sphereRadius;
			int face;

			CollIteratorOBB();
		};

		static bool collIterStart(CollIteratorOBB& iter, const float& sphereRadius, const Vector3& startPos, const Vector3& posDiff, OBB& staticOBB);
		static bool collIterNext_sweptContact(CollIteratorOBB& iter, SweptContact& sweptContact);
		static void collIterEnd(CollIteratorOBB& iter);

		static bool contactOBB(const OBB& b0, const OBB& b1, Vector3& normal, float& depth, const bool& normalToB0);
	};

}

#endif