#ifndef _WESkeletonInstance_h
#define _WESkeletonInstance_h

#include "WESkeleton.h"
#include "../WERef.h"
#include "../WETime.h"

namespace WE {

	class SkeletonInstance : public Ref {
	public:

		//typedef void (*callbackInitBoneInstance)(const String& bone, bool& contributeToVolume, bool& enableColl, void* context);

		inline const bool& needsUpdateAnim() {
			return mNeedsUpdateAnim;
		}

		inline AAB& dirtyGetLocalDynamicAAB() {
			return binding.dynamicAAB;
		}

	public:

		SkeletonInstance(bool useTrackAnimCache = true);
		~SkeletonInstance();

		void destroy();

		//if NULL allocates and uses own matrix
		void useWorldMatrix(Matrix4x3Base* pExternalWorldMatrix);

		SceneTransforms& getWorldTransformArray();
		AAB& getLocalDynamicAAB();

		void bind(Skeleton* pSkeleton, float boneBoxScale = -1.0f); //-1.0f uses the skeleton mBoneBoxScale
		bool hasValidSkeleton();
		Skeleton* getSkeleton();

		bool isAnimating();
		bool reachedAnimationEnd(); //only for none looped animations
		float getTime(bool loopClipped);

		float getAnimationLength(const AnimationIndex::Type& anim, float speedFactor = 1.0f, bool includeSpeed = true);
		float getRemainingTime() { return (getAnimationLength(mCurrAnim, mSpeedFactor, true) - getTime(true)); }

		inline bool getAnimationIndex(const TCHAR* name, AnimationIndex::Type& index) { return dref(mpSkeleton).getAnimationIndex(name, index); }
		inline AnimationIndex::Type getAnimationCount() { dref(mpSkeleton).getAnimationCount();  }

		void setAnimation(const AnimationIndex::Type& anim, const float& speedFactor);
		void setSpeedFactor(const float& speedFactor);
		void getAnimation(AnimationIndex::Type& anim, float& speedFactor);
		void setLoop(bool loop = true);
		void addTime(const Time& time);
		void setTime(const Time& time);

		inline const AnimationIndex::Type& getAnimation() { return mCurrAnim; }
		inline const float& getSpeedFactor() { return mSpeedFactor; }

		inline const bool& isEnabledColl(const BoneIndex::Type& instanceIndex) { return mpSkeleton->getBone(binding.boneInstances[instanceIndex].paletteIndex).enableColl; }
		inline const bool& isEnabledColl(const BoneInstance& instance) { return mpSkeleton->getBone(instance.paletteIndex).enableColl; }

		//pass NULL when using external matrix to signal matrix change
		void setWorldMatrix(const Matrix4x3Base* pWorldMatrix); 
		void signalExternalWorldMatrixChanged();
		
		//void prepareForRendering();
		bool update();

		void setState(const Matrix4x3Base& worldMatrix);
		bool updateIfNeeded();

		void setDeviceMatrices(Renderer& pRenderer, BYTE* boneIndices, USHORT boneCount);
		void setDeviceMatrixPalette(Renderer& pRenderer);
		void renderBoundingBoxes(Renderer& renderer, const RenderColor* pColor = NULL, const RenderColor* pMarkedColor = NULL);

		void test(const SkeletonInstance* pCollInstance);
		void test(const OBB* pBox);

		BoneInstance* findBoneInstanceByGameName(const TCHAR* name);
		BoneInstance* findBoneInstanceByName(const TCHAR* name);


		void setAnimationWeight(const float& weight);
		void addBlendAnimation(const AnimationIndex::Type& anim, const float& weight);
		void resetBlendAnimations();

	public:

		struct BlendAnim : SkeletonBlendAnim {

			Time length;
			Time time;
			Time loopClippedTime;
			float speedFactor;
		};

		Skeleton* mpSkeleton;

		AnimationIndex mCurrAnim;
		bool mLoop;
		Time mTime;
		Time mLoopClippedTime;
		float mSpeedFactor;
		Time mCurrentLength;
		float mCurrAnimWeight;

		typedef WETL::CountedArray<BlendAnim, false, unsigned int, WETL::ResizeExact> BlendAnims;
		BlendAnims mBlendAnims;

		FlexiblePtr<Matrix4x3Base> mWorldMatrix;
		
		bool mNeedsUpdateAnim;
		bool mNeedsUpdateWorldMatrix;

		SkeletonInstanceBoneBinding binding;

		HardPtr<TrackAnimCache> mTrackAnimCache;
	};

}

#endif