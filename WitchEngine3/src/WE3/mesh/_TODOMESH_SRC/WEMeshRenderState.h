#ifndef _WEMeshRenderState_h
#define _WEMeshRenderState_h

#include "../WETL/WETLDataTypes.h"

namespace WE {

	enum MeshRenderStateCompareEnum {

		RSC_Different = 0, RSC_Equal, RSC_EqualType
	};

	class MeshRenderState {
	public:

		MeshRenderStateCompareEnum compare(const MeshRenderState& comp) const {return RSC_Different;}
	};

}


#endif