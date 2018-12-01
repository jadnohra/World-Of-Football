#ifndef _WESphere_h
#define _WESphere_h

#include "WEVector.h"
#include "WEAAB.h"

namespace WE {

	bool sphereContains(const Point& center, float radius, const Point& pt);
	bool sphereSquaredContains(const Point& center, float radiusSquared, const Point& pt);
	void sphereClip(const Point& center, float radius, Vector3& pt);

	class Sphere {
	public:
		Vector center;
		float radius;

	public:

		Sphere();
		Sphere(const Sphere& ref) {*this = ref; }
		Sphere(float rad) {radius = rad; center.zero(); }
		Sphere(const CtVector3& center, float rad) { this->center = center; radius = rad; }

		void reset();
		void init(AAB& refAABox, bool sphereEnglobingBox); //false for box englobing sphere
		void add(const Sphere& mergedVolume);

		inline bool containts(const Point& pt) { return sphereContains(center, radius, pt); }
		inline void clip(Vector3& pt) { return sphereClip(center, radius, pt); } 
	};
}

#endif