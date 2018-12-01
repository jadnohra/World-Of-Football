#ifndef _WERenderElementProfile_h
#define _WERenderElementProfile_h

#include "../binding/directx/9/d3d9/includeD3D9.h"
#include "../data/WEDataSource.h"
#include "../coordSys/WECoordSysConv.h"
#include "../WETL/WETLSizeAlloc.h"
#include "WERenderDataTypes.h"
#include "../math/WEMatrix.h"
#include "../geometry/WEGeometryAnchor.h"


namespace WE {

	/*
		We Add types to use for Index Buffers (simulating D3DFORMAT for Index Buffer)
	*/
	enum _WE_D3DDECLTYPE {
		_WE_D3DDECLTYPE_INDEX16 = D3DDECLTYPE_UNUSED + 1, _WE_D3DDECLTYPE_INDEX32
	};

	enum _WE_D3DDECLUSAGE {
		 _WE_D3DDECLUSAGE_INDEX = MAXD3DDECLUSAGE + 1
	};

	//typedef D3DVERTEXELEMENT9 RenderSubElement;
	struct RenderSubElement {
	
		//for now remove 'unused' properties
		//WORD    Stream;     // Stream index
		WORD    Offset;     // Offset in the stream in bytes
		BYTE    Type;       // Data type
		//BYTE    Method;     // Processing method
		BYTE    Usage;      // Semantics
		//BYTE    UsageIndex; // Semantic index
	};

	class RenderElementProfile {
	public:

		typedef WETL::IndexShort SubElementIndex;
		typedef WETL::SizeAllocT<RenderSubElement, SubElementIndex::Type, true> SubElementArray;

		SubElementArray mSubElements;

		RenderUINT mElementSizeBytes;
		
		inline RenderSubElement& index_element() const;

	public:

		union {
			struct {
				DWORD mFVF; //extra for vertex buffers
			};
			struct {
				DWORD mExtra; 
			};
		};
		
	public:

		RenderElementProfile();
		~RenderElementProfile();

		void destroy();

		void set(const RenderElementProfile& ref);

		SubElementIndex findSubByFVF(DWORD fvf) const;
		SubElementIndex findSub(D3DDECLUSAGE usage, BYTE usageIndex = 0) const;

		void* getPtr(void* buffer, UINT32 element, SubElementIndex sub) const;
		void* getRaw(void* buffer, UINT32 element) const;

		void setRaw(void* buffer, UINT32 element, void* raw);
		void setRaw(void* buffer, UINT32 element, void* raw, RenderUINT sizeBytes) const;

		RenderUINT elementSizeBytes() const;
		RenderUINT bufferElementCount(RenderUINT bufferSizeBytes) const;
		RenderUINT bufferSizeBytes(RenderUINT elementCount) const;
		static WORD _sizeof(BYTE type);
		WORD sizeofSub(short sub) const;

		float* getF(void* buffer, UINT32 element, SubElementIndex sub) const;
		BYTE* getB(void* buffer, UINT32 element, SubElementIndex sub) const;
		
		void setF(void* buffer, UINT32 element, SubElementIndex sub, float* values, unsigned short valueCount) const;
		void setB4(void* buffer, UINT32 element, SubElementIndex sub, BYTE values[4]) const;

	public:

		bool isVertexProfile() const;

		void vertex_load(DataChunk& el_vb, DataChunk* pEl_weights, bool scanVBSiblings = false);

		bool vertex_set(DWORD fvf, DWORD fvf_XYZ);
		
		void vertex_simpleConvert(void* src, void* dest, UINT32 element, CoordSysConv* pConv) const;
		void vertex_simpleConvert(void* src, void* dest, UINT32 element, Matrix4x3Base* pConv) const;
		void vertex_reanchor(void* src, void* dest, UINT32 element, float* anchoringOffset) const;
		void vertex_simpleConvertArray(void* src, void* dest, UINT32 elementCount, CoordSysConv* pConv) const;
		void vertex_simpleConvertArray(void* src, void* dest, UINT32 elementCount, Matrix4x3Base* pConv) const;
		void vertex_reanchorArray(void* src, void* dest, UINT32 elementCount, float* anchoringOffset) const;

		/*
		typedef WETL::SizeAllocT<SubElementIndex, SubElementIndex::Type, false> ConvertMap;
		
		void vertex_simpleConvertGenerateMap(const RenderElementProfile& dest, ConvertMap& map) const;

		void vertex_simpleConvert(BYTE* src, BYTE* dest, UINT32 element, RenderElementProfile& destProfile, SubElementMarkArray& markArray) const;
		void vertex_simpleConvertArray(void* src, void* dest, UINT32 elementCount, RenderElementProfile& destProfile) const;
		*/

	public:

		bool isIndexProfile() const;

		void index_set(bool is32Bit/*, bool isStrip*/);
		
		bool index_is32bit() const;
		//bool face_isStrip() const;

		/*
		UINT32 face_faceBase(UINT32 face) const;
		UINT32 face_indexFace(UINT32 index) const;

		UINT32 face_indexCount(UINT32 faceCount) const;
		UINT32 face_faceCount(UINT32 indexCount) const;
		UINT32 face_bufferSizeBytes(UINT32 faceCount) const;
		*/
		
		UINT32 index_get(void* buffer, UINT32 element) const;
		void index_get(void* buffer, UINT32 startElement, UINT32* values, UINT32 count) const;

		void index_set(void* buffer, UINT32 element, UINT32 value) const;
		void index_set(void* buffer, UINT32 startElement, UINT32* values, UINT32 count) const;
		
		bool index_isOptimizable(UINT32 maxIndex, RenderUINT& newElementSize, void*& reserved);
		void index_setOptimized(void* reserved);
		static void index_optimizeIndexDataType(UINT32 indexCount, UINT32 maxIndex, void* pOptimizableIB, void* pNewIB, void* reserved);

	public:

		//void face_setRaw(void* buffer, UINT32 face, void* data);
		void face_get(void* buffer, UINT32 firstIndex, UINT32* indices) const;
		void face_set(void* buffer, UINT32 firstIndex, UINT32* indices) const;

		void face_simpleConvert(void* src, void* dest, UINT32 firstIndex, CoordSysConv* pConv) const;
		void face_simpleConvertArray(void* src, void* dest, UINT32 indexCount, CoordSysConv* pConv) const;

		//void face_optimizeIndexDataType(UINT32 faceCount, UINT32 maxIndex, void*& pIB);
	};

	typedef RenderElementProfile::SubElementIndex RenderSubElIndex;
	#define RenderElSubNull ((RenderSubElIndex) -1)

	struct RenderVertexProfile_CachedSubs {
	
		RenderSubElIndex el_XYZ;
		RenderSubElIndex el_NORMAL;
		RenderSubElIndex el_TEX1;
		RenderSubElIndex el_XYZB4;
		RenderSubElIndex el_LASTBETA_UBYTE4;

		RenderVertexProfile_CachedSubs();
		
		void set(RenderElementProfile& profile);
	};
}

#endif