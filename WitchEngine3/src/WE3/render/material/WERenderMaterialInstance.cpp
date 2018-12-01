#include "WERenderMaterialInstance.h"

#include "WERenderMaterial.h"

namespace WE {

RenderMaterialInstance::RenderMaterialInstance() {
}

RenderMaterialInstance::~RenderMaterialInstance() {
}

RenderMaterialInstance* RenderMaterialInstance::createClone() {

	SoftPtr<RenderMaterialInstance> ret;

	MMemNew(ret.ptrRef(), RenderMaterialInstance());

	ret->mTexture.set(mTexture, true);
	ret->mSpecularMap.set(mSpecularMap, true);
	ret->mMaterial.set(mMaterial, true);

	return ret;
}

}