#ifndef _WEPhysRigidBody_h
#define _WEPhysRigidBody_h

#include "WEPhysTypes.h"
#include "WEInertiaTensor.h"
#include "WEPhysForce.h"

namespace WE {

	struct PhysRigidBody {

		PhysIndex index;

		bool isFixed;

		//Constants
		PhysScalar mass;
		PhysScalar massInv;
		InertiaTensor ITensor;

		//Time Based
		Vector3 pos; //x
		Quaternion orient; //q
		Vector3 linMom; //P
		Vector3 angMom; //L

		//Auxiliaries (Time Based)
		//Matrix Iinv; //included inside ITensor
		Matrix33 orientMat;//R
		Matrix33 qDot; //dR = w * R
		Vector3 v; //v
		Vector3 w; //w, rads per sec.
		
		//Computed
		Vector3 force;
		Vector3 torque;

		int tempTag;

		PhysForcePtrArray externalForcePtrs;

		//Save
		Vector3 savedPos; //x

		PhysRigidBody() {
		}

		~PhysRigidBody() {
		}


		void savePos(Point& newPos) {

			savedPos = pos;
			pos = newPos;
		}

		void loadSavedPos() {

			pos = savedPos;
		}

		void init(bool isFixed) {

			memset(this, 0, sizeof(PhysRigidBody));

			this->isFixed = isFixed;
			index = -1;

			if (isFixed) {

				mass = PhysInf;
				massInv = PhysZero;

				ITensor.type = IT_Simple;
				ITensor.tensorSimple.IBody = PhysInf;
				ITensor.tensorSimple.IBodyInv = PhysZero;
			}

		}

		void getBodyPointVector(Vector3& bodyPos, Vector3& bodyPoint, Vector3& bodyPointVector) {

			if (isFixed) {

				bodyPointVector.zero();
			} else {

				bodyPoint.subtract(bodyPos, bodyPointVector);
			}
		}

		void getBodyPointVector(Vector3& bodyPoint, Vector3& bodyPointVector) {

			if (isFixed) {

				bodyPointVector.zero();
			} else {

				getBodyPointVector(pos, bodyPoint, bodyPointVector);
			}
		}

		

		void getBodyPointVelocity(const Vector3& bodyPointVector, Vector3& velocity) {

			if (isFixed) {

				velocity.zero();
			} else {
			
				w.cross(bodyPointVector, velocity);
				velocity += v;
			}
		}

		void getBodyPointVelocityT(const Vector3& bodyPointVector, const Vector3& normal, Vector3& vt) {

			if (isFixed) {

				vt.zero();
			} else {
			
				Vector3 velocity;

				w.cross(bodyPointVector, velocity);
				velocity += v;

				projectionRest(velocity, normal, vt);
			}
		}

		void getBodyPointVelocitiesT(const Vector3& bodyPointVector, const Vector3& normal, Vector3& vtFromV, Vector3& vtFromW) {

			if (isFixed) {

				vtFromV.zero();
				vtFromW.zero();

			} else {
			
				Vector3 velocity;

				w.cross(bodyPointVector, velocity);
				projectionRest(velocity, normal, vtFromW);

				projectionRest(v, normal, vtFromV);
			}
		}

		void getBodyPointVelocityFromW(const Vector3& bodyPointVector, Vector3& velocity) {

			if (isFixed) {

				velocity.zero();
			} else {
			
				w.cross(bodyPointVector, velocity);
			}
		}


		void doPositionUpdate(const PhysScalar& dt, const PhysScalar& halfDt, Vector3& dPos, Quaternion& tempQ);

		void doPositionUpdate(const PhysScalar& dt) {

			PhysScalar halfDt = 0.5f * dt;
			Vector3 dPos;
			Quaternion tempQ;

			doPositionUpdate(dt, halfDt, dPos, tempQ);
		}


		inline void setLinMomFromV() {
			
			//P = m v
			v.mul(mass, linMom);
		}

		inline void setVFromLinMom() {
			
			//P = m v
			linMom.mul(massInv, v);
		}

		inline void setAngMomFromW() {
			
			//w = IInv * L
			ITensor.computeMom(w, angMom);
		}

		inline void setWFromAngMom() {
			
			//w = IInv * L
			ITensor.computeW(angMom, w);
		}

	};

}

#endif