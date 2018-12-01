#ifndef _Effect_Constant_h_WE_Input
#define _Effect_Constant_h_WE_Input

#include "../string/WEString.h"
#include "SourceHolder.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class Effect_Constant : public Source, public Component_Generic1D {
	public:

		static bool create(const float& value, const TCHAR* name, SourceHolder& source) {

			SoftRef<Effect_Constant> ret;

			WE_MMemNew(ret.ptrRef(), Effect_Constant(value, name));

			source.set(ret);

			return source.isValid();
		}

	public:		

		virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
		virtual Time getTime() { return 0.0f; }

		virtual const TCHAR* getName() { return mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Virtual; }

		virtual bool addTracker(InputManager& manager, Tracker* pTracker) { return true; }
		virtual bool removeTracker(InputManager& manager, Tracker* pTracker) { return true; }
	
		virtual void mark(Controller* pOwner) { mMarkOwner = pOwner; }
		virtual Controller* getMark() { return mMarkOwner; }

	public:

		virtual bool isDiscrete() { return true; }

		//discrete
		virtual int getStepCount() { return 1; }
		
		//continuous
		virtual float getMin() { return mValue; }
		virtual float getMax() { return mValue; }

		virtual float getValue() { return mValue; }

	protected:


		Effect_Constant(const float& value, const TCHAR* name) 
			: mValue(value), mName(name) {
		}

	protected:

		float mValue;
		String mName;

		SoftPtr<Controller> mMarkOwner;
	};

} }

#endif