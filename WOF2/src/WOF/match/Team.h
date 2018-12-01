#ifndef h_WOF_match_Team
#define h_WOF_match_Team

#include "WE3/WETL/WETLHashMap.h"
#include "WE3/render/WERenderer.h"
#include "WE3/script/WEScriptKeyArray.h"
#include "WE3/input/include.h"
using namespace WE;

#include "DataTypes.h"
#include "FootballerIterator.h"
#include "TeamFormation.h"
#include "scene/SceneNodeFactory.h"

namespace WOF { namespace match {

	class Footballer;
	class Player;

	class Team : public FootballerIterator {
	public:

		Team();
		~Team();

		bool init(Match& match, TeamEnum team);

		bool loadMatchSetup(TeamEnum thisTeam, Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool loadScene(Match& match, BufferString& tempStr, CoordSysConv* pConv, SceneNodeFactory& nodeFactory, int& errCount);
		
		void prepare(Match& match);

		inline void setIsDeclared(bool isDeclared) { mIsDeclared = isDeclared; }
		inline const bool& isDeclared() { return mIsDeclared; }

		inline const bool& isTeamWithFwdDir() { return mIsTeamWithFwdDir; }

		void onGoal() { ++mRecievedGoalCount; }
		inline const unsigned int& getRecievedGoalCount() { return mRecievedGoalCount; }

		bool addFootballer(Footballer* pFootballer);
		bool removeFootballer(Footballer* pFootballer);
		bool removeFootballer(const FootballerNumber& num);
		bool hasFootballer(Footballer* pFootballer);
		bool hasFootballer(const FootballerNumber& num);
		bool getFootballer(const FootballerNumber& num, Footballer*& pFootballer);

		inline unsigned int getActiveFootballerCountLeft() { return mActiveFootballerCountLeft;  }
		inline void decreaseActiveFootballerCountLeft() { if (mActiveFootballerCountLeft > 0) --mActiveFootballerCountLeft;  }

		inline unsigned int getActiveFootballerStartIndexLeft() { return mActiveFootballerStartIndexLeft;  }
		inline void decreaseActiveFootballerStartIndexLeft() { if (mActiveFootballerStartIndexLeft > 0) --mActiveFootballerStartIndexLeft;  }

		void addPlayer(TeamEnum team, bool attachController, unsigned int switchPriority, float inputReactionTime, const TCHAR* pSelect = NULL, unsigned int instanceIndex = 0, String* pID = NULL);
		
		void addPlayer(Input::Controller* pController);
		void removePlayerWithController(Input::Controller* pController);

		inline const PlayerIndex& getPlayerCount() { return mPlayers.count; }
		Player& getPlayer(const PlayerIndex& index);


		void onPlayerAttachedFootballer(Footballer* pFootballer);
		void onPlayerDetachedFootballer(Footballer* pFootballer);

		inline const TeamEnum& getTeamEnum() { return mTeam; }

		void init_prepareScene();

		void frameMove(const Clock& clock);
		void render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);

		void frameMovePlayers(const DiscreeteTime& time);
		void renderPlayers(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);

		CtVector3& getForwardDir() { return mFwdDir; }

		void formationToWorldPos(const Vector2& inPos, Vector3& pos, float height);
		void formationToWorldDir(const Vector2& inDir, Vector3& dir);

		void teamLocalToWorld(const CtVector2& v, Vector2& vWorld);
		void teamLocalToWorld(const CtVector3& v, Vector3& vWorld);
		
		inline void teamLocalToWorld(Vector2& v) {

			if (!isTeamWithFwdDir())
				v.negate();
		}

		void teamLocalToWorld(Vector3& v);

		inline const bool& didSwitchFormation() { return mSwitchedFormationFlag; }
		bool switchFormation(const TCHAR* formation = NULL, const TCHAR* mode = NULL);

		bool addFormation(TeamFormation* pFormation);
		const TeamFormation::Element* getFormationInfo(StringHash code, FootballerNumber* pFallbackNumber = NULL);
		TeamFormation::Element* getFormationInfoRef(StringHash code, FootballerNumber* pFallbackNumber = NULL);

		inline TeamFormation::Mode* getCurrFormationMode() { return mCurrFormationMode; }

		RenderSprite::Skin* getScannerSkin();

	public:

		inline const FootballerIndex& getRawFootballerCount() { return mFootballerArray.count; }
		inline Footballer& getRawFootballer(const FootballerIndex& i) { return dref(mFootballerArray[i]); }

	public:

		virtual FootballerIndex getFootballerCount(Filter filter = F_None);
		virtual bool iterateFootballers(Footballer*& pInOutFootballer, FootballerIndex& iter, Filter filter = F_None);
	
	public:

		
		inline const float& getTacticalInstantaneousBallOwnership() { return mTacticalInstantaneousBallOwnership; }
		inline const float& getTacticalBallOwnership() { return mTacticalBallOwnership; }

	protected:

		void constructorTactical();
		void updateTactical(const Clock& clock);

	protected:

		bool loadTeam(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool loadFormations(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		
	protected:

		class Skin {
		public:

			Skin();

			void load(Match& match, Team& team, BufferString& tempStr, DataChunk& chunk);
			void randomize(Match& match);

		public:

			typedef WETL::CountedPtrArrayEx<PlayerSkin, unsigned int, WETL::ResizeExact> PlayerSkins;

			String name;
			String kitPath;
			RenderSprite::Skin scannerSkin;
			PlayerSkins playerSkins;
		};

	protected:

		String mLoadPathKey;
		String mSelectedSkinName;

		HardPtr<Skin> mSkin;
		
	protected:

		typedef WETL::HashMap<FootballerNumber, Footballer*, FootballerIndex> FootballerMap;
		typedef WETL::CountedArray<Footballer*, false, FootballerIndex> FootballerArray;
		typedef WETL::CountedPtrArray<Player*, false, PlayerIndex> Players;

		SoftPtr<Match> mMatch;
		TeamEnum mTeam;
		FootballerMap mFootballerMap;
		FootballerArray mFootballerArray;
		Players mPlayers;

		bool mIsDeclared;
		bool mIsTeamWithFwdDir;
		Vector3 mFwdDir;

		FootballerIndex mPlayerAttachedFootballerCount;
		
		unsigned int mRecievedGoalCount;
		FootballerIndex mActiveFootballerCount;
		FootballerIndex mActiveFootballerStartIndex;
		FootballerIndex mActiveFootballerCountLeft;
		FootballerIndex mActiveFootballerStartIndexLeft;
		

		bool mSwitchedFormationFlag;
		TeamFormations mFormations;
		SoftPtr<TeamFormation> mCurrFormation;
		SoftPtr<TeamFormation::Mode> mCurrFormationMode;

	protected:

		float mTacticalInstantaneousBallOwnership;
		float mTacticalBallOwnership;
		float mTacticalBallOwnershipCatchupDelay; 
		float mTacticalInstantaneousBallOwnershipChangeTime;

	public:

		inline void script_teamLocalToWorld2D(CtVector2& v, Vector2& vWorld) { teamLocalToWorld(v, vWorld); }
		inline void script_teamLocalToWorld3D(CtVector3& v, Vector3& vWorld) { teamLocalToWorld(v, vWorld); }
		
		inline void script_teamLocalToWorld2DInPlace(Vector2& v) { teamLocalToWorld(v); }
		inline void script_teamLocalToWorld3DInPlace(Vector3& v) { teamLocalToWorld(v); }

		inline float script_getTacticalBallOwnership() { return getTacticalBallOwnership(); }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

} }

#endif