#ifndef _ComponentUtil_h_WE_Input
#define _ComponentUtil_h_WE_Input

#include "Source.h"
#include "ComponentIDs.h"
#include "Component_Generic1D.h"
#include "Component_Button.h"

namespace WE { namespace Input {

	inline Component_Generic1D* comp1D(Source& source) { return (Component_Generic1D*) source.getComponent(ComponentID_Generic1D); }
	inline Component_Button* compButton(Source& source) { return (Component_Button*) source.getComponent(ComponentID_Button); }
		
} }

#endif