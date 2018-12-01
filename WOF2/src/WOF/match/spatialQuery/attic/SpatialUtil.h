#ifndef h_WOF_match_SpatialUtil
#define h_WOF_match_SpatialUtil

#include "WE3/script/WEScriptEngine.h"
using namespace WE;

#include "../DataTypes.h"
#include "PitchRegionShape.h"

namespace WOF { namespace match {

	/*
	struct SpatialBlockEstimate {

		enum IdealBlockResult {

			Result_None = -1, Result_Success,  Result_SourceIsGoingAway, Result_SourceIsCloser, Result_SuccessAngleClose
		};

		IdealBlockResult result;
		float blockOffset;
		int fallbackCount;
		float toleranceDist;
		float frameInterval;
		bool isWithinToleranceDist;
		float blockerDirDotSourceVel;
		Vector3 idealBlockPos;
		float idealBlockerSpeed;

		SpatialBlockEstimate();

		IdealBlockResult estimate(CtVector3& sourcePos, CtVector3& sourceVel, 
									CtVector3& targetPos, CtVector3& targetVel, 
									CtVector3& blockerPos, float blockerSpeed);

		IdealBlockResult estimateConstantSpeeds(CtVector3& sourcePos, CtVector3& sourceVel, 
									CtVector3& targetPos, CtVector3& targetVel, 
									CtVector3& blockerPos, float blockerSpeed, 
									float timeLimit, bool fallBackToZeroVelVelIfIsGoingAway, int iterCount = 0, CtVector3* pSourceVelOverride = NULL);

	protected:

		IdealBlockResult exitEstimate(CtVector3& blockerPos, CtVector3& sourceVel, const IdealBlockResult& result);

	public:

		inline int script_getResult() { return result; }
		inline bool script_isWithinToleranceDist() { return isWithinToleranceDist; }
		inline float script_getBlockerDirDotSourceVel() { return blockerDirDotSourceVel; }
		inline CtVector3* script_getIdealBlockPos() { return &idealBlockPos; }
		inline Vector3* script_idealBlockPos() { return &idealBlockPos; }

		inline int script_estimate(CtVector3& sourcePos, CtVector3& sourceVel, 
									CtVector3& targetPos, CtVector3& targetVel, 
									CtVector3& blockerPos, float blockerSpeed) {

			return estimate(sourcePos, sourceVel, targetPos, targetVel, 
								blockerPos, blockerSpeed);
		}


	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	};
	*/

	struct SpatialBlockEstimate2D {

		enum IdealBlockResult {

			Result_None = -1, Result_Success,  Result_SourceIsGoingAway, Result_SourceIsCloser, Result_SuccessAngleClose
		};

		IdealBlockResult result;
		float blockOffset;
		int fallbackCount;
		float toleranceDist;
		float frameInterval;
		Vector2 idealBlockPos;
		float idealBlockerSpeed;
		Vector2 sourceTangentialVel;

		SpatialBlockEstimate2D();

		IdealBlockResult estimate(CtVector2& sourcePos, CtVector2& sourceVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed);


		/*
		//useful if speeds will not change for a long time
		IdealBlockResult estimateConstantSpeeds(CtVector2& sourcePos, CtVector2& sourceVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed, 
									float timeLimit, bool fallBackToZeroVelVelIfIsGoingAway, int iterCount = 0, CtVector2* pSourceVelOverride = NULL);

		*/
	protected:

		IdealBlockResult exitEstimate(CtVector2& sourcePos, CtVector2& sourceVel, CtVector2& targetPos, 
										CtVector2& blockerPos, const float& blockerSpeed, const IdealBlockResult& result);

	public:

		inline int script_getResult() { return result; }
		inline CtVector2* script_getIdealBlockPos() { return &idealBlockPos; }
		inline Vector2* script_idealBlockPos() { return &idealBlockPos; }
		inline float script_getIdealBlockerSpeed() { return idealBlockerSpeed; }
		inline CtVector2* script_getSourceTangentialVel() { return &sourceTangentialVel; }

		inline int script_estimate(CtVector2& sourcePos, CtVector2& sourceVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed) {

			return estimate(sourcePos, sourceVel, targetPos, targetVel, 
								blockerPos, blockerSpeed);
		}

		int script_estimateWithRegion(CtVector2& sourcePos, CtVector2& sourceVel, 
										CtVector2& targetPos, CtVector2& targetVel, 
										CtVector2& blockerPos, float blockerSpeed, 
										PitchRegionShape* pBlockerRegion);


	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);

	};

} } 

#endif

