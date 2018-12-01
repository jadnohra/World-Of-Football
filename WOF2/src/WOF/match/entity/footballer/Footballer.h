#ifndef h_WOF_match_Footballer
#define h_WOF_match_Footballer

#include "../scriptedFootballer/ScriptedFootballer.h"

namespace WOF { namespace match {

	class Footballer : public ScriptedFootballer {
	public:

		static inline Footballer* footballerFromObject(Object* pObject) {
			return (Footballer*) pObject;
		}

		static inline const Footballer* footballerFromObject(const Object* pObject) {
			return (const Footballer*) pObject;
		}

		static inline Footballer* footballerFromNode(SceneNode* pNode) {
			return (Footballer*) pNode;
		}
	};

} }

#endif