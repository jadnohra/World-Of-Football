#include "SpeedProfile.h"

#include "WE3/WEAssert.h"
using namespace WE;

#include "float.h"

namespace WOF { 


void Distance::zero() {

	dist.zero();
	dir.zero();
	mag = 0.0f;
}

void Distance::update() {

	dist.normalize(dir, mag);
}

void Distance::subtract(const Vector3& diff) {

	dist.subtract(diff);

	update();
}


void Distance::add(const Vector3& added) {

	dist.add(added);

	update();
}

bool Distance::moveTo(const Vector3& target, const float& targetMag, const float& moveMag) {

	if (targetMag > 0.0f) {

		Distance diff;

		target.subtract(dist, diff.dist);

		diff.update();

		if (diff.mag <= moveMag) {

			dist = target;
			update();

			return true;
		} 
		
		Vector3 move;

		diff.dir.mul(moveMag, move);

		add(move);

		return false;

	} else {

		if (mag != 0.0f) {

			if (moveMag == 0.0f) {

				zero();
			
			} else {

				dist.substractNoReverse(moveMag);
			}

			update();
			return false;
		}

		return true;
	}
}

SpeedProfile::Setup::Setup() 
	: type(T_None), distance(0.0f), time(0.0f) {
}

void SpeedProfile::Setup::invalidate() {

	type = T_None;
}

bool SpeedProfile::Setup::isValid() {

	return type != T_None;
}

void SpeedProfile::Setup::init(BufferString& tempStr, DataChunk& chunk, CoordSysConv* pConv) {

	if (!chunk.getAttribute(L"type", tempStr)) {

		tempStr.assign(L"speed");
	}

	if (tempStr.equals(L"ratios")) {

		type = T_Ratios;

		accRatio = 0.0f;
		decRatio = 0.0f;

		chunk.scanAttribute(tempStr, L"acc", L"%f", &accRatio);
		chunk.scanAttribute(tempStr, L"dec", L"%f", &decRatio);

	} else if (tempStr.equals(L"intervals")) {

		type = T_Intervals;

		accInterval = 0.0f;
		decInterval = 0.0f;

		chunk.scanAttribute(tempStr, L"acc", L"%f", &accInterval);
		chunk.scanAttribute(tempStr, L"dec", L"%f", &decInterval);
				

	} else if (tempStr.equals(L"accelerations")) {

		type = T_Accelerations;

		accRef = 0.0f;
		decRef = 0.0f;

		chunk.scanAttribute(tempStr, L"acc", L"%f", &accRef);
		chunk.scanAttribute(tempStr, L"dec", L"%f", &decRef);

	} else {

		

		speed = 0.0f;

		if (chunk.scanAttribute(tempStr, L"speed", L"%f", &speed)) {

			type = T_Speed;

		} else if (chunk.scanAttribute(tempStr, L"interval", L"%f", &speedInterval)) {

			type = T_SpeedInterval;

		} else {

			type = T_None;
		}
	}

	distance = 0.0f;
	chunk.scanAttribute(tempStr, L"distance", L"%f", &distance);

	time = 0.0f;
	chunk.scanAttribute(tempStr, L"time", L"%f", &time);
}


SpeedProfile::SpeedProfile() : mIsValid(false) {
}

void SpeedProfile::invalidate() {

	mIsValid = false;
}

bool SpeedProfile::isValid() {

	return mIsValid;
}

void SpeedProfile::setup(const Setup& settings, CoordSysConv* pConv, Time totalDist, Time totalTime) {

	switch (settings.type) {

		//T_Speed
		default: {

			setupUsingSpeed(settings.speed, pConv);

		} break;

		case (Setup::T_SpeedInterval): {

			setupUsingSpeedInterval(totalDist, totalTime, settings.speedInterval, pConv);

		} break;
		
		case (Setup::T_Ratios): {

			setupUsingRatios(totalDist, totalTime, settings.accRatio, settings.decRatio, pConv);

		} break;

		case (Setup::T_Intervals): {

			setupUsingRatios(totalDist, totalTime, settings.accInterval, settings.decInterval, pConv);

		} break;

		case (Setup::T_Accelerations): {

			setupUsingAccelerations(totalDist, totalTime, settings.accRef, settings.decRef, pConv);

		} break;
	}
}

void SpeedProfile::setupUsingAccelerations(const float& totalDist, const float& totalTime, const float& refAcc, const float& refDec, CoordSysConv* pConv) {

	mIsValid = true;

	const float& d = totalDist;
	const float& t = totalTime;

	acc = refAcc;
	dec = refDec;
	
	float accDec = 0.1f * acc;
	float decDec = 0.1f * dec;

	bool retry = true;

	float decInterval;

	while(retry) {

		retry = false;
		
		if (!retry && acc > 0.0f) {

			const float& v1 = acc;
			const float& v2 = dec;
			float* pInterval = &accEndTime;


			float A = -v1 * (0.5f + v1 / v2);
			float B = v1 * (t + 0.5f);
			float C = -totalDist;

			float part = B * B - 4.0f * A * C;

			if (part > 0.0f) {

				part = sqrtf(part);

				*pInterval = -B + part / 2.0f * A;

			} else {

				retry = true;
			}
		}

		if (!retry && dec > 0.0f) {

			const float& v1 = dec;
			const float& v2 = acc;
			float* pInterval = &decInterval;


			float A = -v1 * (0.5f + v1 / v2);
			float B = v1 * (t + 0.5f);
			float C = -totalDist;

			float part = B * B - 4.0f * A * C;

			if (part > 0.0f) {

				part = sqrtf(part);

				*pInterval = -B + part / 2.0f * A;

			} else {

				retry = true;
			}

			if (!retry) {

				decStartTime = t - decInterval;
			}
		}

		if (retry) {

			acc -= accDec;

			if (acc < 0.0f)
				acc = 0.0f;

			dec -= decDec;

			if (dec < 0.0f)
				dec = 0.0f;
		}
	}

	speed = acc * accEndTime;

	if (accEndTime == 0.0f)
		acc = FLT_MAX;

	if (decInterval == 0.0f)
		dec = FLT_MAX;

	updateType();
}


void SpeedProfile::setupUsingRatios(const float& totalDist, const float& totalTime, const float& accRatio, const Time& decRatio, CoordSysConv* pConv) {

	mIsValid = true;

	setupUsingIntervals(totalDist, totalTime, totalTime * accRatio, totalTime * decRatio, pConv);
}

void SpeedProfile::setupUsingIntervals(const float& totalDist, const float& totalTime, const Time& inAccInterval, const Time& inDecInterval, CoordSysConv* pConv) {
	
	mIsValid = true;

	Time speedInterval;
	Time accInterval = inAccInterval;
	Time decInterval = inDecInterval;

	int maxIter = 10;

	float accDecrement = 0.1f * accInterval;
	float decDecrement = 0.1f * decInterval;

	while (maxIter-- >= 0) {

		speedInterval = totalTime - (accInterval + decInterval);
		speed = totalDist / ( 0.5f * accInterval + speedInterval + 0.5f * decInterval);

		if (speed > 0.0f)
			break;

		accInterval -= accDecrement;
			
		if (accInterval < 0.0f)
			accInterval = 0.0f;

		decInterval -= decDecrement;

		if (decInterval < 0.0f)
			decInterval = 0.0f;
	}

	if (speed <= 0.0f) {

		assrt(false);

		accInterval = 0.0f;
		decInterval = 0.0f;

		if (speedInterval == 0.0f) 
			speedInterval = 1.0f;

		speed = totalDist / speedInterval;
		
		acc = 0.0f;
		accInterval = 0.0f;

		dec = 0.0f;
		decInterval = 0.0f;

		
	} else {

		acc = speed / accInterval;
		dec = speed / decInterval;
	}

	decStartTime = accInterval + speedInterval;

	if (pConv) {

		if (acc != FLT_MAX)
			pConv->toTargetUnits(acc);

		pConv->toTargetUnits(speed);

		if (dec != FLT_MAX)
			pConv->toTargetUnits(dec);
	}

	updateType();
}

void SpeedProfile::setupUsingSpeedInterval(const float& totalDist, const float& totalTime, const float& speedInterval, CoordSysConv* pConv) {

	setupUsingSpeed(totalDist / speedInterval, pConv);
}

void SpeedProfile::setupUsingSpeed(const float& ctSpeed, CoordSysConv* pConv) {

	mIsValid = true;

	speed = ctSpeed;
	type = T_Speed;

	if (pConv) {

		pConv->toTargetUnits(speed);
	}

	acc = FLT_MAX;
	dec = FLT_MAX;
}

void SpeedProfile::updateType() {

	if (acc < FLT_MAX) {

		if (dec < FLT_MAX) {

			type = T_Acc_Dec;

		} else {

			type = T_Acc;
		}

	} else {

		if (dec < FLT_MAX) {

			type = T_Dec;

		} else {

			type = T_Speed;
		}
	}
}

float SpeedProfile::calcSpeed(const Time& startTime, const Time& time, const Time& dt, const float& currSpeed) {

	float outSpeed = currSpeed;

	switch(type) {

		case T_Acc: {

			Time timeDiff = time - startTime;

			if (timeDiff < accEndTime) {

				outSpeed += dt * acc;

			} else {

				outSpeed = speed;
			}

		} break;

		case T_Dec: {

			Time timeDiff = time - startTime;

			if (timeDiff >= decStartTime) {

				outSpeed -= dt * dec;

				if (outSpeed < 0.0f)
					outSpeed = 0.0f;

			}  else {

				outSpeed = speed;
			}

		} break;

		case T_Acc_Dec: {

			Time timeDiff = time - startTime;

			if (timeDiff < accEndTime) {

				outSpeed += dt * acc;

			} else if (timeDiff >= decStartTime) {

				outSpeed -= dt * dec;

			} else {

				outSpeed = speed;
			}

			if (outSpeed < 0.0f)
				outSpeed = 0.0f;

		} break;

		default: {

			outSpeed = speed;
		} break;
	}

	return outSpeed;
}

}