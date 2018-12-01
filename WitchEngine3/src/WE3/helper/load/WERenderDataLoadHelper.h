#ifndef _WERenderDataLoadHelper_h
#define _WERenderDataLoadHelper_h

#include "../../string/WEBufferString.h"
#include "../../data/WEDataSource.h"

#include "../../render/WERenderer.h"

#include "../../script/WEScriptVarTable.h"

namespace WE {

	bool fromString(BufferString& tempStr, const RenderTextureSamplerEnum& targetSampler, RenderTextureFilterEnum& filter);

	class RenderDataLoadHelper {
	public:

		static bool extract(BufferString& tempString, DataChunk& chunk, RenderColor& color, bool allowSingleValue = true, const TCHAR* attrName = L"value", float defaultAlpha = 0.5f, bool assrtOnFailure = true);

		/*
		static void extractPoolInfoHelper(BufferString& tempStr, DataChunk* pPoolChunk, bool& isPoolableSet, bool& isPoolable, bool& poolIdSet, StringHash& poolId); 
		static void extractPoolInfo(BufferString& tempStr, DataChunk* pPoolParentChunk, bool& isPoolableSet, bool& isPoolable, bool& poolIdSet, StringHash& poolId); 

		static StringHash generateVarPathHash(BufferString& tempStr, DataChunk* pPoolChunk, ScriptVarTable* pVarTable); 
		
		static StringHash generatePoolId(BufferString& tempStr, DataChunk* pChunk, bool includeSourcePath, bool includeChunkPath); 
		static StringHash generatePoolId(BufferString& tempStr, DataChunk* pPoolParentChunk, const TCHAR* sourcePath, ScriptVarTable* pVarTable, bool includeVarPath); 
		*/
		

		static bool loadRenderTextureFilters(BufferString& tempStr, DataChunk& chunk, RenderTextureFilterEnum filters[3]);
	};
}

#endif