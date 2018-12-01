#include "../Match.h"

#include "SpatialPitchModel.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialPitchModel, CSpatialPitchModel);
DECLARE_INSTANCE_TYPE_NAME(WOF::match::SpatialPitchModel::GoalInfo, CSpatialPitchModelGoalInfo);

namespace WOF { namespace match {

const TCHAR* SpatialPitchModel::ScriptClassName = L"CSpatialPitchModel";
const TCHAR* SpatialPitchModel::GoalInfo::ScriptClassName = L"CSpatialPitchModelGoalInfo";

void SpatialPitchModel::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<SpatialPitchModel>(ScriptClassName).
		func(&SpatialPitchModel::script_getTeamEnumGoalInfo, L"getTeamEnumGoalInfo").
		func(&SpatialPitchModel::script_getTeamGoalInfo, L"getTeamGoalInfo");

	GoalInfo::declareInVM(target);
}

void SpatialPitchModel::GoalInfo::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<GoalInfo>(ScriptClassName).
		func(&GoalInfo::script_getCenterPos, L"getCenterPos").
		func(&GoalInfo::script_getBarLeftPos, L"getBarLeftPos").
		func(&GoalInfo::script_getBarRightPos, L"getBarRightPos").
		func(&GoalInfo::script_getPenaltyRegion, L"getPenaltyRegion").
		func(&GoalInfo::script_getGoalBoxRegion, L"getGoalBoxRegion");

}

SpatialPitchModel::GoalInfo::GoalInfo() {

	pPlaneFace = NULL;
	pPlaneLeft = NULL;
	pPlaneRight = NULL;

	pPosCenter = NULL;
	pPosBarLeft = NULL;
	pPosBarRight = NULL;

	pFaceBox = NULL;

	pRegionPenalty = NULL;
	pRegionGoalBox = NULL;
}

SpatialPitchModel::SpatialPitchModel() {
}

void SpatialPitchModel::update(Match& match) {

	PitchInfo::update(match);

	mGoalInfo[Goal_Back].pPlaneFace = &getPlane_Back();
	mGoalInfo[Goal_Back].pPlaneLeft = &getPlane_GoalLeft();
	mGoalInfo[Goal_Back].pPlaneRight = &getPlane_GoalRight();
	mGoalInfo[Goal_Back].pPosCenter = &goalCenterBack;
	mGoalInfo[Goal_Back].pPosBarLeft = &goalSideBarBackLeft;
	mGoalInfo[Goal_Back].pPosBarRight = &goalSideBarBackRight;
	mGoalInfo[Goal_Back].pFaceBox = &mGoalFaceBoxes[Goal_Back];
	mGoalInfo[Goal_Back].pRegionPenalty = &getRegionPenaltyBack();
	mGoalInfo[Goal_Back].pRegionGoalBox = &getRegionGoalBoxBack();

	mGoalInfo[Goal_Fwd].pPlaneFace = &getPlane_Fwd();
	mGoalInfo[Goal_Fwd].pPlaneLeft = &getPlane_GoalRight();
	mGoalInfo[Goal_Fwd].pPlaneRight = &getPlane_GoalLeft();
	mGoalInfo[Goal_Fwd].pPosCenter = &goalCenterFwd;
	mGoalInfo[Goal_Fwd].pPosBarLeft = &goalSideBarFwdLeft;
	mGoalInfo[Goal_Fwd].pPosBarRight = &goalSideBarFwdRight;
	mGoalInfo[Goal_Fwd].pFaceBox = &mGoalFaceBoxes[Goal_Fwd];
	mGoalInfo[Goal_Fwd].pRegionPenalty = &getRegionPenaltyFwd();
	mGoalInfo[Goal_Fwd].pRegionGoalBox = &getRegionGoalBoxFwd();
}

void SpatialPitchModel::init(Match& match) {

	mMatch = &match;
	PitchInfo& pitchInfo = *this;

	assrt(pitchInfo.isValid);

	{
		Plane& plane = mPlanes[Plane_Pitch];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;

		plane.init(position, match.getCoordAxis(Scene_Up), true);
	}

	{
		Plane& plane = mPlanes[Plane_Back];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Forward] = -pitchInfo.halfLength;

		plane.init(position, match.getCoordAxis(Scene_Forward), true);
	}

	{
		Plane& plane = mPlanes[Plane_Fwd];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Forward] = pitchInfo.halfLength;

		plane.init(position, match.getNegCoordAxis(Scene_Forward), true);
	}

	{
		Plane& plane = mPlanes[Plane_Left];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Right] = -pitchInfo.halfWidth;

		plane.init(position, match.getCoordAxis(Scene_Right), true);
	}

	{
		Plane& plane = mPlanes[Plane_Right];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Right] = pitchInfo.halfWidth;

		plane.init(position, match.getNegCoordAxis(Scene_Right), true);
	}

	{
		Plane& plane = mPlanes[Plane_GoalLeft];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Right] = -pitchInfo.goalHalfWidth;

		plane.init(position, match.getCoordAxis(Scene_Right), true);
	}

	{
		Plane& plane = mPlanes[Plane_GoalRight];
		Point position;

		position.zero();
		position.el[Scene_Up] = pitchInfo.height;
		position.el[Scene_Right] = pitchInfo.goalHalfWidth;

		plane.init(position, match.getNegCoordAxis(Scene_Right), true);
	}

	{
		OBB& obb = mGoalFaceBoxes[Goal_Back];

		obb.coordFrame.identity();
		Vector3& pos = obb.coordFrame.row[3];

		pos.zero();
		pos.el[Scene_Forward] = -pitchInfo.halfLength;
		pos.el[Scene_Up] = 0.5f * pitchInfo.goalHeight;

		obb.extents.zero();
		obb.extents.el[Scene_Right] = pitchInfo.goalHalfWidth;
		obb.extents.el[Scene_Up] = 0.5f * pitchInfo.goalHeight;
	}

	{
		OBB& obb = mGoalFaceBoxes[Goal_Fwd];

		obb.coordFrame.identity();
		Vector3& pos = obb.coordFrame.row[3];

		pos.zero();
		pos.el[Scene_Forward] = pitchInfo.halfLength;
		pos.el[Scene_Up] = 0.5f * pitchInfo.goalHeight;

		obb.extents.zero();
		obb.extents.el[Scene_Right] = pitchInfo.goalHalfWidth;
		obb.extents.el[Scene_Up] = 0.5f * pitchInfo.goalHeight;
	}
}

SpatialPitchModel::GoalInfo* SpatialPitchModel::script_getTeamEnumGoalInfo(int team) { 
	
	return mMatch->getTeam((TeamEnum) team).isTeamWithFwdDir() ? &mGoalInfo[Goal_Back] : &mGoalInfo[Goal_Fwd]; 
}

SpatialPitchModel::GoalInfo* SpatialPitchModel::script_getTeamGoalInfo(Team* team) { 
	
	return team->isTeamWithFwdDir() ? &mGoalInfo[Goal_Back] : &mGoalInfo[Goal_Fwd]; 
}


} }