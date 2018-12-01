#ifndef _WEDeclareClasses_h_Math
#define _WEDeclareClasses_h_Math

#include "../../script/WEScriptEngine.h"
#include "WEScriptVector3.h"
#include "WEScriptVector2.h"
#include "../WEDistance.h"
#include "../WEIntersect.h"

namespace WE { namespace math {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			ScriptVector3::declareInVM(target);
			ScriptCtVector3::declareInVM(target);

			ScriptVector2::declareInVM(target);
			ScriptCtVector2::declareInVM(target);

			SqPlus::RegisterGlobal(&DeclareClasses::script_degToRad, L"degToRad");
			SqPlus::RegisterGlobal(&DeclareClasses::script_normalDistribution, L"normalDistribution");
			SqPlus::RegisterGlobal(&DeclareClasses::script_lerpf, L"lerpf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_sqrtf, L"sqrtf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_minf, L"minf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_maxf, L"maxf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_randf, L"randf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_rand2f, L"rand2f");
			SqPlus::RegisterGlobal(&DeclareClasses::script_rand, L"rand");
			SqPlus::RegisterGlobal(&DeclareClasses::script_powf, L"powf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_sinf, L"sinf");
			SqPlus::RegisterGlobal(&DeclareClasses::script_sinfDeg, L"sinfDeg");
			SqPlus::RegisterGlobal(&DeclareClasses::script_fabs, L"fabs");
			SqPlus::RegisterGlobal(&DeclareClasses::script_angleLH, L"angle");
			SqPlus::RegisterGlobal(&DeclareClasses::script_angleRH, L"angleRH");
			SqPlus::RegisterGlobal(&DeclareClasses::script_distSq, L"distSq");
			SqPlus::RegisterGlobal(&DeclareClasses::script_dist, L"dist");
			SqPlus::RegisterGlobal(&DeclareClasses::script_distSq2Dim, L"distSq2Dim");
			SqPlus::RegisterGlobal(&DeclareClasses::script_dist2Dim, L"dist2Dim");
			SqPlus::RegisterGlobal(&DeclareClasses::script_discretizeDirection, L"discretizeDirection");
			SqPlus::RegisterGlobal(&DeclareClasses::script_lerpv3, L"lerpv3");
			SqPlus::RegisterGlobal(&DeclareClasses::script_lerpv3ToSource, L"lerpv3ToSource");
			SqPlus::RegisterGlobal(&DeclareClasses::script_getZeroVector3, L"getZeroVector3");
			SqPlus::RegisterGlobal(&DeclareClasses::script_getZeroVector2, L"getZeroVector2");
			SqPlus::RegisterGlobal(&DeclareClasses::script_pointToSegClosestPt, L"pointToSegClosestPt");
			SqPlus::RegisterGlobal(&DeclareClasses::script_pointToSegClosestPt2D, L"pointToSegClosestPt2D");
			SqPlus::RegisterGlobal(&DeclareClasses::script_intersectLinePlane, L"intersectLinePlane");
			SqPlus::RegisterGlobal(&DeclareClasses::script_intersectSegPlane, L"intersectSegPlane");
		}

	public:

		inline static float script_degToRad(float val) { return degToRad(val); }
		inline static float script_normalDistribution(float mean, float variance) { return normalDist(mean, variance); }
		inline static void script_lerpv3(CtVector3& from, CtVector3& to, float factor, Vector3& result) { lerp(from, to, factor, result); }
		inline static void script_lerpv3ToSource(Vector3& from, CtVector3& to, float factor) { lerp(Vector3(from), to, factor, from);  }
		inline static float script_lerpf(float x, float y, float factor) { return lerp(x, y, factor); }
		inline static float script_maxf(float x, float y) { return tmax(x, y); }
		inline static float script_minf(float x, float y) { return tmin(x, y); }
		inline static float script_powf(float x, float y) { return powf(x, y); }
		inline static float script_randf(float min, float max) { return trand2<float>(min, max); }
		inline static float script_rand2f(float amount) { return trand2<float>(-amount * 1.0f, amount * 1.0f); }
		inline static int script_rand(float min, float max) { return (int) trand2<float>(min, max); }
		inline static float script_sqrtf(float v) { return sqrtf(v); }
		inline static float script_sinf(float v) { return sinf(v); }
		inline static float script_sinfDeg(float v) { return sinf(degToRad(v)); }
		inline static float script_fabs(float v) { return fabs(v); }
		inline static float script_distSq(CtVector3* p1, CtVector3* p2) { return distanceSq(dref(p1), dref(p2)); }
		inline static float script_dist(CtVector3* p1, CtVector3* p2) { return distance(dref(p1), dref(p2)); }
		
		inline static float script_distSq2Dim(CtVector3* p1, CtVector3* p2, int dim1, int dim2) { return distanceSq(dref(p1), dref(p2), dim1, dim2); }
		inline static float script_dist2Dim(CtVector3* p1, CtVector3* p2, int dim1, int dim2) { return distance(dref(p1), dref(p2), dim1, dim2); }
		
		inline static float script_angleLH(CtVector3* refDir, CtVector3* toDir, CtVector3* normalDir) { return angle(dref(refDir), dref(toDir), dref(normalDir), true); }
		inline static float script_angleRH(CtVector3* refDir, CtVector3* toDir, CtVector3* normalDir) { return angle(dref(refDir), dref(toDir), dref(normalDir), false); }
		inline static int script_discretizeDirection(CtVector3* refDir, CtVector3* normalDir, CtVector3* dir, int directionCount, Vector3* discreeteDir) { return discretizeDirection(dref(refDir), dref(normalDir), dref(dir), directionCount, dref(discreeteDir)); }
		inline static CtVector3* script_getZeroVector3() { return &(CtVector3::kZero); }
		inline static CtVector2* script_getZeroVector2() { return &(CtVector2::kZero); }
		
		inline static float script_pointToSegClosestPt(CtVector3* segA, CtVector3* segB, CtVector3* point, Vector3* closestPoint) {

			float u;

			return distanceSq(dref(segA), dref(segB), dref(point), dref(closestPoint), u);
		}

		inline static float script_pointToSegClosestPt2D(CtVector2* segA, CtVector2* segB, CtVector2* point, Vector2* closestPoint) {

			float u;

			return distanceSq(dref(segA), dref(segB), dref(point), dref(closestPoint), u);
		}

		inline static bool script_intersectLinePlane(CtVector3& planePos, CtVector3& planeNormal, CtVector3& lineOrig, CtVector3& lineDir, ScriptFloat& u, Vector3* pIntersection) {

			return intersectPlane(planePos, planeNormal, lineOrig, lineDir, u.value(), pIntersection);
		}

		inline static bool script_intersectSegPlane(CtVector3& planePos, CtVector3& planeNormal, CtVector3& segPt1, CtVector3& segPt2, ScriptFloat& u, Vector3* pIntersection) {

			return intersectSegPlane(planePos, planeNormal, segPt1, segPt2, u.value(), pIntersection);
		}
	};

} }

#endif