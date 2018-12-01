#ifndef h_WOF_match_TeamFormation
#define h_WOF_match_TeamFormation

#include "WE3/WETL/WETLHashMap.h"
#include "WE3/math/WEVector.h"
#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "spatialQuery/PitchRegion.h"
#include "DataTypes.h"

namespace WOF { namespace match {

	class TeamFormation {
	public:

		class Mode;

		class Element {
		protected:
	
			friend class Mode;

		public:

			Element();

			inline bool hasNumber() const { return mNumber != FootballerNumberInvalid; }
			inline const FootballerNumber& getNumber() const { return mNumber; }
			inline const String& getCode() const { return mCode; }
			const Vector2& getPos() const { return mPos; }
			DataChunk* getChunk() { return mChunk; } //no release needed

		protected:

			bool load(BufferString& tempStr, TeamFormation::Mode& parent, DataChunk& chunk, Team* pTeam);

		protected:

			String mCode;
			Vector2 mPos;
			FootballerNumber mNumber;
			HardRef<DataChunk> mChunk;
		};

		class Mode {
		protected:

			friend class TeamFormation;

		public:

			inline const TCHAR* getName() { return mName.c_tstr(); }

			Element* findElement(const TCHAR* code, FootballerNumber* pFallbackNumber = NULL);
			Element* findElement(StringHash code, FootballerNumber* pFallbackNumber = NULL);
			PitchRegionShape* findRegion(StringHash name);

		protected:

			bool load(BufferString& tempStr, TeamFormation& parent, DataChunk& chunk, Team* pTeam);

		protected:

			typedef WETL::PtrHashMap<StringHash, Element*, unsigned int, WETL::ResizeDouble, 12> ElementMap;
			typedef WETL::HashMap<FootballerNumber, Element*, unsigned int, WETL::ResizeDouble, 12> ElementMapByNumber;
			typedef WETL::PtrHashMap<StringHash, PitchRegionShape*, unsigned int, WETL::ResizeDouble, 6> RegionsMap;

			String mName;
			ElementMap mElementMap;
			ElementMapByNumber mElementMapByNumber;
			RegionsMap mRegionMap;
		};

	public:

		TeamFormation();
		~TeamFormation();

		void destroy();

		inline const TCHAR* getName() { return mName.c_tstr(); }

		unsigned int getModeCount();
		Mode* getMode(unsigned int index);

		Mode* findMode(const TCHAR* name);

		bool load(BufferString& tempStr, Match& match, DataChunk* pChunk, Team* pTeam);

	protected:

		typedef WETL::PtrHashMap<StringHash, Mode*, unsigned int, WETL::ResizeDouble, 4> ModeMap;

		String mName;
		ModeMap mModeMap;
	};

	class TeamFormations {
	public:

		TeamFormations();
		~TeamFormations();

		void destroy();

		unsigned int getFormationCount();
		TeamFormation* getFormation(unsigned int index);

		TeamFormation* findFormation(const TCHAR* name);
		bool insertFormation(TeamFormation* formation); //formation will be deleted automatically if inserted

		bool load(BufferString& tempStr, Match& match, DataChunk* pChunk, Team* pTeam);

	protected:

		typedef WETL::PtrHashMap<StringHash, TeamFormation*, unsigned int, WETL::ResizeDouble, 4> FormationMap;

		FormationMap mFormationMap;
	};

} }

#endif