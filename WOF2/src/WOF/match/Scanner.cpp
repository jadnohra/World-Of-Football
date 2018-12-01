#include "Scanner.h"

#include "WE3/WEAppPath.h"
#include "WE3/filePath/WEPathResolver.h"
#include "WE3/helper/load/WERenderDataLoadHelper.h"
using namespace WE;

#include "match.h"
#include "Team.h"
#include "inc/Entities.h"
#include "spatialQuery/PitchRegion.h"

namespace WOF { namespace match {

Scanner::Scanner() : mScannerSprite(true, true, true)
					, mFootballerSprite(true, false, true)
					, mPlayerFootballerSprite(true, false, true)
					, mBallSprite(true, true, true) {

	mOffsetHoriz = 0.0f;
	mOffsetVert = 0.0f;
	
	mSize = 0.15f;

	mScannerSprite.color() = RenderColor(1.0f, 1.0f, 1.0f, 0.5f);
	
	mFootballerScale = 0.05f;
	mFootballerAlpha = 0.75;

	mBallSprite.color() = RenderColor(1.0f, 1.0f, 1.0f, 0.5f);
	mBallScale = 0.025f;
}

bool Scanner::load(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {


	chunk.scanAttribute(tempStr, L"offsetHoriz", L"%f", &mOffsetHoriz);
	chunk.scanAttribute(tempStr, L"offsetVert", L"%f", &mOffsetVert);
	chunk.scanAttribute(tempStr, L"size", L"%f", &mSize);

	RenderDataLoadHelper::extract(tempStr, chunk, mScannerSprite.color(), true, L"color", 0.5f);

	mScannerSprite.skin().load(tempStr, &chunk, match.getLoadContext().render(), 0.5f, NULL, L"color", L"textureFilePath", NULL, L"blendType");

	/*
	if (chunk.getAttribute(L"textureFilePath", tempStr)) {

		chunk.resolveFilePath(tempStr);

		mScannerSprite.loadTexture(tempStr.c_tstr(), match.getLoadContext().render(), true);
	}
	*/

	chunk.scanAttribute(tempStr, L"footballerSize", L"%f", &mFootballerScale);

	chunk.scanAttribute(tempStr, L"footballerAlpha", L"%f", &mFootballerAlpha);

	if (chunk.getAttribute(L"footballerTextureFilePath", tempStr)) {

		chunk.resolveFilePath(tempStr);

		mFootballerSprite.loadTexture(tempStr.c_tstr(), match.getLoadContext().render(), true);
	}

	if (chunk.getAttribute(L"playerFootballerTextureFilePath", tempStr)) {

		chunk.resolveFilePath(tempStr);

		mPlayerFootballerSprite.loadTexture(tempStr.c_tstr(), match.getLoadContext().render(), true);

	} else if (chunk.getAttribute(L"footballerTextureFilePath", tempStr)) {

		mFootballerSprite.loadTexture(tempStr.c_tstr(), match.getLoadContext().render(), true, NULL);
	}

	if (chunk.getAttribute(L"ballTextureFilePath", tempStr)) {

		chunk.resolveFilePath(tempStr);

		mBallSprite.loadTexture(tempStr.c_tstr(), match.getLoadContext().render(), true);
	}


	chunk.scanAttribute(tempStr, L"ballSize", L"%f", &mBallScale);
	RenderDataLoadHelper::extract(tempStr, chunk, mBallSprite.skin().color(), true, L"ballColor", 0.5f);

	updateSpriteSizes(match.getRenderer());

	return true;
}

void Scanner::updateSpriteSizes(Renderer& renderer) {

	mScannerSprite.setSize(mSize * renderer.getViewportWidth(true));

	float sizeX;
	float sizeY;

	mScannerSprite.getSize(sizeX, sizeY);
	mFootballerSprite.setSize(sizeX * mFootballerScale);
	mPlayerFootballerSprite.setScale(mFootballerSprite.getScaleForSize(sizeX * mFootballerScale));
	mBallSprite.setSize(sizeX * mBallScale);
}

void Scanner::render(Match& match, Renderer& renderer) {

	if (!match.mTweakShowScanner)
		return;

	RenderViewportInfo viewport;

	if (!renderer.getViewportInfo(viewport)) {

		return;
	}

	if (renderer.wasReset()) {

		updateSpriteSizes(renderer);
	}

	SceneTransform scannerViewTransform;
	scannerViewTransform.identity();

	SceneDirectionEnum camLookDir;
	bool camLookNeg;

	float scannerSizeX; 
	float scannerSizeY; 

	mScannerSprite.getSize(scannerSizeX, scannerSizeY);

	match.getActiveCamera().getMainDirection2D(match, &camLookDir, &camLookNeg, NULL);

	if (camLookDir == Scene_Forward) {

		scannerViewTransform.setOrientation2D(0.0f, camLookNeg ? -1.0f : 1.0f, true);

		scannerViewTransform.position().x = (mOffsetHoriz * (float) viewport.width) + 0.5f * scannerSizeX;
		scannerViewTransform.position().y = 0.5f * (float) viewport.height; 

	} else if (camLookDir == Scene_Right) {

		scannerViewTransform.setOrientation2D(camLookNeg ? 1.0f : -1.0f, 0.0f, true);

		scannerViewTransform.position().x = 0.5f * (float) viewport.width;
		scannerViewTransform.position().y = (mOffsetVert * (float) viewport.height) + 0.5f * scannerSizeX; //((1.0f - (mOffsetVert)) * (float) viewport.height) - 0.5f * scannerSizeY;

	} else {

		assrt(false);
		return;
	}

	renderer.setViewTransform2D(&scannerViewTransform, true);
		
	mScannerSprite.render2D(renderer);

	const PitchInfo& pitchInfo = match.getPitchInfo();

	SoftPtr<Footballer> testFootballer;

	for (int ti = 0; ti < 2; ++ti) {

		Team& team = match.getTeam((TeamEnum) ti);
		SoftPtr<RenderSprite::Skin> teamSkin = team.getScannerSkin();

		FootballerIndex iter;
		SoftPtr<Footballer> footballer;

		while (team.iterateFootballers(footballer.ptrRef(), iter)) {

			SoftPtr<Player> player = footballer->getPlayer();
			SoftPtr<RenderSprite> sprite;
			SoftPtr<RenderSprite::SoftSkin> skin;

			if (player.isValid()) {

				if (!testFootballer.isValid())
					testFootballer = footballer;

				sprite = &mPlayerFootballerSprite;
				skin = &player->getSkin().scanner;

			} else {

				sprite = &mFootballerSprite;
			}

			const Vector3& pos = footballer->mTransformWorld.getPosition();
			const Vector3& orient = footballer->mTransformWorld.getDirection(Scene_Forward);

			sprite->setPosition2D((0.5f * scannerSizeX) * tcap(pos.el[Scene_Right] / pitchInfo.halfWidth, -1.0f, 1.0f), 
											(0.5f * scannerSizeY) * tcap(pos.el[Scene_Forward] / pitchInfo.halfLength, -1.0f, 1.0f));

			sprite->setOrientation2D(orient.el[Scene_Right], orient.el[Scene_Forward]);

			if (skin.isValid())
				sprite->render2D(renderer, skin.dref());
			else 
				sprite->render2D(renderer, teamSkin.dref());
		}
	}
	

	{
		Ball& ball = match.getBall();	

		const Vector3& pos = ball.getPos();

		mBallSprite.setPosition2D((0.5f * scannerSizeX) * tcap(pos.el[Scene_Right] / pitchInfo.halfWidth, -1.0f, 1.0f), 
									(0.5f * scannerSizeY) * tcap(pos.el[Scene_Forward] / pitchInfo.halfLength, -1.0f, 1.0f));

		mBallSprite.render2D(renderer);
	}

	
	/*
	{
		
		float pitchWidthRatio = match.getPitchInfo().width / match.getPitchInfo().length;
		float shapeSpriteScaleY = 0.5f * scannerSizeY;
		float shapeSpriteScaleX = (0.5f * scannerSizeX) / pitchWidthRatio;

		if (testFootballer.isValid()) {

			Vector2 footballerPos;
			Vector2 footballerDir;

			{
				const Vector3& pos = testFootballer->mTransformWorld.getPosition();
				const Vector3& orient = testFootballer->mTransformWorld.getDirection(Scene_Forward);

				footballerPos.x = pitchWidthRatio * (pos.el[Scene_Right] / pitchInfo.halfWidth);
				footballerPos.y = (pos.el[Scene_Forward] / pitchInfo.halfLength);

				footballerDir.x = orient.el[Scene_Right];
				footballerDir.y = orient.el[Scene_Forward];

			}

			RenderSprite& footballerSprite = match.getShapeSprite_Circle();

			HardPtr<PitchRegionShape_AAQuad> testShapeAAQuad;
			HardPtr<PitchRegionShape_Circle> testShapeCircle;
			HardPtr<PitchRegionShape_Quad> testShapeQuad;
			PitchRegion testShapeComplex;

			{
				WE_MMemNew(testShapeAAQuad.ptrRef(), PitchRegionShape_AAQuad());

				testShapeAAQuad->shape().min.set(-1.0f * pitchWidthRatio, -1.0f);
				testShapeAAQuad->shape().max.set(1.0f * pitchWidthRatio, 1.0f);
			}

			{
				WE_MMemNew(testShapeQuad.ptrRef(), PitchRegionShape_Quad());

				testShapeQuad->shape().extents.set(0.4f * pitchWidthRatio, 0.6f);
				testShapeQuad->shape().coordFrame.identity();

				testShapeQuad->shape().coordFrame.setRotate(RigidMatrix3x2::Axis_X, degToRad(45.0f));
				testShapeQuad->shape().position().set(0.1f * pitchWidthRatio, 0.2f);
			}

			{
				WE_MMemNew(testShapeCircle.ptrRef(), PitchRegionShape_Circle());

				testShapeCircle->shape().center.set(-0.1f * pitchWidthRatio, 0.2f);
				testShapeCircle->shape().radius = 0.35f;
			}

			{
				//testShapeComplex.add(testShapeAAQuad);
				//testShapeAAQuad.ptrRef() = NULL;

				testShapeComplex.add(testShapeCircle);
				testShapeCircle.ptrRef() = NULL;


				testShapeComplex.add(testShapeQuad);
				testShapeQuad.ptrRef() = NULL;
			}
			
			{
				//RenderSprite& shapeSprite = match.getShapeSprite_Quad();
				//PitchRegionShape_AAQuad& testShape = testShapeAAQuad;
				//RenderSprite& shapeSprite = match.getShapeSprite_Circle();
				//PitchRegionShape_Circle& testShape = testShapeCircle;
				//RenderSprite& shapeSprite = match.getShapeSprite_Quad();
				//PitchRegionShape& testShape = testShapeQuad;
				RenderSprite& shapeSprite = match.getShapeSprite_Quad();
				PitchRegionShape& testShape = testShapeComplex;
				
				bool showClipped = false;

				if (testShape.contains(footballerPos)) {

					shapeSprite.skin().color().set(1.0f, 0.0f, 0.0f, shapeSprite.skin().color().alpha);

				} else {

					showClipped = true;
					shapeSprite.skin().color().set(0.0f, 0.0f, 1.0f, shapeSprite.skin().color().alpha);
				}

				testShape.renderSprite(renderer, shapeSprite, shapeSpriteScaleX, shapeSpriteScaleY, shapeSpriteScaleY);

				{

					footballerSprite.setPosition2D((0.5f * scannerSizeX / pitchWidthRatio) * footballerPos.x, (0.5f * scannerSizeY) * footballerPos.y);
					footballerSprite.setSize(scannerSizeX * 0.09f);
					footballerSprite.skin().color().set(0.0f, 0.0f, 1.0f, footballerSprite.skin().color().alpha);
					
					footballerSprite.render2D(renderer);
				}

				{
					float inter[2];
					int interCount;
					Vector2 interPos;

					interCount = testShape.rayIntersect(footballerPos, footballerDir, inter);

					for (int i = 0; i < interCount; ++i) {

						testShape.getRayPoint(footballerPos, footballerDir, inter[i], interPos);

						footballerSprite.setPosition2D((0.5f * scannerSizeX / pitchWidthRatio) * interPos.x, (0.5f * scannerSizeY) * interPos.y);
						footballerSprite.setSize(scannerSizeX * 0.05f);
						footballerSprite.skin().color().set(1.0f, 1.0f, 0.0f, footballerSprite.skin().color().alpha);
						
						footballerSprite.render2D(renderer);
					}
				}


				if (showClipped) {

					Vector2 clippedPos;

					testShape.clip(footballerPos, clippedPos);

					footballerSprite.setPosition2D((0.5f * scannerSizeX / pitchWidthRatio) * clippedPos.x, (0.5f * scannerSizeY) * clippedPos.y);
					footballerSprite.setSize(scannerSizeX * 0.07f);
					footballerSprite.skin().color().set(1.0f, 0.0f, 1.0f, footballerSprite.skin().color().alpha);
					
					footballerSprite.render2D(renderer);
				}
			}
		}
	}
	*/

	{

		/*
		RenderSprite shapeSprite;

		shapeSprite.enableTransform(true);
		shapeSprite.enableSkin(true);
		shapeSprite.skin().enableColor(true);
		shapeSprite.skin().color().set(1.0f, 1.0f, 1.0f, 0.5f);
		*/

		/*
		{

			RenderSprite& shapeSprite = match.getShapeSprite_Quad();

			PitchRegionShape_AAQuad testShape;
			testShape.shape().min.y = -0.2f;
			testShape.shape().max.y = 0.3f;
			testShape.shape().min.x = -0.3f;
			testShape.shape().max.x = 0.3f;

			testShape.prepareSprite(shapeSprite, 0.5f * scannerSizeY);
			
			shapeSprite.render2D(renderer);
		}

		{
			RenderSprite& shapeSprite = match.getShapeSprite_Circle();

			PitchRegionShape_Circle testShape;
			testShape.shape().center.set(0.6f, 0.4f);
			testShape.shape().radius = 0.15f;

			testShape.prepareSprite(shapeSprite, 0.5f * scannerSizeY);
			
			shapeSprite.render2D(renderer);
		}

		{
			RenderSprite& shapeSprite = match.getShapeSprite_Quad();

			PitchRegionShape_Quad testShape;
			testShape.shape().coordFrame.setRotate(RigidMatrix3x2::Axis_X, degToRad(45.0f));
			testShape.shape().coordFrame.row[2].set(-0.3f, -0.5f);
			testShape.shape().extents.set(0.05f, 0.4f);

			testShape.prepareSprite(shapeSprite, 0.5f * scannerSizeY);
			
			shapeSprite.render2D(renderer);
		}
		*/

	}
	
}


} }