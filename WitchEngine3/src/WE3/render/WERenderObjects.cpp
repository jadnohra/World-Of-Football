#include "WERenderObjects.h"

#include "../WEMacros.h"

namespace WE {

const DimMap_2_3 DimMap_2_3::k; 

const RenderQuad RenderQuad::kUnit(true);

RenderQuad::AnchorEnum fromString(const String& str, RenderQuad::AnchorEnum defaultVal) {

	if (str.equals(L"start") || str.equals(L"left") || str.equals(L"top")) {

		return RenderQuad::Anchor_Start;

	} else if (str.equals(L"end") || str.equals(L"right") || str.equals(L"bottom")) {

		return RenderQuad::Anchor_End;

	} else if (str.equals(L"center")) {

		return RenderQuad::Anchor_Center;
	}

	return defaultVal;
}

RenderQuad::RenderQuad(float val) {

	points[0].set(val);
	points[1].set(val);
	points[2].set(val);
	points[3].set(val);
}

RenderQuad::RenderQuad(bool setUnit, AnchorEnum anchorX, AnchorEnum anchorY, const DimMap_2_3& dimMap) {

	if (setUnit) {

		points[0].el[dimMap.mapNone] = 0.0f;
		points[1].el[dimMap.mapNone] = 0.0f;
		points[2].el[dimMap.mapNone] = 0.0f;
		points[3].el[dimMap.mapNone] = 0.0f;

		setupBySize(1.0f, 1.0f, anchorX, anchorY, dimMap);
	}
}

void RenderQuad::setupBySize(float sizeX, float sizeY, AnchorEnum anchorX, AnchorEnum anchorY, const DimMap_2_3& dimMap) {


	sizeX *= 0.5f;
	sizeY *= 0.5f;

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	switch (anchorX) {

		case Anchor_Start: offsetX = sizeX; break;
		case Anchor_End: offsetX = -sizeX; break;
	}

	switch (anchorY) {

		case Anchor_Start: offsetY = sizeY; break;
		case Anchor_End: offsetY = -sizeY; break;
	}

	points[0].el[dimMap.mapX] = -sizeX + offsetX;
	points[0].el[dimMap.mapY] = -sizeY + offsetY;
	//points[0].el[dimMap.mapNone] = 0.0f;

	points[1].el[dimMap.mapX] = sizeX + offsetX;
	points[1].el[dimMap.mapY] = -sizeY + offsetY;
	//points[1].el[dimMap.mapNone] = 0.0f;

	points[2].el[dimMap.mapX] = sizeX + offsetX;
	points[2].el[dimMap.mapY] = sizeY + offsetY;
	//points[2].el[dimMap.mapNone] = 0.0f;

	points[3].el[dimMap.mapX] = -sizeX + offsetX;
	points[3].el[dimMap.mapY] = sizeY + offsetY;
	//points[3].el[dimMap.mapNone] = 0.0f;
}

void RenderQuad::getSize(float& sizeX, float& sizeY, const DimMap_2_3& dimMap) {

	sizeX = points[1].el[dimMap.mapX] - points[0].el[dimMap.mapX];
	sizeY = points[2].el[dimMap.mapY] - points[1].el[dimMap.mapY];
}

void RenderQuad::setOffset(float offset, const DimMap_2_3& dimMap) {

	points[0].el[dimMap.mapNone] = offset;
	points[1].el[dimMap.mapNone] = offset;
	points[2].el[dimMap.mapNone] = offset;
	points[3].el[dimMap.mapNone] = offset;
}


}