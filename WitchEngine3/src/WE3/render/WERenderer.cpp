#include "WERenderer.h"

namespace WE {

const RenderStateValue kRenderStateTrue = (RenderStateValue) true;
const RenderStateValue kRenderStateFalse = (RenderStateValue) false;

const RenderColor RenderColor::kBlack(0.0f, 0.0f, 0.0f);
const RenderColor RenderColor::kWhite(1.0f, 1.0f, 1.0f);
const RenderColor RenderColor::kRed(1.0f, 0.0f, 0.0f);
const RenderColor RenderColor::kGreen(0.0f, 1.0f, 0.0f);
const RenderColor RenderColor::kBlue(0.0f, 0.0f, 1.0f);
const RenderColor RenderColor::kYellow(1.0f, 1.0f, 0.0f);
const RenderColor RenderColor::kMagenta(1.0f, 0.0f, 1.0f);
const RenderColor RenderColor::kBluish1(0.176f, 0.196f, 0.666f);
const RenderColor RenderColor::kHotPink(255, 105, 180);
const RenderColor RenderColor::kDeepPink(255, 20, 147);
const RenderColor RenderColor::kLightPink(255, 182, 193);

RenderColor::RenderColor() {
}

RenderColor::RenderColor(float r, float g, float b, float _alpha) {

	set(r, g, b, _alpha);
}

RenderColor::RenderColor(int r, int g, int b, int _alpha) {

	set(r, g, b, _alpha);
}

RenderColor::RenderColor(int scale, int alpha) {

	set(scale, alpha);
}

void RenderColor::set(float r, float g, float b, float _alpha) {

	el[RC_Red] = r;
	el[RC_Green] = g;
	el[RC_Blue] = b;
	alpha = _alpha;
}


void RenderColor::set(int r, int g, int b, int _alpha) {

	el[RC_Red] = (float) r / 255.0f;
	el[RC_Green] = (float) g / 255.0f;
	el[RC_Blue] = (float) b / 255.0f;
	alpha = (float) _alpha / 255.0f;
}

void RenderColor::set(float scale, float _alpha) {

	set(scale, scale, scale, _alpha);
}

void RenderColor::set(int scale, int _alpha) {

	set(scale, scale, scale, _alpha);
}

void RenderColor::toColorComplement() {

	el[RC_Red] = 1.0f - el[RC_Red];
	el[RC_Green] = 1.0f - el[RC_Green];
	el[RC_Blue] = 1.0f - el[RC_Blue];
}

RenderBufferUsage::RenderBufferUsage(bool isDynamic, bool isWriteOnly) {

	dynamic = isDynamic;
	writeOnly = isWriteOnly;
}

bool RenderBufferUsage::equals(const RenderBufferUsage& comp) const {

	return (dynamic == comp.dynamic) && (writeOnly == comp.writeOnly);
}

RenderVertexBlendingQuery::RenderVertexBlendingQuery() {

	memset(this, 0, sizeof(RenderVertexBlendingQuery));
}

RenderVertexBlendingQuery::~RenderVertexBlendingQuery() {

	if (pRenderer) {
		pRenderer->freeQuery(*this);
	}

}

RenderMaterialOptions::RenderMaterialOptions() {

	memset(this, 0, sizeof(RenderMaterialOptions));
}

float RenderViewportInfo::getAspectRatio() {

	return (float) width / (float) height;
}

Renderer::Renderer() {

	//pConverter = NULL;

	mIsValidViewTransform2D = false;
	mTransform2DFlipY = false;
}

Renderer::~Renderer() {

	//setConverter(NULL);
}

/*
void Renderer::setDefaultConverter(CoordSys& semRenderer) {

	CoordSys semWE;
	
	getStockCoordSys(SCSST_WitchEngine, semWE);
	getSemantics(semRenderer);
	
	CoordSysConv* pConverter = createConverterFor(semWE, semRenderer, 1.0f);
	setConverter(pConverter);
}

void Renderer::setConverter(CoordSysConv* pConvert) {

	MMemDelete(pConverter);
	
	pConverter = pConvert;
}
*/

RenderStateValue Renderer::toRenderStateValue(const RenderColor& color) {
	
	RenderStateValue ret;

	toRenderStateValue(color, ret);
	
	return ret;
}

void Renderer::drawVol(VolumeStruct& vol, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	switch (vol.type) {
		case V_Sphere:
			draw(vol.toSphere(), pWorldMatrix, pColor);
			break;
		case V_AAB:
			draw(vol.toAAB(), pWorldMatrix, pColor);
			break;
		case V_Box:
		case V_OBB:
			draw(vol.toOBB(), pWorldMatrix, pColor);
			break;
		case V_Triangle:
			draw(vol.toTriangle(), pWorldMatrix, pColor);
			break;
		//case V_TriangleNormals:
		//	draw(vol.toTriangleNormals(), worldMatrix, pColor);
		//	break;
	}
}

void Renderer::drawQueuedVols() {

	if (mQueuedVolumes.isValid() == true) {

		for (QueuedVolumes::Index i = 0; i < mQueuedVolumes->count; ++i) {

			SoftPtr<QueuedVolume> queuedVol = mQueuedVolumes->el[i];

			drawVol(queuedVol->volume, 
						queuedVol->hasMatrix ? &queuedVol->matrix : &Matrix4x3Base::kIdentity, 
						queuedVol->hasColor ? &queuedVol->color : NULL);
									
		}

		mQueuedVolumes->count = 0;
	}
}

void Renderer::enableQueuedVols(bool enable) {

	if (enable) {

		if (mQueuedVolumes.isValid() != true) {

			WE_MMemNew(mQueuedVolumes.ptrRef(), QueuedVolumes());
		}

	} else {

		mQueuedVolumes->destroy();
	}
}

void Renderer::queueVol(VolumeStruct& vol, const Matrix4x3Base* pWorldMatrix, const RenderColor* pColor) {

	if (mQueuedVolumes.isValid()) {

		SoftPtr<QueuedVolume> queuedVol = mQueuedVolumes->addNewOne();
		
		queuedVol->volume.init(vol);
		
		queuedVol->hasMatrix = (pWorldMatrix != NULL);
		if (queuedVol->hasMatrix) {

				queuedVol->matrix = dref(pWorldMatrix);
		}

		queuedVol->hasColor = (pColor != NULL);
		if (queuedVol->hasColor) {

				queuedVol->color = dref(pColor);
		}
	}
}

void Renderer::setViewTransform2D(const Matrix4x3Base* pTrans, bool flipY) {

	if (pTrans) {

		mIsValidViewTransform2D = true;
		mViewTransform2D = dref(pTrans);

	} else {
		
		mIsValidViewTransform2D = false;
	}

	mTransform2DFlipY = flipY;
}


void Renderer::draw2D(const RenderQuad& quad, const Matrix4x3Base* pWorldTrans, Render2DScaling viewportScaling, const RenderColor* pColor, RenderTexture* pTexture, bool fixCoordOffsets) {

	const RenderQuad* transformedQuad;
	RenderQuad transformedQuadHolder(false);

	unsigned int flipIndices [] = {3, 2, 1, 0};

	if (pWorldTrans || mIsValidViewTransform2D || mTransform2DFlipY) {

		transformedQuad = &transformedQuadHolder;
		bool isCopied = false;

		if (pWorldTrans) {

			isCopied = true;

			pWorldTrans->transformPoint(quad.points[0], transformedQuadHolder.points[0]);
			pWorldTrans->transformPoint(quad.points[1], transformedQuadHolder.points[1]);
			pWorldTrans->transformPoint(quad.points[2], transformedQuadHolder.points[2]);
			pWorldTrans->transformPoint(quad.points[3], transformedQuadHolder.points[3]);
		}

		if (mIsValidViewTransform2D) {

			if (isCopied) {

				mViewTransform2D.transformPoint(transformedQuadHolder.points[0]);
				mViewTransform2D.transformPoint(transformedQuadHolder.points[1]);
				mViewTransform2D.transformPoint(transformedQuadHolder.points[2]);
				mViewTransform2D.transformPoint(transformedQuadHolder.points[3]);

			} else {

				isCopied = true;

				mViewTransform2D.transformPoint(quad.points[0], transformedQuadHolder.points[0]);
				mViewTransform2D.transformPoint(quad.points[1], transformedQuadHolder.points[1]);
				mViewTransform2D.transformPoint(quad.points[2], transformedQuadHolder.points[2]);
				mViewTransform2D.transformPoint(quad.points[3], transformedQuadHolder.points[3]);
			}
		}

		if (mTransform2DFlipY) {

			float viewportHeight = 0.0f;
			getViewportWidth(false, &viewportHeight);

			if (isCopied) {

				transformedQuadHolder.points[0].y = viewportHeight - transformedQuadHolder.points[0].y;
				transformedQuadHolder.points[1].y = viewportHeight - transformedQuadHolder.points[1].y;
				transformedQuadHolder.points[2].y = viewportHeight - transformedQuadHolder.points[2].y;
				transformedQuadHolder.points[3].y = viewportHeight - transformedQuadHolder.points[3].y;

			} else {

				isCopied = true;

				//this can also be achieved by using a 4x4 transform and mofifying it to make the flip
				transformedQuadHolder.points[0].x = quad.points[0].x;
				transformedQuadHolder.points[0].y = viewportHeight - quad.points[0].y;
				transformedQuadHolder.points[0].z = quad.points[0].z;
				transformedQuadHolder.points[1].x = quad.points[1].x;
				transformedQuadHolder.points[1].y = viewportHeight - quad.points[1].y;
				transformedQuadHolder.points[1].z = quad.points[1].z;
				transformedQuadHolder.points[2].x = quad.points[2].x;
				transformedQuadHolder.points[2].y = viewportHeight - quad.points[2].y;
				transformedQuadHolder.points[2].z = quad.points[2].z;
				transformedQuadHolder.points[3].x = quad.points[3].x;
				transformedQuadHolder.points[3].y = viewportHeight - quad.points[3].y;
				transformedQuadHolder.points[3].z = quad.points[3].z;
			}

			//swapt(transformedQuadHolder.points[0], transformedQuadHolder.points[2]);
			//swapt(transformedQuadHolder.points[1], transformedQuadHolder.points[3]);
		}


	} else {

		transformedQuad = &quad;
	}

	this->draw2DQuad(dref(transformedQuad), viewportScaling, pColor, pTexture, fixCoordOffsets, mTransform2DFlipY ? flipIndices : NULL);
}

}