#ifndef _WECoordSys_h
#define _WECoordSys_h

#include "../math/WEVector.h"
#include "../math/WEMatrix.h"

namespace WE {

	enum CoordSysDirection {

		CSD_Unused = -1, CSD_Left, CSD_Right, CSD_Up, CSD_Down, CSD_Forward, CSD_Back
	};

	enum CoordSysUnitsPerMeter {

		CSU_None = 0, CSU_1, CSU_01, CSU_001, CSU_10, CSU_100
	};

	struct CoordSys {

		union {

			struct {

				unsigned el0 : 3;
				unsigned el1 : 3;
				unsigned el2 : 3;
				unsigned rotationLeftHanded : 1;
				unsigned unitsPerMeter : 3;
				unsigned bitValid : 1;
				
			};
			struct {

				unsigned _bit_dir_10 : 10;
			};
			struct {

				unsigned _bit_14 : 14;
			};

			struct {

				unsigned _size : 16;
			};
		};

		static const float kCSU_1;
		static const float kCSU_10;
		static const float kCSU_100;
		static const float kCSU_01;
		static const float kCSU_001;
		static const float kCSU_e;
		static const float kCSU_None;


		CoordSys();
		CoordSys(bool valid);
		CoordSys(const CoordSys& ref);
		CoordSys(const CoordSys* pRef);

		bool isValid() const;
		void setValid(bool valid);

		bool hasUnitsPerMeter() const;
		bool setFloatUnitsPerMeter(const float& value);
		const float& getFloatUnitsPerMeter() const;

		void set(const CoordSys& ref, bool setUnitsPerMeterIfSpecified = true);
		bool equals(const CoordSys& comp) const;
		bool isCompatibleWith(const CoordSys& comp) const;

		/*
		 * these funcntions are pretty slow, for heavy use, use CoordSysExpanded instead
		*/

		void setRUF(const float& right, const float& up, const float& forward, float* target) const;

		short findTarget(const CoordSysDirection& dir, const CoordSysDirection& dirNeg, short& target, bool& neg) const;
		short findTarget(const CoordSysDirection& dir, short& target, bool& neg) const;

		void setValue(const CoordSysDirection& dir, const float& val, float* target) const;
		void getValue(const CoordSysDirection& dir, const float* source, float& val) const;

		void getUnitVector(const CoordSysDirection& dir, float* target) const;

		//dont use these operators
		//use set, equals and isCompatibleWith
		bool operator==(const CoordSys& comp) const;
		CoordSys& operator=(const CoordSys& ref);
	};


	
	struct FastCoordSys {

		int target[6];
		bool neg[6];

		bool init(const CoordSys& ref);

		void setRUF(const float& right, const float& up, const float& forward, float* target) const;

		void setValue(const CoordSysDirection& dir, const float& val, float* target) const;
		void getValue(const CoordSysDirection& dir, const float* source, float& val) const;

		void getUnitVector(const CoordSysDirection& dir, float* target) const;
	};
	
	struct FastUnitCoordSys {

		int target[6];
		bool neg[6];

		float unitConv;
		float invUnitConv;

		bool init(const CoordSys& ref, const float& userUnit);

		void setRUF(const float& right, const float& up, const float& forward, float* target) const;

		void setValue(const CoordSysDirection& dir, const float& val, float* target) const;
		void getValue(const CoordSysDirection& dir, const float* source, float& val) const;


		inline void convUnit(const float& val, float& targetVal) const {targetVal = unitConv * val; }
		inline float convUnit(const float& val) const {return unitConv * val; }

		inline void convUnitPow(const float& val, float& targetVal, const int& pow) const {targetVal = powf(unitConv, (float) pow) * val; }
		inline float convUnitPow(const float& val, const int& pow) const {return powf(unitConv, (float) pow) * val; }

		inline void invConvUnit(const float& val, float& targetVal) const {targetVal = invUnitConv * val; }
		inline float invConvUnit(const float& val) const {return invUnitConv * val; }

		inline void invConvUnitPow(const float& val, float& targetVal, const int& pow) const {targetVal = powf(invUnitConv, (float) pow) * val; }
		inline float invConvUnitPow(const float& val, const int& pow) const {return powf(invUnitConv, (float) pow) * val; }

		inline void convVectorUnits(float* val3) const { return convVectorUnits(val3, val3); }
		inline void invConvVectorUnits(float* val3) const { return invConvVectorUnits(val3, val3); }

		void convVectorUnits(const float* val3, float* targetVal3) const;
		void invConvVectorUnits(const float* val3, float* targetVal3) const;

		
		void getUnitVector(const CoordSysDirection& dir, float* target) const;
	};


	struct CoordSysExpanded {

		CoordSysDirection el[3];
		bool rotationLeftHanded;
		float unitsPerMeter;

		CoordSysExpanded();
		CoordSysExpanded(const CoordSys& ref);

		void convertTo(CoordSys& dest) const;

		bool equals(const CoordSysExpanded& comp) const;
	};



	void setupCoordSysConvTransorm(const CoordSys& _source, const CoordSys& _target, Matrix33& transf);
}

#endif