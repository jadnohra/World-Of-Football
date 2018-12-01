#ifndef _SpatialQueryDataTypes_h_WOF_match
#define _SpatialQueryDataTypes_h_WOF_match

#include "../entity/ball/ballSimul/BallSimulation2.h"
#include "../WOFMatchDataTypes.h"

namespace WOF { namespace match {

	enum PathInterceptType {

		PI_None = 0, PI_InterceptorWait, PI_InterceptedWait
	};

	struct PathIntercept {

		PathInterceptType type;
		float analysisTime;
		SimulationID simulID;

		float arrivalTime;
		float waitTime; 

		unsigned int sampleIndex;

		Vector3 interceptPos;
		float expiryTime;

		PathIntercept() : type(PI_None), analysisTime(0.0f), simulID(-1) {}

		inline bool isValid() { return type != PI_None; }

		inline float getInterceptTime() const { return arrivalTime + waitTime;  } 

		inline void prepareForAnalysis(const SimulationID& _simulID, const float& time) {

			type = PI_None;
			simulID = _simulID;
			analysisTime = time;
		}

		void setWithoutData(const PathIntercept& ref) {

			type = ref.type;

			arrivalTime = ref.arrivalTime;
			waitTime = ref.waitTime;
		}

		void fillData(const BallSimulation2::Index& _sampleIndex, const BallSimulation2::Sample& sample) {

			interceptPos = sample.pos;
			expiryTime = sample.time;
			sampleIndex = _sampleIndex;
		}

		void setData(const Time& _expiryTime, const BallSimulation2::Index& _sampleIndex, const Vector3& _interceptPos) {

			interceptPos = _interceptPos;
			expiryTime = _expiryTime;
			sampleIndex = _sampleIndex;
		}

		inline void fillData(BallSimulation2& simul, const BallSimulation2::Index& sampleIndex) {

			fillData(sampleIndex, simul.getSample(sampleIndex));
		}
	};

	
} }

#endif