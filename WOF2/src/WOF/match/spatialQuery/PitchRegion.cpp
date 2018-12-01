#include "PitchRegion.h"

namespace WOF { namespace match {

void PitchRegion::add(PitchRegionShape* pShape) {

	mShapes.addOne(pShape);
}

bool PitchRegion::contains(const CtVector2& point) {

	for (Shapes::Index i = 0; i < mShapes.size; ++i) {

		if (mShapes[i]->contains(point))
			return true;
	}

	return false;
}

void PitchRegion::clip(const CtVector2& point, Vector2& result) {

	Vector2 tempResult;

	bool resultIsValid = false;
	float resultDistSq = 0.0f;

	for (Shapes::Index i = 0; i < mShapes.size; ++i) {

		if (!mShapes[i]->contains(point)) {

			mShapes[i]->clip(point, tempResult);

			if (resultIsValid) {

				float distSq = distanceSq(point, tempResult);

				if (distSq < resultDistSq) {

					result = tempResult;
					resultDistSq = distSq;
				}

			} else {

				result = tempResult;
				resultDistSq = distanceSq(point, result);
				resultIsValid = true;
			}
		}
	}
}

void PitchRegion::mergeU(float tempResult[2], int& tempResultCount, float result[2], int& resultCount) {

	for (int i = 0; i < tempResultCount; ++i) {

		if (resultCount == 0) {

			result[resultCount] = tempResult[i];
			++resultCount;

		} else if (resultCount == 1) {

			if (tempResult[i] < result[resultCount]) {

				result[1] = result[0];
				result[0] = tempResult[i];
				++resultCount;

			} else if (tempResult[i] > result[resultCount]) {

				result[1] = tempResult[i];
				++resultCount;
			}

		} else if (resultCount == 2) {

			if (tempResult[i] < result[0]) {

				result[0] = tempResult[i];

			} else if (tempResult[i] > result[1]) {

				result[1] = tempResult[i];
			}
		}
	}
}

int PitchRegion::rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) {

	int resultCount = 0;

	float tempResult[2];
	int tempResultCount = 0;

	bool isInsideOfAll = true;

	for (Shapes::Index i = 0; i < mShapes.size; ++i) {

		if ((tempResultCount = mShapes[i]->rayIntersect(point, dir, tempResult)) > 0) {

			mergeU(tempResult, tempResultCount, result, resultCount);
		}

		if (tempResultCount != 1)
			isInsideOfAll = false;
	}

	if (isInsideOfAll && resultCount == 2) {

		result[0] = result[1];
		resultCount = 1;
	}
	
	return resultCount;
}

int PitchRegion::segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) {

	int resultCount = 0;

	float tempResult[2];
	int tempResultCount = 0;

	for (Shapes::Index i = 0; i < mShapes.size; ++i) {

		if ((tempResultCount = mShapes[i]->segIntersect(point, point2, tempResult)) > 0) {

			mergeU(tempResult, tempResultCount, result, resultCount);
		}
	}

	return resultCount;
}

void PitchRegion::renderSprite(Renderer& renderer, RenderSprite& sprite, float scaleX, float scaleY, float scaleMain) {

	for (Shapes::Index i = 0; i < mShapes.size; ++i) {

		mShapes[i]->renderSprite(renderer, sprite, scaleX, scaleY, scaleMain);
	}
}

} }

#include "WE3/helper/load/WEMathDataLoadHelper.h"
#include "../Team.h"

namespace WOF { namespace match {

PitchRegionShape* PitchRegionFactory::createRegion(BufferString& tempStr, DataChunk* pChunk, Team* pTeam) {

	SoftPtr<PitchRegionShape> ret;

	if (pChunk && pChunk->getAttribute(L"type", tempStr)) {

		if (tempStr.equals(L"Box") || tempStr.equals(L"AAQuad")) {

			SoftPtr<PitchRegionShape_AAQuad> shape;
			bool wasInited = true;

			WE_MMemNew(shape.ptrRef(), PitchRegionShape_AAQuad());

			if (!MathDataLoadHelper::extract(tempStr, dref(pChunk), shape->shape().min, true, true, false, L"min")) {

				wasInited = false;
			}

			if (!MathDataLoadHelper::extract(tempStr, dref(pChunk), shape->shape().max, true, true, false, L"max")) {

				wasInited = false;
			}

			if (pTeam) {
				
				pTeam->teamLocalToWorld(shape->shape().min);
				pTeam->teamLocalToWorld(shape->shape().max);
			}

			for (int i = 0; i < 2; ++i) {

				if (shape->shape().min.el[i] > shape->shape().max.el[i]) {
					swapt(shape->shape().min.el[i], shape->shape().max.el[i]);
				}
			}

			if (!wasInited) {

				WE_MMemDelete(shape.ptrRef());
			}

			ret = shape.ptr();

		} else if (tempStr.equals(L"Cricle")) {

			SoftPtr<PitchRegionShape_Circle> shape;
			bool wasInited = true;

			WE_MMemNew(shape.ptrRef(), PitchRegionShape_Circle());

			if (!MathDataLoadHelper::extract(tempStr, dref(pChunk), shape->shape().center, true, true, false, L"center")) {

				wasInited = false;
			}

			if (!pChunk->scanAttribute(tempStr, L"radius", L"%f", &shape->shape().radius)) {

				wasInited = false;
			}

			if (pTeam) {
				
				pTeam->teamLocalToWorld(shape->shape().center);
			}

			if (!wasInited) {

				WE_MMemDelete(shape.ptrRef());
			}

			ret = shape.ptr();
		} 
	}

	if (ret.isValid() && pChunk) {
	
		pChunk->getAttribute(L"name", ret->name());
	}

	return ret;
}


} }