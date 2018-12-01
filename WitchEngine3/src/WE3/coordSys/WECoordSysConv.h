#ifndef _WECoordSysConv_h
#define _WECoordSysConv_h

#include "WECoordSys.h"
#include "../render/WERenderDataTypes.h"
#include "../WERef.h"
#include "../math/WEAAB.h"

namespace WE {

	class CoordSysConv : public Ref {
	public:

		virtual ~CoordSysConv();

		//returns fals if this converter 'does nothing'
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

		void toTargetVolume(AAB& aab);


	public:

		static void _toTargetPoint(const float* from, float* to) ;
		static void _toTargetVector(const float* from, float* to) ;
		static void _toTargetRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) ;
		static void _toTargetFaceIndices(const RenderUINT* from, RenderUINT* to) ;
		static void _toTargetSwizzle(const float* from, float* to) ;

		static inline void _toTargetPoint(float* vect) {}
		static inline void _toTargetVector(float* vect) {}
		static inline void _toTargetRotation(float* axis, float& angle) {}
		static inline void _toTargetFaceIndices(RenderUINT* indices) {}
		static inline void _toTargetSwizzle(float* vect) {}

		static void _toSourcePoint(const float* from, float* to) ;
		static void _toSourceVector(const float* from, float* to) ;
		static void _toSourceRotation(const float* fromAxis, float fromAngle, float* toAxis, float& toAngle) ;
		static void _toSourceFaceIndices(const RenderUINT* from, RenderUINT* to) ;
		static void _toSourceSwizzle(const float* from, float* to) ;

		static inline void _toSourcePoint(float* vect) {}
		static inline void _toSourceVector(float* vect) {}
		static inline void _toSourceRotation(float* axis, float& angle) {}
		static inline void _toSourceFaceIndices(RenderUINT* indices) {}
		static inline void _toSourceSwizzle(float* vect) {}

		static inline void _toTargetUnits(float& units) {};
		static inline void _fromTargetUnits(float& units) {};
	};

	

}

#endif