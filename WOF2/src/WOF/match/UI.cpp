#include "UI.h"

#include "Match.h"

namespace WOF { namespace match {

String& UI::addUITextRequest(const TCHAR* text, float x, float y) {

	SoftPtr<UITextRequest> newReq = mUITextRequests.addNewOne();

	newReq->text.assign(text);
	newReq->x = x;
	newReq->y = y;

	return newReq->text;
}

UI::UI() {
}

UI::~UI() {

	mUITextRequests.count = mUITextRequests.size;
	mUITextRequests.destroy();
}

void UI::load(Match& match, BufferString& tempStr, DataChunk* pChunk, CoordSysConv* pConv) {

	if (pChunk) {

		RenderLoadContext& loadContext = match.getLoadContext().render();

		SoftRef<DataChunk> child = pChunk->getFirstChild();

		while (child.isValid()) {

			SoftPtr<RenderSprite> sprite = mSprites.addNewOne();

			sprite->enableSkin(true);
			sprite->skin().enableColor(true);

			sprite->skin().mBlendType = RenderSprite::Blend_Alpha;
			sprite->skin().mTexFilter = RTF_Anisotropic;

			sprite->load(tempStr, child, loadContext, 1.0f, NULL, 
					L"color", L"texture", L"blendType", L"texFilter", 
					L"size", L"scale", L"alpha", pConv,
					L"pos", L"orient", L"anchor", L"scaleType", L"posType");

			toNextSibling(child);
		}
	}
}

void UI::render(Renderer& renderer, ID3DXSprite* pSprite, ID3DXFont* pFont, ID3DXFont* pBigFont) {

	RenderViewportInfo viewport;

	if (!renderer.getViewportInfo(viewport)) 
		return;

	{

		TEXTMETRIC textMetricsHolder;
		SoftPtr<TEXTMETRIC> textMetrics;
		
		if (pFont->GetTextMetrics(&textMetricsHolder)) {

			textMetrics = &textMetricsHolder;
		}

		float centerX = (float) (viewport.width / 2);
		float centerY = (float) (viewport.height / 2);

		CDXUTTextHelper texter(pFont, pSprite, 20);
		texter.Begin();

		texter.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );

		for (UITextRequests::Index i = 0; i < mUITextRequests.count; ++i) {

			UITextRequest& req = dref(mUITextRequests[i]);

			float offsetX = 0.0f;
			float offsetY = 0.0f;

			if (textMetrics.isValid()) {

				offsetX = -0.5f * ((float)(textMetrics->tmAveCharWidth * req.text.length()));
			}

			texter.SetInsertionPos((int) (offsetX + centerX + req.x * centerX), (int) (offsetY + centerY - req.y * centerY));
			texter.DrawTextLine(req.text.c_tstr());
		}

		mUITextRequests.count = 0;

		texter.End();
	}

	renderer.setViewTransform2D(NULL, false);
	{

		for (Sprites::Index i = 0; i < mSprites.count; ++i) {

			mSprites[i]->render2D(renderer);
		}
	}
}

} }