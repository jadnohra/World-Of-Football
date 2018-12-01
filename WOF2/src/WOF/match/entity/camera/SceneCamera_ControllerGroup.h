#ifndef h_WOF_match_SceneCamera_ControllerGroup_h
#define h_WOF_match_SceneCamera_ControllerGroup_h

#include "WE3/WETL/WETLArray.h"
#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "SceneCamera_Controller.h"
#include "SceneCamera_ControllerTable.h"

namespace WOF { namespace match { namespace sceneCamera {

		
	class ControllerGroup : public ControllerBase {
	public:

		virtual ~ControllerGroup();

		virtual void start(CamState& state);
		virtual void stop(CamState& state);

		virtual void forceStarted(CamState& state);

		virtual void update(CamState& state);
		virtual void getModifs(Vector3& eyePosModif, Vector3& targetPosModif, Vector3& targetOffsetModif);
		virtual void getPostModifs(Vector3& postModif);

	public:

		void init(BufferString& tempStr, DataChunk& chunk, ControllerTable* pTable);
		void add(ControllerBase* pController);
		
	protected:

		typedef WETL::CountedRefArray<ControllerBase*, true, unsigned int> Controllers;
		
		Controllers mControllers;
	};


} } }

#endif