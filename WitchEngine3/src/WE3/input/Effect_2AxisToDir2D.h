#ifndef _Effect_2AxisToDir2D_h_WE_Input
#define _Effect_2AxisToDir2D_h_WE_Input

#include "../string/WEString.h"

#include "Source.h"
#include "Tracker.h"
#include "Component_Dir2D.h"
#include "ComponentIDs.h"
#include "SourceType.h"

namespace WE { namespace Input {

	class SourceHolder;

	class Effect_2AxisToDir2D : public Source, public Component_Dir2D, public Tracker {
	public:

		static bool create(InputManager& manager, Source& axis1, Source& axis2, const TCHAR* name, SourceHolder& source);

	public:		

		virtual void* getComponent(int compID) { return compID == ComponentID_Dir2D ? (Component_Dir2D*) this : NULL; }
		virtual Time getTime() { return MMax(mAxis1->getTime(), mAxis2->getTime()); }

		virtual const TCHAR* getName() { return mName.isEmpty() ? mAxis2->getName() : mName.c_tstr(); }
		virtual const TCHAR* getType() { return SourceType::Dir2D; }
	
		virtual void mark(Controller* pOwner) { mAxis1->mark(pOwner); mAxis2->mark(pOwner); }
		virtual Controller* getMark() { return mAxis1->getMark(); }

	public:

		virtual float getValue1() { return mValues[0]; }
		virtual float getValue2() { return mValues[1]; }

		virtual float getRawValue1();
		virtual float getRawValue2();

		virtual ResponseCurve& createResponseCurve1();
		virtual ResponseCurve& createResponseCurve2();

		virtual void destroyResponseCurve1();
		virtual void destroyResponseCurve2();

		virtual ResponseCurve* getResponseCurve1() { return mCurves[0]; }
		virtual ResponseCurve* getResponseCurve2() { return mCurves[1]; }

		virtual void onResponseCurvesChanged();

	protected:

		virtual void onSourceChanged(Source* pVarImpl);

	protected:


		Effect_2AxisToDir2D(InputManager& manager, Source& axis1, Source& axis2, const TCHAR* name);
		bool init();
		
		virtual ~Effect_2AxisToDir2D();

		void setValues(const float& v1, const float& v2, const bool& notify);

	protected:

		String mName;

		SoftPtr<InputManager> mManager;
		HardRef<Source> mAxis1;
		HardRef<Source> mAxis2;
		
		float mValues[2];
		HardPtr<ResponseCurve> mCurves[2];
	};

} }

#endif