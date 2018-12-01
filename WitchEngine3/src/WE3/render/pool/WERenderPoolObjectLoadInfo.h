#ifndef _WERenderPoolObjectLoadInfo_h
#define _WERenderPoolObjectLoadInfo_h

#include "../../pool/WERefPoolObjectLoadInfo.h"
#include "../loadContext/WERenderLoadContext.h"

namespace WE {

	struct RenderPoolObjectLoadInfo : RefPoolObjectLoadInfo {

		SoftPtr<RenderLoadContext> renderLoadContext;

		RenderPoolObjectLoadInfo(RenderLoadContext* pRenderObjSource) : RefPoolObjectLoadInfo() {

			renderLoadContext = pRenderObjSource;
		}
	};

}

#endif