#ifndef h_WOF_match_Scanner
#define h_WOF_match_Scanner

#include "WE3/WEPTr.h"
#include "WE3/render/WERenderer.h"
#include "WE3/render/WERenderSprite.h"
using namespace WE;

#include "DataTypes.h"

namespace WOF { namespace match {

	class Match;


	class Scanner {
	public:

		Scanner();

		bool load(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void render(Match& match, Renderer& renderer);

		inline const float& getDefaultFootballerAlpha() { return mFootballerAlpha; }

	protected:

		void updateSpriteSizes(Renderer& renderer);

	protected:

		float mOffsetHoriz;
		float mOffsetVert;
		float mSize;
		
		float mFootballerScale;
		float mBallScale;
		float mFootballerAlpha;

		RenderSprite mScannerSprite;
		RenderSprite mFootballerSprite;
		RenderSprite mPlayerFootballerSprite;
		RenderSprite mBallSprite;
	};

} }

#endif

