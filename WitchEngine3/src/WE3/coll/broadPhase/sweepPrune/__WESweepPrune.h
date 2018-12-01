#ifndef _WESweepAndPrune_h
#define _WESweepAndPrune_h


#include "../../../object/WEObject.h"
#include "../../../object/WEObjectData.h"
#include "../../../WETL/WETLArray.h"
#include "../../../math/WEVolume.h"
#include "../../../pool/WEReusableObjectPool.h"
#include "../../../WEDataTypes.h"
#include "../../../render/WERenderer.h"
#include "../WEICollDetector.h"

namespace WE {

	typedef CollIndex SweepPruneIndex;
	typedef CollIndex SweepPruneIndexShort;
	
	struct SweepPruneBinding;
	struct SweepPruneBindingRay;

	enum SweepPruneSpanType {

		SPST_Min = 0, SPST_Max = 1, SPST_RayOrigin = 4, SPST_RayPoint = 5
	};

	struct SweepPruneEndPoint {

		SweepPruneBinding* pBind;
		SweepPruneIndex::Type sortIndex;

		float sortValue;
		SweepPruneSpanType type;
	};

	struct SweepPruneSpan {

		SweepPruneEndPoint endPoints[2];
	};

	struct SweepPrunePair {

		SweepPruneBinding* pBind[2];
		SweepPruneIndexShort::Type inBindingIndex[2];

		SweepPruneIndex::Type inCurrPairsIndex;
	};

	typedef WETL::CountedArray<SweepPruneIndexShort::Type, false, SweepPruneIndexShort::Type> SweepPrunePairMarker;

	struct SweepPruneSpans {

		SweepPruneSpan spans[3]; 

		void spansCopyTo(SweepPruneSpan* target);
		//spansCopyTo0 proved to be the fastest with a primitive perf. test
		//void spansCopyTo0(SweepPruneSpan* target);
		//void spansCopyTo1(SweepPruneSpan* target, const short& dimCount);
		//void spansCopyTo2(SweepPruneSpan* target, const short& dimCount);
		//void spansCopyTo3(SweepPruneSpan* target, const short& dimCount);
		//void spansCopyTo4(SweepPruneSpan* target, const short& dimCount);
	};

	struct SweepPruneBinding : SweepPruneSpans {
		
		Object* pObject;
		CollMask mask;

		typedef WETL::CountedArray<SweepPrunePair*, false, SweepPruneIndexShort::Type> CollArray;
		CollArray mColls;
		

		SweepPruneBinding(bool isRay = false);
	};

	enum SweepPruneMarkerEnum {

		SPM_StartedColls = 0, SPM_EndedColls
	};

	struct SweepPruneMarker {

		SweepPrunePairMarker collMarks[2];

		void reserveMarks(const SweepPruneIndexShort::Type& count);
	};

	struct SweepPruneTest : SweepPruneSpans, SweepPruneMarker {

		bool isActive;
		SweepPruneIndex::Type iterator;
		short markerIterator;
	};

	struct SweepPruneState : SweepPruneSpans {
	};
	
	struct SweepPruneEvent {

		bool isStartColl;
		SweepPruneIndexShort::Type pairIndex;
	};

	class SweepPrune : public ICollDetector {
	public:

		SweepPrune();
		virtual ~SweepPrune();

		void destroy();
		void init(bool usePrincipalAxis, short dimCount, SweepPruneIndex::Type objectCount, SweepPruneIndex::Type collCount);
		void initAxis(short dim, const Vector3& axis);
		void initAxis(short dim, Axis axis);

		virtual bool hasBinding(Object& object, ObjectData& data);
		virtual bool createBinding(Object& object, ObjectData& data, const CollMask& mask);
		virtual void destroyBinding(Object& object, ObjectData& data);

		virtual void add(Object& object, ObjectData& data, VolumeStruct& volume);
		virtual void update(Object& object, ObjectData& data, VolumeStruct& volume);
		virtual void remove(Object& object, ObjectData& data);

		virtual bool startPairIterate(void*& iterator);
		virtual bool nextPairIterate(void*& iterator, Object*& pObject1, Object*& pObject2);
		virtual void endPairIterate(void*& iterator);

		virtual bool canIterateObjectColls();
		virtual bool objectHasColls(Object& object, ObjectData& data);
		virtual bool startObjectPairIterate(Object& object, ObjectData& data, void*& iterator);
		virtual bool nextObjectPairIterate(Object& object, ObjectData& data, void*& iterator, Object*& pObjectColl);
		virtual void endObjectPairIterate(void*& iterator);

		virtual void addRay(Object& object, ObjectData& data, const CollMask& mask, Point& origin);
		virtual void updateRay(Object& object, ObjectData& data, Point& origin);
		virtual void fireRay(Object& object, ObjectData& data, Vector& direction, Object*& pHitObject);
		virtual void removeRay(Object& object, ObjectData& data);

		//works only with CoordSys axis'
		//void fireRayDebug(Object& object, ObjectData& data, Vector& _direction, Object*& pHitObject, int& iterCount, int limitIterCount, Renderer& renderer, const RenderColor* pColor);

		virtual void createObjectState(ObjectData& state);
		virtual void destroyObjectState(ObjectData& state);

		virtual void syncStateToObject(Object& object, ObjectData& data, ObjectData& state);
		virtual void syncObjectToState(Object& object, ObjectData& data, ObjectData& state);

		
		virtual void createTest(ObjectData& testData);
		virtual void destroyTest(ObjectData& testData);

		virtual void startTest(Object& object, ObjectData& data, VolumeStruct& testVolume, ObjectData& testData);
		virtual void endTest(Object& object, ObjectData& data, VolumeStruct& testVolume, ObjectData& testData, const bool& accepted);

		virtual bool startTestPairIterate(ObjectData& testData);
		virtual bool nextTestPairIterate(ObjectData& testData, Object* pTestObject, Object*& pCollObject, bool& isAddedCollision);
		virtual void endTestPairIterate(ObjectData& testData);

		virtual void setEventRegister(EventRegister* pEventRegister);
		virtual void clearEvents();
		virtual void getEventPair(const EventPointer& evt, Object*& pObject1, Object*& pObject2, bool& isAddedCollision);

	protected:

		enum SwapDirectionType {

			SDT_Back = -1, SDT_Forward = 1, SDT_None = 0
		};

		enum PreSwapOp {

			PSOP_None = 0, PSOP_StartColl = 1, PSOP_EndColl = -1, PSOP_StartCollRay = 4, PSOP_EndCollRay = -4,
			PSOP_StartCollRay2 = -3, PSOP_EndCollRay2 = 3
		};

		typedef WETL::CountedArray<SweepPruneEndPoint*, false, SweepPruneIndex::Type> EndPointArray;
		typedef ReusableObjectPool<SweepPrunePair, SweepPruneIndex::Type> ReusablePairs;
		typedef WETL::CountedPtrArray<SweepPrunePair*, false, SweepPruneIndex::Type> PairPtrs;
		typedef WETL::CountedArray<SweepPruneEvent, false, EventPointerIndex> Events;
	

	protected:		

		SweepPruneBinding& newBinding(Object& object, ObjectData& data, bool isRay = false);
		void deleteBinding(ObjectData& data);
		inline SweepPruneBinding*& bindingPtr(ObjectData& data);
		inline SweepPruneBinding& binding(ObjectData& data);

		inline SweepPruneState& toState(ObjectData& tasteData);
		inline SweepPruneState*& toStatePtr(ObjectData& tasteData);

		inline SweepPruneTest& toTest(ObjectData& testData);
		inline SweepPruneTest*& toTestPtr(ObjectData& testData);
		void endMarkedCollPairs(SweepPrunePairMarker& marker);

		void onStartColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2, SweepPruneMarker* pMarker);
		void onEndColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2, SweepPruneMarker* pMarker);
		void removeCurrPair(SweepPrunePair* pPair);
		static void removeCollPair(SweepPruneBinding& bind, SweepPrunePair* pPair, const short& indexInPair);
		void removeCollPair(SweepPrunePair* pPair, SweepPruneBinding* pBind1, SweepPruneBinding* pBind2, short bind1IndexInPair);

		inline void _toForwardSwapDir(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& lastIndex);
		inline bool _shoulContinueSwapping(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& sortIndex, SweepPruneIndex::Type& compIndex, SweepPruneIndex::Type& lastIndex, bool& didSwap);
		inline bool _needSwap(SwapDirectionType& dir, SweepPruneEndPoint& endPoint, SweepPruneIndex::Type& compIndex, EndPointArray& sortArray);
		inline void _preSwapOp(short& dim, SwapDirectionType& dir, SweepPruneEndPoint& endPoint, SweepPruneEndPoint& crossedEndPoint, SweepPruneMarker* pMarker);
		void _swap(short& dim, SweepPruneEndPoint& endPoint, EndPointArray& sortArray, SweepPruneIndex::Type& compIndex, bool& didSwap, SweepPruneIndex::Type& lastIndex, SweepPruneMarker* pMarker, const bool& floatOver);
		void _computeCollisions(SweepPruneSpan* spans, VolumeStruct& volume, SweepPruneMarker* pMarker, const bool& floatOver);
		
		void _blindSwap(SweepPruneEndPoint& endPoint, EndPointArray& sortArray, SweepPruneIndex::Type& targetIndex);
		void _blindMove(SweepPruneSpan* spans, SweepPruneSpan* targetSpans);

		inline void swapEndPoints(EndPointArray& arr, SweepPruneIndex::Type index1, SweepPruneIndex::Type index2);

		/*
		inline void _swapBackMin(SweepPruneEndPoint* pEndPoint, EndPointArray* pSortArray, 
							 SweepPruneIndex& compIndex, bool& didSwap);
		inline void _swapBackMax(SweepPruneEndPoint* pEndPoint, EndPointArray* pSortArray, 
							 SweepPruneIndex& compIndex, bool& didSwap);
		inline void _swapForwardMin(SweepPruneEndPoint* pEndPoint, EndPointArray* pSortArray, 
							 SweepPruneIndex& compIndex, SweepPruneIndex& lastIndex);
		inline void _swapForwardMax(SweepPruneEndPoint* pEndPoint, EndPointArray* pSortArray, 
							 SweepPruneIndex& compIndex, SweepPruneIndex& lastIndex);

							 */
	protected:

		short mDimCount;
		bool mUsePrincipalAxis;
		Vector3 mAxis[3];
		Axis mPrincAxis[3];

		/*
		typedef WETL::CountedPtrArray<SweepPruneBinding*, false, SweepPruneIndex> BindingArray;
		BindingArray mBindings;
		typedef WETL::CountedArray<SweepPruneIndex, false, SweepPruneIndex> EmptyIndexArray;
		EmptyIndexArray mEmptyBindings;
		*/
		EndPointArray mEndPoints[3];
		ReusablePairs mReusablePairs;
		PairPtrs mCurrPairs;

		Events mEvents;
		PairPtrs mEventRemovedPairs; 
	};

}


#endif