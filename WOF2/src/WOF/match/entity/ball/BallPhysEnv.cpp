#include "BallPhysEnv.h"

#include "WE3/math/WEMathUtil.h"
using namespace WE;

namespace WOF { namespace match {

BallPhysEnv::BallPhysEnv() { 
}

void BallPhysEnv::prepare(float _mass, float _radius, float _inertia, float _inertiaCoeff, float _airDensity, float _airDragEffectCoeff) {

	unsigned int i;

	{
		air.density = _airDensity;
	}

	{
		dragBehaviour.effectCoeff = _airDragEffectCoeff;

		i = 0;
		dragBehaviour.graph[i].data[0] = 10.2f;
		dragBehaviour.graph[i].data[1] = 0.5f;
		i++;

		dragBehaviour.graph[i].data[0] = 10.22f;
		dragBehaviour.graph[i].data[1] = 0.1f;
		i++;

		dragBehaviour.graph[i].data[0] = 10.35f;
		dragBehaviour.graph[i].data[1] = 0.17f;
		i++;

		dragBehaviour.graph[i].data[0] = 11.5f;
		dragBehaviour.graph[i].data[1] = 0.2f;
		i++;

		dragBehaviour.prepare(i);
	}


	{
		magnusBehaviour.effectCoeff = 0.0f;

		i = 0;
		magnusBehaviour.graph[i].data[0] = 0.0f;
		magnusBehaviour.graph[i].data[1] = 0.0f;
		i++;

		magnusBehaviour.graph[i].data[0] = 0.3f;
		magnusBehaviour.graph[i].data[1] = -0.05f;
		i++;

		magnusBehaviour.graph[i].data[0] = 0.6f;
		magnusBehaviour.graph[i].data[1] = 0.2f;
		i++;

		magnusBehaviour.prepare(i);
	}

	{
		radius = _radius;

		mass = _mass;

		inverseRadius = 1.0f / radius;
		radius2 = (float) powf(radius, 2);
		radius3 = (float) powf(radius, 3);
		radius4 = (float) powf(radius, 4);

		areaProjection2D = kPif * radius2;

		inertia = _inertia;
		inertiaCoeff = _inertiaCoeff;

		if (inertia == 0.0f) {

			//inertia = (2.0f * mass * (radius2) / 3.0f) * 500.0f * inertiaCoeff;
			inertia = (2.0f * mass * (radius2) / 3.0f) * inertiaCoeff;
		}
		
		inverseMass = 1.0f / mass;
		inverseInertia = 1.0f / inertia;
	}
}

inline float getGraphCoefficient(PhysGraph& graph, const float& at) {

	return Graph::getValue(at, graph.graph, graph.graphSize);
}

float BallPhysEnv::getDragAccelerationMag(const float& v) {

	float rt = inverseMass * ( 0.5f * air.density * (v * v) * areaProjection2D * getGraphCoefficient(dragBehaviour, v) );

	return rt;
}

float BallPhysEnv::getMagnusMag(const float& v, const float& w) {

	float coeff = getGraphCoefficient(magnusBehaviour, (v / (radius * w)));
	float rt = inverseMass * ( (kPi2) * air.density * v * radius3 * w * coeff );

	return rt;
}




void BallPhysEnv::calcDragEffect(const Vector& v, const float& dt, Vector& vDiff) {

	float magDrag;
	float magV = v.mag();
	const float& dragCoeff = dragBehaviour.effectCoeff;

	Vector vDir;

	if (magV > 0.0f) {
		
		//Vector vLoss;
		
		v.normalize(vDir);
		magDrag = getDragAccelerationMag(magV) * dragCoeff;
		
		vDir.mul(-magDrag * dt, vDiff);
	
		//applyVLoss(*pBall, vLoss, isRollingWithoutSlipping(pBall), pBall->contactNormal);

	} else {

		vDiff.zero();
	}
}


void BallPhysEnv::calcMagnusEffect(const Vector& v, const Vector& w, Vector& accelDiff) {

	float magW;
	float magV;
	float magMagnus;
	float magnusCoeff = magnusBehaviour.effectCoeff;
	Vector magnusAxis;

	if (magnusCoeff != 0.0f /*pBall->isAirborne == true*/) {

		v.mag(magV);
		w.mag(magW);

		if (magV != 0.0f && magW != 0.0f) {

			w.cross(w, magnusAxis);
			float temp;

			magnusAxis.normalize(temp);

			if (temp == 0.0f) {

				v.normalize(magnusAxis);

				magnusCoeff *= 0.6f;
			} 

			magMagnus = magnusCoeff * getMagnusMag(magV, magW);
			magnusAxis.mul(magMagnus, accelDiff);
		}

	} else {

		accelDiff.zero();
	}
}


/*

inline void applyVLoss(WOFSceneBallInfo& ballInfo, KE3DCoords lossV, bool syncW = false, KE3DCoords normal = NULL) {
	KEDeclare3DCoords(vNewToOldRatio);
	KEDeclare3DCoords(wNewToOldRatio);
	
	unsigned short d;
	float lossTest;

	if (syncW == false) {

		for (d = 0; d < 3; d++) {
						
			lossTest = ballInfo.v[d] - lossV[d];
			if (KEMathUtil::isEqualSign(ballInfo.v[d], lossTest) == false) {
				ballInfo.v[d] = 0.0f;
			} else {
				ballInfo.v[d] = lossTest;
			}
		}

	} else {

		for (d = 0; d < 3; d++) {
						
			vNewToOldRatio[d] = ballInfo.v[d];
						
			lossTest = ballInfo.v[d] - lossV[d];
			if (KEMathUtil::isEqualSign(ballInfo.v[d], lossTest) == false) {
				ballInfo.v[d] = 0.0f;
			} else {
				ballInfo.v[d] = lossTest;
			}
						
			if (vNewToOldRatio[d] != 0.0f) {
				vNewToOldRatio[d] = ballInfo.v[d] / vNewToOldRatio[d];
			}
		}
					
		KEMathUtil::crossProduct(vNewToOldRatio, normal, wNewToOldRatio);
					
		for (d = 0; d < 3; d++) {
			if (wNewToOldRatio[d] < 0.0f) {
				wNewToOldRatio[d] = -wNewToOldRatio[d];
			}
			ballInfo.w[d] = ballInfo.w[d] * wNewToOldRatio[d];
		}
	}
}

*/


/*
inline float getDragAccelerationMag(WOFPhysFluid& fluid, WOFPhysBall& ball, const float& v) {

	float rt = ball.inverseMass * ( 0.5f * fluid.density * (v * v) * ball.areaProjection2D * getGraphCoefficient(ball.dragBehaviour, v) );
	return rt;
}

inline float getMagnusMag(WOFPhysFluid& fluid, WOFPhysBall& ball, const float& v, const float& w) {

	float coeff = getMagnusCoeficient(ball.magnusBehaviour, (v / (ball.radius * w)));
	float rt = ball.inverseMass * ( (physPi2) * fluid.density * v * ball.radius3 * w * coeff );
	return rt;
}
*/


/*
void setupGamePhysics(WOFActiveGame& game) {

	game.physics.time.simulStepSizeSecs = 0.0025f;
	game.physics.time.prepare();
	
	game.physics.environment.gravityMag = 9.8f;


	game.physics.air.density = 0.5f;

	
	game.physics.pitchSurface.elasticity = 1.0f;
	game.physics.pitchSurface.kineticFrictionCoeff = 0.4f;
	game.physics.pitchSurface.miscLossesPerMeter = 0.1f;
	game.physics.pitchSurface.minRollVelocity = 0.2f;

	unsigned int i;

	i = 0;
	game.physics.ball.dragBehaviour.graph[i].data[0] = 10.2f;
	game.physics.ball.dragBehaviour.graph[i].data[1] = 0.5f;
	i++;

	game.physics.ball.dragBehaviour.graph[i].data[0] = 10.22f;
	game.physics.ball.dragBehaviour.graph[i].data[1] = 0.1f;
	i++;

	game.physics.ball.dragBehaviour.graph[i].data[0] = 10.35f;
	game.physics.ball.dragBehaviour.graph[i].data[1] = 0.17f;
	i++;

	game.physics.ball.dragBehaviour.graph[i].data[0] = 11.5f;
	game.physics.ball.dragBehaviour.graph[i].data[1] = 0.2f;
	i++;

	game.physics.ball.dragBehaviour.prepare(i);


	i = 0;
	game.physics.ball.magnusBehaviour.graph[i].data[0] = 0.0f;
	game.physics.ball.magnusBehaviour.graph[i].data[1] = 0.0f;
	i++;

	game.physics.ball.magnusBehaviour.graph[i].data[0] = 0.3f;
	game.physics.ball.magnusBehaviour.graph[i].data[1] = -0.05f;
	i++;

	game.physics.ball.magnusBehaviour.graph[i].data[0] = 0.6f;
	game.physics.ball.magnusBehaviour.graph[i].data[1] = 0.2f;
	i++;


	game.physics.ball.magnusBehaviour.prepare(i);


	game.physics.ball.mass = 0.425f;
	game.physics.ball.radius = 0.184f;
	game.physics.ball.inertiaCoeff = 1.0f;
	game.physics.ball.shockTime = 0.04;
	game.physics.ball.surfaceContact.elasticity = 0.65f;
	game.physics.ball.surfaceContact.kineticFrictionCoeff = 0.5f;
	game.physics.ball.surfaceContact.miscLossesPerMeter = 0.05f;
	game.physics.ball.surfaceContact.minRollVelocity = 0.1f;


	game.physics.ball.rollResistanceCoeff = 1.3f;
	game.physics.ball.compressionVelocityLoss = 0.6f;
	
	game.physics.ball.dragCoeffInAir = 0.9f;
	game.physics.ball.dragCoeffOnGround = 0.85f;

	game.physics.ball.magnusCoeff = 0.25f;

	game.physics.ball.prepare();

}
*/

/*
inline float getDragCoeficient(WOFPhysGraph& drag, float v) {
	return KEGraph::getValue(v, drag.graph, drag.graphSize);
}

inline float getMagnusCoeficient(WOFPhysGraph& magnus, float x) {
	return KEGraph::getValue(x, magnus.graph, magnus.graphSize);
}
*/

/*
inline float getDragCoeficient(WOFPhysFluid& fluid, float v) {

	if (v > fluid.speedLimit + fluid.speedCrossRangeHalf) {
		return fluid.highSpeedDrag;
	} else if (v < fluid.speedLimit - fluid.speedCrossRangeHalf) {
		return fluid.lowSpeedDrag;
	} 

	return fluid.lowSpeedDrag - (((v - (fluid.speedLimit - fluid.speedCrossRangeHalf)) / fluid.speedCrossRange) * (fluid.dragValueDiff));
}
*/

/*
inline float getDragAccelerationMag(WOFPhysFluid& fluid, WOFPhysBall& ball, float v) {
	float rt = ball.inverseMass * ( 0.5f * fluid.density * (v * v) * ball.areaProjection2D * getDragCoeficient(ball.dragBehaviour, v) );
	return rt;
}



inline float getMagnusMag(WOFPhysFluid& fluid, WOFPhysBall& ball, float v, float w) {
	float coeff = getMagnusCoeficient(ball.magnusBehaviour, (v / (ball.radius * w)));
	float rt = ball.inverseMass * ( (physPi2) * fluid.density * v * ball.radius3 * w * coeff );
	return rt;
}
*/


/*
void applyDrag(WOFPhysBall& ballPhys, WOFSceneBallInfo* pBall, WOFPhysFluid& air, float dt) {

	float magDrag;
	float magV;
	float dragCoeff;
	KEDeclare3DCoords(vDir);


	if (pBall->isAirborne == true) {
		dragCoeff = ballPhys.dragCoeffInAir;
	} else {
		dragCoeff = ballPhys.dragCoeffOnGround;
	}
	
	KEMathUtil::extractVectorLength(pBall->v, magV);
	if (magV > 0.0f) {
		
		KEDeclare3DCoords(vLoss);
		KEMathUtil::extractUnitVector(pBall->v, vDir);
		magDrag = getDragAccelerationMag(air, ballPhys, magV) * dragCoeff;
		
		KEMathUtil::ctProduct(vDir, magDrag * dt, vLoss);
		
	
		applyVLoss(*pBall, vLoss, isRollingWithoutSlipping(pBall), pBall->contactNormal);
	}
}

void applyMagnus(WOFPhysBall& ballPhys, WOFSceneBallInfo* pBall, WOFPhysFluid& air) {

	unsigned short di;
	float magW;
	float magV;
	float magMagnus;
	float magnusCoeff = ballPhys.magnusCoeff;
	KEDeclare3DCoords(magnusAxis);
	KEDeclare3DCoords(magnusA);

	if (pBall->isAirborne == true) {
		KEMathUtil::extractVectorLength(pBall->v, magV);
		KEMathUtil::extractVectorLength(pBall->w, magW);

		if (magV != 0.0f && magW != 0.0f) {
			KEMathUtil::crossProduct(pBall->w, pBall->v, magnusAxis);
			
			if (KEMathUtil::extractUnitVector(magnusAxis, magnusAxis) == 0.0f) {
				KEMathUtil::extractUnitVector(pBall->v, magnusAxis);
				magnusCoeff *= 0.6f;
			} 

			magMagnus = magnusCoeff * getMagnusMag(air, ballPhys, magV, magW);
			KEMathUtil::ctProduct(magnusAxis, magMagnus, magnusA);


			for (di = 0; di < 3; di++) {
				pBall->a[di] += magnusA[di];
			}
		}
	}
}
*/

} }

