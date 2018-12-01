#ifndef _UI_h_WOF_match
#define _UI_h_WOF_match

#include "WE3/string/WEString.h"
#include "WE3/binding/directx/9/dxut9/includeDXUT9.h"
#include "WE3/WETL/WETLArray.h"
#include "WE3/render/WERenderer.h"
#include "WE3/data/WEDataSource.h"
#include "WE3/coordSys/WECoordSysConv.h"
#include "WE3/render/WERenderSprite.h"
using namespace WE;

namespace WOF { namespace match {

	class Match;

	class UITextRequest {
	public:

		String text;
		float x;
		float y;
	};


	class UI {
	public:

		String& addUITextRequest(const TCHAR* text, float x, float y);
		inline void add(RenderSprite* pSprite) { mSprites.addOne(pSprite); }

	public:

		UI();
		~UI();

		void load(Match& match, BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv);

		void render(Renderer& renderer, ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont);

	protected:

		typedef WETL::CountedPtrArrayEx<UITextRequest, unsigned int, WETL::ResizeDouble, true> UITextRequests;
		typedef WETL::CountedPtrArrayEx<RenderSprite, unsigned int> Sprites;

		UITextRequests mUITextRequests;
		Sprites mSprites;
	};

} }

#endif