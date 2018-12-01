#ifndef _WESkeleton_h
#define _WESkeleton_h

#include "../WETL/WETLArray.h"
#include "../string/WEString.h"
#include "../render/WERenderer.h"
#include "../geometry/WEGeometryIndexBuffer.h"
#include "../geometry/WEGeometryVertexBuffer.h"
#include "WESkeletonBone.h"

#include "WESkeletonDataTypes.h"
#include "../binding/directx/9/d3dx9/includeD3DX9.h"

namespace WE {

	
	struct AnimationKey : BoneTransformation {

		float time;
	};

	struct AnimationTrack {

		WETL::CountedArray<AnimationKey, false, AnimationIndex::Type> mKeys;
		
		UINT boundId;

		AnimationTrack();
		~AnimationTrack();
		
		
		void destroy();
		void init(UINT size);

		AnimationKey* addKey();
	};

	struct Animation {

		WETL::CountedPtrArray<AnimationTrack*, true, AnimationIndex::Type> mTracks;
		
		String mName;

		float mLength;
		//float mSpeed;
		bool mLoop;

		Animation();
		~Animation();
		
		void destroy();
		void init(UINT size);

		AnimationTrack* addTrack();
	};

	struct AnimationGroup {

		WETL::CountedPtrArray<Animation*, true, AnimationIndex::Type> mAnimations;
		
		AnimationGroup();
		~AnimationGroup();
		
		void destroy();
		void init(UINT size);

		Animation* addAnimation();
	};
	
	struct TrackAnimState {

		AnimationIndex::Type keyIndex;
		
		TrackAnimState();

		void reset();
	};

	class Skeleton;

	struct TrackAnimCache {

		typedef WETL::Array<TrackAnimState, false, AnimationIndex::Type> States;

		AnimationIndex::Type animIndex;
		States mStates;

		TrackAnimCache();

		void init(Skeleton& skeleton);
	};

	struct SkeletonBlendAnim {

		AnimationIndex anim;
		float weight;
	};
	
	class Skeleton : public Ref {
	friend class SkeletonInstance;
	public:

		typedef WETL::StaticArray<Bone, false, BoneIndex::Type> BoneArray;
		typedef WETL::HashMap<StringHash, AnimationIndex::Type, unsigned int, WETL::ResizeExact, 0> AnimMap;
		typedef WETL::HashMap<unsigned int, Bone*, unsigned int, WETL::ResizeExact, 0> BoneMapByGameID;

		BoneArray mBoneArray;
		BoneMapByGameID mBoneMapByGameID;
		AnimationGroup mAnimationGroup;
		AnimMap mAnimMap;

		CoordSys mCoordSys;

		float mBoneBoxScale;

		void updateRecurse(BoneNodeRecurseContext& context);
		void stopAnimating();

		void getInterpolatedKey(Animation* pAnim, float time, 
				AnimationIndex::Type track, AnimationIndex::Type key, AnimationKey* pKey, Bone** ppBone);

		bool gatherBlendState(AnimationIndex::Type anim, AnimationIndex::Type track, float time, TrackAnimCache* pTrackAnimCache, Bone** ppBone, AnimationKey& key);
		void setState(AnimationIndex::Type anim, float time, TrackAnimCache* pTrackAnimCache, float weight, bool initBones, bool finalizeBones);

	public:
		
		Skeleton();
		~Skeleton();

		void destroy();

		void applyCoordSys(CoordSysConvPool& convPool, const CoordSys& coordSys, bool setAsStartPose);

		/*
			The Problem with matching mesh and skeleton transformatiosn is that a mesh's
			vertices are absolute positions, whereas a skeleton's vertices (bone positions)
			are relation to a root, like a scene tree, the 'correct' way to transform a skeleton
			is to build the absolute bone postitions (like world transform in a scene tree)
			transform those, and then find the new relative position of each node to its parent
			using those as the new values. (a scene tree could probably be used for this).

			the same should probably be done with every key of the animations, pretty 
			expensive process probably
		*/
		void transform(const Matrix4x3Base* pTrans, bool setAsStartPose);

		void addToBoneBoundingBoxes(GeometryVertexBuffer& VB, GeometryIndexBuffer& IB, bool byFace = false, short maxInfluence = 2, float minWeight = 0.3f);
		void prepareForRenderer(Renderer& renderer, CoordSysConvPool& convPool, bool setAsStartPose = true);

		Bone* getBoneByBlendIndex(BoneIndex::Type blendIndex);
		inline Bone& getBone(const BoneIndex::Type& paletteIndex) { return (mBoneArray[paletteIndex]); }
		Bone* findBoneByGameName(const TCHAR* name);

		BoneIndex::Type getBoneCount();
		AnimationIndex::Type getAnimationCount();
		const TCHAR* getAnimationName(AnimationIndex::Type anim);
		float getAnimationLength(AnimationIndex::Type anim);

		inline Animation& getAnimation(AnimationIndex::Type anim) {

			return dref(mAnimationGroup.mAnimations.el[anim]);
		}

		inline bool getAnimationIndex(const TCHAR* name, AnimationIndex::Type& index) {

			return mAnimMap.find(String::hash(name), index);
		}

		void setAsStartPose();
		void updateRecurse(SkeletonInstanceBoneBinding* pInstanceBinding);
		bool createAnimMap();

		//void setState(AnimationIndex::Type anim, float time, TrackAnimCache* pTrackAnimCache = NULL, float weight = 1.0f);
		//void setState(AnimationIndex::Type anim, float time, TrackAnimCache* pTrackAnimCache, float weight, unsigned int blendAnimCount, SkeletonBlendAnim* blendAnims, int stride);
		void setState(AnimationIndex::Type anim, float time, TrackAnimCache* pTrackAnimCache = NULL, float weight = 1.0f, unsigned int blendAnimCount = 0, SkeletonBlendAnim* blendAnims = NULL, int stride = 0);


	};
	

}

#endif