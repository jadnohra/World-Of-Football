#include "SpecializedCollisionLaws.h"

namespace WOF { namespace match {

void SpecializedCollisionLaw::applyImpulseToBall(PhysRigidBody& body, const Vector3& impulse, Point& contactNormal, Vector3& contactPoint, float dt, bool isBodyI) {
	
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

	PhysScalar JNMag;
	Vector3 JN;
	Vector3 JT;

	JNMag = contactNormal.dot(J);
	contactNormal.mul(JNMag, JN);
	J.subtract(JN, JT);

	body.getBodyPointVector(contactPoint, r);

	Vector3 temp[2];

	JN.mul(body.massInv, temp[0]);
	body.v += temp[0];

	//r.cross(J, temp[0]);
	//body.ITensor.invMulVector(temp[0], temp[1]);
	//body.w += temp[1];

}


void SpecializedCollisionLaw::computeBallImpulse(PhysRigidBody& bodyI, PhysRigidBody& bodyBall, PhysMaterial* pMaterial, 
														Vector3& normal, Point& contactPoint, Vector& relVel, PhysScalar relVelNormalMag, Vector3& JV, Vector3& JW) {

	PhysScalar en = pMaterial->nRestCoeff;
	PhysScalar uNBefore = relVelNormalMag;

	Vector3 r[2];

	if (uNBefore >= PhysZero) {

		JV.zero();
		JW.zero();
		return;
	}


	PhysScalar mu = pMaterial->frictionCoeff;

	Matrix33 K;

	bodyI.getBodyPointVector(bodyI.pos, contactPoint, r[PhysConstraint::BodyI]);
	bodyBall.getBodyPointVector(bodyBall.pos, contactPoint, r[PhysConstraint::BodyJ]);

	computeCollisionMatrix(bodyI.massInv, bodyI.ITensor, r[PhysConstraint::BodyI],
							bodyBall.massInv, bodyBall.ITensor, r[PhysConstraint::BodyJ], K);

	Matrix33 invK;

	K.inverse(invK);


	Vector3 temp;

	//use en and apply it only to vrel normal component
	//--- J_N = inv(K)(u_after- u_before )
	PhysScalar uNAfter = -uNBefore * en;

	normal.mul(uNAfter - uNBefore, temp);
	mul(invK, temp, JV);


	//determin ut before
	Vector3 vBallW;
	Vector3 uTBefore;

	normal.mul(uNBefore, temp);
	relVel.subtract(temp, uTBefore);

	bodyBall.getBodyPointVelocityFromW(r[PhysConstraint::BodyJ], vBallW);

	Vector3 equilibriumVT;

	lerp(uTBefore, vBallW, 0.5f, equilibriumVT);

	//now friction tries to bring w and 

	return;
	
	/*
	

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
		/simulate a corresponding mu
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
	*/

}

} }