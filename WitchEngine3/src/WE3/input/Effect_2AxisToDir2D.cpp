#include "Effect_2AxisToDir2D.h"

#include "SourceHolder.h"
#include "InputManager.h"
#include "ComponentUtil.h"
#include "../math/WEMathUtil.h"

namespace WE { namespace Input {

bool Effect_2AxisToDir2D::create(InputManager& manager, Source& axis1, Source& axis2, const TCHAR* name, SourceHolder& source) {

	SoftRef<Effect_2AxisToDir2D> ret;

	WE_MMemNew(ret.ptrRef(), Effect_2AxisToDir2D(manager, axis1, axis2, name));

	if (!ret->init()) {

		ret.destroy();
	}

	source.set(ret);

	return source.isValid();
}

Effect_2AxisToDir2D::Effect_2AxisToDir2D(InputManager& manager, Source& axis1, Source& axis2, const TCHAR* name) 
	: mManager(&manager), mAxis1(&axis1), mAxis2(&axis2), mName(name) {
}


bool Effect_2AxisToDir2D::init() {

	SoftPtr<Component_Generic1D> compAxis1 = comp1D(mAxis1);
	SoftPtr<Component_Generic1D> compAxis2 = comp1D(mAxis2);

	if( compAxis1.isValid() && compAxis2.isValid()
			&& mAxis1->addTracker(mManager, this) 
			&& mAxis2->addTracker(mManager, this)) {

		setValues(compAxis1->getValue(), compAxis2->getValue(), false);

		return true;
	}

	return false;
}

Effect_2AxisToDir2D::~Effect_2AxisToDir2D() {

	mAxis1->removeTracker(mManager, this);
	mAxis2->removeTracker(mManager, this);
}

void Effect_2AxisToDir2D::onSourceChanged(Source* pVarImpl) {

	SoftPtr<Component_Generic1D> compAxis1 = comp1D(mAxis1);
	SoftPtr<Component_Generic1D> compAxis2 = comp1D(mAxis2);

	setValues(compAxis1->getValue(), compAxis2->getValue(), true);
}

float Effect_2AxisToDir2D::getRawValue1() {

	return comp1D(mAxis1)->getValue();
}

float Effect_2AxisToDir2D::getRawValue2() {

	return comp1D(mAxis2)->getValue();
}

ResponseCurve& Effect_2AxisToDir2D::createResponseCurve1() {

	if (!mCurves[0].isValid()) {
	
		WE_MMemNew(mCurves[0].ptrRef(), ResponseCurve(1.0f));
	}

	return mCurves[0].dref();
}

ResponseCurve& Effect_2AxisToDir2D::createResponseCurve2() {

	if (!mCurves[1].isValid()) {
	
		WE_MMemNew(mCurves[1].ptrRef(), ResponseCurve(1.0f));
	}

	return mCurves[1].dref();
}

void Effect_2AxisToDir2D::destroyResponseCurve1() {

	mCurves[0].destroy();
}

void Effect_2AxisToDir2D::destroyResponseCurve2() {

	mCurves[1].destroy();
}

void Effect_2AxisToDir2D::onResponseCurvesChanged() {

	onSourceChanged(mAxis1);
}

void Effect_2AxisToDir2D::setValues(const float& v1, const float& v2, const bool& notify) {

	float oldValues[2];
	
	if (notify) {

		oldValues[0] = mValues[0];
		oldValues[1] = mValues[1];
	}
	
	mValues[0] = mCurves[0].isValid() ? mCurves[0]->map(v1) : v1;
	mValues[1] = mCurves[1].isValid() ? mCurves[1]->map(v2) : v2;


	float magSq = mValues[0] * mValues[0] + mValues[1] * mValues[1];
	
	/*
	if (magSq > 0.0f) { 
	
		float oneOverMag = 1.0f / sqrtf(magSq);

		mValues[0] *= oneOverMag;
		mValues[1] *= oneOverMag;
	}
	*/
	
	if (mValues[1] == 0.0f) {

		if (mValues[0] != 0.0f)
			mValues[0] = v1 >= 0.0f ? 1.0f : -1.0f;

	} else {

		float mv0 = mValues[0];
		float mv1 = mValues[1];

		bool v0Larger = fabs(mValues[0]) >= fabs(mValues[1]);
		bool v0Neg = mValues[0] < 0.0f;
		bool v1Neg = mValues[1] < 0.0f;

		float ratio = fabs(mValues[0]) / fabs(mValues[1]);
		bool reverse = ratio > 1.0f;

		if (reverse)
			ratio = 1.0f / ratio;

		float angle = 0.5f * kPiOver2 * ratio;

		float x = reverse ? fabs(cosf(angle)) : fabs(sinf(angle));
		float y = reverse ? fabs(sinf(angle)) : fabs(cosf(angle));

		bool xLarger = x >= y;

		if (v0Larger == xLarger) {

			mValues[0] = v0Neg ? -x : x;
			mValues[1] = v1Neg ? -y : y;

		} else {

			mValues[0] = v0Neg ? -y : y;
			mValues[1] = v1Neg ? -x : x;
		}
	}
	
	if (notify && (oldValues[0] != mValues[0] || oldValues[1] != mValues[1]))
		notifyTrackers();
}

} }