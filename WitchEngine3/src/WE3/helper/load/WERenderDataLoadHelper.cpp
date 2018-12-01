#include "WERenderDataLoadHelper.h"

#include "tchar.h"
#include "../../WEAssert.h"

namespace WE {

bool fromString(BufferString& tempStr, const RenderTextureSamplerEnum& targetSampler, RenderTextureFilterEnum& filter) {

	if (tempStr.equals(L"point") || tempStr.equals(L"low") || tempStr.equals(L"1")) {

		filter = RTF_Point;

	} else if (tempStr.equals(L"linear") || tempStr.equals(L"normal") || tempStr.equals(L"2")) {

		filter = RTF_Linear;

	} else if (tempStr.equals(L"aniso") || tempStr.equals(L"high") || tempStr.equals(L"anisotropic") || tempStr.equals(L"3")) {

		filter = RTF_Anisotropic;

	} else if (tempStr.equals(L"none") || tempStr.equals(L"off") || tempStr.equals(L"0")) {

		filter = (targetSampler == RTS_Mip ? RTF_None : RTF_Point);

	} else {

		filter = RTF_Point;
	}

	return true;
}


bool RenderDataLoadHelper::loadRenderTextureFilters(BufferString& tempStr, DataChunk& chunk, RenderTextureFilterEnum filters[3]) {

	chunk.getAttribute(L"minFilter", tempStr);
	fromString(tempStr, RTS_Min, filters[RTS_Min]);

	chunk.getAttribute(L"magFilter", tempStr);
	fromString(tempStr, RTS_Mag, filters[RTS_Mag]);

	chunk.getAttribute(L"mipFilter", tempStr);
	fromString(tempStr, RTS_Mip, filters[RTS_Mip]);

	return true;
}


bool RenderDataLoadHelper::extract(BufferString& tempStr, DataChunk& chunk, RenderColor& color, bool allowSingleValue, const TCHAR* attrName, float defaultAlpha, bool assrtOnFailure) {

	if (chunk.getAttribute(attrName, tempStr)) {

		{
			{
				bool found = false;
				int startIndex = 0;
				int count = 1;

				if (!tempStr.isEmpty() && tempStr.findNext(L'#', 0, found, startIndex, count, true) == 0) {

					int col[4];

					switch (_stscanf(tempStr.c_tstr(), L"#%d, %d, %d, %d", &col[0], &col[1], &col[2], &col[3])) {
						case 4:
							color.set(col[0], col[1], col[2], col[3]);
							return true;
						case 3:
							color.set(col[0], col[1], col[2], 0);
							color.alpha = defaultAlpha;
							return true;	
						case 1:

							color.set(col[0], 0);
							color.alpha = defaultAlpha;
							return allowSingleValue;
					}

					if (assrtOnFailure)
						assrt(false);

					return false;
				}
			}

			{
				float temp;

				if (_stscanf(tempStr.c_tstr(), L"%f", &temp) <= 0) {

					StringPart namePart(&tempStr);
					bool found = false;

					tempStr.findNext(L',', 0, found, namePart.startIndex, namePart.count, false);

					if (found) {

						String name;

						namePart.assignTo(name);
						const RenderColor* pRefColor = NULL;
						RenderColor colorHolder;
						
						if (name.equalsIgnoreCase(L"white")) {

							pRefColor = &RenderColor::kWhite;

						} else if (name.equalsIgnoreCase(L"red")) {

							pRefColor = &RenderColor::kRed;

						} else if (name.equalsIgnoreCase(L"green")) {

							pRefColor = &RenderColor::kGreen;

						} else if (name.equalsIgnoreCase(L"blue")) {

							pRefColor = &RenderColor::kBlue;

						} else if (name.equalsIgnoreCase(L"yellow")) {

							pRefColor = &RenderColor::kYellow;

						} else if (name.equalsIgnoreCase(L"black")) {

							pRefColor = &RenderColor::kBlack;

						} else if (name.equalsIgnoreCase(L"random") || name.equalsIgnoreCase(L"rand")) {

							pRefColor = &colorHolder;
							colorHolder.el[0] = trand(1.0f);
							colorHolder.el[1] = trand(1.0f);
							colorHolder.el[2] = trand(1.0f);
							colorHolder.alpha = trand(1.0f);

						} else if (name.equalsIgnoreCase(L"nrandom") || name.equalsIgnoreCase(L"nrand")) {

							pRefColor = &colorHolder;
							colorHolder.el[0] = trand(1.0f);
							colorHolder.el[1] = trand(1.0f);
							colorHolder.el[2] = trand(1.0f);
							colorHolder.alpha = trand(1.0f);

							colorHolder.normalize();

						} else {

							assrt(false);
							return false;
						}

						color = dref(pRefColor);

						StringPart alphaPart(&tempStr);
						namePart.getRemainingPart(tempStr.c_tstr(), alphaPart, tempStr.length());

						String alphaValue;
						alphaPart.assignTo(alphaValue);

						if (!alphaValue.fromString(L",%f", &color.alpha)) {

							color.alpha = defaultAlpha;
						}

						return true;
					}
				}
			} 
		}

		switch (_stscanf(tempStr.c_tstr(), L"%f, %f, %f, %f", &color.el[0], &color.el[1], &color.el[2], &color.alpha)) {
			case 4:
				return true;
			case 3:
				color.alpha = defaultAlpha;
				return true;	
			case 1:

				color.set(color[0], defaultAlpha);
				return allowSingleValue;
		}
	}

	if (assrtOnFailure)
		assrt(false);

	return false;
}

/*
void RenderDataLoadHelper::extractPoolInfoHelper(BufferString& tempStr, DataChunk* pPoolParentChunk, bool& isPoolableSet, bool& isPoolable, bool& poolIdSet, StringHash& poolId) {

	isPoolableSet = pPoolChunk->scanAttribute(tempStr, L"enable", isPoolable);
	poolIdSet = pPoolChunk->getAttribute(L"poolId", tempStr);

	if (poolIdSet) {

		poolId = tempStr.hash();
	} else {

		poolId = 0;
	}
}

void RenderDataLoadHelper::extractPoolInfo(BufferString& tempStr, DataChunk* pPoolParentChunk, bool& isPoolableSet, bool& isPoolable, bool& poolIdSet, StringHash& poolId) {

	RefWrap<DataChunk> poolChunk;

	if (pPoolParentChunk != NULL) {

		poolChunk.set(pPoolParentChunk->getChild(L"pool"), false);

		if (poolChunk.isNull()) {

			if (pPoolParentChunk->getAttribute(L"poolId", tempStr)) {

				isPoolable = true;
				poolId = tempStr.hash();

				return;
			} else {

				isPoolable = false;
				return;
			}
		}
	} 

	extractPoolInfoHelper(tempStr, poolChunk.ptr(), isPoolable, poolId);
}

StringHash RenderDataLoadHelper::generateVarPathHash(BufferString& tempStr, DataChunk* pPoolChunk, ScriptVarTable* pVarTable) {

	StringHash ret = 0;

	RefWrap<DataChunk> var;

	if (pPoolChunk) {

		var.set(pPoolChunk->getFirstChild(), false);

		if (var.isValid()) {

			assrt(pVarTable != NULL);

			if (pVarTable) {

				StringHash varHash;

				while(var.isValid()) {

					varHash = String::hash(pVarTable->getValue(var->getValue(tempStr)));
					ret = merge_StringHash<StringHash>(varHash, ret);

					toNextSibling(var);
				}
			}
		}

	} else {

		//assrt(false);
	}

	return ret;
}

StringHash RenderDataLoadHelper::generatePoolId(BufferString& tempStr, DataChunk* pChunk, bool includeSourcePath, bool includeChunkPath) {

	if (pChunk == NULL) {

		return 0;
	}


	StringHash ret = 0;
	StringHash varPath = 0;
	StringHash sourcePath = 0;
	
	return pPoolParentChunk->generatePathId(includeSourcePath, includeChunkPath);

		if (includeVarPath == false) {

			return sourcePath;
		}
	}

	return merge_StringHash<StringHash>(sourcePath, varPath);

}

StringHash RenderDataLoadHelper::generatePoolId(BufferString& tempStr, DataChunk* pPoolParentChunk, ScriptVarTable* pVarTable, bool includeSourcePath, bool includeChunkPath, bool includeVarPath) {

	if (pPoolParentChunk == NULL) {

		return 0;
	}


	StringHash ret = 0;
	StringHash varPath = 0;
	StringHash sourcePath = 0;
	
	if (includeSourcePath || includeChunkPath) {

		sourcePath = pPoolParentChunk->generatePathId(includeSourcePath, includeChunkPath);

		if (includeVarPath == false) {

			return sourcePath;
		}
	}

	if (includeVarPath) {

		RefWrap<DataChunk> poolChunk;
		poolChunk.set(pPoolParentChunk->getChild(L"pool"), false);

		varPath = generateVarPathHash(tempStr, poolChunk.ptr(), pVarTable);

		if (includeSourcePath == false && includeChunkPath == false) {

			return varPath;
		}
	}

	return merge_StringHash<StringHash>(sourcePath, varPath);
}


StringHash RenderDataLoadHelper::generatePoolId(BufferString& tempStr, DataChunk* pPoolParentChunk, const TCHAR* sourcePath, ScriptVarTable* pVarTable, bool includeVarPath) {

	if (pPoolParentChunk == NULL) {

		return 0;
	}

	RefWrap<DataChunk> var;

	StringHash ret = 0;
	StringHash varPath = 0;
	StringHash sourcePathHash = 0;
	
	if (sourcePath != NULL) {

		sourcePathHash = String::hash(sourcePath);

		if (includeVarPath == false) {

			return sourcePathHash;
		}
	}

	if (includeVarPath) {

		RefWrap<DataChunk> poolChunk;
		poolChunk.set(pPoolParentChunk->getChild(L"pool"), false);

		varPath = generateVarPathHash(tempStr, poolChunk.ptr(), pVarTable);
	}

	return merge_StringHash<StringHash>(sourcePathHash, varPath);
}
*/


/*
StringHash RenderDataLoadHelper::generatePoolIdParent(DataChunk* pPoolIdParentChunk, ScriptVarTable* pVarTable) {

	RefWrap<DataChunk> poolIdChunk;

	if (pPoolIdParentChunk != NULL) {

		poolIdChunk.set(pPoolIdParentChunk->getChild(L"poolId"), false);
	}
	
	StringHash ret = generatePoolId(poolIdChunk.ptr(), pVarTable);

	if ((ret == 0) && (pPoolIdParentChunk != NULL)) {

		//no poolId chunk
		//be nice, this might cuase 2 different resource to get the same poolId!!
		//but we assume the user knows what hes doing
		warn(L"missing pPoolIdChunk, resource clashes might occur");
		ret = pPoolIdParentChunk->getSourcePathId();
	}

	return ret;
}
*/

}