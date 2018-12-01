
/*
	This can be improved by adding a ball hit offset, which will also provide rotation
*/

class FootballerBallShotConfiguration {

	opposite = false;
	difficulty = 1.0;
	soundMag = 0.0;
};

::FootballerBallShot_None <- -1;
::FootballerBallShot_Foot <- 0;
::FootballerBallShot_Body <- 1;
::FootballerBallShot_Header <- 2;

class FootballerBallShot {

	isValid = false;
	speed = 0.0;
	horizRatio = 1.0;
	wHorizRatio = 0.0;
	wVertRatio = 0.0;
	
	enableAfterTouch = false;
	type = -1;
	
	constructor(_isValid, _type, _speed, angleDeg, _wHorizRatio, _wVertRatio, _enableAfterTouch) {
	
		isValid = _isValid;
		
		type = _type;
	
		speed = _speed;
		horizRatio = 1.0 - sinfDeg(angleDeg);
		wHorizRatio = _wHorizRatio;
		wVertRatio = _wVertRatio;
		enableAfterTouch = _enableAfterTouch;
	}
	
	function getShotConfiguration(footballer, ball, shotV, shotVMag, maxShotSpeed) {
	
		local settings = FootballerBallShotConfiguration();
	
		local ballVel = ball.getVel();
		local ballVelMag = ballVel.mag();
		
		local velMagDiff = ballVelMag - shotVMag;
		local velDirDot = ballVel.dot(shotV) / (ballVelMag * shotVMag);
		
		settings.opposite = velDirDot < 0.0;
		
		if (ballVelMag < 0.1 * maxShotSpeed) {
		
			settings.difficulty = 0.65;
			settings.soundMag = fabs(velMagDiff) * 0.25;
			
		} else {
		
			if (velDirDot > 0.0) {
			
				if (velMagDiff < 0.0) {
				
					settings.difficulty = 2.0;
					settings.soundMag = fabs(velMagDiff) * 0.5;
					
				} else {
				
					settings.difficulty = 1.2;
					settings.soundMag = fabs(velMagDiff);
				}
			
			} else {
			
				settings.difficulty = 1.2;
				settings.soundMag = ballVelMag + shotVMag;
			}
		}
		
		return settings;
	}
	
	function syncShotWToShotV(match, ball, shotV, shotW, shotConfig, residualFactor) {
	
		if (wHorizRatio && wVertRatio) {
			
			local temp = Vector3();
					
			ball.calcVelocitySyncedW(shotV, match.SceneUpAxis(), temp);
			temp.mulToSelf(wHorizRatio);
			
			ball.calcVelocitySyncedW(shotV, match.SceneRightAxis(), shotW);
			shotW.mulToSelf(wVertRatio);
			
			shotW.addToSelf(temp);
			
		} else if (wHorizRatio) {
		
			ball.calcVelocitySyncedW(shotV, match.SceneRightAxis(), shotW);
			shotW.mulToSelf(wHorizRatio);
			
		} else if (wVertRatio) {
		
			ball.calcVelocitySyncedW(shotV, match.SceneUpAxis(), shotW);
			shotW.mulToSelf(wVertRatio);
			
		} else {
		
			shotW.zero();
		}
		
		if (residualFactor != 0.0) {
		
			local residualW = Vector3();
			
			ball.getW().mul(shotConfig.opposite ? 0.5 : 1.0, residualW);
		
			//printV3("residualW", residualW);
		
			lerpv3ToSource(shotW, residualW, residualFactor);
		}
	}
	
	function getVelocity(footballer, ball, shotDir, _shotSpeed, addedVel, shotV, shotW, speedVariance, dirVariance, controlSkill, allowDownShot) {
	
		local match = footballer.impl.getMatch();
		local shotSoundMag = 0.0;
		local isHeader = type == ::FootballerBallShot_Header;
		local ballSpeed = ball.getVel().mag();
		local maxShotSpeed = maxf(ballSpeed, footballer.getPhysicalPowerShotMaxSpeed());
		
		local shotSpeed = 0.0;
		
		//print("_shotSpeed " + _shotSpeed);
		
		if (isHeader) {
		
			shotSpeed = _shotSpeed + ballSpeed * 0.8;
			maxShotSpeed = shotSpeed;
			
		} else {
		
			shotSpeed = minf(_shotSpeed, maxShotSpeed);
		}		
		
		//print("shotSpeed " + shotSpeed);
		
		shotDir.mul(shotSpeed * horizRatio, shotV);
		match.SceneUpAxis().mulAndAdd(shotSpeed * (1.0 - horizRatio), shotV);
	
		//printV3("shotVStart", shotV);
	
		if (addedVel != null) {
		
			//printV3("preAddedVel-shotV", shotV);
		
			shotV.addToSelf(addedVel);
			shotSpeed = shotV.mag();
			
			//printV3("postAddedVel-shotV", shotV);
		}
			
		local shotConfig = getShotConfiguration(footballer, ball, shotV, isHeader ? _shotSpeed : shotSpeed, maxShotSpeed);
				
		if (speedVariance != 0.0) {
		
			speedVariance *= (0.5 * shotConfig.difficulty);
			
			if (isHeader)
				speedVariance *= 0.3;
			
			//print("speedVariance" + speedVariance);
			
			local speedDiffVariance = normalDistribution(0.0, speedVariance);
					
			local newShotVMag = minf(maxf(0.0, shotSpeed + speedDiffVariance), maxShotSpeed);
			
			shotV.normalize();
			shotV.mulToSelf(newShotVMag);
			
			//printV3("speedVar-shotV", shotV);
			
			shotConfig.soundMag *= newShotVMag / shotSpeed;
			shotSpeed = newShotVMag;
		}
		
		if (dirVariance != 0.0) {
		
			dirVariance *= shotConfig.difficulty;
		
			if (isHeader && _shotSpeed > 0.0 && ballSpeed > _shotSpeed) {
			
				dirVariance *= minf(ballSpeed / _shotSpeed, 2.0);
			}
		
			local dirDiffVariance = normalDistribution(0.0, dirVariance);
			
			//printV3("dirVar-pre-shotV", shotV);
			//print ("dirDiff: " + dirDiffVariance);
			
			//we multiply by 2 because the rotation axis is random
			shotV.randomizeDirToSelf(2.0 * dirDiffVariance);
			
			//printV3("dirVar-post-shotV", shotV);
			
			if (!allowDownShot) {
			
				if (shotV.getEl(match.Scene_Up) < 0.0) {
				
					shotV.setEl(match.Scene_Up, - shotV.getEl(match.Scene_Up));
				}
			}
		}
		
		local residualFactor = 0.0;
		
		if (controlSkill < 1.0) {
		
			local val1 = fabs(normalDistribution(0.0, 1.0 - controlSkill));
			local val2 = minf(1.0, val1);
			
			//print("vals: " + val1 + ", " + val2);
		
			residualFactor = val2;//minf(1.0, fabs((normalDistribution(0.0, 1.0 - controlSkill) / dirVariance)));
			local residualV = Vector3();
			
			ball.getVel().mul(residualFactor, residualV);
			
			if (residualV.dot(shotV) < 0.0) {
			
				local invRight = randf(0.0, 1.0) > 0.5;
				
				if (invRight)
					residualV.setEl(match.Scene_Right, -residualV.getEl(match.Scene_Right));
				
				if (residualV.dot(shotV) < 0.0) 
					residualV.setEl(match.Scene_Fwd, -residualV.getEl(match.Scene_Fwd));
				
				
				if (residualV.dot(shotV) < 0.0) 
					residualV.zero();
			}
			
			//printV3("residualV", residualV);
			//print ("residualFactor: " + residualFactor);
			
			//lerpv3ToSource(shotV, residualV, residualFactor);
			shotV.addToSelf(residualV);
			
			shotSpeed = shotV.mag();
			
			if (shotSpeed > maxShotSpeed) {
			
				shotV.mulToSelf(shotSpeed / maxShotSpeed);
				shotSpeed = maxShotSpeed;
			}
		}
	
		//printV3("shotVEnd", shotV);
	
		syncShotWToShotV(match, ball, shotV, shotW, shotConfig, residualFactor);
	
		return 0.7 * sqrtf(distSq(shotV, ball.getVel()));
	}
	

	function set(ref) {
	
		if (ref == null) {
		
			isValid = false;
		
		} else {
		
			isValid = ref.isValid;
			
			type = ref.type;
			speed = ref.speed;
			horizRatio = ref.horizRatio;
			wHorizRatio = ref.wHorizRatio;
			wVertRatio = ref.wVertRatio;
			enableAfterTouch = ref.enableAfterTouch;
		}
	}
}