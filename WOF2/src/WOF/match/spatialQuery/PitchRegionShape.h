#ifndef _PitchRegionShape_h_WOF_match
#define _PitchRegionShape_h_WOF_match

#include "WE3/math/WECircle.h"
#include "WE3/math/WEQuad.h"
#include "WE3/render/WERenderSprite.h"
#include "WE3/WERef.h"
#include "WE3/string/WEString.h"
#include "WE3/script/WEScriptEngine.h"
using namespace WE;

#include "../DataTypes.h"

namespace WOF { namespace match {

	class Match;

	class PitchRegionShape {
	public:

		inline String& name() { return mName; }
		inline const String& getName() { return mName; }

	public:

		PitchRegionShape();
		virtual ~PitchRegionShape() {}

		virtual bool contains(const CtVector2& point) { return false; }
		virtual void clip(const CtVector2& point, Vector2& result) {}
		virtual int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) { return 0; }
		virtual int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) { return 0; }

		static inline void getRayPoint(const CtVector2& point, const CtVector2& dir, const float& result, Vector2& resultPoint) {

			point.addMultiplication(dir, result, resultPoint);
		}

		static inline void getSegPoint(const CtVector2& point, const CtVector2& point2, const float& result, Vector2& resultPoint) {

			lerp(point, point2, result, resultPoint);
		}

		virtual bool contains(const CtVector2& point, Vector2& clippedPoint);

		virtual bool prepareSprite(RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f) { return false; }
		virtual void renderSprite(Renderer& renderer, RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f);

		bool clipByDest(CtVector2& point, CtVector2& dest, Vector2& result); //returns true if was clipped

	public:


		bool contains(Match& match, const CtVector3& point);
		void clip(Match& match, const CtVector3& point, Vector3& result, float height);
		int rayIntersect(Match& match, const CtVector3& point, const CtVector3& dir, float result[2]);
		int segIntersect(Match& match, const CtVector3& point, const CtVector3& point2, float result[2]);

		static inline void getRayPoint(const CtVector3& point, const CtVector3& dir, const float& result, Vector3& resultPoint, float height) {

			point.addMultiplication(dir, result, resultPoint);
			resultPoint.el[Scene_Up] = height;
		}

		static inline void getSegPoint(const CtVector3& point, const CtVector3& point2, const float& result, Vector3& resultPoint, float height) {

			lerp(point, point2, result, resultPoint);
			resultPoint.el[Scene_Up] = height;
		}
		
		bool contains(Match& match, const CtVector3& point, Vector3& clippedPoint, float height);

		bool clipByDest(Match& match, CtVector3& point, CtVector3& dest, Vector3& result, float height); //returns true if was clipped

	protected:

		static int mAutoIDCounter;
		
		String mName;
		int mID;

	public:

		inline int script_getID() { return mID; }

		inline bool script_contains2D(CtVector2& point) { return contains(point); }

		inline void script_clipTo2D(CtVector2& point, Vector2& result) { clip(point, result); }
		inline void script_clip2D(Vector2& point) { clip(Vector2(point), point); }

		inline void script_clipByDestTo2D(CtVector2& point, CtVector2& dest, Vector2& result) { clipByDest(point, dest, result); }
		inline void script_clipByDest2D(Vector2& point, CtVector2& dest) { script_clipByDestTo2D(Vector2(point), dest, point); }

		inline bool script_contains(Match& match, CtVector3& point) { return contains(match, point); }

		inline void script_clipTo(Match& match, CtVector3& point, Vector3& result, float height) { clip(match, point, result, height); }
		inline void script_clip2(Match& match, Vector3& point, float height) { clip(match, Vector3(point), point, height); }
		inline void script_clip(Match& match, Vector3& point) { clip(match, Vector3(point), point, point.el[Scene_Up]); }

		inline void script_clipByDestTo(Match& match, CtVector3& point, CtVector3& dest, Vector3& result, float height) { clipByDest(match, point, dest, result, height); }
		inline void script_clipByDest(Match& match, Vector3& point, CtVector3& dest, float height) { script_clipByDestTo(match, Vector3(point), dest, point, height); }

	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	template<typename BaseShapeT>
	class PitchRegionShapeT_BaseShape : public PitchRegionShape {
	public:

		inline BaseShapeT& shape() { return mShape; }
		inline const BaseShapeT& getShape() { return mShape; }

		virtual bool contains(const CtVector2& point) { return mShape.contains(point); }
		virtual void clip(const CtVector2& point, Vector2& result) { mShape.clip(point, result); }
		virtual int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]) { return mShape.rayIntersect(point, dir, result); }
		virtual int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]) { return mShape.segIntersect(point, point2, result); }

	protected:

		BaseShapeT mShape;
	};

	class PitchRegionShape_Circle : public PitchRegionShapeT_BaseShape<Circle> {
	public:

		virtual bool prepareSprite(RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f);
	};

	class PitchRegionShape_AAQuad : public PitchRegionShapeT_BaseShape<AAQuad> {
	public:

		virtual bool prepareSprite(RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f);
	};

	class PitchRegionShape_Quad : public PitchRegionShapeT_BaseShape<Quad> {
	public:

		virtual bool prepareSprite(RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f);
	};

} }

#endif