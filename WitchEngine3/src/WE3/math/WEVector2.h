#ifndef _WEVector2_h
#define _WEVector2_h

#include "math.h"
#include "WEMathUtil.h"


namespace WE {

	struct Vector2;

	struct CtVector2 {

		union {
			struct {
				float x,y;
			};
			struct {
				float el[2];
			};
			struct {
				float mX,mY;
			};
			struct {
				float min,max;
			};
		};

		static CtVector2 kZero;
		static CtVector2 kX;
		static CtVector2 kY;

		// Constructors

		// indeterminate state
		CtVector2() {}

		CtVector2(const CtVector2 &a) : x(a.x), y(a.y) {}
		CtVector2(const float& val) : x(val), y(val) {}
		CtVector2(const float& nx, const float& ny) : x(nx), y(ny) {}
		CtVector2(float* ptr) : x(ptr[0]), y(ptr[1]) {}


		// Check for equality

		bool equals(const CtVector2 &a) const {
			return x==a.x && y==a.y;
		}

		bool operator ==(const CtVector2 &a) const {
			return x==a.x && y==a.y;
		}

		bool operator !=(const CtVector2 &a) const {
			return x!=a.x || y!=a.y;
		}

		void negate(Vector2& result) const;

		void add(const CtVector2 &a, Vector2& result) const;
		void mul(const CtVector2 &a, Vector2& result) const;
		void subtract(const CtVector2 &b, Vector2& selfMinusB) const;
		void mul(float a, Vector2& result) const;
		void mulAndAdd(float a, Vector2& result) const;
		void div(float a, Vector2& result) const;

		void addMultiplication(const CtVector2& mula, float mulb, Vector2& result) const;

		//WitchEngine MOD
		inline float operator [] ( const short& i ) const{
			return el[i];
		}

		//WitchEngine MOD
		inline float operator () ( const short& i ) const{
			return el[i];
		}

		bool isZero() const {
			return (x == 0.0f) && (y == 0.0f);
		}

		bool isInverse(const CtVector2 &a) const {
			return (x == -a.x) && (y == -a.y);
		}

		void normalize(Vector2& result) const;
		void normalize(Vector2& result, float& mag) const;


		float operator *(const CtVector2 &a) const {
			return x*a.x + y*a.y;
		}

		//WitchEngine MOD
		float dot (const CtVector2 &a) const {
			return x*a.x + y*a.y;
		}

		// Compute the magnitude of a vector
		float mag() const {
			return sqrtf(x*x + y*y);
		}

		float magSquared() const {
			return x*x + y*y;
		}

		void mag(float& val) const {
			val = sqrtf(x*x + y*y);
		}

		void magSquared(float& val) const {
			val = x*x + y*y;
		}

		void randomize(const float& magFactorMin, const float& magFactorMax, 
						  const float& magAddMin, const float& magAddMax,
						  const float& dirFactorMin, const float& dirFactorMax, const bool& allowNegDir, Vector2& result);

		inline bool script_isZero() { return isZero();}
		inline float script_dot (CtVector2 &a) { return dot(a); }
		inline bool script_equals (CtVector2 &a) { return equals(a); }
		void script_add(CtVector2 &a, Vector2& res);
		void script_subtract(CtVector2 &a, Vector2& res);
		inline float script_get(int i) { return el[i]; }
		void script_compMul (CtVector2 &a, Vector2& res);
		void script_mul (float a, Vector2& res);
		void script_div (float a, Vector2& res);
		inline void script_mulAndAdd(float a, Vector2& res) { mulAndAdd(a, res); }
		inline float script_magSq() { return magSquared(); }
		inline float script_mag() { return mag(); }
		inline void script_normalizeTo(Vector2& res) { normalize(res); }
		inline void script_negateTo(Vector2& res) { negate(res); }
		void script_addMultiplication(CtVector2& mula, float mulb, Vector2& result);

		inline void script_randomize(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir, Vector2& result) {

			randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, result);
		}
	};

	struct Vector2 : CtVector2 {
	public:

		Vector2() : CtVector2() {}
		Vector2(const CtVector2 &a) : CtVector2(a) {}
		Vector2(const float& val) : CtVector2(val) {}
		Vector2(const float& nx, const float& ny) : CtVector2(nx, ny) {}
		Vector2(float* ptr) : CtVector2(ptr) {}
		
		inline void set(const float& nx, const float& ny) {x = nx; y = ny;}
		inline void set(const Vector2& ref) {x = ref.x; y = ref.y;}
		inline void set(const float* refPtr) {x = refPtr[0]; y = refPtr[1];}
		
	
		Vector2 &operator =(float val) {
			x = y = val;
			return *this;
		}

		Vector2 &operator =(const Vector2 &a) {
			x = a.x; y = a.y;
			return *this;
		}

		void zero() { x = y = 0.0f; }

		void negate() {
			x = -x; y = -y;;
		}

		void sameSignCombine(const Vector2 &a) {

			WE::sameSignCombine(x, a.x);
			WE::sameSignCombine(y, a.y);
		};

		void sameSignCombine(const Vector2 &a, SameSignCombineState state[2]) {

			WE::sameSignCombine(x, a.x, state[0]);
			WE::sameSignCombine(y, a.y, state[1]);
		};

		void add(const Vector2 &a) {
			x = x + a.x;
			y = y + a.y;
		};

		void addNegative(const Vector2 &a) {
			x = x - a.x;
			y = y - a.y;
		};

		void subtract(const Vector2 &b) {
			x = x - b.x;
			y = y - b.y;
		};
		
		void mul(float a) {
			x = x * a;
			y = y * a;
		};

		inline void subtract(const Vector2& a, Vector2& result) const { CtVector2::subtract(a, result); }
		inline void mul(float a, Vector2& result) const { CtVector2::mul(a, result); }
		inline void mulAndAdd(float a, Vector2& result) const { CtVector2::mulAndAdd(a, result); }
		inline void mul(const Vector2& a, Vector2& result) const { CtVector2::mul(a, result); }
		inline void div(const float& a, Vector2& result) const { CtVector2::div(a, result); }
		inline void add(const Vector2& a, Vector2& result) const { CtVector2::add(a, result); }
		inline void normalize(Vector2& result) const { CtVector2::normalize(result); }
		inline void normalize(Vector2& result, float& mag) const { CtVector2::normalize(result, mag); }
		inline void negate(Vector2& result) const { CtVector2::negate(result); }


		void div(float a) {

			a = 1.0f / a;

			x = x * a;
			y = y * a;
		};
	

		Vector2 &operator +=(const Vector2 &a) {
			x += a.x; y += a.y;
			return *this;
		}

		Vector2 &operator -=(const Vector2 &a) {
			x -= a.x; y -= a.y;
			return *this;
		}

		Vector2 &operator *=(float a) {
			x *= a; y *= a;
			return *this;
		}

		Vector2 &operator /=(float a) {
			float	oneOverA = 1.0f / a;
			x *= oneOverA; y *= oneOverA;
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
			float magSq = x*x + y*y;
			if (magSq > 0.0f) { // check for divide-by-zero
				float oneOverMag = 1.0f / sqrtf(magSq);
				x *= oneOverMag;
				y *= oneOverMag;
			}
		}

		void	normalize(float& mag) {

			float magSq = x*x + y*y;

			if (magSq > 0.0f) { // check for divide-by-zero
			
				mag = sqrtf(magSq);
				float oneOverMag = 1.0f / mag;
				x *= oneOverMag;
				y *= oneOverMag;
			
			} else {

				zero();
				mag = 0.0f;
			}
		}


		
		// Vector dot product.  We overload the standard
		// multiplication symbol to do this

		float operator *(const Vector2 &a) const {
			return x*a.x + y*a.y;
		}

		void abs() {

			x = fabs(x);
			y = fabs(y);
		}
		

		//return true if still none zero
		bool substractNoReverse(float magnitude, Vector2& result) {

			float magSq = x*x + y*y;

			if (magSq < magnitude * magnitude) {
				x = 0.0f;
				y = 0.0f;
				return false;
			}

			float mag = sqrtf(magSq);
			Vector2 dir;

			mul(1.0f / mag, dir);
			
			mag -= magnitude;
			dir.mul(mag, result);

			return true;
		}

		//return true if still none zero
		bool substractNoReverse(float magnitude) {

			float magSq = x*x + y*y ;

			if (magSq <= magnitude * magnitude) {

				x = 0.0f;
				y = 0.0f;
				return false;
			}

			float mag = sqrtf(magSq);
			Vector2 dir;

			mul(1.0f / mag, dir);
			
			mag -= magnitude;
			dir.mul(mag, *this);

			return true;
		}

		inline bool script_isZero() { return isZero();}
		inline void script_set2(float nx, float ny) {x = nx; y = ny; }
		inline bool script_equals(CtVector2& comp) { return x == comp.x && y == comp.y ;}
		inline void script_set(CtVector2& v) { set(v);}
		inline float script_dot (CtVector2 &a) { return dot(a); }
		inline void script_add (CtVector2 &a, Vector2& res) { add(a, res); }
		inline void script_subtract (CtVector2 &a, Vector2& res) { subtract(a, res); }
		inline void script_addToSelf (CtVector2 &a) { add(a); }
		inline void script_mulToSelf (float a) { mul(a); }
		inline void script_divToSelf (float a) { div(a); }
		inline void script_subtractFromSelf (CtVector2 &a) { subtract(a); }
		inline void script_vmul (CtVector2 &a, Vector2& res) { mul(a, res); }
		inline void script_mul (float a, Vector2& res) { mul(a, res); }
		inline void script_div (float a, Vector2& res) { div(a, res); }
		inline void script_mulAndAdd (float a, Vector2& res) { mulAndAdd(a, res); }
		inline void script_setEl(int i, float v) { el[i] = v; }
		inline float script_get(int i) { return el[i]; }
		inline void script_normalize() { normalize(); }
		inline void script_normalizeTo(Vector2& res) { normalize(res); }
		inline float script_mag() { return mag(); }
		inline float script_magSq() { return magSquared(); }
		inline bool script_subtractNoReverse(float val) { return substractNoReverse(val); }
		inline void script_negateTo(Vector2& res) { negate(res); }
		inline void script_negate() { negate(); }

		inline void script_randomize(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir, Vector2& result) {

			randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, result);
		}

		inline void script_randomizeToSelf(float magFactorMin, float magFactorMax, 
							  float magAddMin, float magAddMax,
							  float dirFactorMin, float dirFactorMax, bool allowNegDir) {

			Vector2 temp = *this;

			temp.randomize(magFactorMin, magFactorMax, magAddMin, magAddMax, 
							  dirFactorMin, dirFactorMax, allowNegDir, *this);
		}

		inline void script_addMultiplication(CtVector2& mula, float mulb, Vector2& result) {

			addMultiplication(mula, mulb, result);
		}
	};

	static const Vector2 kAxis2[2] = {Vector2(1.0f,0.0f), Vector2(0.0f,1.0f)};
	
	// Scalar on the left multiplication, for symmetry

	//inline Vector2 operator *(float k, const Vector2 &v) {
	//	return Vector2(k*v.x, k*v.y, k*v.z);
	//}

	//projects vector on direction, then returns the difference between the vector and its projection
	void projectionRest(const CtVector2& vector, const CtVector2& direction, Vector2& rest);

	void project(const Vector2& vector, const Vector2& direction, Vector2& result);
	//puts result back in vector
	void project(Vector2& vector, const Vector2& direction);

	void decompose(CtVector2& vector, const Vector2& direction, Vector2& alongDir, Vector2& rest);

	float angle(const Vector2 &ref, const Vector2& to);
	
	void lerp(const Vector2& from, const Vector2& to, float factor, Vector2& result);
	void lerp(CtVector2& from, CtVector2& to, float factor, Vector2& result);

}


#endif 
