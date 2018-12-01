#ifndef h_WOF_match_SceneCamera_ControllerTable_h
#define h_WOF_match_SceneCamera_ControllerTable_h

#include "WE3/string/WEString.h"
#include "WE3/WETL/WETLHashMap.h"
using namespace WE;

#include "SceneCamera_Controller.h"

namespace WOF { namespace match { namespace sceneCamera {


	typedef WETL::RefHashMap<StringHash, ControllerBase*, unsigned int> ControllerTable;

} } }

#endif