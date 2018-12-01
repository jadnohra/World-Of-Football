#ifndef h_WOF_match_Ballcommand
#define h_WOF_match_Ballcommand

#include "WE3/object/WEObject.h"
using namespace WE;

#include "../../DataTypes.h"
#include "../../../script/ScriptEngine.h"

namespace WOF { namespace match {

	class Ball;
	class BallListener;
	class BallOwner;

	enum BallCommandType {
				
		BallCommand_None = -1, BallCommand_Pos, BallCommand_Vel, BallCommand_AfterTouch, BallCommand_Guard
	};

	struct BallCommandVW {

		Vector3 v;
		Vector3 w;
	};

	struct BallCommandPos : BallCommandVW {

		Vector3 pos;
	};

	struct BallCommandVel : BallCommandVW {

		bool enableAfterTouch;
		float switchControlInfluence;
	};

	struct BallCommandAfterTouch {

		Vector3 acceleration;
		float rotSyncRatio;
	};

	struct BallCommand {

		SoftPtr<Ball> ball;
		BallCommandType type;
		unsigned int ID;
		SoftPtr<Object> source;
		float controlStrength;

		union {

			struct {
				BallCommandPos pos;
			};

			struct {
				BallCommandVel vel;
			};

			struct {

				BallCommandAfterTouch afterTouch;
			};
		};

		BallCommand() : type(BallCommand_None) {}

		inline bool isValid() const { return type != BallCommand_None; }
		inline void invalidate() { type = BallCommand_None; }
		void invalidateAndNotify();

		TeamEnum extractTeam(TeamEnum defaultTeam = Team_Invalid) const;

		inline void setNone() { type = BallCommand_None; }
		void setPos(Ball& ball, unsigned int commandID, Object& source, const float& controlStrength, const Vector3& _pos, const bool& _rotate, const CtVector3* pRotateNormal);
		void setVel(Ball& ball, unsigned int commandID, Object& source, const float& controlStrength, const Vector3& _v, const Vector3& _w, float _switchControlInfluence, bool _enableAfterTouch);
		void setAfterTouch(Ball& ball, unsigned int commandID, Object& source, const float& controlStrength, const Vector3& _acc, const float& _rotSyncRatio);
		void setGuard(Ball& ball, unsigned int commandID, Object& source, const float& controlStrength);
	
		void mergeWith(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);

		void notifyRejected();

		inline unsigned int script_getID() { return ID; }
		
		inline bool script_isValid() { return type != BallCommand_None; }
		inline int script_getType() { return type; }
		inline void script_setType(int _type) { type = (BallCommandType) _type; }
		inline float script_getControlStrength() { return controlStrength; }

		//inefficient and slow, shoudl be used on if pre-processing
		void script__setVel(Ball* pBall, unsigned int commandID, float controlStrength, CtVector3* v, CtVector3* w, float switchControlInfluence, bool enableAfterTouch);

		inline ScriptGameObject script_getSource() { return scriptArg(source.ptr()); }
		inline void script_mergeWith(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio) { mergeWith(ball, command, selfMergeRatio, randomRatio); }

		void mergeWith_None_None(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_None_Pos(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_None_Vel(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_None_AfterTouch(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_None_Guard(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Pos_None(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Pos_Pos(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Pos_Vel(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Pos_AfterTouch(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Pos_Guard(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Vel_None(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Vel_Pos(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Vel_Vel(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Vel_AfterTouch(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Vel_Guard(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_AfterTouch_None(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_AfterTouch_Pos(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_AfterTouch_Vel(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_AfterTouch_AfterTouch(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_AfterTouch_Guard(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Guard_None(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Guard_Pos(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Guard_Vel(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Guard_AfterTouch(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		void mergeWith_Guard_Guard(Ball& ball, BallCommand& command, float selfMergeRatio, float randomRatio);
		
		void fillGuardAsPos(Ball& ball);

		static const TCHAR* ScriptClassName;
		static void declareInVM(SquirrelVM& target);
	};

	inline BallCommand* scriptArg(BallCommand& cmd) { return &cmd; }
	
} }

#endif