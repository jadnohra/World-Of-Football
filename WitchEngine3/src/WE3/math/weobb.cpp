#include "WEOBB.h"
#include "WESpan.h"

#include "../WEAssert.h"

namespace WE {

	
OBB::OBB() {
}

OBB::OBB(const OBB& ref) {

	*this = ref;
}

#define MOBBTransPt(from, to) coordFrame.transformPoint(from, to);
#define MOBBTransDim(from, dim) coordFrame.transformPointDim(from, dim);

void OBB::toPoints(Vector points[8]) const {

	Point pt;

	short pointCount = 0;

	//0 -x,-y,-z
	pt.x = -extents.x; pt.y = -extents.y; pt.z = -extents.z;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//1 x,-y,-z
	pt.x = extents.x;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//2 x,y,-z
	pt.y = extents.y;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;
	
	//3 -x,y,-z
	pt.x = -extents.x;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//4 -x,-y,z
	pt.y = -extents.y; pt.z = extents.z;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//5 x,-y,z
	pt.x = extents.x;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//6 x,y,z
	pt.y = extents.y;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	//7 -x,y,z
	pt.x = -extents.x;
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

}

void OBB::getFaceCW(const int& index, int face[4]) {

	switch(index) {
		case 0:
			face[0] = 0;
			face[1] = 3;
			face[2] = 2;
			face[3] = 1;
			break;
		case 1:
			face[0] = 4;
			face[1] = 7;
			face[2] = 3;
			face[3] = 0;
			break;
		case 2:
			face[0] = 5;
			face[1] = 6;
			face[2] = 7;
			face[3] = 4;
			break;
		case 3:
			face[0] = 1;
			face[1] = 2;
			face[2] = 6;
			face[3] = 5;
			break;
		case 4:
			face[0] = 4;
			face[1] = 0;
			face[2] = 1;
			face[3] = 5;
			break;
		case 5:
			face[0] = 3;
			face[1] = 7;
			face[2] = 6;
			face[3] = 2;
			break;
		default:
			assrt(false);
	}
}

void OBB::getFaceCCW(const int& index, int face[4]) {

	switch(index) {
		case 0:
			face[0] = 1;
			face[1] = 2;
			face[2] = 3;
			face[3] = 0;
			break;
		case 1:
			face[0] = 0;
			face[1] = 3;
			face[2] = 7;
			face[3] = 4;
			break;
		case 2:
			face[0] = 4;
			face[1] = 7;
			face[2] = 6;
			face[3] = 5;
			break;
		case 3:
			face[0] = 5;
			face[1] = 6;
			face[2] = 2;
			face[3] = 1;
			break;
		case 4:
			face[0] = 5;
			face[1] = 1;
			face[2] = 0;
			face[3] = 4;
			break;
		case 5:
			face[0] = 2;
			face[1] = 6;
			face[2] = 7;
			face[3] = 3;
			break;
		default:
			assrt(false);
	}
}

float OBB::toDim(const float& extent, short dim) const {

	return MOBBTransDim(extent, dim);
}


/*
void OBB::toPoints(Vector points[4], short axis) const {

	Point pt;

	short pointCount = 0;
	short dim;

	pt.x = -extents[0]; pt.y = -extents[1]; pt.z = -extents[2];
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	dim = (axis + 1) % 3;
	pt[dim] = extents[dim];
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	dim = (axis + 2) % 3;
	pt[dim] = extents[dim];
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;

	dim = (axis + 1) % 3;
	pt[dim] = -extents[dim];
	MOBBTransPt(pt, points[pointCount]);
	pointCount++;
}
*/

/*
void OBB::nextPoint(Point& tempPoint, short& index, Point& res) const {

	switch (index) {
		case 0:
			tempPoint.x = -extents.x; tempPoint.y = -extents.y; tempPoint.z = -extents.z;
			MOBBTransPt(tempPoint, res);
			index++;
			break;
		case 1:
			pt.x = extents.x;
			MOBBTransPt(pt, points[pointCount]);
			pointCount++;
			break;
			
	}

}	
*/

}