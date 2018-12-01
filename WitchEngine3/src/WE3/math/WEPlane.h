#ifndef _WEPlane_h
#define _WEPlane_h

#include "WEOBB.h"
#include "WEAAB.h"
#include "WETriangle.h"
#include "WESphere.h"
#include "../WETL/WETLDataTypes.h"

namespace WE {

	enum PlanePartitionResult {
		PP_Back = 0, PP_Front = 1, PP_BackAndFront = 2, PP_None
	};

	struct AAPlane {

		static float kPlaneThickness; 

		//Vector normal;
		short normalDim;
		short negative;
		Point position;

		AAPlane();
		void init(const Point& position, const Vector& normal, bool isNormalized);

		void toPoints(Point points[4], float size) const;

		PlanePartitionResult classify(const Point& point) const;
		PlanePartitionResult classify(const AAB& box) const;
		PlanePartitionResult classify(const OBB& box) const;
		PlanePartitionResult classify(const Triangle& tri) const;
		PlanePartitionResult classify(const Point* points, WETL::Index::Type count) const;
		PlanePartitionResult classify(const Sphere& sphere) const;
		PlanePartitionResult classify(const Point& point, float& classifDiff) const;
	};

	struct AAPlaneEx : AAPlane {

		Vector normal;

		void init(const Point& position, const Vector& normal, bool isNormalized) {

			AAPlane::init(position, normal, isNormalized);

			if (isNormalized) {

				this->normal = normal;

			} else {

				normal.normalize(this->normal);
			}
		}
	};

	struct Plane {

		static float kPlaneThickness; 

		Vector normal;
		Point position;

		//n.p = -d

		Vector t[2];

		Plane();
		void init(const Point& position, const Vector& normal, bool isNormalized);

		inline float calcD() const { return -normal.dot(position); }

		void toPlaneEquation(float& a, float& b, float& c, float& d) const; 
		void toPoints(Point points[4], float size) const;

		PlanePartitionResult classify(const Point& point, float& classifDiff) const;
		inline PlanePartitionResult classify(const Point& point) const { float classifDiff; return classify(point, classifDiff); }
	};

};


#endif