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

	struct SweepPruneSpans {

		SweepPruneSpan spans[3]; 

		void sortValuesCopyTo(SweepPruneSpan* target);
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

		SweepPruneBinding(bool isRay = false);
	};

	
	struct SweepPruneVolCache : SweepPruneSpans {
	};
	
	
	class SweepPrune : public ICollDetector {
	public:

		SweepPrune();
		virtual ~SweepPrune();

		void destroy();
		void init(bool usePrincipalAxis, short dimCount, SweepPruneIndex::Type objectCount);
		void initAxis(short dim, const Vector3& axis);
		void initAxis(short dim, Axis axis);

		virtual bool hasBinding(Object& object, ObjectData& data);
		virtual bool createBinding(Object& object, ObjectData& data, const CollMask& mask);
		virtual void destroyBinding(Object& object, ObjectData& data);

		virtual bool extractCollMask(Object& object, ObjectData& data, CollMask& mask);

		virtual void add(Object& object, ObjectData& data, VolumeStruct& volume);
		virtual void update(Object& object, ObjectData& data, VolumeStruct& volume);
		virtual void update(Object& object, ObjectData& data, VolumeStruct& volume, ObjectData& volumeCache);
		virtual void remove(Object& object, ObjectData& data, bool destroyBinding, bool isRemovingAllObjects);

		
		virtual void addRay(Object& object, ObjectData& data, const CollMask& mask, Point& origin);
		virtual void updateRay(Object& object, ObjectData& data, Point& origin);
		virtual void fireRay(Object& object, ObjectData& data, Vector& direction, Object*& pHitObject);
		virtual void removeRay(Object& object, ObjectData& data, bool isRemovingAllObjects);

		//works only with CoordSys axis'
		//void fireRayDebug(Object& object, ObjectData& data, Vector& _direction, Object*& pHitObject, int& iterCount, int limitIterCount, Renderer& renderer, const RenderColor* pColor);

		virtual void createVolumeCache(ObjectData& cache);
		virtual void destroyVolumeCache(ObjectData& cache);
		virtual void initVolumeCache(VolumeStruct& volume, ObjectData& cache);
		virtual void initVolumeCache(Object& object, ObjectData& data, ObjectData& cache);

		virtual void render(Renderer& renderer, Object& object, ObjectData& data, const RenderColor* pRenderColor);

	protected:

		enum SwapDirectionType {

			SDT_Back = -1, SDT_Forward = 1, SDT_None = 0
		};

		enum PreSwapOp {

			PSOP_None = 0, PSOP_StartColl = 1, PSOP_EndColl = -1, PSOP_StartCollRay = 4, PSOP_EndCollRay = -4,
			PSOP_StartCollRay2 = -3, PSOP_EndCollRay2 = 3
		};

		typedef WETL::CountedArray<SweepPruneEndPoint*, false, SweepPruneIndex::Type> EndPointArray;

	protected:		

		SweepPruneBinding& newBinding(Object& object, ObjectData& data, bool isRay = false);
		void deleteBinding(ObjectData& data);
		inline SweepPruneBinding*& bindingPtr(ObjectData& data);
		inline SweepPruneBinding& binding(ObjectData& data);

		inline SweepPruneVolCache& toCache(ObjectData& cacheData);
		inline SweepPruneVolCache*& toCachePtr(ObjectData& cacheData);

		inline void onStartColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2);
		inline void onEndColl(SweepPruneBinding* pBind1, SweepPruneBinding* pBind2);

		void removeSpansFromEndPoints(SweepPruneSpan* spans);

		inline void _toForwardSwapDir(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& lastIndex);
		inline bool _shoulContinueSwapping(SwapDirectionType& dir, EndPointArray& sortArray, SweepPruneIndex::Type& sortIndex, SweepPruneIndex::Type& compIndex, SweepPruneIndex::Type& lastIndex, bool& didSwap);
		inline bool _needSwap(SwapDirectionType& dir, SweepPruneEndPoint& endPoint, SweepPruneIndex::Type& compIndex, EndPointArray& sortArray);
		inline void _preSwapOp(int& dim, SwapDirectionType& dir, SweepPruneEndPoint& endPoint, SweepPruneEndPoint& crossedEndPoint);
		void _swap(int& dim, SweepPruneEndPoint& endPoint, EndPointArray& sortArray, SweepPruneIndex::Type& compIndex, bool& didSwap, SweepPruneIndex::Type& lastIndex);
		void _swap(SweepPruneSpan* spans);

		inline void _computeSpans(SweepPruneSpan* spans, VolumeStruct& volume);
		inline void _computeRaySpans(SweepPruneSpan* spans, Point& origin);
		
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

		int mDimCount;
		bool mUsePrincipalAxis;
		Vector3 mAxis[3];
		Axis mPrincAxis[3];

		EndPointArray mEndPoints[3];
	};

}


#endif