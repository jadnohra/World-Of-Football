#ifndef _WECoordSysConv_Swizzle_h
#define _WECoordSysConv_Swizzle_h

#include "WECoordSysConv.h"

namespace WE {

	class CoordSysConv_Swizzle : public CoordSysConv {
	public:

		bool convDir;
		short swizzleSrcToTarget[3];
		bool negSrcToTarget[3];

		short swizzleTargetToSrc[3];
		bool negTargetToSrc[3];

		bool invertAngles;


		bool convUnit;
		float unitSourceToTarget;
		float unitTargetToSource;

	public:
		
		CoordSysConv_Swizzle();
		virtual ~CoordSysConv_Swizzle();

		void init(const CoordSys& src, const CoordSys& dest);

		virtual bool isConverting() const;

		virtual void toTargetPoint(const float* from, float* to) const;
		virtual void toTargetVector(const float* from, float* to) const;
		virtual void toTargetRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) const;
		virtual void toTargetFaceIndices(const RenderUINT* from, RenderUINT* to) const;
		virtual void toTargetSwizzle(const float* from, float* to) const;

		virtual void toTargetPoint(float* vect) const;
		virtual void toTargetVector(float* vect) const;
		virtual void toTargetRotation(float* axis, float& angle) const;
		virtual void toTargetFaceIndices(RenderUINT* indices) const;
		virtual void toTargetSwizzle(float* vect) const;


		virtual void toSourcePoint(const float* from, float* to) const;
		virtual void toSourceVector(const float* from, float* to) const;
		virtual void toSourceRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) const;
		virtual void toSourceFaceIndices(const RenderUINT* from, RenderUINT* to) const;
		virtual void toSourceSwizzle(const float* from, float* to) const;

		virtual void toSourcePoint(float* vect) const;
		virtual void toSourceVector(float* vect) const;
		virtual void toSourceRotation(float* axis, float& angle) const;
		virtual void toSourceFaceIndices(RenderUINT* indices) const;
		virtual void toSourceSwizzle(float* vect) const;

		virtual void toTargetUnits(float& units) const;
		virtual void fromTargetUnits(float& units) const;

	protected:

		void scale(float* values, const float& scale) const;
		
		void scale(float& value, const float& scale) const {
			value *= scale;
		}

	};

}

#endif