#ifndef _WEVector_h
#define _WEVector_h

#include "math.h"
#include "WEMathUtil.h"


namespace WE {

	//The(ta) is the cylindrical component
	enum Spherical {

		Ro = 0, Phi = 1, The = 2
	};

	enum Axis {

		Axis_X = 0, Axis_Y = 1, Axis_Z = 2
	};

	struct Vector3;

	struct CtVector3 {

		union {
			struct {
				float x,y,z;
			};
			struct {
				float el[3];
			};
			struct {
				float mX,mY,mZ;
			};
		};

		static CtVector3 kZero;
		static CtVector3 kX;
		static CtVector3 kY;
		static CtVector3 kZ;

		// Constructors

		// indeterminate state
		CtVector3() {}

		CtVector3(const CtVector3 &a) : x(a.x), y(a.y), z(a.z) {}
		CtVector3(const float& val) : x(val), y(val), z(val) {}
		CtVector3(const float& nx, const float& ny, const float& nz) : x(nx), y(ny), z(nz) {}
		CtVector3(float* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) {}


		// Check for equality

		bool equals(const CtVector3 &a) const {
			return x==a.x && y==a.y && z==a.z;
		}

		bool operator ==(const CtVector3 &a) const {
			return x==a.x && y==a.y && z==a.z;
		}

		bool operator !=(const CtVector3 &a) const {
			return x!=a.x || y!=a.y || z!=a.z;
		}

		void negate(Vector3& result) const;

		void add(const CtVector3 &a, Vector3& result) const;
		void mul(const CtVector3 &a, Vector3& result) const;
		void subtract(const CtVector3 &b, Vector3& selfMinusB) const;
		void mul(float a, Vector3& result) const;
		void mulAndAdd(float a, Vector3& result) const;
		void div(float a, Vector3& result) const;

		void addMultiplication(const CtVector3& mula, float mulb, Vector3& result) const;

		//WitchEngine MOD
		inline float operator [] ( const short& i ) const{
			return el[i];
		}

		//WitchEngine MOD
		inline float operator () ( const short& i ) const{
			return el[i];
		}

		bool isZero() const {
			return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
		}

		bool isInverse(const CtVector3 &a) const {
			return (x == -a.x) && (y == -a.y) && (z == -a.z);
		}

		void normalize(Vector3& result) const;
		void normalize(Vector3& result, float& mag) const;


		float operator *(const CtVector3 &a) const {
			return x*a.x + y*a.y + z*a.z;
		}

		//WitchEngine MOD
		float dot (const CtVector3 &a) const {
			return x*a.x + y*a.y + z*a.z;
		}

		// Compute the magnitude of a vector
		float mag() const {
			return sqrtf(x*x + y*y + z*z);
		}

		float magSquared() const {
			return x*x + y*y + z*z;
		}

		void mag(float& val) const {
			val = sqrtf(x*x + y*y + z*z);
		}

		void magSquared(float& val) const {
			val = x*x + y*y + z*z;
		}

		void cross(const CtVector3 &b, Vector3& result) const;

		void randomize(const float& magFactorMin, const float& magFactorMax, 
						  const float& magAddMin, const float& magAddMax,
						  const float& dirFactorMin, const float& dirFactorMax, const bool& allowNegDir, Vector3& result);

		//0 is no difference 1 is 90 degrees, -1 is -90 degress, 2/-2 is 180 degrees
		void randomizeDir(float angle, Vector3& result);

		inline bool script_isZero() { return isZero();}
		inline float script_dot (CtVector3 &a) { return dot(a); }
		inline bool script_equals (CtVector3 &a) { return equals(a); }
		void script_add(CtVector3 &a, Vector3& res);
		void script_subtract(CtVector3 &a, Vector3& res);
		void script_cross(CtVector3 &a, Vector3& res);
		inline float script_get(int i) { return el[i]; }
		void script_compMul (CtVector3 &a, Vector3& res);
		void script_mul (float a, Vector3& res);
		void script_div (float a, Vector3& res);
		inline void script_mulAndAdd(float a, Vector3& res) { mulAndAdd(a, res); }
		inline float script_magSq() { return magSquared(); }
		inline float script_mag() { return mag(); }
		inline void script_normalizeTo(Vector3& res) { normalize(res); }
		inline void script_negateTo(Vector3& res) { negate(res); }
		void script_addMultiplication(CtVector3& mula, float mulb, Vector3& result);

		inline void script_randomize(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir, Vector3& result) {

			randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, result);
		}

		inline void script_randomizeDir(float angle, Vector3& result) { randomizeDir(angle, result); }
	};

	struct Vector3 : CtVector3 {
	public:

		Vector3() : CtVector3() {}
		Vector3(const CtVector3 &a) : CtVector3(a) {}
		Vector3(const float& val) : CtVector3(val) {}
		Vector3(const float& nx, const float& ny, const float& nz) : CtVector3(nx, ny, nz) {}
		Vector3(float* ptr) : CtVector3(ptr) {}
		
		inline void set(const float& nx, const float& ny, const float& nz) {x = nx; y = ny; z = nz;}
		inline void set(const Vector3& ref) {x = ref.x; y = ref.y; z = ref.z;}
		inline void set(const float* refPtr) {x = refPtr[0]; y = refPtr[1]; z = refPtr[2];}
		
	
		Vector3 &operator =(float val) {
			x = y = z = val;
			return *this;
		}

		Vector3 &operator =(const Vector3 &a) {
			x = a.x; y = a.y; z = a.z;
			return *this;
		}

		void zero() { x = y = z = 0.0f; }

		void negate() {
			x = -x; y = -y; z = -z;
		}

		void sameSignCombine(const Vector3 &a) {

			WE::sameSignCombine(x, a.x);
			WE::sameSignCombine(y, a.y);
			WE::sameSignCombine(z, a.z);
		};

		void sameSignCombine(const Vector3 &a, SameSignCombineState state[3]) {

			WE::sameSignCombine(x, a.x, state[0]);
			WE::sameSignCombine(y, a.y, state[1]);
			WE::sameSignCombine(z, a.z, state[2]);
		};

		void add(const Vector3 &a) {
			x = x + a.x;
			y = y + a.y;
			z = z + a.z;
		};

		void addNegative(const Vector3 &a) {
			x = x - a.x;
			y = y - a.y;
			z = z - a.z;
		};

		void subtract(const Vector3 &b) {
			x = x - b.x;
			y = y - b.y;
			z = z - b.z;
		};
		
		void mul(float a) {
			x = x * a;
			y = y * a;
			z = z * a;
		};

		inline void subtract(const Vector3& a, Vector3& result) const { CtVector3::subtract(a, result); }
		inline void mul(float a, Vector3& result) const { CtVector3::mul(a, result); }
		inline void mulAndAdd(float a, Vector3& result) const { CtVector3::mulAndAdd(a, result); }
		inline void mul(const Vector3& a, Vector3& result) const { CtVector3::mul(a, result); }
		inline void div(const float& a, Vector3& result) const { CtVector3::div(a, result); }
		inline void add(const Vector3& a, Vector3& result) const { CtVector3::add(a, result); }
		inline void normalize(Vector3& result) const { CtVector3::normalize(result); }
		inline void normalize(Vector3& result, float& mag) const { CtVector3::normalize(result, mag); }
		inline void negate(Vector3& result) const { CtVector3::negate(result); }


		void div(float a) {

			a = 1.0f / a;

			x = x * a;
			y = y * a;
			z = z * a;
		};
	

		Vector3 &operator +=(const Vector3 &a) {
			x += a.x; y += a.y; z += a.z;
			return *this;
		}

		Vector3 &operator -=(const Vector3 &a) {
			x -= a.x; y -= a.y; z -= a.z;
			return *this;
		}

		Vector3 &operator *=(float a) {
			x *= a; y *= a; z *= a;
			return *this;
		}

		Vector3 &operator /=(float a) {
			float	oneOverA = 1.0f / a;
			x *= oneOverA; y *= oneOverA; z *= oneOverA;
			return *this;
		}

		//WitchEngine MOD
		inline float operator [] ( const short& i ) const{
			return el[i];
		}

		//WitchEngine MOD
		inline float& operator [] ( const short& i ) {
			return el[i];
		}

		//WitchEngine MOD
		inline float operator () ( const short& i ) const{
			return el[i];
		}

		//WitchEngine MOD
		inline float& operator () ( const short& i ) {
			return el[i];
		}

		
		
		// Normalize the vector

		void	normalize() {
			float magSq = x*x + y*y + z*z;
			if (magSq > 0.0f) { // check for divide-by-zero
				float oneOverMag = 1.0f / sqrtf(magSq);
				x *= oneOverMag;
				y *= oneOverMag;
				z *= oneOverMag;
			}
		}

		void	normalize(float& mag) {

			float magSq = x*x + y*y + z*z;

			if (magSq > 0.0f) { // check for divide-by-zero
			
				mag = sqrtf(magSq);
				float oneOverMag = 1.0f / mag;
				x *= oneOverMag;
				y *= oneOverMag;
				z *= oneOverMag;

			} else {

				zero();
				mag = 0.0f;
			}
		}


		
		// Vector dot product.  We overload the standard
		// multiplication symbol to do this

		float operator *(const Vector3 &a) const {
			return x*a.x + y*a.y + z*a.z;
		}

		void abs() {

			x = fabs(x);
			y = fabs(y);
			z = fabs(z);
		};
		

		//return true if still none zero
		bool substractNoReverse(float magnitude, Vector3& result) {

			float magSq = x*x + y*y + z*z;

			if (magSq < magnitude * magnitude) {
				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
				return false;
			}

			float mag = sqrtf(magSq);
			Vector3 dir;

			mul(1.0f / mag, dir);
			
			mag -= magnitude;
			dir.mul(mag, result);

			return true;
		}

		//return true if still none zero
		bool substractNoReverse(float magnitude) {

			float magSq = x*x + y*y + z*z;

			if (magSq <= magnitude * magnitude) {

				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
				return false;
			}

			float mag = sqrtf(magSq);
			Vector3 dir;

			mul(1.0f / mag, dir);
			
			mag -= magnitude;
			dir.mul(mag, *this);

			return true;
		}

		void sphericalToCartesian(Vector3& result) {

			/*
			result.x = (*this)[Ro] * sin((*this)[Phi]) * cos((*this)[The]);
			result.y = (*this)[Ro] * sin((*this)[Phi]) * sin((*this)[The]);
			result.z = (*this)[Ro] * cos((*this)[Phi]);
			*/
			result.z = - ((*this)[Ro] * sin((*this)[Phi]) * cos((*this)[The]));
			result.x = (*this)[Ro] * sin((*this)[Phi]) * sin((*this)[The]);
			result.y = (*this)[Ro] * cos((*this)[Phi]);
		}

		void cartesianToSpherical(Vector3& result) {
			
			result[Ro] = mag();

			/*
			float S = sqrtf(x*x + y*y);

			result[Phi] = acos(z / result[Ro]);
			if (x >= 0) {
				result[The] = asin(y / S);
			} else {
				result[The] = kPi - asin(y / S);
			}
			*/

			float S = sqrtf(x*x + z*z);

			result[Phi] = acos(y / result[Ro]);
			if (-z >= 0) {
				result[The] = asin(-z / S);
			} else {
				result[The] = kPi - asin(-z / S);
			}
		}

		inline bool script_isZero() { return isZero();}
		inline void script_set3(float nx, float ny, float nz) {x = nx; y = ny; z = nz;}
		inline bool script_equals(CtVector3& comp) { return x == comp.x && y == comp.y && z == comp.z;}
		inline void script_set(CtVector3& v) { set(v);}
		inline float script_dot (CtVector3 &a) { return dot(a); }
		inline void script_add (CtVector3 &a, Vector3& res) { add(a, res); }
		inline void script_subtract (CtVector3 &a, Vector3& res) { subtract(a, res); }
		inline void script_cross (CtVector3 &a, Vector3& res) { cross(a, res); }
		inline void script_addToSelf (CtVector3 &a) { add(a); }
		inline void script_mulToSelf (float a) { mul(a); }
		inline void script_divToSelf (float a) { div(a); }
		inline void script_subtractFromSelf (CtVector3 &a) { subtract(a); }
		inline void script_vmul (CtVector3 &a, Vector3& res) { mul(a, res); }
		inline void script_mul (float a, Vector3& res) { mul(a, res); }
		inline void script_div (float a, Vector3& res) { div(a, res); }
		inline void script_mulAndAdd (float a, Vector3& res) { mulAndAdd(a, res); }
		inline void script_setEl(int i, float v) { el[i] = v; }
		inline float script_get(int i) { return el[i]; }
		inline void script_normalize() { normalize(); }
		inline void script_normalizeTo(Vector3& res) { normalize(res); }
		inline float script_mag() { return mag(); }
		inline float script_magSq() { return magSquared(); }
		inline bool script_subtractNoReverse(float val) { return substractNoReverse(val); }
		inline void script_negateTo(Vector3& res) { negate(res); }
		inline void script_negate() { negate(); }

		inline void script_randomize(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir, Vector3& result) {

			randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, result);
		}

		inline void script_randomizeToSelf(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir) {

			Vector3 temp = *this;

			temp.randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, *this);
		}

		inline void script_randomizeDir(float angle, Vector3& result) { randomizeDir(angle, result); }

		inline void script_randomizeDirToSelf(float angle) { 
			
			Vector3 temp = *this;

			temp.randomizeDir(angle, *this); 
		}

		inline void script_addMultiplication(CtVector3& mula, float mulb, Vector3& result) {

			addMultiplication(mula, mulb, result);
		}
	};

	static const Vector3 kAxis[3] = {Vector3(1.0f,0.0f,0.0f), Vector3(0.0f,1.0f,0.0f), Vector3(0.0f,0.0f,1.0f)};
	
	// Scalar on the left multiplication, for symmetry

	//inline Vector3 operator *(float k, const Vector3 &v) {
	//	return Vector3(k*v.x, k*v.y, k*v.z);
	//}

	//projects vector on direction, then returns the difference between the vector and its projection
	void projectionRest(const Vector3& vector, const Vector3& direction, Vector3& rest);

	void project(const Vector3& vector, const Vector3& direction, Vector3& result);
	//puts result back in vector
	void project(Vector3& vector, const Vector3& direction);

	void decompose(CtVector3& vector, const Vector3& direction, Vector3& alongDir, Vector3& rest);

	float angle(const Vector3 &ref, const Vector3& to, bool leftHanded = true);
	float angle(const Vector3 &ref, const Vector3& to, const Vector3& normalUnit, bool leftHanded = true);
	float angle(CtVector3 &ref, CtVector3& to, CtVector3& normalUnit, bool leftHanded = true);
	float angleUnit(const Vector3 &refUnit, const Vector3& toUnit, bool leftHanded = true);
	float angleUnit(const Vector3 &refUnit, const Vector3& toUnit, const Vector3& normalUnit, bool leftHanded = true);

	void lerp(const Vector3& from, const Vector3& to, float factor, Vector3& result);
	void lerp(CtVector3& from, CtVector3& to, float factor, Vector3& result);

	typedef Vector3 Vector;
	typedef Vector3 Point;


	int discretizeDirection(CtVector3& refDir, CtVector3& normalDir, CtVector3& dir, const int& directionCount, Vector3& discreeteDir, float precision = k1DegreeInRad * 0.5f);

}


#endif 
