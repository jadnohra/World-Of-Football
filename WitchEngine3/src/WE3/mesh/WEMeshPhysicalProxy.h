#ifndef _WEMeshPhysicalProxy_h
#define _WEMeshPhysicalProxy_h

#include "WEMeshMaterialGroup.h"
#include "../WETL/WETLArray.h"
#include "../geometry/WEGeometryTriBuffer.h"


namespace WE {

	/*
		This is obviously and simple and restrictive impl, a physical proxy 
		should not be part of mesh ....
	*/

	class MeshPhysicalProxy {
	public:

		typedef unsigned int Index;

		enum MappingType {

			M_None = -1, M_Clone, M_Merge, M_Complex
		};

		typedef GeometryTriBuffer::Simplification Simplification;

		class Part {
		public:

			typedef WETL::CountedArray<MeshMaterialIndex::Type, false, Index, WETL::ResizeExact> MeshMaterials;

			StringHash material;
			MeshMaterials srcMaterials;
		};
	

	public:

		MeshPhysicalProxy() : mMappingType(M_None), mMaterial(0) {}

		void destroy() { mMappingType = M_None; mParts.destroy(); mSimplification.destroy(); }

		void setMappingType(const MappingType& type) { mMappingType = type; }
		void setMaterial(const StringHash& mat) { mMaterial = mat; }

		Part* createPart() { return mParts.addNewOne(); }

		Simplification& createSimplification() { if (!mSimplification.isValid()) WE_MMemNew(mSimplification.ptrRef(), Simplification()); return mSimplification; }
		void destroySimplification() { mSimplification.destroy(); }

		const MappingType& getMappingType() { return mMappingType; }
		const StringHash& getMaterial() { return mMaterial; }
		const Simplification* getSimplification() { return mSimplification; }

		const Index& getPartCount() { return mParts.count; }
		Part* getPart(const Index& i) { return mParts.el[i]; }

	protected:

		typedef WETL::CountedPtrArrayEx<Part, Index, WETL::ResizeExact> Parts;

		MappingType mMappingType;
		StringHash mMaterial; //if M_Merge is used
		Parts mParts;

		HardPtr<Simplification> mSimplification;
	};

}

#endif