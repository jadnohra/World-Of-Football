#ifndef _Effect_And_h_WE_Input
#define _Effect_And_h_WE_Input

#include "../string/WEString.h"

#include "Source.h"
#include "Tracker.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class SourceHolder;

	class Effect_And : public Source, public Component_Generic1D, public Tracker {
	public:

		static bool create(InputManager& manager, Source& arg1, Source& arg2, const TCHAR* name, SourceHolder& source);

	public:		

		virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
		virtual Time getTime() { return MMax(mArg1->getTime(), mArg2->getTime()); }

		virtual const TCHAR* getName() { return mName.isEmpty() ? mArg2->getName() : mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Virtual; }
	
		virtual void mark(Controller* pOwner) { mArg1->mark(pOwner); mArg2->mark(pOwner); }
		virtual Controller* getMark() { return mArg1->getMark(); }

	public:

		virtual bool isDiscrete() { return true; }
		virtual int getStepCount() { return 2; }

		//continuous
		virtual float getMin() { return 0.0f; }
		virtual float getMax() { return 1.0f; }

		virtual float getValue() { return mValue; }

	protected:

		virtual void onSourceChanged(Source* pVarImpl);

	protected:


		Effect_And(InputManager& manager, Source& arg1, Source& arg2, const TCHAR* name);
		bool init();
		void setValue(const float& v1, const float& v2, const bool& notify);

		virtual ~Effect_And();

	protected:

		String mName;

		SoftPtr<InputManager> mManager;
		HardRef<Source> mArg1;
		HardRef<Source> mArg2;
		
		float mValue;
	};

} }

#endif