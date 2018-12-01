#ifndef _WEInertiaTensor_h
#define _WEInertiaTensor_h

#include "WEPhysTypes.h"
#include "../WEAssert.h"

namespace WE {


	enum InertiaTensorType {
		IT_Simple = 0, IT_Matrix
	};

	struct InertiaTensorSimple {
		
		//Ref D21

		//ct.
		PhysScalar IBody;
		PhysScalar IBodyInv;

		//time based
		//PhysScalar IInv; same as IBodyInv

		inline void computeIInv(const Matrix33& orient) {
		}

		inline void computeW(const Vector3& angMom, Vector3& w) {

			angMom.mul(IBodyInv, w);
		}

		inline void computeMom(const Vector3& w, Vector3& angMom) {

			w.mul(IBody, angMom);
		}

		inline void invMulVector(const Vector3& vect, Vector3& IInvMulVec) {

			vect.mul(IBodyInv, IInvMulVec);
		}

		inline void fillInvMatrix(PhysBigMatrix& matrix, PhysBigMatrixOffset& offset) {

			offset.el(0, 0, matrix) = IBodyInv;
			offset.el(1, 1, matrix) = IBodyInv;
			offset.el(2, 2, matrix) = IBodyInv;
		}

		inline void fillInvMatrix(Matrix33& identityMatrix) {

			identityMatrix.row[0].el[0] = IBodyInv;
			identityMatrix.row[1].el[1] = IBodyInv;
			identityMatrix.row[2].el[2] = IBodyInv;
		}

		inline const PhysScalar& invEl(int row, int col) {

			if (row == col) {

				return IBodyInv;
			}

			return PhysZero;
		}
	};

	struct InertiaTensorMatrix {

		//Ref D21

		//ct.
		Matrix33 IBody;
		Matrix33 IBodyInv;

		//time based
		Matrix33 IInv;

		inline void computeIInv(const Matrix33& orient) {

			Matrix33 temp;

			mul(orient, IBodyInv, temp);
			mul(temp, orient, IInv, false, true);
		}

		inline void computeW(const Vector3& angMom, Vector3& w) {
			
			IInv.transform(angMom, w);
		}

		inline void computeMom(const Vector3& w, Vector3& angMom) {

			IInv.transform(w, angMom, true);
		}

		inline void invMulVector(const Vector3& vect, Vector3& IInvMulVec) {

			IInv.transform(vect, IInvMulVec);
		}

		inline void fillInvMatrix(PhysBigMatrix& matrix, PhysBigMatrixOffset& offset) {

			offset.setRow3(0, 0, matrix, IInv.row[0].el);
			offset.setRow3(1, 0, matrix, IInv.row[1].el);
			offset.setRow3(2, 0, matrix, IInv.row[2].el);
		}

		inline const PhysScalar& invEl(int row, int col) {

			return IInv.row[row].el[col];
		}
	};

	struct InertiaTensor {

		InertiaTensorType type;
		union {
			struct {
				InertiaTensorSimple tensorSimple;
			};
			struct {
				InertiaTensorMatrix tensorMatrix;
			};
		};

		void computeIInv(const Matrix33& orient) {
			
			switch(type) {
				case IT_Simple:
					tensorSimple.computeIInv(orient);
					break;
				case IT_Matrix:
					tensorMatrix.computeIInv(orient);
					break;
			}
		}

		void computeW(const Vector3& angMom, Vector3& w) {
			
			switch(type) {
				case IT_Simple:
					tensorSimple.computeW(angMom, w);
					break;
				case IT_Matrix:
					tensorMatrix.computeW(angMom, w);
					break;
			}
		}

		void computeMom(const Vector3& w, Vector3& angMom) {

			switch(type) {
				case IT_Simple:
					tensorSimple.computeMom(w, angMom);
					break;
				case IT_Matrix:
					tensorMatrix.computeMom(w, angMom);
					break;
			}
		}

		void invMulVector(const Vector3& vect, Vector3& IInvMulVec) {

			switch(type) {
				case IT_Simple:
					tensorSimple.invMulVector(vect, IInvMulVec);
					break;
				case IT_Matrix:
					tensorMatrix.invMulVector(vect, IInvMulVec);
					break;
			}
		}

		const PhysScalar& invEl(int row, int col) {

			switch(type) {
				case IT_Simple:
					return tensorSimple.invEl(row, col);
					break;
				case IT_Matrix:
					return tensorMatrix.invEl(row, col);
					break;
			}

			assert(false);
			return PhysZero;
		}

		void fillInvMatrix(PhysBigMatrix& matrix, PhysBigMatrixOffset& offset) {

			switch(type) {
				case IT_Simple:
					tensorSimple.fillInvMatrix(matrix, offset);
					break;
				case IT_Matrix:
					tensorMatrix.fillInvMatrix(matrix, offset);
					break;
			}
		}

		const Matrix33& getInvMatrix(Matrix33& tempIdentityMatrix) {

			if (type == IT_Simple) {

				tensorSimple.fillInvMatrix(tempIdentityMatrix);
				return tempIdentityMatrix;
			}
			
			
			return tensorMatrix.IInv;
		}
		
	};

}

#endif