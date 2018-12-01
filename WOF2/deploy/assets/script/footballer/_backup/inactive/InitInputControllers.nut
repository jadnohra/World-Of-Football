
class InputKeyFilter {

	filterCompID = null;
	filterMin = null;
	filterMax = null;
	
	constructor(inFilterCompID, inFilterMin, inFilterMax) {
	
		filterCompID = inFilterCompID;
		filterMin = inFilterMin;
		filterMax = inFilterMax;
	}
	
	function accept(source) {
	
		if (source == null || !source.isValid())
			return false;		
	
		if (filterCompID != null && !source.hasComponent(filterCompID)) 
			return false;	
			
		if (filterMin != null) {
		
			local comp1D = source.getGeneric1D();
			
			if (!comp1D.isValid())
				return false;
				
			if (comp1D.getMin() != filterMin)
				return false;
		}
			
		if (filterMax != null) {
		
			local comp1D = source.getGeneric1D();
			
			if (!comp1D.isValid())
				return false;
				
			if (comp1D.getMax() != filterMax)
				return false;
		}
		
		return true;
	}
}

function initInputControllers(manager) {
	
	log("\nInit Input Controllers Start");
	log("----------------------------");
	
	local count = manager.getControllerCount();
		
	local joystickCount = 0;
	
	for (local i = 0; i < count; i+=1) {
	
		local controller = manager.getController(i);
		
		if (controller.isValid() && controller.isDevice()) {
		
			controller.setVisible(false);
			
			if (controller.getName() == "Keyboard") {
			
				createKeyboard1(manager, controller);
				createKeyboard2(manager, controller);
				
			} else if (controller.getName() == "Joystick") {
			
				if (createJoystick(manager, controller, joystickCount)) {
				
					joystickCount+=1;
				}
			}
		}
	}
	
	log("--------------------------");
	
	return manager.getControllerCount() > count;
}

function createJoystickDualShock(manager, controller, index, useAlternatives) {

	local useLeftStick = true;

	local controllerName = "Joystick" + (index + 1);
	
	local t = 0.0;
	local usedAlternatives = [];
	local target = manager.createVirtualController(controllerName);
		
	if (!target.isValid())
		return false;
	
	
	/*
		Button 0 - Triangle
		Button 1 - Circle
		Button 2 - X
		Button 3 - Square
		
		Button 4 - Left Shoulder lower
		Button 5 - Right Shoulder lower
		
		Button 6 - Left Shoulder upper
		Button 7 - Right Shoulder upper
		
		Button 12 - Digital Up
		Button 13 - Digital Right
		Button 14 - Digital Down
		Button 15 - Digital Left
		
		X Axis - Left Stick Horiz
		Y Axis - Left Stick Vert (inverted)
		
		Z Rotation - Right Stick Horiz
		Z Axis - Right Stick Verti (inverted)
	*/
	local from = ["Button 1",   "Button 3",    "Button 0",   "Button 6",  "Button 4", "Button 5", "Button 2", "Button 7"];
	local to = 	 ["ShotLow",    "ShotHigh",    "ShotExtra",  "Modifier",    "Sprint",   "Jump", "Pass", "Modifier2"];

	if (!mapKeys(controller, target, from, to, useAlternatives, usedAlternatives, InputKeyFilter(null, 0.0, null))) 
		return false;

	{
	
		local leftStickAxisName = "Y Axis";
		local rightStickAxisName = "Z Axis";
	
		local key = null;
		
		if (useLeftStick) 
			key = controller.getSource(leftStickAxisName, t); 
		else
			key = controller.getSource(rightStickAxisName, t); 
	
		if (!key.isValid() && useAlternatives) {
		
			if (useLeftStick)
				key = controller.getSource(rightStickAxisName, t); 
			else 
				key = controller.getSource(leftStickAxisName, t); 
						
			if (!key.isValid()) {
			
				key = findAlternative(controller, leftStickAxisName, t, null, usedAlternatives,  InputKeyFilter(null, -1.0, 1.0));
			}
		}
	
		if (key.isValid()) {

			local negkey = null;	
			
			negkey = key.createNegative(manager, "FwdAxis");	
			
			if (!target.addSource(negkey, "FwdAxis"))
				return false;
				
		} else {
		
			return false;
		}
	}	
	
	{
	
		local leftStickAxisName = "X Axis";
		local rightStickAxisName = "Z Rotation";
	
		local key = null;
	
		if (useLeftStick) 
			key = controller.getSource(leftStickAxisName, t); 
		else 
			key = controller.getSource(rightStickAxisName, t); 
		
	
		if (!key.isValid() && useAlternatives) {
		
			if (useLeftStick) 
				key = controller.getSource(rightStickAxisName, t); 
			else
				key = controller.getSource(leftStickAxisName, t); 
			
			
			if (!key.isValid()) {
			
				key = findAlternative(controller, leftStickAxisName, t, null, usedAlternatives, InputKeyFilter(null, -1.0, 1.0));
			}
		}
	
	
		if (key.isValid()) {

			if (!target.addSource(key, "RightAxis"))
				return false;
				
		} else {
		
			return false;
		}
	}	
		
	{
		local key1 = controller.getSource("Button 0", t);
		
		if (!key1.isValid() && useAlternatives) {
		
			key1 = target.getSource("ShotExtra", t);
		}
		
		local key2 = controller.getSource("Button 2", t);
		
		if (!key2.isValid() && useAlternatives) {
		
			key2 = target.getSource("Pass", t);
		}
			
		local key = CInputSource.createAnd(manager, key1, key2, "SetDeadZone");
			
		if (!target.addSource(key, "SetDeadZone"))
			return false;
	}
	
	manager.addController(target);

	return true;
}

function createJoystick(manager, controller, index) {

	local deviceName = controller.getDeviceName().toupper();
	
	/*
	if (deviceName.find("DUAL") != null
		&& deviceName.find("PS") != null) {
		
		return createJoystickDualShock(manager, controller, index, false);
	}
	*/
	
	//try to create even if name doesnt match
	if (createJoystickDualShock(manager, controller, index, false)) {
	
		log("Using DualShock Joystick: " + deviceName);
		return true;
	}
		
	//try to create using alternatives
	if (createJoystickDualShock(manager, controller, index, true)) {
	
		log("Using Generic Joystick: " + deviceName);
		return true;
	}
		
	log("Failed to figure out how to use Joystick: " + deviceName);
	
	return false;	
}

function createKeyboard1(manager, controller) {

	local target = manager.createVirtualController("Keyboard1");
		
	if (!target.isValid())
		return false;
	
	
	local from = ["Num1", "Num2",   "Num3",     "Num4",    "Num5",    "Num6",      ];
	local to = 	 ["Pass","ShotHigh","ShotExtra","ShotLow", "Modifier","Modifier2"];

	if (!mapKeys(controller, target, from, to, false, null, null)) 
		return false;

	if (!mapButtonsToAxisKey(manager, controller, target, "Down", "Up", "FwdAxis"))
		return false;
		
	if (!mapButtonsToAxisKey(manager, controller, target, "Left", "Right", "RightAxis"))
		return false;
	
			
	manager.addController(target);	
	
	return true;
}

function createKeyboard2(manager, controller) {

	local target = manager.createVirtualController("Keyboard2");
		
	if (!target.isValid())
		return false;
	
	
	local from = ["u", "i",   "o",     "k",    "j",    "l",      ];
	local to = 	 ["Pass","ShotHigh","ShotExtra","ShotLow", "Modifier","Modifier2"];

	if (!mapKeys(controller, target, from, to, false, null, null)) 
		return false;
		
	if (!mapButtonsToAxisKey(manager, controller, target, "s", "w", "FwdAxis"))
		return false;
		
		
	if (!mapButtonsToAxisKey(manager, controller, target, "a", "d", "RightAxis"))
		return false;	
	
	manager.addController(target);	
	
	return true;
}

function mapButtonsToAxisKey(manager, source, target, neg, pos, axis) {

	local t = 0.0;
	
	{
		local keyNeg = source.getSource(neg, t);
		local keyPos = source.getSource(pos, t);
	
		if (keyNeg.isValid() && keyPos.isValid()) {
		
			local axisVar = CInputSource.createAxisFrom2Sources(manager, keyNeg, keyPos, axis);
			
			if (axisVar.isValid()) {
		
				if (!target.addSource(axisVar, axis))
					return false;
			}
				
		} else {
		
			return false;
		}
	}
	
	return true;
}

function findAlternative(controller, origKeyName, t, excludedNames, usedAlternatives, altFilter) {

	local sourceCount = controller.getSourceCount();
	
	for (local i = 0; i < sourceCount; i+=1) {
	
		local source = controller.getSourceAt(i, t);
		
		if (source.isValid() && altFilter != null) {
		
			if (!altFilter.accept(source)) {
			
				source = null;
			}
		}
		
		if (source != null && source.isValid() && excludedNames != null) {
		
			foreach (name in excludedNames) {
			
				if (source.getName() == name) {
				
					source = null;
					break;
				}
			}
		}
		
		if (source != null && source.isValid() && usedAlternatives != null) {
		
			foreach (name in usedAlternatives) {
			
				if (source.getName() == name) {
				
					source = null;
					break;
				}
			}
		}
		
		if (source != null && source.isValid()) {
		
			local sourceName = source.getName();
		
			usedAlternatives.append(sourceName);
			log(controller.getDeviceName() + ", Alt. for " + origKeyName + " is " + sourceName);
		
			return source;
		}
	}
	
	log(controller.getDeviceName() + ", No Alt. for " + origKeyName);
	
	return null;
}

function mapKeys(source, target, from, to, findAlternatives, usedAlternatives, alternativeFilter) {

	local t = 0.0;

	foreach (i, val in from) {
	
		local key = source.getSource(val, t);
	
		if (!key.isValid() && findAlternatives) {

			key = findAlternative(source, val, t, from, usedAlternatives, alternativeFilter);
		}
		
		if (key != null && key.isValid()) {
		
			if (!target.addSource(key, to[i]))
				return false;
				
		} else {
		
			return false;
		}
	}
	
	return true;
}
