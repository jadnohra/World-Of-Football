#ifndef h_WOF_match_BallPhysEnv
#define h_WOF_match_BallPhysEnv

#include "PhysGraph.h"

#include "WE3/math/WEVector.h"
#include "WE3/coordSys/WECoordSys.h"
using namespace WE;

namespace WOF { namespace match {

	struct PhysFluid {

		float density;

		PhysFluid() : density(0.0f) {}
	};


	class BallPhysEnv {
	public:

		BallPhysEnv();

		// using inertia=0.0f means it will be calculated automatically in the prepare() fct. using sphere inertia + inertiaCoeff
		//watch your units!
		void prepare(float mass = 0.425f, float radius = 0.184f, float inertia = 0.0f, float inertiaCoeff = 1.0f, float airDensity = 1.2f, float airDragEffectCoeff = 1.0f);

		float getDragAccelerationMag(const float& v);
		float getMagnusMag(const float& v, const float& w);

		void calcDragEffect(const Vector& v, const float& dt, Vector& vDiff);
		void calcMagnusEffect(const Vector& v, const Vector& w, Vector& accelDiff);

	public:

		PhysFluid air;

		PhysGraph dragBehaviour;
		PhysGraph magnusBehaviour;

		float radius;
		float radius2;
		float radius3;
		float radius4;
		float inverseRadius;
		float areaProjection2D; 
		float mass;
		float inverseMass;
		float inertiaCoeff;
		float inertia;
		float inverseInertia;
	};

} }

#endif