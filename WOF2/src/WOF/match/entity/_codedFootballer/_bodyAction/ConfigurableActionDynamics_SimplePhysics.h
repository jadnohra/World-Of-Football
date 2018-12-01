#ifndef _ConfigurableActionDynamics_SimplePhysics_h
#define _ConfigurableActionDynamics_SimplePhysics_h

#include "WOFMatchFootballer_ConfigurableAction.h"

namespace WOF { namespace match {


	/*
	class ConfigurableActionDynamics_SimplePhysics : public Footballer_ConfigurableAction::Dynamics {
	public:

		virtual bool load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		virtual void start(const Vector3& startAcc, const Vector3& startVel, const Time& t);
		virtual void stop(const Time& t);

		virtual const Vector3& getAcc() { return mAcc; }
		virtual const Vector3& getVel() { return mVel; }

		virtual bool frameMove(const Time& t, const Time& dt);

	protected:

		class AccelerationController {
		public:

			virtual ~AccelerationController() {}

			virtual bool load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) = 0;
			virtual bool frameMove(const Time& t, const Time& dt, Vector3& inOutVel) = 0;
		};

		class AccelerationController_Acceleration : public AccelerationController {
		public:

			virtual bool load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
			virtual bool frameMove(const Time& t, const Time& dt, Vector3& inOutVel);

		protected:

			InitValue mInitAcc;
			Vector3 mAcc;
		};

	protected:

		struct InitValue {

			Vector3 base;
			Vector3 inputMultiplier;

			InitValue();

			bool load(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
			void setup(const Vector3& input, Vector3& result);
		};
		
		InitValue mInitAcc;

		Vector3 mAcc;
		Vector3 mVel;
	};
	*/

} }

#endif