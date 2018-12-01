#include "WEGeometryTriBufferContainer.h"

namespace WE {

GeometryTriBufferContainer::GeometryTriBufferContainer() 
	: mTriCount(0) {
}

GeometryTriBufferContainer::Index GeometryTriBufferContainer::getTriCount() {

	if (mTriCount == 0 && mTriBuffers.size) {
		
		mTriCount = 0;

		for (Index bufferIndex = 0; bufferIndex < mTriBuffers.size; ++bufferIndex) {

			TriBuffer& buff = dref(mTriBuffers.el[bufferIndex]);
			mTriCount += buff.getCount();
		}
	}

	return mTriCount;
}

void GeometryTriBufferContainer::render(Renderer& renderer, const RenderColor* pColor) {

	for (Index i = 0; i < mTriBuffers.size; ++i) {

		mTriBuffers.el[i]->render(renderer, pColor);
	}
}

void GeometryTriBufferContainer::simplify(const TriBuffer::Simplification* pSettings) {

	for (Index i = 0; i < mTriBuffers.size; ++i) {

		mTriBuffers.el[i]->simplify(pSettings);
	}
}

void GeometryTriBufferContainer::addToVol(AAB& vol) {

	for (Index i = 0; i < mTriBuffers.size; ++i) {

		mTriBuffers.el[i]->addToVol(vol);
	}
}

}

