#ifndef _WEVolume_h
#define _WEVolume_h

#include "WEAAB.h"
#include "WESphere.h"
#include "WEOBB.h"
#include "WEBox.h"
#include "WEPlane.h"
#include "WESpan.h"
#include "WETriangle.h"
#include "WEFrustum.h"
#include "../WEString.h"

namespace WE {

	enum VoumeType {
		V_AAB = 0, V_OBB, V_Box, V_Sphere, V_Triangle, V_TriangleNormals, V_Empty
	};

	struct VolumeStruct {

		VoumeType type;
		void* pVolume;

		bool isValid();
		bool isNull();

		AAB& toAAB();
		const OBB& toBoxOBB();
		OBB& toOBB();
		Box& toBox();
		Sphere& toSphere();
		TriangleEx1& toTriangle();

		const AAB& ctToAAB() const;
		const OBB& ctToBoxOBB() const;
		const OBB& ctToOBB() const;
		const Box& ctToBox() const;
		const Sphere& ctToSphere() const;
		const TriangleEx1& ctToTriangle() const;

		PlanePartitionResult classify(AAPlane& plane) const;
		void computeSpan(const Vector& axis, float& spanMin, float& spanMax);
		void computeSpan(Axis axis, float& spanMin, float& spanMax);
	};

	struct VolumeRef : VolumeStruct {

		VolumeRef();
		
		VolumeRef(AAB& vol);
		VolumeRef(OBB& vol);
		VolumeRef(Box& vol);
		VolumeRef(Sphere& vol);
		VolumeRef(TriangleEx1& vol);

		void init();
		void init(AAB& vol);
		void init(OBB& vol);
		void init(Box& vol);
		void init(Sphere& vol);
		void init(TriangleEx1& vol);
		
	};

	struct Volume : VolumeStruct {
		
		Volume();
		Volume(bool empty);
		~Volume();

		void destroy();

		void init();
		void init(VoumeType type);
		void init(VolumeStruct& vol);
		void init(const AAB* pvol);
		void init(const OBB* pvol);
		void init(Box* pvol);
		void init(const Sphere* pvol);
		void init(const TriangleEx1* pvol);

		AAB& initAAB();
		OBB& initOBB();
		Box& initBox();
		Sphere& initSphere();
		TriangleEx1& initTriangle();
	};

	bool intersect(VolumeStruct& vol0, VolumeStruct& vol1);
	bool intersect(VolumeStruct& vol0, const AAB& vol1);
	bool intersect(VolumeStruct& vol0, const OBB& vol1);
	bool intersect(VolumeStruct& vol0, const Sphere& vol1);
	bool intersect(VolumeStruct& vol0, const TriangleEx1& vol1);
	bool intersect(VolumeStruct& vol0, Frustum& vol1);
	/*
	bool intersect(VolumeWrapper& vol0, const Vector& normal, 
					const Point& v0, const Point& v1, const Point& v2);
	
	*/

	void transform(const AAB& vol, const Matrix4x3Base& matrix, AAB& result);
	void transform(const Matrix4x3Base& matrix, AAB& vol);

	void transform(const AAB& vol, const Matrix4x3Base& matrix, Volume& result);
	void transform(const OBB& vol, const Matrix4x3Base& matrix, Volume& result);
	void transform(const Box& vol, const Matrix4x3Base& matrix, Volume& result);
	void transform(const Sphere& vol, const Matrix4x3Base& matrix, Volume& result, bool translateOnly);
	void transform(const Triangle& vol, const Matrix4x3Base& matrix, Volume& result);

	
	void translate(const AAB& vol, const Vector3& pos, AAB& result);
	inline void translate(const OBB& vol, const Vector3& pos, OBB& result) { MOBBCenter(vol).add(pos, MOBBCenter(result)); }
	inline void translate(Box& vol, const Vector3& pos, Box& result) { (vol).position(true).add(pos, result.position(false)); }
	inline void translate(const Sphere& vol, const Vector3& pos, Sphere& result) { vol.center.add(pos, result.center); }
	void translate(const TriangleEx1& vol, const Vector3& pos, TriangleEx1& result);


	void transform(const VolumeStruct& vol, const Matrix4x3Base& matrix, Volume& result);
	void translate(const VolumeStruct& vol, const Vector3& matrix, Volume& result);

	void transform(const Sphere& sphere, OBB& obb, bool initOBBAxis);
	void transform(const AAB& aab, OBB& obb, bool initOBBAxis);
}

#endif