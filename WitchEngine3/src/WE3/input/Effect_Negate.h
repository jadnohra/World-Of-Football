#ifndef _Effect_Negate_h_WE_Input
#define _Effect_Negate_h_WE_Input

#include "../string/WEString.h"
#include "SourceHolder.h"
#include "Tracker.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class Effect_Negate : public Source, public Component_Generic1D, public Tracker {
	public:

		static bool create(Source& ref, const TCHAR* name, SourceHolder& source) {

			SoftRef<Effect_Negate> ret;

			WE_MMemNew(ret.ptrRef(), Effect_Negate(ref, name));

			if (!ret->init()) {

				assrt(false);
				ret.destroy();
			}

			source.set(ret);

			return source.isValid();
		}

	public:		

		virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
		virtual Time getTime() { return mRef->getTime(); }

		virtual const TCHAR* getName() { return mName.isEmpty() ? mRef->getName() : mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Virtual; }

		virtual bool addTracker(InputManager& manager, Tracker* pTracker);
		virtual bool removeTracker(InputManager& manager, Tracker* pTracker);
	
		virtual void mark(Controller* pOwner) { mRef->mark(pOwner); }
		virtual Controller* getMark() { return mRef->getMark(); }

	public:

		virtual bool isDiscrete() { return mRefComp->isDiscrete(); }

		//discrete
		virtual int getStepCount() { return mRefComp->getStepCount(); }
		
		//continuous
		virtual float getMin() { return -mRefComp->getMin(); }
		virtual float getMax() { return -mRefComp->getMax(); }

		virtual float getValue() { return -mRefComp->getValue(); }

	protected:

		virtual void onSourceChanged(Source* pVarImpl);

	protected:


		Effect_Negate(Source& ref, const TCHAR* name) 
			: mRef(&ref), mName(name) {
		}

		bool init() {

			mRefComp = (Component_Generic1D*) mRef->getComponent(ComponentID_Generic1D);

			return mRefComp.isValid();
		}

	protected:

		HardRef<Source> mRef;
		SoftPtr<Component_Generic1D> mRefComp;
		String mName;
	};

} }

#endif