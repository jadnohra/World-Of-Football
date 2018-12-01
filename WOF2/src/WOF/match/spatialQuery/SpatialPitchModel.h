#ifndef _PitchModel_h_WOF_match
#define _PitchModel_h_WOF_match

#include "WE3/math/WEPlane.h"
#include "WE3/math/WEOBB.h"
using namespace WE;

#include "../Team.h"
#include "../../script/ScriptEngine.h"

namespace WOF { namespace match {

	class Match;

	class SpatialPitchModel : public PitchInfo {
	public:

		struct GoalInfo {

			GoalInfo();

			const Plane* pPlaneFace;
			const Plane* pPlaneLeft;
			const Plane* pPlaneRight;

			CtVector3* pPosCenter;
			CtVector3* pPosBarLeft;
			CtVector3* pPosBarRight;

			const OBB* pFaceBox;

			PitchRegionShape* pRegionPenalty;
			PitchRegionShape* pRegionGoalBox;

			static const TCHAR* ScriptClassName;
			static void declareInVM(SquirrelVM& target);

			inline CtVector3* script_getCenterPos() { return pPosCenter; }
			inline CtVector3* script_getBarLeftPos() { return pPosBarLeft; }
			inline CtVector3* script_getBarRightPos() { return pPosBarRight; }
			inline PitchRegionShape* script_getPenaltyRegion() { return pRegionPenalty; }
			inline PitchRegionShape* script_getGoalBoxRegion() { return pRegionGoalBox; }
		};

	public:

		SpatialPitchModel();

		void init(Match& match);
		void update(Match& match);

	protected:

		//the directions mentioned here are in world coord system
		//so the team info is needed to determin team relative direction

		enum PlaneType {

			Plane_None = -1, Plane_Pitch, Plane_Back, Plane_Fwd, Plane_Left, Plane_Right,
				Plane_GoalLeft, Plane_GoalRight, PlaneTypeCount
		};

		enum GoalType {

			Goal_None = -1, Goal_Back, Goal_Fwd, GoalTypeCount
		};

		SoftPtr<Match> mMatch;

		Plane mPlanes[PlaneTypeCount];
		OBB mGoalFaceBoxes[GoalTypeCount]; 
		GoalInfo mGoalInfo[GoalTypeCount];

	public:

		inline const Plane& getPlane_Pitch() { return mPlanes[Plane_Pitch]; }
		inline const Plane& getPlane_Back() { return mPlanes[Plane_Back]; }
		inline const Plane& getPlane_Fwd() { return mPlanes[Plane_Fwd]; }
		inline const Plane& getPlane_Left() { return mPlanes[Plane_Left]; }
		inline const Plane& getPlane_Right() { return mPlanes[Plane_Right]; }
		inline const Plane& getPlane_GoalLeft() { return mPlanes[Plane_GoalLeft]; }
		inline const Plane& getPlane_GoalRight() { return mPlanes[Plane_GoalRight]; }

		inline const Plane& getPlane_Back(Team& team) { return team.isTeamWithFwdDir() ? getPlane_Back() : getPlane_Fwd(); }
		inline const Plane& getPlane_Fwd(Team& team) { return team.isTeamWithFwdDir() ? getPlane_Fwd() : getPlane_Back(); }
		inline const Plane& getPlane_Left(Team& team) { return team.isTeamWithFwdDir() ? getPlane_Left() : getPlane_Right(); }
		inline const Plane& getPlane_Right(Team& team) { return team.isTeamWithFwdDir() ? getPlane_Right() : getPlane_Left(); }
		inline const Plane& getPlane_GoalLeft(Team& team) { return team.isTeamWithFwdDir() ? getPlane_GoalLeft() : getPlane_GoalRight(); }
		inline const Plane& getPlane_GoalRight(Team& team) { return team.isTeamWithFwdDir() ? getPlane_GoalRight() : getPlane_GoalLeft(); }

		inline const GoalInfo& getGoalInfoBack() { return mGoalInfo[Goal_Back]; }
		inline const GoalInfo& getGoalInfoFwd() { return mGoalInfo[Goal_Fwd]; }
		inline const GoalInfo& getGoalInfo(Team& team) { return team.isTeamWithFwdDir() ? getGoalInfoBack() : getGoalInfoFwd(); }

		PitchRegionShape& getRegionPitch() { return regionPitch; }
		PitchRegionShape& getRegionPenaltyBack() { return regionPenaltyBack; }
		PitchRegionShape& getRegionGoalBoxBack() { return regionGoalBoxBack; }
		PitchRegionShape& getRegionPenaltyFwd() { return regionPenaltyFwd; }
		PitchRegionShape& getRegionGoalBoxFwd() { return regionGoalBoxFwd; }

		PitchRegionShape& getRegionPenalty(Team& team) { return team.isTeamWithFwdDir() ? regionPenaltyBack : regionPenaltyFwd; }
		PitchRegionShape& getRegionOppPenalty(Team& team) { return team.isTeamWithFwdDir() ? regionPenaltyFwd : regionPenaltyBack; }

	public:

		GoalInfo* script_getTeamEnumGoalInfo(int team);
		GoalInfo* script_getTeamGoalInfo(Team* team);

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif