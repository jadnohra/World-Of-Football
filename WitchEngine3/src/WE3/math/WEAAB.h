#ifndef _WEAAB_h
#define _WEAAB_h

#include "WEVector.h"

namespace WE {

	
	struct AAB3 {
	public:

		// Public data

		// Min and max values.  Pretty simple.
		Vector3	min;
		Vector3	max;

		AAB3() {};
		AAB3(bool _empty) {if (_empty) empty();};

		//static Axis 
		static const Vector3 axis[3];

		// Query for dimentions

		void    size(Vector3& result) const { max.subtract(min, result); }
		float	xSize() const { return max.x - min.x; }
		float	ySize() const { return max.y - min.y; }
		float	zSize() const { return max.z - min.z; }
		float	size(int d) const { return max[d] - min[d]; }
		void	center(Vector3& result) const { min.add(max, result); result.mul(0.5f); }
		
		void	centeredRadius(Vector3& center, Vector3& radius) const { 

			min.add(max, center); 
			center.mul(0.5f);

			max.subtract(min, radius);
			radius.mul(0.5f);
		}

		short getMinSizeIndex(float& minSize) const {

			short minIndex = 0;
			
			float dx = max.x - min.x;
			float dy = max.y - min.y;
			float dz = max.z - min.z;

			minSize = dx;

			if (dy < minSize) {

				minIndex = 1;
				minSize = dy;
			}

			if (dz < minSize) {

				minIndex = 2;
				minSize = dz;
			}

			return minIndex;
		}

		int getNeededPointCount(const Vector3& maxSize, int pointCount[3], 
							   int firstPointIndex[3], Vector3& splitSize) const;
		bool inline_advancePointIndex(const int pointCount[3], int pointIndex[3]) const;
		bool advancePointIndex(const int pointCount[3], int pointIndex[3]) const;
		void inline_toPoint(const Vector3& splitSize, const int pointIndex[3], Vector& point) const;
		void toPoint(const Vector3& splitSize, const int pointIndex[3], Vector& point) const;
		void toPoints(const Vector3& splitSize, const int pointCount[3], int firstPointIndex[3], Vector* points) const;



		int getNeededBoxCount(const Vector3& maxSize, int boxCount[3], 
							   int firstBoxIndex[3], Vector3& splitSize) const;
		bool inline_advanceBoxIndex(const int boxCount[3], int boxIndex[3]) const;
		bool advanceBoxIndex(const int boxCount[3], int boxIndex[3]) const;
		void inline_toBox(const Vector3& splitSize, const int boxIndex[3], AAB3& box) const;
		void toBox(const Vector3& splitSize, const int boxIndex[3], AAB3& box) const;
		void toBoxes(const Vector3& splitSize, const int boxCount[3], int firstBoxIndex[3], AAB3* boxes) const;

		void initFromSphere(const Vector3 &center, const float& radius);

		// Fetch one of the eight corner points.  See the
		// .cpp for numbering conventions

		Vector3	corner(int i) const;

		//WitchEngine MOD
		void getCorners(Vector3 corners[8]) const;
		void toPoints(Vector3 points[8]) const;

		// Box operations

		// "Empty" the box, by setting the values to really
		// large/small numbers

		void	empty();
		
		// Add a point to the box

		void	add(const Vector3 &p);

		void merge(const float* p);

		// Add an AABB to the box

		void	add(const AAB3 &box);

		// Transform the box and compute the new AABB

		//void	setToTransformedBox(const AABB3 &box, const Matrix4x3 &m);
		//void	setToTransformedBox(const AABB3 &box, const RotationMatrix &m, bool inverse = false);

		// Containment/intersection tests

		// Return true if the box is enmpty

		bool	isEmpty() const;

		// Return true if the box contains a point

		bool	contains(const Vector3 &p) const;
		bool	contains(const float* p) const;

		// Return the closest point on this box to another point

		Vector3	closestPointTo(const Vector3 &p) const;

		// Return true if we intersect a sphere

		bool	intersectsSphere(const Vector3 &center, float radius) const;

		// Parametric intersection with a ray.  Returns >1 if no intresection

		float	rayIntersect(const Vector3 &rayOrg, const Vector3 &rayDelta,
			Vector3 *returnNormal = 0) const;

		// Classify box as being on one side or the other of a plane

		int	classifyPlane(const Vector3 &n, float d) const;

		// Dynamic intersection with plane

		float	intersectPlane(const Vector3 &n, float planeD,
			const Vector3 &dir) const;
	};

	// Check if two AABBs intersect, and return true if so.  Optionally return
	// the AABB of their intersection if an intersection is detected

	bool	intersectAABs(const AAB3 &box1, const AAB3 &box2,
		AAB3 *boxIntersect = 0);

	// Return parametric point in time when a moving AABB collides
	// with a stationary AABB.  Returns > 1 if no intersection

	float	intersectMovingAAB(
		const AAB3 &stationaryBox,
		const AAB3 &movingBox,
		const Vector3 &d
		);

	typedef AAB3 AAB;

}

#endif 
