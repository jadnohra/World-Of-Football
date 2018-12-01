#include "WEPhysCollisionLaw.h"
#include "../WEString.h"

namespace WE {


/**
    * Compute Collision Matrix.
    * This method computes the collision matrix, K, which is part
    * of the impulse-momemtum relation between two bodies in a
    * single point collision.
    *
    *    K = (1/Ma + 1/Mb) Identity  - ( star(ra) Ia^-1 star(ra)  +  star(rb) Ib^-1 star(rb))
    *
    * Where star() is the cross product matrix and Identity is a
    * diagonal matrix of ones.
    *
    * The computations have been optimized to exploit symmetry and zero pattern's as
    * much as possible. It is still possible to optimize further by exploiting
    * common sub-terms in the computations.
    *
    * @param m_a  Inverse mass of object A.
    * @param I_a  Inverse Intertia Tensor of object A.
    * @param r_a  The arm from center of mas of object A to the point of contact.
    * @param m_b  Inverse mass of object B.
    * @param I_b  Inverse Intertia Tensor of object B.
    * @param r_b  The arm from center of mas of object B to the point of contact.
    *
    * @return     The collision matrix K.
    */
void PhysCollisionLaw::computeCollisionMatrix(const PhysScalar& invMassA, InertiaTensor& tensorA, const Vector3& ra, 
												const PhysScalar& invMassB, InertiaTensor& tensorB, const Vector3& rb,
												Matrix33& K) {

	#define _NOROT_COLL_MATRIX

	PhysScalar K00, K11, K22;
	PhysScalar K01, K02, K12;

	#ifdef _NOROT_COLL_MATRIX
	K01 = 0.0f;
	K02 = 0.0f;
	K12 = 0.0f;
	#endif


	if (tensorA.type == IT_Simple) {

		PhysScalar invIA = tensorA.tensorSimple.IBodyInv;

		K00 = ra(2)*invIA*ra(2) + ra(1)*invIA*ra(1);
		K11 = ra(2)*invIA*ra(2) + ra(0)*invIA*ra(0);
		K22 = ra(1)*invIA*ra(1) + ra(0)*invIA*ra(0);
		#ifndef _NOROT_COLL_MATRIX
		K01 = - ra(1)*invIA*ra(0);
		K02 = ra(2)*invIA*ra(0);
		K12 = -ra(2)*invIA*ra(1);
		#endif
		

	} else {

		const Matrix33& invIA = tensorA.tensorMatrix.IInv;

		K00 = ra(2)*invIA(1, 1)*ra(2) - 2.0f*ra(2)*invIA(1, 2)*ra(1) + ra(1)*invIA(2, 2)*ra(1);
		K11 = ra(2)*invIA(0, 0)*ra(2) - 2.0f*ra(0)*invIA(0, 2)*ra(2) + ra(0)*invIA(2, 2)*ra(0);
		K22 = ra(1)*invIA(0, 0)*ra(1) - 2.0f*ra(0)*invIA(0, 1)*ra(1) + ra(0)*invIA(1, 1)*ra(0);
		#ifndef _NOROT_COLL_MATRIX
		K01 = -ra(2)*invIA(0, 1)*ra(2) + ra(1)*invIA(0, 2)*ra(2) + ra(2)*invIA(1, 2)*ra(0) - ra(1)*invIA(2, 2)*ra(0);
		K02 = ra(2)*invIA(0, 1)*ra(1) - ra(1)*invIA(0, 2)*ra(1) - ra(2)*invIA(1, 1)*ra(0) + ra(1)*invIA(1, 2)*ra(0);
		K12 = -ra(2)*invIA(0, 0)*ra(1) + ra(2)*invIA(0, 1)*ra(0) + ra(0)*invIA(0, 2)*ra(1) - ra(0)*invIA(1, 2)*ra(0);
		#endif
	}

	K(0, 0) = invMassA + K00;
	K(1, 1) = invMassA + K11;
	K(2, 2) = invMassA + K22;
	K(0, 1) = K01;
	K(0, 2) = K02;
	K(1, 0) = K01;
	K(1, 2) = K12;
	K(2, 0) = K02;
	K(2, 1) = K12;
	

	if (tensorB.type == IT_Simple) {

		PhysScalar invIB = tensorB.tensorSimple.IBodyInv;

		K00 = rb(2)*invIB*rb(2) + rb(1)*invIB*rb(1);
		K11 = rb(2)*invIB*rb(2) + rb(0)*invIB*rb(0);
		K22 = rb(1)*invIB*rb(1) + rb(0)*invIB*rb(0);
		#ifndef _NOROT_COLL_MATRIX
		K01 = - rb(1)*invIB*rb(0);
		K02 = rb(2)*invIB*rb(0);
		K12 = -rb(2)*invIB*rb(1);
		#endif
				

	} else {

		const Matrix33& invIB = tensorB.tensorMatrix.IInv;

		K00 = rb(2)*invIB(1, 1)*rb(2) - 2.0f*rb(2)*invIB(1, 2)*rb(1) + rb(1)*invIB(2, 2)*rb(1);
		K11 = rb(2)*invIB(0, 0)*rb(2) - 2.0f*rb(0)*invIB(0, 2)*rb(2) + rb(0)*invIB(2, 2)*rb(0);
		K22 = rb(1)*invIB(0, 0)*rb(1) - 2.0f*rb(0)*invIB(0, 1)*rb(1) + rb(0)*invIB(1, 1)*rb(0);
		#ifndef _NOROT_COLL_MATRIX
		K01 = -rb(2)*invIB(0, 1)*rb(2) + rb(1)*invIB(0, 2)*rb(2) + rb(2)*invIB(1, 2)*rb(0) - rb(1)*invIB(2, 2)*rb(0);
		K02 = rb(2)*invIB(0, 1)*rb(1) - rb(1)*invIB(0, 2)*rb(1) - rb(2)*invIB(1, 1)*rb(0) + rb(1)*invIB(1, 2)*rb(0);
		K12 = -rb(2)*invIB(0, 0)*rb(1) + rb(2)*invIB(0, 1)*rb(0) + rb(0)*invIB(0, 2)*rb(1) - rb(0)*invIB(1, 2)*rb(0);
		#endif
	}

	K(0, 0) += invMassB + K00;
	K(1, 1) += invMassB + K11;
	K(2, 2) += invMassB + K22;
	K(0, 1) += K01;
	K(0, 2) += K02;
	K(1, 0) += K01;
	K(1, 2) += K12;
	K(2, 0) += K02;
	K(2, 1) += K12;
	
}


void PhysCollisionLaw::applyImpulse(PhysRigidBody& body, const Vector3& impulse, Vector3& contactPoint, bool isBodyI) {

	if (body.isFixed) {
		return;
	}

	Vector3 J;
	Vector3 r;

	if (isBodyI) {

		impulse.negate(J);

	} else {

		J = impulse;
	}

	body.getBodyPointVector(contactPoint, r);

	Vector3 temp[2];

	J.mul(body.massInv, temp[0]);
	body.v += temp[0];

	r.cross(J, temp[0]);
	body.ITensor.invMulVector(temp[0], temp[1]);
	body.w += temp[1];
}

/**
    * Apply Impulse to Body.
    *
    * @param body    The body where the impulse should be applied to.
    * @param r       The arm from the center of mass of the body to the point of contact, where the impulse is applied.
    * @param J       The impulse that should be applied.
    */
void PhysCollisionLaw::applyImpulse(PhysRigidBody* pBody, const Vector3& r, const Vector3& J) {

	if (pBody->isFixed) {

		return;
	}

	Vector3 temp[2];

	J.mul(pBody->massInv, temp[0]);
	pBody->v += temp[0];

	r.cross(J, temp[0]);
	pBody->ITensor.invMulVector(temp[0], temp[1]);
	pBody->w += temp[1];
}

void PhysCollisionLaw::applyImpulse(PhysConstraintContact& contact, const Vector3& J) {

	Vector3 JInv;
	J.negate(JInv);

	applyImpulse(contact.mpBody[PhysConstraint::BodyJ], contact.r[PhysConstraint::BodyJ], J);
	applyImpulse(contact.mpBody[PhysConstraint::BodyI], contact.r[PhysConstraint::BodyI], JInv);
}

void PhysCollisionLawFrictionlessNewton::computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
														Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J) {

	PhysScalar en = pMaterial->nRestCoeff;
	PhysScalar unBefore = relVelNormalMag;
	Vector3 r[2];

	if (unBefore >= PhysZero) {

		J.zero();
		return;
	}


	Matrix33 K;

	bodyI.getBodyPointVector(bodyI.pos, contactPoint, r[PhysConstraint::BodyI]);
	bodyJ.getBodyPointVector(bodyJ.pos, contactPoint, r[PhysConstraint::BodyJ]);

	computeCollisionMatrix(bodyI.massInv, bodyI.ITensor, r[PhysConstraint::BodyI],
							bodyJ.massInv, bodyJ.ITensor, r[PhysConstraint::BodyJ], K);
							
	mul(K, normal, J);
	PhysScalar nKn = normal.dot(J);
	PhysScalar minus_1_en_u_before = -(1.0f + en) * unBefore;

	assert(nKn != PhysZero);

	normal.mul(minus_1_en_u_before / nKn, J);
	/*
	//---  u_n_after = u_n_before+ N^T K N j_n
      //---  u_n_after = - eps u_n_before
      //---  =>
      //---  - eps u_n_before - u_n_before = N^T K N j_n
      //---  - (1+eps) u_n_before / N^T K N =  j_n
     */
}

 /*	Ref.
   *	"Nonconvex Rigid Bodies with Stacking" by Guendelman, Bridson, and Fedkiw,
   *            SIGGRAPH 2003, ACM TOG 22, 871-878 (2003).
   */

void PhysCollisionLawFrictionNewton::computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
														Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J) {

	PhysScalar en = pMaterial->nRestCoeff;
	PhysScalar uNBefore = relVelNormalMag;

	Vector3 r[2];

	if (uNBefore >= PhysZero) {

		J.zero();
		return;
	}


	PhysScalar mu = pMaterial->frictionCoeff;

	Matrix33 K;

	bodyI.getBodyPointVector(bodyI.pos, contactPoint, r[PhysConstraint::BodyI]);
	bodyJ.getBodyPointVector(bodyJ.pos, contactPoint, r[PhysConstraint::BodyJ]);

	computeCollisionMatrix(bodyI.massInv, bodyI.ITensor, r[PhysConstraint::BodyI],
							bodyJ.massInv, bodyJ.ITensor, r[PhysConstraint::BodyJ], K);

	Matrix33 invK;

	K.inverse(invK);

	//--- Asume sticking, that is:
	//---
	//--- u_t_after = 0
	//--- u_n_after = -eps u_n_before N
	//---
	//--- Now from the impulse momentum law
	//---
	//---  u_after = u_before + K J
	//---
	//--- compute
	//---
	//--- J = inv(K)(u_after- u_before )
	//---

	Vector3 uAfter;
	Vector3 temp;

	normal.mul(-en * uNBefore, uAfter);
	uAfter.subtract(relVel, temp);
	mul(invK, temp, J);
	
	//String::debug(L"J = %f, %f, %f\n", J.x, J.y, J.z);

	//---
	//---  Compute the impulse component in the tangent plane
	//---
	//---    J_N = (J.N)N
	//---    J_T = J-J_N
	//---
	//--- if
	//---
	//---      |J_T|  <= mu |J_N|
	//---
	//--- then J is in the friction cone and we use it. Otherwise we need to consider sliding friction.

	PhysScalar j_n = normal.dot(J);
	Vector3 J_N, J_T;

	normal.mul(j_n, J_N);
	J.subtract(J_N, J_T);

	//String::debug(L"JT = %f, %f, %f\n", J_T.x, J_T.y, J_T.z);

	PhysScalar norm_j_t = J_T.mag();
	PhysScalar friction_limit = physAbs(mu * j_n);

	//skip checking frition_limit until its updated with baked in no-tVel reverse condition

	//--- Determine direction of sliding
	//---
	//---    T = u_rel,t/|u_rel,t|
	//---
	//---
	Vector3 uTBefore;

	normal.mul(uNBefore, temp);
	relVel.subtract(temp, uTBefore);

	PhysScalar normUTBefore = uTBefore.mag();

	
	if (friction_limit > normUTBefore) {

		//String::debug(L" ****mu Limited\n");

		friction_limit = normUTBefore;
		//simulate a corresponding mu
		mu = physAbs(friction_limit / j_n);
	}

	if(mu == PhysZero || norm_j_t < friction_limit) {
		return;
	}

	if (normUTBefore == PhysZero) {
		
		//--- We are in trouble, easy solution project J back onto friction cone
        //--- and return the projected impulse

		if (norm_j_t) {

			J_T.mul(friction_limit / norm_j_t, temp);
			J_N.add(temp, J);

			return;
		}
		
		J.zero();
		return;
	}

	Vector3 T;

	uTBefore.mul(1.0f / normUTBefore, T);

	//--- Define impulse as
	//---
	//---  J = j_n N - mu j_n T
	//---
	//--- Take the impulse-momentum law and dot it with the normal
	//---
	//---   u_n_after = u_n_before + N^T K J
	//---
	//--- Use Newton's Impact Law
	//---
	//---  - eps u_n_before   = u_n_before + N^T K J
	//---
	//--- Insert definition of J
	//---
	//---  - eps u_n_before   = u_n_before + N^T K (j_n N - mu j_n T)
	//---
	//--- Solve for j_n
	//---
	//---  - (1+ eps) u_n_before =  N^T K (j_n N - mu j_n T)
	//---  - (1+ eps) u_n_before =  N^T K N j_n  - N^T K T mu j_n
	//---  - (1+ eps) u_n_before =  N^T K (N - T mu) j_n
	//---  j_n = (- (1+ eps) u_n_before)/  N^T K (N - T mu)
	//---

	Vector3 temp2;
	PhysScalar tempf;

	T.mul(mu, temp);
	normal.subtract(temp, temp2);
	mul(K, temp2, temp);

	tempf = normal.dot(temp);

	j_n = (-(1.0f + en) * uNBefore) / tempf;

	T.mul(mu * j_n, temp);
	normal.mul(j_n, temp2);

	temp2.subtract(temp, J);

}


 /*	Ref.
   * Non-increasing Energy Collision Law.
   * This is an algebraic collision law presented in:
   *
   *    "A new algebraic rigid body collision law based on impulse space considerations" by Chatterjee and Ruina.
   *    Journal of Applied Mechanics, Vol 65, #4, 939-951, Dec 1998.
   */
void PhysCollisionLawChatterjeeRuina::computeImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyJ, PhysMaterial* pMaterial, 
													 Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& J) {

	PhysScalar e_n = pMaterial->nRestCoeff;
	PhysScalar e_t = pMaterial->tRestCoeff;
	PhysScalar u_n = relVelNormalMag;
	const Vector3& u = relVel;
	Vector3 r[2];

	if (u_n >= PhysZero) {

		J.zero();
		return;
	}

	PhysScalar mu = pMaterial->frictionCoeff;

	Matrix33 K;

	bodyI.getBodyPointVector(bodyI.pos, contactPoint, r[PhysConstraint::BodyI]);
	bodyJ.getBodyPointVector(bodyJ.pos, contactPoint, r[PhysConstraint::BodyJ]);

	computeCollisionMatrix(bodyI.massInv, bodyI.ITensor, r[PhysConstraint::BodyI],
							bodyJ.massInv, bodyJ.ITensor, r[PhysConstraint::BodyJ], K);

	Matrix33 invK;

	K.inverse(invK);

	//--- Perfectly plastic and fritionless impulse: J_I = \left( \frac{ - \vec n \cdot \vec u_i }{ \vec n^T K \vec n } \right) \vec n
	//--- This impulse brings the normal velocity to zero.
	//vector3 JI = K * contact->m_n;
	//value_type nKn = contact->m_n * JI;
	//JI = contact->m_n*(-u_n/nKn);
	Vector3 JI;
	PhysScalar nKn;

	mul(K, normal, JI);
	nKn = normal.dot(JI);
	normal.mul(-u_n / nKn, JI);


	//--- Perfectly plastic and sticking impulse: J_{II} = - K^-1 \vec u_i
	//vector3 JII = -invK*u;
	Vector3 JII;
	mul(invK, u, JII);
	JII.negate();

	//--- Compute candidate impulse
	//--- \vec \hat J = \left( 1 + e \right) \vec J_I + \left( 1 + et \right) \left( \vec J_{II}-\vec J_I \right)
	//vector3 Jhat = JI*( 1.0 + e_n) + (JII - JI)*( 1.0 + e_t);
	Vector3 temp[2];
	Vector3 Jhat;
	JI.mul(1.0f + e_n, temp[0]);
	JII.subtract(JI, temp[1]);
	temp[1].mul(1.0f + e_t);
	temp[0].add(temp[1], Jhat);
	
	
	//--- Friction cone test
	//value_type Pn = Jhat * contact->m_n;
	//vector3 tmp = Jhat - contact->m_n*Pn;
	//value_type norm = sqrt(tmp*tmp);
	PhysScalar Pn;
	PhysScalar norm;

	Pn = Jhat.dot(normal);
	normal.mul(Pn, temp[0]);
	Jhat.subtract(temp[0], temp[1]);
	norm = temp[1].mag();

	/*
	if((mu>0) && (norm > mu*Pn))
	{
	//--- kappa =
	//---        \frac{
	//---           \mu \left( 1+e \right) \vec n \cdot \vec J_I
	//---        }{
	//---           ||
	//---                \vec J_{II}
	//---                   -
	//---                \left( \vec n \cdot \vec J_{II} \right) \vec n
	//---           ||
	//---            -
	//---           \mu \vec n \cdot \left( \vec J_{II} - \vec J_I \right)
	//---        }
	//---
	vector3 tmp = JII - contact->m_n*(JII*contact->m_n);
	value_type w = sqrt(tmp*tmp);
	tmp = JII - JI;
	w -= mu*contact->m_n*tmp;
	value_type kappa = mu*(1. + e_n)*(contact->m_n*JI) / w;
	//--- \vec J = \left( 1 + e \right) \vec J_I + \kappa \left( \vec J_{II}-\vec J_I \right)
	vector3 J = JI*(1.0 + e_n) + (JII-JI)*kappa;
	return J;
	}
	else
	{
	//--- kappa = (1+et) =>
	//--- \vec J = \left( 1 + e \right) \vec J_I + \kappa \left( \vec J_{II}-\vec J_I \right)
	return Jhat;
	}
	*/

	if((mu > PhysZero) && (norm > mu*Pn)) {

		normal.mul(JII.dot(normal), temp[0]);
		JII.subtract(temp[0], temp[1]);

		PhysScalar w = temp[1].mag();
		JII.subtract(JI, temp[0]);

		w -= mu * normal.dot(temp[0]);

		PhysScalar kappa = mu * (1.0f + e_n) * normal.dot(JI) / w;
		
		JII.subtract(JI, temp[0]);
		temp[0].mul(kappa);

		JI.mul(1.0f + e_n, temp[1]);

		temp[0].add(temp[1], J);

		return;
	}


	J = Jhat;
	return;
}


}