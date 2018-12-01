-- SUMMARY --

Basic Mechanics working, I am though somehow skeptical they are ready for gameplay though :(
they feel for me not so easy to use, maybe its becuase of lack of AI, maybe not ...
needs to be tested by others .....

footballer
1* running
2* running + modifier
3* jumping

footballer-footballer
4* tackling 
5* falling

footballer-ball
6* running with ball
7* stabilizing (with body) ball while idle
8* sweet spotting ball while idle (missing anim)
9* running with ball
10* stabilizing (with body) ball while running
11* turning with ball even 180 (no anim)
12* modifier + ball
13* shooting ball + power bar + directional
14* tackling footballer with ball + directional + control tackle mode (hold)
15* heading ball + directional
16* running -> heading ball + directional

Attributes
Attributes are set in the team xml 

example:

<footballer1_1 type="scriptedCharacter" scriptClass="Footballer" source="footballer" coll="true" collMaterial="human/skin" gameType="footballer" shadowing="true" >
	<params kit="manchester_devils/01" skin="cornrows_black" boot="boot_black_black" /> 
	<game team="0" number="20" />
	<attributes 
		PhysicalStaminaFitness="90" 
		PhysicalAgilityReflex="90"
		PhysicalPowerTackle="90"
		PhysicalAgilityBalance="90"
		PhysicalPowerShot="90"
		SkillBallTackleClean="90"
		SkillBallProtection="90"
		SkillBallControlFeet="90"
		SkillBallControlBody="90"
		SkillShootAccuracy="90"
		SkillBallControlConsistancy="90"
	/>
</footballer1_1>

* PhysicalStaminaFitness 
	fitness in minutes based on Menace's Design, the footballer is 100% fit until this expires, 
	when it does it slowly goes down to zero affecting almost all other attributes

* PhysicalAgilityReflex	
	for any place where reflex speed is needed, 
	currently used for when footballer looses ball to another footballer while idle

* PhysicalPowerTackle 
	the power of tackle, compared to tackled footballers PhysicalAgilityBalance (+random) to see 
	if he falls or not

* PhysicalPowerShot
	the maximum shot power the footballer has (max is 100 = 85 m/s)
	
* SkillBallTackleClean
	the skill to take the ball from tackled owner
	
* SkillBallProtection
	when 2 footballers give simultaneous commands to ball, the one with higher
	SkillBallProtection will win (+random)	

* SkillBallControlFeet
	skill to control ball with feet: if ball comign at high speed	
	
* SkillBallControlBody
	skill to control ball with body: chesting down
	
* SkillShootAccuracy
	how accurately the footballer can make the perfect shot
		(the perfect shot for each button is set in a script file)

* SkillBallControlConsistancy
	how consistant (random) the SkillShootAccuracy is 
	this is based on gaussian distribution, http://en.wikipedia.org/wiki/Normal_distribution

	
Tweaks:

* Shots

	in assets\script\footballer\FootballerSettings.nut
	
	Shots = ...
	and 
	HeadShots = ...
	
	example:
	FootballerBallShot(true, ::FootballerBallShot_Foot, 54.0, 5.0, 0.0, 0.1, true, false),
	
	54.0 shot speed
	5.0 shot veritcal angle
	0.0 the amount of horizontal ball rotation
	0.1 the amount of veritcal ball rotation
	true do tackle if not ball owner?
	false enable aftertouch?
	

* Running with ball
		
	in assets\script\footballer\FootballerSettings_Run.nut
	
	Shots = ...
	
* Running and modifier Speeds	

	in assets\script\footballer\FootballerSettings_Run.nut

	SpeedTable = 
	
	P.S in the default stamina model, where there is no sprint
	the sprint values are ignored
	

* Stamina model

	the default model is menace's stamina model:
		no sprint
		no recharge
		after minutes expire starts going down except if idle
		
	it is possible to set a different model:
		with sprint
		rechargeable
		recharges on idle, stays same on normal run, decreases on sprint
		
		and the fitness defines the speed of stamina gain and loss
		
	To switch between modes 
	
	assets\script\footballer\FootballerSettings.nut
	
	static StaminaModel = 1; //Menace Model
	static StaminaModel = 0; //Alternate Model
		
	
-- Console --	
* ~ to open console
* setAutoSwitch(true/false) (controls automatic switching, if off, right shoulder button is used to switch)

	
-- TWEAKS / BUTTONS --

* button configuration: http://www.kimerastudios.com/wof_wiki/index.php/Button_config
* F2: enable debug info
* F3->F6: select debug info
* F8: wireframe
* F9: stop random AI -> all players idle
* F10 enable anti aliasing and nicer texture mapping

* Numeric-Keypad + Mouse : Camera

* C: switch between cameras (cam1/cam2/cam3) 
* V: detach CURRENTLY SELECTED CAM 
* B: attach CURRENTLY SELECTED CAM to ball
* M: attach CURRENTLY SELECTED CAM to controlled footballer, Press several times to cycle footballers

* H: attach the ball to camera
* G: drop ball from scene start position
* Shift-G: drop bal next to selected and attached (M) footballer
* J: drop ball
* K: drop ball with some rotation
* N: hold/release to shoot ball
* N: hit key to drop ball from where camera is (works a bit weird if camera attached to ball)
* L: repeat last drop/shot (even if camera moved to other place) so u can drop, go somewhere else, and press L to watch the drop from another place
* S: step mode, simulation is frozen but camera can still be moved use SPACE to step simulation
	
-- Known Bugs --
* screen tears sometime, will be fixed with renderer revamp.
	