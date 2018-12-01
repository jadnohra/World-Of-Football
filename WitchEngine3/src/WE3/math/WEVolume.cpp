#include "WEVolume.h"

#include "WEIntersect.h"
#include "../WEAssert.h"
#include "../WEMem.h"
#include "float.h"

namespace WE {


#define MToPtrAAB(pVol) ((AAB*) pVol)
#define MToPtrOBB(pVol) ((OBB*) pVol)
#define MToPtrSphere(pVol) ((Sphere*) pVol)
#define MToPtrBox(pVol) ((Box*) pVol)
#define MToPtrTriangle(pVol) ((TriangleEx1*) pVol)

#define MToAAB(pVol) (*((AAB*) pVol))
#define MToOBB(pVol) (*((OBB*) pVol))
#define MToSphere(pVol) (*((Sphere*) pVol))
#define MToBox(pVol) (*((Box*) pVol))
#define MToTriangle(pVol) (*((TriangleEx1*) pVol))

Volume::Volume() {

	pVolume = NULL;
	type = V_Empty;
}


Volume::~Volume() {
	
	destroy();
}

void Volume::destroy() {
	
	if(pVolume) {

		switch (type) {
			case V_AAB:
				MMemCtDelete( MToPtrAAB(pVolume) );
				break;
			case V_OBB:
				MMemCtDelete( MToPtrOBB(pVolume) );
				break;
			case V_Box:
				MMemCtDelete( MToPtrBox(pVolume) );
				break;
			case V_Sphere:
				MMemCtDelete( MToPtrSphere(pVolume) );
				break;
			case V_Triangle:
				MMemCtDelete( MToPtrTriangle(pVolume) );
				break;
			default:
				assrt(false);
				break;
		}

		pVolume = NULL;
	}

	type = V_Empty;
}

void Volume::init() {
	
	if (type == V_Empty) {

		return;
	}

	destroy();

	type = V_Empty;
}

void Volume::init(VolumeStruct& vol) {

	if (vol.pVolume == NULL) {

		init();
		return;
	}

	/*
	if (type == vol.type) {

		return;
	}
	*/

	switch (vol.type) {

		case V_AAB:
			init(&vol.ctToAAB());
			break;
		case V_OBB:
			init(&vol.ctToOBB());
			break;
		case V_Box:
			init(&vol.toBox());
			break;
		case V_Sphere:
			init(&vol.ctToSphere());
			break;
		case V_Triangle:
			init(&vol.ctToTriangle());
			break;
		case V_Empty:
			init();
			break;
		default:
			assrt(false);
			break;
	}

}

void Volume::init(VoumeType _type) {

	if (type == _type) {

		return;
	}

	type = _type;

	switch (type) {

		case V_AAB:
			MMemNew(pVolume, AAB());
			break;
		case V_OBB:
			MMemNew(pVolume, OBB());
			break;
		case V_Box:
			MMemNew(pVolume, Box());
			break;
		case V_Sphere:
			MMemNew(pVolume, Sphere());
			break;
		case V_Triangle:
			MMemNew(pVolume, TriangleEx1());
			break;
		case V_Empty:
			init();
			break;
		default:
			assrt(false);
			break;
	}

}


AAB& Volume::initAAB() {

	if (type != V_AAB) {

		destroy();

		type = V_AAB;
		MMemNew(pVolume, AAB());
	}

	return MToAAB(pVolume);
}

OBB& Volume::initOBB() {

	if (type != V_OBB) {

		destroy();

		type = V_OBB;
		MMemNew(pVolume, OBB());
	}

	return MToOBB(pVolume);
}

Box& Volume::initBox() {

	if (type != V_Box) {

		destroy();

		type = V_Box;
		MMemNew(pVolume, Box());
	}

	return MToBox(pVolume);
}

Sphere& Volume::initSphere() {

	if (type != V_Sphere) {

		destroy();

		type = V_Sphere;
		MMemNew(pVolume, Sphere());
	}

	return MToSphere(pVolume);
}

TriangleEx1& Volume::initTriangle() {

	if (type != V_Triangle) {

		destroy();

		type = V_Triangle;
		MMemNew(pVolume, TriangleEx1());
	}

	return MToTriangle(pVolume);
}

void Volume::init(const AAB* pvol) {

	if (type != V_AAB) {

		destroy();

		type = V_AAB;
		MMemNew(pVolume, AAB());
	}

	if (pvol) {
		
		MToAAB(pVolume) = *pvol;
	}
	
}

void Volume::init(const OBB* pvol) {

	if (type != V_OBB) {

		destroy();

		type = V_OBB;
		MMemNew(pVolume, OBB());
	}

	if (pvol) {
		
		MToOBB(pVolume) = *pvol;
	}

	
}

void Volume::init(Box* pvol) {

	if (type != V_Box) {
		
		destroy();

		type = V_Box;
		MMemNew(pVolume, Box());
	}

	if (pvol) {

		MToBox(pVolume) = *pvol;
	}
}

void Volume::init(const Sphere* pvol) {

	if (type != V_Sphere) {

		destroy();

		type = V_Sphere;
		MMemNew(pVolume, Sphere());
	}

	if (pvol) {

		MToSphere(pVolume) = *pvol;
	}
	
}

void Volume::init(const TriangleEx1* pvol) {

	if (type != V_Triangle) {

		destroy();

		type = V_Triangle;
		MMemNew(pVolume, TriangleEx1());
	}

	if (pvol) {

		MToTriangle(pVolume) = *pvol;
	}
	
}


VolumeRef::VolumeRef() {

	type = V_Empty;
}


void VolumeRef::init() {

	type = V_Empty;
}

VolumeRef::VolumeRef(AAB& vol) {

	type = V_AAB;
	pVolume = &vol;
}

VolumeRef::VolumeRef(OBB& vol) {

	type = V_OBB;
	pVolume = &vol;
}

VolumeRef::VolumeRef(Box& vol) {

	type = V_Box;
	pVolume = &vol;
}

VolumeRef::VolumeRef(Sphere& vol) {

	type = V_Sphere;
	pVolume = &vol;
}

VolumeRef::VolumeRef(TriangleEx1& vol) {

	type = V_Triangle;
	pVolume = &vol;
}

void VolumeRef::init(AAB& vol) {

	type = V_AAB;
	pVolume = &vol;
}

void VolumeRef::init(OBB& vol) {

	type = V_OBB;
	pVolume = &vol;
}

void VolumeRef::init(Box& vol) {

	type = V_Box;
	pVolume = &vol;
}

void VolumeRef::init(Sphere& vol) {

	type = V_Sphere;
	pVolume = &vol;
}

void VolumeRef::init(TriangleEx1& vol) {

	type = V_Triangle;
	pVolume = &vol;
}

bool VolumeStruct::isValid() {

	return type != V_Empty;
}

bool VolumeStruct::isNull() {

	return type == V_Empty;
}

AAB& VolumeStruct::toAAB() {

	assrt(type == V_AAB);

	return MToAAB(pVolume);
}

const OBB& VolumeStruct::toBoxOBB() {

	assrt(type == V_Box);

	return MToBox(pVolume).toOBB();
}

OBB& VolumeStruct::toOBB() {

	assrt(type == V_OBB);

	return MToOBB(pVolume);
}

Box& VolumeStruct::toBox() {

	assrt(type == V_Box);

	return MToBox(pVolume);
}

Sphere& VolumeStruct::toSphere() {

	assrt(type == V_Sphere);

	return MToSphere(pVolume);
}

TriangleEx1& VolumeStruct::toTriangle() {

	assrt(type == V_Triangle);

	return MToTriangle(pVolume);
}


const AAB& VolumeStruct::ctToAAB() const {

	assrt(type == V_AAB);

	return MToAAB(pVolume);
}

const OBB& VolumeStruct::ctToBoxOBB() const {

	assrt(type == V_Box);

	return MToBox(pVolume).toOBB();
}

const OBB& VolumeStruct::ctToOBB() const {

	assrt(type == V_OBB);

	return MToOBB(pVolume);
}

const Box& VolumeStruct::ctToBox() const {

	assrt(type == V_Box);

	return MToBox(pVolume);
}

const Sphere& VolumeStruct::ctToSphere() const {

	assrt(type == V_Sphere);

	return MToSphere(pVolume);
}

const TriangleEx1& VolumeStruct::ctToTriangle() const {

	assrt(type == V_Triangle);

	return MToTriangle(pVolume);
}

PlanePartitionResult VolumeStruct::classify(AAPlane& plane) const {

	assrt(pVolume != NULL);

	switch (type) {
		case V_AAB:
			return plane.classify(MToAAB(pVolume));
		case V_OBB:
			return plane.classify(MToOBB(pVolume));	
		case V_Box:
			return plane.classify(MToBox(pVolume).toOBB());	
		case V_Sphere:
			return plane.classify(MToSphere(pVolume));
		case V_Triangle:
			return plane.classify(MToTriangle(pVolume));
		case V_Empty:
			return PP_None;
	}

	assrt(false);
	return PP_None;
}

void VolumeStruct::computeSpan(const Vector& axis, float& spanMin, float& spanMax) {

	assrt(pVolume != NULL);

	switch (type) {
		case V_AAB:
			WE::computeSpan(axis, MToAAB(pVolume), spanMin, spanMax);
			return;
		case V_OBB:
			WE::computeSpan(axis, MToOBB(pVolume), spanMin, spanMax);
			return;
		case V_Box:
			WE::computeSpan(axis, MToBox(pVolume).toOBB(), spanMin, spanMax);
			return;
		case V_Sphere:
			WE::computeSpan(axis, MToSphere(pVolume), spanMin, spanMax);
			return;
		case V_Triangle:
			WE::computeSpan(axis, MToTriangle(pVolume), spanMin, spanMax);
			return;
		case V_Empty:
			spanMin = -FLT_MAX;
			spanMax = -FLT_MAX;
			return;
	}

	assrt(false);
	spanMin = -FLT_MAX;
	spanMax = -FLT_MAX;
}

void VolumeStruct::computeSpan(Axis axis, float& spanMin, float& spanMax) {

	assrt(pVolume != NULL);

	switch (type) {
		case V_AAB:
			WE::computeSpan(axis, MToAAB(pVolume), spanMin, spanMax);
			return;
		case V_OBB:
			WE::computeSpan(axis, MToOBB(pVolume), spanMin, spanMax);
			return;
		case V_Box:
			WE::computeSpan(axis, MToBox(pVolume).toOBB(), spanMin, spanMax);
			return;
		case V_Sphere:
			WE::computeSpan(axis, MToSphere(pVolume), spanMin, spanMax);
			return;
		case V_Triangle:
			WE::computeSpan(axis, MToTriangle(pVolume), spanMin, spanMax);
			return;
		case V_Empty:
			spanMin = -FLT_MAX;
			spanMax = -FLT_MAX;
			return;
	}

	assrt(false);
	spanMin = -FLT_MAX;
	spanMax = -FLT_MAX;

}


bool intersect(const AAB& vol0, VolumeStruct& vol1) {

	switch (vol1.type) {
		case V_AAB:
			return intersect(vol0, MToAAB(vol1.pVolume));
		case V_OBB:
			return intersect(MToOBB(vol1.pVolume), vol0);
		case V_Box:
			return intersect(MToBox(vol1.pVolume).toOBB(), vol0);
		case V_Sphere:
			return intersect(vol0, MToSphere(vol1.pVolume));
		case V_Triangle:
			return intersect(vol0, MToTriangle(vol1.pVolume));
		case V_Empty:
			return false;
	}

	assrt(false);
	return (false);
}


bool intersect(const OBB& vol0, VolumeStruct& vol1) {

	switch (vol1.type) {
		case V_AAB:
			return intersect(vol0, MToAAB(vol1.pVolume));
		case V_OBB:
			return intersect(vol0, MToOBB(vol1.pVolume));
		case V_Box:
			return intersect(vol0, MToBox(vol1.pVolume).toOBB());
		case V_Sphere:
			return intersect(vol0, MToSphere(vol1.pVolume));
		case V_Triangle:
			return intersect(vol0, MToTriangle(vol1.pVolume));
		case V_Empty:
			return false;
	}

	assrt(false);
	return (false);
}

bool intersect(const Sphere& vol0, VolumeStruct& vol1) {

	switch (vol1.type) {
		case V_AAB:
			return intersect(MToAAB(vol1.pVolume), vol0);
		case V_OBB:
			return intersect(MToOBB(vol1.pVolume), vol0);
		case V_Box:
			return intersect(MToBox(vol1.pVolume).toOBB(), vol0);
		case V_Sphere:
			return intersect(vol0, MToSphere(vol1.pVolume));
		case V_Triangle:
				return intersect(vol0, MToTriangle(vol1.pVolume));
		case V_Empty:
			return false;
	}

	assrt(false);
	return (false);
}

bool intersect(const TriangleEx1& tri, VolumeStruct& vol1) {

	static Vector edge[3];
	

	switch (vol1.type) {
		case V_AAB:
			{
				tri.computeEdges(edge);
				return intersect(MToAAB(vol1.pVolume), tri);
			}
		case V_OBB:
			{
				tri.computeEdges(edge);
				return intersect(MToOBB(vol1.pVolume), tri);
			}
		case V_Box:
			{
				tri.computeEdges(edge);
				return intersect(MToBox(vol1.pVolume).toOBB(),  tri);
			}
		case V_Sphere:
			{
				return intersect(MToSphere(vol1.pVolume), tri, 0.1f, true);
			}
		case V_Triangle:
				return intersect(tri, MToTriangle(vol1.pVolume));
		case V_Empty:
			return false;	
	}

	assrt(false);
	return (false);
}

bool intersect(Frustum& vol0, VolumeStruct& vol1) {

	switch (vol1.type) {
		case V_OBB:
			{
				return intersect(vol0, MToOBB(vol1.pVolume));
			}
		case V_Empty:
			return false;
	}

	assrt(false);
	return false;
}

bool intersect(VolumeStruct& vol0, VolumeStruct& vol1) {

	assrt(vol0.pVolume != NULL);
	assrt(vol1.pVolume != NULL);

	switch (vol0.type) {
		case V_AAB:
			return intersect(MToAAB(vol0.pVolume), vol1);
		case V_OBB:
			return intersect(MToOBB(vol0.pVolume), vol1);
		case V_Box:
			return intersect(MToBox(vol0.pVolume).toOBB(), vol1);
		case V_Sphere:
			return intersect(MToSphere(vol0.pVolume), vol1);
		case V_Triangle:
			return intersect(MToTriangle(vol0.pVolume), vol1);
		case V_Empty:
			return false;
	}

	assrt(false);
	return (false);
}

bool intersect(VolumeStruct& vol0, const AAB& vol1) {

	return intersect(vol1, vol0);
}

bool intersect(VolumeStruct& vol0, const OBB& vol1) {

	return intersect(vol1, vol0);
}

bool intersect(VolumeStruct& vol0, const Sphere& vol1) {

	return intersect(vol1, vol0);
}

bool intersect(VolumeStruct& vol0, const TriangleEx1& vol1) {

	return intersect(vol1, vol0);
}

bool intersect(VolumeStruct& vol0, Frustum& vol1) {

	return intersect(vol1, vol0);
}


void transform(const AAB& vol, const Matrix4x3Base& matrix, Volume& result) {

	result.init(V_OBB);

	Box::transform(vol, matrix, result.toOBB());
}

void transform(const OBB& vol, const Matrix4x3Base& matrix, Volume& result) {

	assrt(false);
}

void transform(const Box& vol, const Matrix4x3Base& matrix, Volume& result) {

	assrt(false);
}

void transform(const Sphere& vol, const Matrix4x3Base& matrix, Volume& result, bool translateOnly) {

	if (translateOnly) {

		Sphere& resultSphere = result.toSphere();

		vol.center.add(matrix.row[3], resultSphere.center);
		resultSphere.radius = vol.radius;

	} else {

		assrt(false);
	}
}

void transform(const Triangle& vol, const Matrix4x3Base& matrix, Volume& result) {

	assrt(false);
}

void transform(const VolumeStruct& vol, const Matrix4x3Base& matrix, Volume& result) {

	switch (vol.type) {

		case V_AAB:
			transform(MToAAB(vol.pVolume), matrix, result);
			break;
		case V_OBB:
			transform(MToOBB(vol.pVolume), matrix, result);
			break;
		case V_Box:
			transform(MToBox(vol.pVolume), matrix, result);
			break;
		case V_Sphere:
			transform(MToSphere(vol.pVolume), matrix, result, true);
			break;
		case V_Triangle:
			transform(MToTriangle(vol.pVolume), matrix, result);
			break;
		case V_Empty:
			result.destroy();
			return;
		default:
			assrt(false);
			break;
	}
}

void translate(const AAB& vol, const Vector3& pos, AAB& result) {

	vol.min.add(pos, result.min);
	vol.max.add(pos, result.max);
}

void translate(const TriangleEx1& vol, const Vector3& pos, TriangleEx1& result) {

	vol.vertices[0].add(pos, result.vertices[0]);
	vol.vertices[2].add(pos, result.vertices[1]);
	vol.vertices[2].add(pos, result.vertices[1]);
}

void translate(const VolumeStruct& vol, const Vector3& pos, Volume& result) {

	switch (vol.type) {

		case V_AAB:
			translate(MToAAB(vol.pVolume), pos, result.initAAB());
			break;
		case V_OBB:
			translate(MToOBB(vol.pVolume), pos, result.initOBB());
			break;
		case V_Box:
			translate(MToBox(vol.pVolume), pos, result.initBox());
			break;
		case V_Sphere:
			translate(MToSphere(vol.pVolume), pos, result.initSphere());
			break;
		case V_Triangle:
			translate(MToTriangle(vol.pVolume), pos, result.initTriangle());
			break;
		case V_Empty:
			result.destroy();
			return;
		default:
			assrt(false);
			break;
	}
}

void transform(const AAB& vol, const Matrix4x3Base& matrix, AAB& result) {

	Vector3 min;
	Vector3 max;

	matrix.transformPoint(vol.min, min);
	matrix.transformPoint(vol.max, max);

	result.empty();
	result.add(min);
	result.add(max);
}

void transform(const Matrix4x3Base& matrix, AAB& vol) {

	Vector3 min;
	Vector3 max;

	matrix.transformPoint(vol.min, min);
	matrix.transformPoint(vol.max, max);

	vol.empty();
	vol.add(min);
	vol.add(max);
}

void transform(const Sphere& sphere, OBB& obb, bool initOBBAxis) {

	if (initOBBAxis) {

		MOBBAxis(obb, 0).set(1.0f, 0.0f, 0.0f);
		MOBBAxis(obb, 1).set(0.0f, 1.0f, 0.0f);
		MOBBAxis(obb, 2).set(0.0f, 0.0f, 1.0f);
	}

	MOBBCenter(obb).set(sphere.center);
	MOBBExtents(obb).set(sphere.radius);
}

void transform(const AAB& aab, OBB& obb, bool initOBBAxis) {

	if (initOBBAxis) {

		MOBBAxis(obb, 0).set(1.0f, 0.0f, 0.0f);
		MOBBAxis(obb, 1).set(0.0f, 1.0f, 0.0f);
		MOBBAxis(obb, 2).set(0.0f, 0.0f, 1.0f);
	}

	aab.centeredRadius(MOBBCenter(obb), MOBBExtents(obb));
}

/*
bool intersect(VolumeWrapper& vol0, const Vector& normal, 
			   const Point& v0, const Point& v1, const Point& v2) {

	assrt(vol0.pVolume);

	Vector axis[3];

	switch (vol0.type) {
		case V_AAB:

			v0.subtract(v1, axis[0]);
			v0.subtract(v2, axis[1]);
			v1.subtract(v2, axis[2]);

			return intersect(MToAAB(vol0.pVolume), normal, v0, v1, v2, axis[0], axis[1], axis[2]);
		case V_OBB:

			v0.subtract(v1, axis[0]);
			v0.subtract(v2, axis[1]);
			v1.subtract(v2, axis[2]);

			return intersect(MToOBB(vol0.pVolume), normal, v0, v1, v2, axis[0], axis[1], axis[2]);
		case V_Box:

			v0.subtract(v1, axis[0]);
			v0.subtract(v2, axis[1]);
			v1.subtract(v2, axis[2]);

			return intersect(MToBox(vol0.pVolume).toOBB(), normal, v0, v1, v2, axis[0], axis[1], axis[2]);
		case V_Sphere:
			return intersect(MToSphere(vol0.pVolume), normal, v0, v1, v2);
	}

	assrt(false);
	return (false);
}
*/

/*
void Volume::init(VolumeWrapper& wrapper) {

	switch (wrapper.type) {

		case V_AAB:
			type = V_AAB;
			aab = MToAAB(wrapper.pVolume);
			break;
		case V_OBB:
			type = V_OBB;
			obb = MToOBB(wrapper.pVolume);
			break;
		case V_Box:
			type = V_OBB;
			obb = MToBox(wrapper.pVolume).toOBB();
			break;
		case V_Sphere:
			type = V_Sphere;
			sphere = MToSphere(wrapper.pVolume);
			break;
		case V_Triangle:
			type = V_Triangle;
			tri = MToTriangle(wrapper.pVolume);
			break;

	}
}

void Volume::init() {

	type = V_Empty;
}

void Volume::init(AAB& vol) {

	type = V_AAB;
	aab = vol;
}

void Volume::init(OBB& vol) {

	type = V_OBB;
	obb = vol;
}

void Volume::init(Sphere& vol) {

	type = V_Sphere;
	sphere = vol;
}

void Volume::init(TriangleEx1& vol) {

	type = V_Triangle;
	tri = vol;
}

void Volume::init(TriangleEdgeNormals& vol) {

	type = V_TriangleNormals;
	triN = vol;
}

AAB& Volume::toAAB() {

	assrt(type == V_AAB);

	return aab;
}

const OBB& Volume::toOBB() {

	assrt(type == V_OBB);

	return obb;
}

Sphere& Volume::toSphere() {

	assrt(type == V_Sphere);

	return sphere;
}

TriangleEx1& Volume::toTriangle() {

	switch (type) {
		case V_Triangle:
			return tri;
		case V_TriangleNormals:
			return (TriangleEx1&) triN;
	}

	assrt(false);

	return tri;
}

TriangleEdgeNormals& Volume::toTriangleNormals() {

	assrt(type == V_TriangleNormals);

	return triN;
}
*/


}