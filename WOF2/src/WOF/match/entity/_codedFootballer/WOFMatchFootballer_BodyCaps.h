#ifdef FOOTBALLER_BODY3

#ifndef _WOFMatchFootballer_BodyCaps_h
#define _WOFMatchFootballer_BodyCaps_h


namespace WOF { namespace match {

	/*
		Capabilities are materialized by explicit settings structures
		so that body and AI use the same logic and AI knows the body 
		capabilities clearly, so changes in capabilities (and body behaviour)
		will force changes in AI code which is only logical ...
		the bain is based on knowledge about its body capabilities.
		we dont want a brain with a black box body.

		there is no use decoupling AI from explicit implementation of body caps.
		so even per example if transition length becomes variable, the settings for 
		it must explicitely appear here

		in short this is the way the brain gets its perfect information about 
		what the body can do.

		response time: after the concerned action is taken, respTime secs.
						must elapse b4 the same action is taken again

		all actions have by design 0 response time, this is because the 
		game's full control philosophy. 
		this creates the 'problem' that the AI could per example change the direction
		by 90 degrees every frame without constraints.
		one would be tempted to not allow it doing this by introducing:
			* non-zero reponse time
			* limit on amount of degree change per second.

		however those directly contradict the full and fast control philosophy.
		and in WOF, such behaviour should be allowed with no problems, even 
		though we know it could produce such 'legal' behaviour.

		the solution is obivous: we really dont want to penalize instantaneous
		large movements (like turning 90 degrees in 1 frame). we WANT this capability.
		what we do not want is the abuse of this capability. and this can be achieved
		in a nice way: we penalize the 'abuse' of the capability.
		
		this means fast large movements are still instantaneous, but the more of them 
		are executed in a row the more other parameters are negatively affected.
		such parameters might be 'stamina', 'concentration', 'ball control'
		or even maybe a special parameter designed exactly for this purpose.

	*/

	struct FootballerBodyCaps {

			struct DribbleCap {

			float zoneDirect; //half circle zone in degrees where one can dribble to directly
			float zoneTransition; //zone where one can dribble to but with a transition
			float zoneRest; //unreachable dribble zone, ball control lost

			//current ball control muliplied by controlCoeff
			float controlCoeffDirect; 
			float controlCoeffTransition; 

			float transitionLength; //in seconds
		};

		/*
			empty unless we decide to introduce transitions here as well
		*/
		struct RunCap {
		};
	};

} }

#endif

#endif