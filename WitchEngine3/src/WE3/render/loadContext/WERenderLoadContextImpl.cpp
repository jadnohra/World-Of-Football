#include "WERenderLoadContextImpl.h"

namespace WE {

RenderLoadContextImpl::RenderLoadContextImpl() {

	mTextureSourceImpl.setPool(&mTexturePool);
	mMaterialSourceImpl.setPool(&mMaterialPool);

	setTextureSource(&mTextureSourceImpl);
	setMaterialSource(&mMaterialSourceImpl);
}

RenderLoadContextImpl::~RenderLoadContextImpl() {
}

}