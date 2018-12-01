#include "WOFMatchFootballer_Body.h"

#ifdef FOOTBALLER_BODY1

namespace WOF { namespace match {

Footballer_Body::BallVelocityType Footballer_Body::actBall_getVelocityType(const Ball& ball, const BallVelocityTypeRange& range) {

	return BVT_Slow;

}

void Footballer_Body::actBall_Idle(const DiscreeteTime& time, const float& tickLength) {


}

} }

#endif