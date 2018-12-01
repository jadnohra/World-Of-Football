#ifndef _WOFScriptObject_h
#define _WOFScriptObject_h

#include "WE3/WEAssert.h"
using namespace WE;

#include "includeScriptingAPI.h"

namespace WOF {

	class ScriptObject {
	public:

		ScriptObject() : mIsValid(false) {

			mHandle.Reset();
		}

		inline operator HSQOBJECT*() { return &mHandle.GetObjectHandle(); }
		inline operator HSQOBJECT&() { return mHandle.GetObjectHandle(); }
		inline bool isValid() { return mIsValid; }

		void validate() {

			setValid(isNull() ? false : true);
		}

		bool isNull() {

			return (mHandle.GetType() == OT_NULL);
		}

		void setValid(bool isValid) { 
		
			if (mIsValid && isValid)
				assrt(false);

			mIsValid = isValid; 
		}

		void unbind() { 
		
			if (mIsValid) {

				mIsValid = false; 
				mHandle.Reset();
			}
		}

		inline SquirrelObject& get() { return mHandle; }
		
		void set(const SquirrelObject& obj) { 
			
			mHandle = obj; 
			validate();
		}

		inline void operator=(SquirrelObject& obj) { set(obj); }

	protected:

		SquirrelObject mHandle;
		bool mIsValid;
	};
}

#endif