#ifndef _Effect_2SourcesToAxis_h_WE_Input
#define _Effect_2SourcesToAxis_h_WE_Input

#include "../string/WEString.h"

#include "Source.h"
#include "Tracker.h"
#include "Component_Generic1D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class SourceHolder;

	class Effect_2SourcesToAxis : public Source, public Component_Generic1D, public Tracker {
	public:

		static bool create(InputManager& manager, Source& neg, Source& pos, const TCHAR* name, SourceHolder& source);

	public:		

		virtual void* getComponent(int compID) { return compID == ComponentID_Generic1D ? (Component_Generic1D*) this : NULL; }
		virtual Time getTime() { return MMax(mNeg->getTime(), mPos->getTime()); }

		virtual const TCHAR* getName() { return mName.isEmpty() ? mPos->getName() : mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Axis; }
	
		virtual void mark(Controller* pOwner) { mNeg->mark(pOwner); mPos->mark(pOwner); }
		virtual Controller* getMark() { return mNeg->getMark(); }

	public:

		virtual bool isDiscrete() { return false; }

		//continuous
		virtual float getMin() { return -1.0f; }
		virtual float getMax() { return 1.0f; }

		virtual float getValue() { return mValue; }

	protected:

		virtual void onSourceChanged(Source* pVarImpl);

	protected:


		Effect_2SourcesToAxis(InputManager& manager, Source& neg, Source& pos, const TCHAR* name);
		bool init();

		virtual ~Effect_2SourcesToAxis();

	protected:

		String mName;

		SoftPtr<InputManager> mManager;
		HardRef<Source> mNeg;
		HardRef<Source> mPos;
		
		float mValue;
	};

} }

#endif