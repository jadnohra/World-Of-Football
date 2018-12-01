#ifndef _WOFMatchFootballer_h
#define _WOFMatchFootballer_h

#include "WOFMatchFootballer_Brain.h"

namespace WOF { namespace match {

	class Footballer : public Footballer_Brain {
	public:

		static inline Footballer* footballerFromObject(Object* pObject) {
			return (Footballer*) pObject;
		}

		static inline Footballer* footballerFromNode(SceneNode* pNode) {
			return (Footballer*) pNode;
		}
	
	public:

		bool isInited();
		
		void _cancel_init_create();
		bool _init_create(Match& match, TeamEnum team, FootballerNumber number);

		bool _init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);

		bool init_create(Match& match, BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		bool init_nodeAttached(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv);
		void init_prepareScene();

		Footballer();
		
		void frameMove(const DiscreeteTime& time);
		void render(Renderer& renderer, bool cleanIfNeeded, bool flagExtraRenders, RenderPassEnum pass);

	protected:

		void cancelInit();
	};
} }

#endif