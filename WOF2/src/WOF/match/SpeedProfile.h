#ifndef h_WOF_SpeedProfile
#define h_WOF_SpeedProfile

#include "WE3/WETime.h"
#include "WE3/math/WEVector.h"
#include "WE3/data/WEDataSource.h"
#include "WE3/coordSys/WECoordSysConv.h"
using namespace WE;


namespace WOF {

		struct Distance {

			Vector3 dist;
			Vector3 dir;
			float mag;

			void zero();
			void update();
			void subtract(const Vector3& diff);
			void add(const Vector3& added);

			bool moveTo(const Vector3& target, const float& targetMag, const float& moveMag); //returns true if target reached
		};

		class SpeedProfile {
		public:

			struct Setup {

				enum Type {

					T_None = 0, T_Speed, T_SpeedInterval, T_Ratios, T_Intervals, T_Accelerations
				};

				Type type;

				float distance; //optional
				Time time; //optional

				union {

					//Setup_Speed
					struct {

						float speed;
					};

					//Setup_Speed
					struct {

						float speedInterval;
					};

					//Setup_Ratios
					struct {

						float accRatio; 
						float decRatio;
					};

					//Setup_Intervals
					struct {

						Time accInterval; 
						Time decInterval;
					};

					// Setup_Accelerations 
					struct {

						float accRef;
						float decRef;
					};
				};

				Setup();

				void invalidate();
				bool isValid();

				void init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
			};

			//void init(BufferString& tempStr, DataChunk& chunk, const Time& totalDist, const Time& totalTime, CoordSysConv* pConv);
			
		public:

			SpeedProfile();

			void invalidate();
			bool isValid();

			void setup(const Setup& settings, CoordSysConv* pConv, float totalDist = 0.0f, Time totalTime = 0.0f);

			void setupUsingSpeed(const float& speed, CoordSysConv* pConv);
			void setupUsingSpeedInterval(const float& totalDist, const float& totalTime,const float& speedInterval, CoordSysConv* pConv);
			void setupUsingRatios(const float& totalDist, const float& totalTime, const float& accRatio, const Time& decRatio, CoordSysConv* pConv);
			void setupUsingIntervals(const float& totalDist, const float& totalTime, const float& accInterval, const Time& decInterval, CoordSysConv* pConv);
			void setupUsingAccelerations(const float& totalDist, const float& totalTime, const float& refAcc, const float& refDec, CoordSysConv* pConv);

			float calcSpeed(const Time& startTime, const Time& time, const Time& dt, const float& currSpeed);

		protected:

			void updateType();

		protected:

			bool mIsValid;

			float acc;
			float accEndTime;
			float dec;
			float decStartTime;
			float speed;

			enum Type {

				T_Speed = 0, T_Acc, T_Dec, T_Acc_Dec
			};

			Type type;
		};
}

#endif