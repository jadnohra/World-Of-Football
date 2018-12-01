#ifndef _ScriptInputClient_h_WE_Input
#define _ScriptInputClient_h_WE_Input

#include "../../WEPtr.h"
#include "Source.h"
#include "../InputClient.h"
#include "InputManager.h"

namespace WE { namespace Input {


	class ScriptInputClient : public HardRef<InputClient> {
	public:

		typedef InputClient::IDType IDType;

	public:

		inline bool script_isValid() { return isValid(); }

		void setName(const TCHAR* name) { dref().setName(name); }
		const TCHAR* getName() { return dref().getName(); }

		bool addSource(ScriptSource* pSource) { return dref().addSource(::dref(pSource)); }
		bool addMappedSource(ScriptSource* pSource, IDType ID, bool invMap) { return dref().addSource(::dref(pSource), true, invMap, ID); }
		bool removeSource(ScriptSource* pSource) { return dref().removeSource(::dref(pSource)); }

		bool hasSourceID(ScriptSource* pSource) { return dref().hasSourceID(::dref(pSource)); }
		IDType getSourceID(ScriptSource* pSource) { IDType ID = -1; if (!dref().getSourceID(::dref(pSource), ID)) { assrt(false); } return ID;  }
		
		ScriptSource getSourceByID(IDType ID) { return scriptArg(dref().getSourceByID(ID)); }

		ScriptSource startIterateEvents() { return scriptArg(dref().startIterateEvents()); }
		ScriptSource nextEvent() { return scriptArg(dref().nextEvent()); }
		void breakIterateEvents()  { dref().breakIterateEvents(); }

		void clearEvents()  { dref().clearEvents(); }

		void destroy() { dref().destroy(); }

		void create(ScriptInputManager* pManager) { newClient(::dref(pManager)); }

	public:

		ScriptInputClient(InputClient* pClient = NULL) : HardRef<InputClient>(pClient) {}

		void newClient(InputManager* pManager) {

			SoftRef<InputClient> obj;
			WE_MMemNew(obj.ptrRef(), InputClient(pManager));

			set(obj);
		}

	public:

		ScriptInputClient(const ScriptInputClient& ref) : HardRef<InputClient>(ref.ptr()) {}
		inline ScriptInputClient& operator=(const ScriptInputClient& ref) { set(ref.ptr()); return *this; }


	public:

		static const TCHAR* ScriptClassName;

		static void declareInVM(SquirrelVM& target);
	};

	inline ScriptInputClient scriptArg(InputClient& val) { return ScriptInputClient(&val); }
	inline ScriptInputClient scriptArg(InputClient* val) { return ScriptInputClient(val); }

} }

#endif