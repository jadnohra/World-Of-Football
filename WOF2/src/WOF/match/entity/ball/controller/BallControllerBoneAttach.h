#ifndef h_WOF_match_BallControllerBoneAttach
#define h_WOF_match_BallControllerBoneAttach

#include "WE3/math/WEVector.h"
#include "WE3/skeleton/WESkeletonBone.h"
using namespace WE;

#include "BallController.h"

namespace WOF { namespace match { 

	class BallControllerBoneAttach : public BallController {
	public:

		BallControllerBoneAttach();

		void setAttachOffset(const Vector& offset);
		void attachTo(BoneInstance* pBone);
		bool isAttached();
		BoneInstance* getAttachedBone();

		virtual void setPosition(const Point& pos);

		virtual int getControllerType();

		virtual void frameMove(Match& match, const Clock& time, Time& outEndTime);

	protected:

		SoftPtr<BoneInstance> mBone;

		Vector3 mAttachOffset;
	};

} } 

#endif