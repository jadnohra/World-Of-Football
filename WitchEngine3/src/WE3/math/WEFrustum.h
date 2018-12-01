#ifndef _WEFrustum_h
#define _WEFrustum_h

#include "WEVector.h"
#include "WEMatrix.h"
#include "WEPlane.h"

namespace WE {

	struct FrustumBase {
		
		enum Extent {

			None = -1, Near = 0, Far, Left, Up
		};

		enum Element {

			LVector = 0, UVector, DVector, Origin
		};

		Matrix4x3Base coordFrame; //left-row[0], up-row[1], direction-row[2], origin-row[3]
		float extents[4]; //near, far, left, up

		FrustumBase() : coordFrame(0.0f) { extents[0] = 0.0f; extents[1] = 0.0f; extents[2] = 0.0f; extents[3] = 0.0f;}

		inline const Vector3& origin() const { return coordFrame.row[3]; }
		inline const Vector3& dVector() const { return coordFrame.row[2]; }
		inline const Vector3& uVector() const { return coordFrame.row[1]; }
		inline const Vector3& lVector() const { return coordFrame.row[0]; }

		inline const float& n() const { return extents[Near]; }
		inline const float& f() const { return extents[Far]; }
		inline const float& l() const { return extents[Left]; }
		inline const float& u() const { return extents[Up]; }
		inline const float& dMin() const { return extents[Near]; }
		inline const float& dMax() const { return extents[Far]; }
		inline const float& lBound() const { return extents[Left]; }
		inline const float& uBound() const { return extents[Up]; }

		inline float dRatio() const { return dMax() / dMin(); }
		inline float m2lf() const { return -2.0f * l() * f(); }
		inline float m2uf() const { return -2.0f * u() * f(); }
	};

	struct Frustum : FrustumBase {

		enum PointEnum {

			P_None = -1, P_Near, P_Far, PointCount
		};

		enum ExtraPointEnum {

			EP_None = -1, 
				EP_NearLeft, EP_NearRight, EP_NearUp, EP_NearDown, 
				EP_NearLeftUp, EP_NearRightUp, EP_NearRightDown, EP_NearLeftDown, 
			ExtraPointCount
		};

		enum ExtentEnum {

			E_None = -1, E_FarLeft, E_FarUp, ExtentCount
		};

		enum NormalEnum {

			N_None = -1, N_Far, N_Left, N_Right, N_Top, N_Bottom, NormalCount
		};

		enum EdgeEnum {

			EdgeCount = 6
		};

		enum PlaneEnum {

			Pl_None = -1, Pl_Near, Pl_Far, Pl_Left, Pl_Right, Pl_Top, Pl_Bottom, PlaneCount
		};

		bool isValidPointsExtents;
		Vector3 points[PointCount];
		float extraExtents[ExtentCount];

		bool isValidExtraPoints;
		Vector3 extraPoints[ExtraPointCount];

		bool isValidNormalsEdges;
		Vector3 normals[NormalCount];
		Vector3 edges[EdgeCount];
		
		bool isValidPlanes;
		Plane planes[PlaneCount];


		Frustum();

		void computeSpan(const Vector& axis, float& spanMin, float& spanMax);

		void invalidate();

		void update();
		void updatePointsExtents();
		void updateExtraPoints();
		void updateNormalsEdges();
		void updatePlanes();
	};
}

#endif