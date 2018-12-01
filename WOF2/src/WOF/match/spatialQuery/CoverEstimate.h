#ifndef h_WOF_match_CoverEstimate
#define h_WOF_match_CoverEstimate

#include "WE3/script/WEScriptEngine.h"
using namespace WE;

#include "../DataTypes.h"
#include "PitchRegionShape.h"

namespace WOF { namespace match {

	class Match;

	struct CoverEstimate2D {

		enum IdealBlockResult {

			Result_None = -1, Result_Success,  Result_AttackerIsGoingAway, Result_AttackerIsCloser, Result_AngleClose
		};

		IdealBlockResult result;
		float blockOffset;
		float toleranceDist;
		float frameInterval;
		float idealSpeed;
		HardPtr<Vector2> holderIdealPos2D;
		HardPtr<Vector3> holderIdealPos3D;

		CoverEstimate2D();

		void holdIdealPos2D(bool hold);
		void holdIdealPos3D(bool hold);

		inline Vector2& idealPos2D() { return holderIdealPos2D; }
		inline Vector3& idealPos3D() { return holderIdealPos3D; }

		IdealBlockResult estimate(CtVector2& attackerPos, CtVector2& attackerVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos);

		IdealBlockResult estimateWithRegion(PitchRegionShape* pBlockerRegion, 
										CtVector2& attackerPos, CtVector2& attackerVel, 
										CtVector2& targetPos, CtVector2& targetVel, 
										CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos);


		IdealBlockResult estimateWithRegion(Match& match, PitchRegionShape* pBlockerRegion, 
											CtVector3& attackerPos, CtVector3& attackerVel, 
											CtVector3& targetPos, CtVector3& targetVel, 
											CtVector3& blockerPos, float blockerSpeed, Vector3& idealBlockPos, 
											float blockerHeight);

	protected:

		IdealBlockResult exitEstimate(CtVector2& attackerPos, CtVector2& attackerVel, CtVector2& targetPos, 
										CtVector2& blockerPos, CtVector2& idealBlockPos, const float& blockerSpeed, const IdealBlockResult& result);

	public:

		inline Vector2* script_idealPos2D() { return holderIdealPos2D; }
		inline Vector3* script_idealPos3D() { return holderIdealPos3D; }

		inline int script_getResult() { return result; }
		inline float script_getIdealSpeed() { return idealSpeed; }

		inline int script_estimate(CtVector2& attackerPos, CtVector2& attackerVel, 
									CtVector2& targetPos, CtVector2& targetVel, 
									CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos) {

			return estimate(attackerPos, attackerVel, targetPos, targetVel, 
								blockerPos, blockerSpeed, idealBlockPos);
		}

		inline int script_estimateWithRegion(PitchRegionShape* pBlockerRegion, CtVector2& attackerPos, CtVector2& attackerVel, 
										CtVector2& targetPos, CtVector2& targetVel, 
										CtVector2& blockerPos, float blockerSpeed, Vector2& idealBlockPos) {

			return estimateWithRegion(pBlockerRegion, attackerPos, attackerVel, targetPos, targetVel, 
										blockerPos, blockerSpeed, idealBlockPos);
		}

		inline int script_estimate3DWithRegion(Match& match, PitchRegionShape* pBlockerRegion, 
										CtVector3& attackerPos, CtVector3& attackerVel, 
										CtVector3& targetPos, CtVector3& targetVel, 
										CtVector3& blockerPos, float blockerSpeed, Vector3& idealBlockPos, float blockerHeight) {

			return estimateWithRegion(match, pBlockerRegion, attackerPos, attackerVel, targetPos, targetVel, 
										blockerPos, blockerSpeed, idealBlockPos, blockerHeight);
		}

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} } 

#endif

