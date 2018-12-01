#include "WETime.h"

#include "string.h"

using namespace WE;

DiscreeteTime::DiscreeteTime() {
	memset(this, 0, sizeof(DiscreeteTime));
}

void DiscreeteTime::init(Time _ticksPerSec) {

	ticksPerSec = _ticksPerSec;
	tickLength = 1.0f / ticksPerSec;
}

void DiscreeteTime::start(Time _t0) {
	
	t0 = _t0;
	
	t = 0;
	t_discreet = 0;

	dt = 0;
	dt_discreet = 0;

	frameTicks = 0;

	ticks = 0;
	frames = 0;

	added_dt = 0;
	addded_dt_discreet = 0;
}

/*
void DiscreeteTime::add(Time _dt) {
	
	frames++;
	ticks += frameTicks;

	dt = _dt;
	t += dt;

	frameTicks = (unsigned int) ((t ) / tickLength);
	frameTicks -= ticks;
	
	dt_discreet = ((Time) frameTicks) * tickLength;
	t_discreet += dt_discreet;
}
*/

void DiscreeteTime::consume() {

	dt = 0;
	dt_discreet = 0;
}

void DiscreeteTime::add(Time _dt) {
	
	tentative_dt = _dt;
	tentative_t = t + tentative_dt + added_dt;

	tentative_frameTicks = (unsigned int) ((tentative_t /*- t0*/) / tickLength);
	tentative_frameTicks -= ticks;
	
	tentative_dt_discreet = ((Time) tentative_frameTicks) * tickLength;

	added_dt += tentative_dt;
	addded_dt_discreet += tentative_dt_discreet;
}

void DiscreeteTime::accept() {
	
	++frames;

	dt = tentative_dt;
	dt_discreet = tentative_dt_discreet;
	t = tentative_t;
	t_discreet += dt_discreet;
	frameTicks = tentative_frameTicks;
	ticks += frameTicks;

	added_dt = 0;
	addded_dt_discreet = 0;
	tentative_dt = 0;
	tentative_dt_discreet = 0;
	tentative_frameTicks = 0;
	tentative_t = 0;
}