#ifndef h_WOF_ScriptClasses
#define h_WOF_ScriptClasses

#include "ScriptEngine.h"

#include "WE3/input/script/DeclareClasses.h"
#include "WE3/math/WEResponseCurve.h"
#include "WE3/render/script/DeclareClasses.h"
#include "../match/Match.h"
#include "../match/Team.h"
#include "../match/entity/scriptedCharacter/SceneScriptedCharacter.h"
#include "../match/entity/scriptedFootballer/ScriptedFootballer.h"
#include "../match/entity/ball/BallCommandManager.h"
#include "../match/entity/ball/Ball.h"
#include "../match/entity/ball/simul/BallSimulation.h"
#include "../match/spatialQuery/script/DeclareClasses.h"
#include "../WOF.h"
#include "../match/script/ScriptTeamFootballerIterator.h"

namespace WOF {

	class DeclareClasses {
	public:

		static void declareInVM(SquirrelVM& target) {

			WE::Input::DeclareClasses::declareInVM(target);
			WE::ResponseCurve::declareInVM(target);
			WE::Render::DeclareClasses::declareInVM(target);
			match::BallRelativeDist::declareInVM(target);
			match::BallOwnership::declareInVM(target);
			match::BallInteraction::declareInVM(target);
			match::ScriptGameObject::declareInVM(target);
			match::Match::declareInVM(target);
			match::Team::declareInVM(target);
			Game::declareInVM(target);
			match::SceneScriptedCharacter::declareInVM(target);
			match::ScriptedFootballer::declareInVM(target);
			match::BallCommand::declareInVM(target);
			match::BallCommandManager::declareInVM(target);
			match::Ball::declareInVM(target);
			match::SpatialDeclareClasses::declareInVM(target);
			match::ScriptTeamFootballerIterator::declareInVM(target);
		}
	};

}

#endif