#include "PitchRegionShape.h"
#include "../Match.h"

DECLARE_INSTANCE_TYPE_NAME(WOF::match::PitchRegionShape, CPitchRegionShape);

namespace WOF { namespace match {

const TCHAR* PitchRegionShape::ScriptClassName = L"CPitchRegionShape";

void PitchRegionShape::declareInVM(SquirrelVM& target) {

	SqPlus::SQClassDef<PitchRegionShape>(ScriptClassName).
		func(&PitchRegionShape::script_getID, L"getID").
		func(&PitchRegionShape::script_contains2D, L"contains2D").
		func(&PitchRegionShape::script_clipTo2D, L"clipTo2D").
		func(&PitchRegionShape::script_clip2D, L"clip2D").
		func(&PitchRegionShape::script_clipByDestTo2D, L"clipByDestTo2D").
		func(&PitchRegionShape::script_clipByDest2D, L"clipByDest2D").
		func(&PitchRegionShape::script_contains, L"contains").
		func(&PitchRegionShape::script_clipTo, L"clipTo").
		func(&PitchRegionShape::script_clip, L"clip").
		func(&PitchRegionShape::script_clip2, L"clip2").
		func(&PitchRegionShape::script_clipByDestTo, L"clipByDestTo").
		func(&PitchRegionShape::script_clipByDest, L"clipByDest");
}

int PitchRegionShape::mAutoIDCounter = 0;

PitchRegionShape::PitchRegionShape() {

	mID = ++mAutoIDCounter;
}

bool PitchRegionShape::contains(Match& match, const CtVector3& point) {

	Vector2 _point;

	match.toScene2D(point, _point);

	return contains(_point);
}

void PitchRegionShape::clip(Match& match, const CtVector3& point, Vector3& result, float height) {

	Vector2 _point;
	Vector2 _result;

	match.toScene2D(point, _point);

	clip(_point, _result);

	match.toScene3D(_result, result, height);
}

int PitchRegionShape::rayIntersect(Match& match, const CtVector3& point, const CtVector3& dir, float result[2]) {

	Vector2 _point;
	Vector2 _dir;
	Vector2 _result;

	match.toScene2D(point, _point);
	match.toScene2D(dir, _dir);

	return rayIntersect(_point, _dir, result);
}

int PitchRegionShape::segIntersect(Match& match, const CtVector3& point, const CtVector3& point2, float result[2]) {

	Vector2 _point;
	Vector2 _point2;
	Vector2 _result;

	match.toScene2D(point, _point);
	match.toScene2D(point2, _point2);

	return segIntersect(_point, _point2, result);
}

		
bool PitchRegionShape::contains(Match& match, const CtVector3& point, Vector3& clippedPoint, float height) {

	Vector2 _point;
	Vector2 _clippedPoint;

	match.toScene2D(point, _point);

	bool ret = contains(_point, _clippedPoint);

	match.toScene3D(_clippedPoint, clippedPoint, height);

	return ret;
}

bool PitchRegionShape::clipByDest(Match& match, CtVector3& point, CtVector3& dest, Vector3& result, float height) {

	Vector2 _point;
	Vector2 _dest;
	Vector2 _result;

	match.toScene2D(point, _point);
	match.toScene2D(dest, _dest);

	bool ret = clipByDest(_point, _dest, _result);

	match.toScene3D(_result, result, height);

	return ret;
}


bool PitchRegionShape::clipByDest(CtVector2& point, CtVector2& dest, Vector2& result) {

	bool containsDest = contains(dest);

	if (containsDest) {

		result = dest;
		return false;
	}

	bool containsPoint = contains(point);
	
	Vector2 dir;
	float inter[2];

	dest.subtract(point, dir);
	
	int interCount = rayIntersect(point, dir, inter);

	if (interCount > 0) {

		int interIndex = 0;

		if (inter[0] == 0.0f && interCount > 1)
			interIndex = 1;

		point.addMultiplication(dir, inter[interIndex], result);

			
	} else {

		clip(dest, result);
	}

	return true;
}

void PitchRegionShape::renderSprite(Renderer& renderer, RenderSprite& sprite, float scaleX, float scaleY, float scaleMain) {

	if (prepareSprite(sprite, scaleX, scaleY, scaleMain)) {

		sprite.render2D(renderer);
	}
}

bool PitchRegionShape::contains(const CtVector2& point, Vector2& clippedPoint) {

	if (this->contains(point)) {

		clippedPoint = point;
		return true;
	}

	this->clip(point, clippedPoint);
	
	return false;
}


bool PitchRegionShape_Circle::prepareSprite(RenderSprite& sprite, float scaleX, float scaleY, float scaleMain) {

	sprite.setSize((scaleMain) * shape().radius * 2.0f, (scaleMain) * shape().radius * 2.0f);
	sprite.setOrientation2D(0.0f, 1.0f);

	const CtVector2& center = shape().center;

	sprite.setPosition2D(center.x * scaleX, center.y * scaleY);

	return true;
}

bool PitchRegionShape_AAQuad::prepareSprite(RenderSprite& sprite, float scaleX, float scaleY, float scaleMain) {

	sprite.setSize((scaleX) * shape().xSize(), (scaleY) * shape().ySize());
	sprite.setOrientation2D(0.0f, 1.0f);

	Vector2 center;
	shape().center(center);

	sprite.setPosition2D(center.x * scaleX, center.y * scaleY);

	return true;
}

bool PitchRegionShape_Quad::prepareSprite(RenderSprite& sprite, float scaleX, float scaleY, float scaleMain) {

	sprite.setSize((scaleX) * shape().extents.x * 2.0f, (scaleY) * shape().extents.y * 2.0f);
	const CtVector2& orient = shape().coordFrame.row[Scene2D_Forward];
	sprite.setOrientation2D(orient.x, orient.y);

	const CtVector2& center = shape().coordFrame.row[2];

	sprite.setPosition2D(center.x * scaleX, center.y * scaleY);

	return true;
}

} }