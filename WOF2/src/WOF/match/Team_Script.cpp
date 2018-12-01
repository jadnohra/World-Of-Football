#include "Team.h"

#include "inc/Entities.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::Team, CTeam);

namespace WOF { namespace match {

const TCHAR* Team::ScriptClassName = L"CTeam";

void Team::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<Team>(ScriptClassName).
		func(&Team::script_teamLocalToWorld2D, L"teamLocalToWorld2D").
		func(&Team::script_teamLocalToWorld3D, L"teamLocalToWorld3D").
		func(&Team::script_teamLocalToWorld2DInPlace, L"teamLocalToWorld2DInPlace").
		func(&Team::script_teamLocalToWorld3DInPlace, L"teamLocalToWorld3DInPlace").
		func(&Team::switchFormation, L"switchFormation").
		func(&Team::script_getTacticalBallOwnership, L"getTacticalBallOwnership");
}

} }