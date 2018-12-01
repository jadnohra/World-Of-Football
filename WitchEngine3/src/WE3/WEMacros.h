#ifndef _WEMacros_h
#define _WEMacros_h

#include "WEAssert.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

/*
#define MZeroStructCtr(type) type() {memset(this, 0, sizeof(type));}
#define MZeroStruct(structure) memset(&structure, 0, sizeof(structure));
*/

#define M256ToFloat(val) ((float) val / 255.0f)
#define M256ToFloat3(val1, val2, val3) M256ToFloat(val1), M256ToFloat(val2), M256ToFloat(val3)


#define MSet2(ptr, v0, v1) (ptr)[0] = (v0); (ptr)[1] = (v1); 
#define MSet3(ptr, v0, v1, v2) (ptr)[0] = (v0); (ptr)[1] = (v1); (ptr)[2] = (v2);
#define MSet3From1(ptr, v) (ptr)[0] = (v); (ptr)[1] = (v); (ptr)[2] = (v);
#define MSet4(ptr, v0, v1, v2, v3) (ptr)[0] = (v0); (ptr)[1] = (v1); (ptr)[2] = (v2); (ptr)[3] = (v3);
#define MExpand3(ptr) (ptr)[0], (ptr)[1], (ptr)[2]
#define MExpand4(ptr) (ptr)[0], (ptr)[1], (ptr)[2], (ptr)[3]
#define MCopy3(ptrTo, ptrFrom) (ptrTo)[0] = (ptrFrom)[0]; (ptrTo)[1] = (ptrFrom)[1]; (ptrTo)[2] = (ptrFrom)[2]; 

#define MSafeRelease(ptr) if (ptr) {(ptr)->Release(); (ptr) = NULL;}
#define MSafeAddRef(ptr) if (ptr) {(ptr)->AddRef();}

namespace WE {


	template<class T, class TD>
	bool checkOp(T& val, const TD& diff) {

		T temp = val;
		val += diff;

		if (diff > (TD) 0) {

			if (val <= temp) return(false);

		} else if (diff < (TD) 0) {

			if (val >= temp) return(false);
		}

		return true;
	}

	template<class T, class TD>
	bool checkMul(T& val, const TD& diff) {

		T temp = val;
		val *= diff;

		if (diff > (TD) 1) {

			if (val <= temp) return(false);

		} else if (diff < (TD) 1) {

			if (val >= temp) return(false);
		}

		return true;
	}


	#ifdef _DEBUG

	template<class T> 
	inline T& dref(T* ptr) {

		assrt((ptr) != NULL);
		return (*(ptr));
	}

	template<class T, class Tptr> 
	inline T& drefcast(Tptr ptr) {

		assrt((ptr) != NULL);
		return (* ((T*) (ptr)));
	}


	template<class FromT, class ToT>
	inline ToT safeSizeCast(const FromT& val) {

		assrt(val == (FromT) ((ToT) ((FromT) val)));

		return (ToT) val;
	}


	template<class T, class TD>
	void assertSafeOp(T val, const TD& diff) {

		T temp = val;
		val += diff;

		if (diff > (TD) 0) {

			if (val <= temp) assrt(false);

		} else if (diff < (TD) 0) {

			if (val >= temp) assrt(false);
		}
	}

	template<class T, class TD>
	T& safeOp(T& val, const TD& diff) {

		T temp = val;
		val += diff;

		if (diff > (TD) 0) {

			if (val <= temp) assrt(false);

		} else if (diff < (TD) 0) {

			if (val >= temp) assrt(false);
		}

		return val;
	}

	template<class T, class TD>
	T& safeMul(T& val, const TD& diff) {

		T temp = val;
		val *= diff;

		if (diff > (TD) 1) {

			if (val <= temp) assrt(false);

		} else if (diff < (TD) 1) {

			if (val >= temp) assrt(false);
		}

		return val;
	}

	
	#else

	template<class T> 
	inline T& dref(T* ptr) {

		return (*(ptr));
	}

	template<class T> 
	inline const T& drefct(const T* ptr) {

		return (*(ptr));
	}

	template<class T, class Tptr> 
	inline T& drefcast(Tptr ptr) {

		return (* ((T*) (ptr)));
	}

	template<class FromT, class ToT>
	inline ToT safeSizeCast(const FromT& val) {

		return (ToT) val;
	}

	template<class T>
	inline void safeOpCheck(T val, const T& diff) {
	}

	template<class T, class TD>
	inline void assertSafeOp(T val, const TD& diff) {
	}

	template<class T, class TD>
	inline T& safeOp(T& val, const TD& diff) {

		return (val += diff);
	}

	template<class T, class TD>
	inline T& safeMul(T& val, const TD& diff) {

		return (val *= diff);
	}


	#endif

	template<typename T> 
	T trand(const T& max) { return (T) ( (double) ::rand() / (double)(RAND_MAX) ) * ((double) max); }

	template<typename T> 
	inline T trand2(const T& min, const T& max) { return min + trand<T>(max - min); }

	/*
	#define MRand(max) (( (double)::rand() / (double)(RAND_MAX) ) * ((double) max))
	#define MRandf(max) ((float) trand(max))
	#define MRand2(min, max) (min + trand(max - min))
	#define MRand2f<float>(min, max) ((float) (min + trand(max - min)))
	*/

	#define MMax(v1, v2) ((v1) >= (v2) ? (v1) : (v2))
	#define MMin(v1, v2) ((v1) <= (v2) ? (v1) : (v2))
	#define MBetweenMinMaxIncl(v, min, max) ((v) >= (min) && (v) <= (max))
	#define MBetweenMinMaxIncl_e(v, min, max, e) ((v) >= (min - e) && (v) <= (max + e))
	#define MBetweenMinMaxExcl(v, min, max) ((v) > (min) && (v) < (max))

	template<typename T>
	const T& tmax(const T& v1, const T& v2) { return ((v1) >= (v2) ? (v1) : (v2)); }

	template<typename T>
	const T& tmin(const T& v1, const T& v2) { return ((v1) <= (v2) ? (v1) : (v2)); }

	template<typename T>
	T tcap(const T& val, const T& min, const T& max) { 
		
		if (val < min)
			return min;

		if (val > max)
			return max;

		return val; 
	}

	#define MDefSgnPos 1 
	#define MDefSgnNeg -1 
	#define MDefSgnZero 0 

	#define MSgn(v) ((v) == 0 ? MDefSgnZero : ((v) > 0 ? MDefSgnPos : MDefSgnNeg))


	inline bool equalf(const float &a, const float &b, const float& e) {
		
		return (fabs(a - b) <= e);
	}

	template <class Type>
	inline bool sameSgnZeroTolerantT(const Type &a, const Type &b) {
		
		int sgnA = MSgn(a);

		if (sgnA == 0) {
			return true;
		}

		int sgnB = MSgn(b);

		if (sgnB == 0) {
			return true;
		}

		return sgnA == sgnB;
	}


	template <class Type>
	inline void swapt(Type &a, Type &b, Type& swapVar) {
		swapVar = a;
		a = b;
		b = swapVar;
	}

	template <class Type>
	inline void swapt(Type &a, Type &b) {
		Type tmp(a);
		a = b;
		b = tmp;
	}
}


#endif