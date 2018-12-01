#ifndef _WERenderObjects_h
#define _WERenderObjects_h

#include "../WERef.h"
#include "WERenderDataTypes.h"
#include "../math/WEVector.h"
#include "../string/WEString.h"

namespace WE {

	struct RenderObject {
	};

	struct DimMap_2_3 {

		int mapX;
		int mapY;
		int mapNone;

		DimMap_2_3(int _mapX = 0, int _mapY = 1, int _mapNone = 2)
			: mapX(_mapX), mapY(_mapY), mapNone(_mapNone) {
		}

		static const DimMap_2_3 k; //default unit mapping
	};

	struct RenderQuad {

		enum PointIndex {
			
			Point_None = -1, Point_LeftTop, Point_RightTop, Point_RightBottom, Point_LeftBottom
		};

		enum AnchorEnum {

			Anchor_None = -1, Anchor_Center, Anchor_Start, Anchor_End
		};

		Vector3 points[4];

		RenderQuad(float val);
		RenderQuad(bool setUnit = false, AnchorEnum anchorX = Anchor_None, AnchorEnum anchorY = Anchor_None, const DimMap_2_3& dimMap = DimMap_2_3::k);

		void setupBySize(float sizeX, float sizeY, AnchorEnum anchorX = Anchor_None, AnchorEnum anchorY = Anchor_None, const DimMap_2_3& dimMap = DimMap_2_3::k);
		void getSize(float& sizeX, float& sizeY, const DimMap_2_3& dimMap = DimMap_2_3::k);

		//uses the dimMap.mapNone dimension
		void setOffset(float offset, const DimMap_2_3& dimMap = DimMap_2_3::k);

		static const RenderQuad kUnit;
	};
	
	RenderQuad::AnchorEnum fromString(const String& str, RenderQuad::AnchorEnum defaultVal = RenderQuad::Anchor_None);
	inline RenderQuad::AnchorEnum fromString(const TCHAR* str, RenderQuad::AnchorEnum defaultVal = RenderQuad::Anchor_None) { return fromString(String(str), defaultVal); }

	struct RenderBufferPart : RenderObject {
		
		RenderUINT startIndex;
		RenderUINT primCount;

		void zero() {

			startIndex = 0;
			primCount = 0;
		}
	};

	//Making Some objects inherit from is currently the best way 
	//to make life easier for sharing them in pools and the like
	//it adds one int per object, and one virtual destructor
	//when using pools, this would have to be done anyway,
	//without pool usage there is a wasted int per object
	//but in the majority of the cases pools will be used
	//when not then .....

	class RenderBuffer : public Ref, public RenderObject {
	public:

		virtual ~RenderBuffer() {};
	};
	
	/*
	class RenderVertexBuffer : public Ref, public RenderObject {
	public:

		virtual ~RenderVertexBuffer() {};
	};

	class RenderIndexBuffer : public Ref, public RenderObject {
	public:

		virtual ~RenderIndexBuffer() {};
	};
	*/

	class RenderTexture : public Ref, public RenderObject {
	public:

		virtual ~RenderTexture() {};
		virtual bool getSize(unsigned int& width, unsigned int& height) { return false; }
		virtual bool getLoadSize(unsigned int& width, unsigned int& height) { return false; }
	};
	
}

#endif