#ifndef _WERenderTextureSource_h
#define _WERenderTextureSource_h

#include "WERenderTextureCreator.h"


namespace WE {

	class RenderTextureSource {
	public:

		virtual RenderTexture* get(RenderTextureLoadInfo& loadInfo) = 0;

		RenderTexture* getFromDataChunk(DataChunk& texChunk, RenderLoadContext& renderLoadContext, 
			const RefPoolObjectPoolInfo* pPoolInfo = NULL, int loaderVersion = 1);
	};
}

#endif