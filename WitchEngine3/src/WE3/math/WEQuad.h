#ifndef _WEQuad_h
#define _WEQuad_h

#include "WEVector2.h"
#include "WEMatrix2.h"
#include "WESpan.h"

namespace WE {

	struct AAQuad {

		Vector2	min;
		Vector2	max;

		AAQuad() {};
		AAQuad(bool _empty) {if (_empty) empty();};

		//static Axis 
		static const Vector2 axis[2];

		inline void size(Vector2& result) const { max.subtract(min, result); }
		inline float xSize() const { return max.x - min.x; }
		inline float ySize() const { return max.y - min.y; }
		inline float size(int d) const { return max[d] - min[d]; }
		inline void center(Vector2& result) const { min.add(max, result); result.mul(0.5f); }
		
		void centeredRadius(Vector2& center, Vector2& radius) const;
		int getMinSizeIndex(float& minSize) const;

		void empty();

		void getPoint(const int& index, Vector2& point);

		bool contains(const CtVector2& point);
		void clip(const CtVector2& point, Vector2& result); 
		int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]); 
		int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]); 

		bool rayIntersect(const CtVector2& point, const CtVector2& dir, Span& result); 
		bool segIntersect(const CtVector2& point, const CtVector2& point2, Span& result, Vector2& lineDir);
	};

	
	struct Quad {

		RigidMatrix3x2 coordFrame; //basis and position
		Vector2 extents;

		Quad() {};
		Quad(bool _empty) {if (_empty) empty();};

		void empty();

		inline Vector2& basis(int index) { return coordFrame.row[index]; }
		inline Vector2& position() { return coordFrame.row[2]; }

		void getPoint(const int& index, Vector2& point);

		bool contains(const CtVector2& point);
		void clip(const CtVector2& point, Vector2& result); 
		int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]); 
		int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]); 

		bool rayIntersect(const CtVector2& point, const CtVector2& dir, Span& result); 
		bool segIntersect(const CtVector2& point, const CtVector2& point2, Span& result, Vector2& lineDir);
	};
	

} 

#endif