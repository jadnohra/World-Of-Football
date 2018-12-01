#ifndef _PitchRegion_h_WOF_match
#define _PitchRegion_h_WOF_match

#include "WE3/WETL/WETLArray.h"
#include "WE3/string/WEBufferString.h"
#include "WE3/data/WEDataSource.h"
using namespace WE;

#include "../DataTypes.h"
#include "PitchRegionShape.h"

namespace WOF { namespace match {

	class Team;

	class PitchRegion : public PitchRegionShape {
	public:

		void add(PitchRegionShape* pShape);

		virtual bool contains(const CtVector2& point);
		virtual void clip(const CtVector2& point, Vector2& result); 
		virtual int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]); 
		virtual int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]); 

		virtual void renderSprite(Renderer& renderer, RenderSprite& sprite, float scaleX = 1.0f, float scaleY = 1.0f, float scaleMain = 1.0f);

	protected:

		void mergeU(float tempResult[2], int& tempResultCount, float result[2], int& resultCount);

	protected:

		typedef WETL::PtrArrayEx<PitchRegionShape, unsigned int> Shapes;

		Shapes mShapes;
	};


	class PitchRegionFactory {
	public:

		static PitchRegionShape* createRegion(BufferString& tempStr, DataChunk* pChunk, Team* pTeam);
	};

} }

#endif