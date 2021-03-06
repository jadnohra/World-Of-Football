#ifndef _WEOBB_h
#define _WEOBB_h


#include "WEMatrix.h"
#include "WEAAB.h"
#include "WETriangle.h"

namespace WE {

	#define MOBBAxis(obb, axis) (obb.coordFrame.row[axis])
	#define MOBBCenter(obb) (obb.coordFrame.row[3])
	#define MOBBExtents(obb) (obb.extents)

	class OBB {
	public:

		/*
		Matrix3x3Base basis; 
		Vector position;
		*/
		Matrix4x3Base coordFrame; //3x3 basis and position
		Vector extents;

	public:

		OBB();
		OBB(const OBB& ref);

		void toPoints(Vector points[8]) const;
		float toDim(const float& extent, short axis) const;

		//returns the indices of the face points
		//as generated by toPoints
		//there are 6 faces to an OBB
		static void getFaceCW(const int& index, int face[4]);
		static void getFaceCCW(const int& index, int face[4]);

		inline const Vector3& getCenter() const { return coordFrame.row[3]; }
	};
	
	
}


#endif