#ifndef _WECircle_h
#define _WECircle_h

#include "WEVector2.h"
#include "WEMatrix2.h"
#include "WESpan.h"

namespace WE {

	struct Circle {

		Vector2	center;
		float radius;

		Circle() {};
		Circle(bool _empty) {if (_empty) empty();};

		void empty();

		void getPoint(const int& index, Vector2& point);

		bool contains(const CtVector2& point);
		void clip(const CtVector2& point, Vector2& result); 
		int rayIntersect(const CtVector2& point, const CtVector2& dir, float result[2]); 
		int segIntersect(const CtVector2& point, const CtVector2& point2, float result[2]); 

		bool rayIntersect(const CtVector2& point, const CtVector2& point2, float u[2], int& resultCount);
	};
}

#endif