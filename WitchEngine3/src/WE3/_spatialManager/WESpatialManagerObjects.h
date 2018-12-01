#ifndef _WESpatialManagerObjects_h
#define _WESpatialManagerObjects_h

#include "../object/WEObject.h"
#include "../object/WEObjectData.h"
#include "../WEPtr.h"
#include "../WEDataTypes.h"
#include "../math/WETriangle.h"
#include "../WETL/WETLArray.h"
#include "../WEAbstractContainer.h"
#include "../render/WERenderer.h"

namespace WE {

	struct ObjectData2 {

		SoftPtr<Object> owner;
		ObjectData data;

		ObjectData2(Object* pOwner = NULL) : owner(pOwner) {};

		void bindObject(Object* pOwner) { owner = pOwner; };
		void unbindObject() { owner.destroy(); };

		bool isBoundObject() { return owner.isValid(); }

		void bindData(void* pData) { data.ptr = pData; };
		void unbindData() { data.destroy(); };

		bool isBoundData() { return data.isValid(); }
	};

	
	typedef AbstractContainer::ElementType SpatialType;

	class SpatialManagerTypeDefs {
	public:

		typedef AbstractContainer::Index Index;
		typedef TriangleEx1 Tri1;
	};

	class SpatialManagerTypeBase : public SpatialManagerTypeDefs {
	public:

		enum DefaultTypeEnum {

			DT_None = -1, DT_Tri1
		};

	public:

		SpatialManagerTypeBase() 
			: mType_Tri1(DT_None) {

			setType_Tri1();
		}

		void setType_Tri1(SpatialType type = DT_Tri1) { mType_Tri1 = type; }
		inline const SpatialType& getType_Tri1() const { return mType_Tri1; }

	protected:

		SpatialType mType_Tri1;
	};


	class SpatialObjectBase : public ObjectData2 {
	public:

		SpatialObjectBase(Object* pOwner = NULL) : ObjectData2(pOwner) {};
	};

	template<typename ContainerT>
	struct PartialSpatialContainer {

		typedef typename AbstractContainer::ElementAddressBatch Batch;
		typedef typename SpatialManagerTypeDefs::Index Index;

		SoftPtr<ContainerT> container;
		HardPtr<Batch> batch;

		void cloneTo(PartialSpatialContainer<ContainerT>& target) {

			target.container = container;

			if (batch.isValid() && container.isValid()) {

				target.batch = container->createElementAddressBatch();
				batch->cloneTo(target.batch);
			}
		};

		typedef WETL::PtrArray<PartialSpatialContainer<ContainerT>*, true, Index> PtrArrayT;

		class PtrArray : public PtrArrayT {
		public:
			
			typedef PartialSpatialContainer<ContainerT>* ElTypeT;

			Index elementCount;

			void updateElementCount() {

				elementCount = 0;

				for (Index i = 0; i < size; ++i) {

					elementCount += el[i]->batch->getElementCount();
				}
			}

			void cloneTo(PtrArray& target) {

				target.elementCount = elementCount;

				target.reserve(size);

				for (Index i = 0; i < size; ++i) {

					WE_MMemNew(target.el[i], PartialSpatialContainer<ContainerT>());
					el[i]->cloneTo(dref(target.el[i]));
				}
			}

			void addOne(ContainerT* pContainer, Batch* pBatch) {

				ElTypeT& obj = PtrArrayT::addOne();
				WE_MMemNew(obj, PartialSpatialContainer<ContainerT>());
				
				obj->container = pContainer;
				obj->batch = pBatch;
			}
		};
	};

	class SpatialAbstractContainer : public SpatialObjectBase, public AbstractContainer, public SpatialManagerTypeBase {
	public:

		typedef AbstractContainer::Index Index;

		struct Simplification {

			enum Mode {

				M_Planar = 0
			};

			enum LoopMode {

				LM_Iterate = 0, LM_ClosestToCenter
			};

			int stepLimit;

			Mode mode;
			LoopMode loopMode;
			bool considerAesthetics;
			bool useIslands;

			float planarAngle;

			Simplification() {

				stepLimit = -1;

				mode = M_Planar;
				loopMode = LM_Iterate;

				considerAesthetics = true;
				useIslands = true;

				planarAngle = degToRad(2.5f);
			}
		};

	public:

		SpatialAbstractContainer(Object* pOwner) : SpatialObjectBase(pOwner) {};

		virtual void render(Renderer& renderer, bool renderMain = true, const RenderColor* pColor = NULL, bool renderDetails = false, const RenderColor* pDetailColor = NULL) {};
		virtual void simplify(const Simplification* pSettings) {};
	};

	class SpatialTriContainer : public SpatialAbstractContainer {
	public:

		typedef PartialSpatialContainer<SpatialTriContainer> Partial;

	public:

		SpatialTriContainer(Object* pOwner = NULL) : SpatialAbstractContainer(pOwner) {};
	};

	typedef SpatialTriContainer::Partial PartialSpatialTriContainer;


	class SpatialVolume : public SpatialObjectBase, public SpatialManagerTypeDefs {
	public:

		SpatialVolume(Object* pOwner = NULL) : SpatialObjectBase(pOwner) {};

		const Volume& spatialGetVolume() const { return mSpatialVolume; }
		Volume& spatialVolume() { return mSpatialVolume; }

	protected:

		Volume mSpatialVolume;
	};


	class SpatialDynamicVolume;

	class SpatialManagerCell : public SpatialManagerTypeDefs {
	public:

		virtual Index getTriContainerCount() = 0;
		virtual SpatialTriContainer::Partial& getTriContainer(const Index& index) = 0;

		virtual Index getVolumeCount() = 0;
		virtual SpatialVolume& getVolume(const Index& index) = 0;

		virtual Index getDynamicVolumeCount() = 0;
		virtual SpatialDynamicVolume& getDynamicVolume(const Index& index) = 0;
	};

	class SpatialDynamicVolume : public SpatialObjectBase, public SpatialManagerTypeDefs {
	public:

		SpatialDynamicVolume(Object* pOwner = NULL) : SpatialObjectBase(pOwner) {};

		const Volume& spatialGetVolume() const { return mSpatialVolume; }
		Volume& spatialVolume() { return mSpatialVolume; }

	/*
	public:

		virtual void spatialStartUpdate(void*& pOutContext) = 0;

		virtual void spatialClearCells(void* pContext) = 0;
		virtual void spatialEnteredCell(SpatialManagerCell& cell, void* pContext) = 0;
		virtual void spatialLeftCell(SpatialManagerCell& cell, void* pContext) = 0;

		virtual void spatialEndUpdate(void* pContext) = 0;
	*/
	protected:

		Volume mSpatialVolume;
	};

	

	class SpatialEventListener {
	public:

		virtual void spatialStartUpdate(SpatialDynamicVolume& dynObject, void*& pOutContext) = 0;

		virtual void spatialClearCells(SpatialDynamicVolume& dynObject, void* pContext) = 0;
		virtual void spatialEnteredCell(SpatialDynamicVolume& dynObject, SpatialManagerCell& cell, void* pContext) = 0;
		virtual void spatialLeftCell(SpatialDynamicVolume& dynObject, SpatialManagerCell& cell, void* pContext) = 0;

		virtual void spatialEndUpdate(SpatialDynamicVolume& dynObject, void* pContext) = 0;
	};
}

#endif