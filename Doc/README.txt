-- SUMMARY --

Another Ubaka special, this time with VERY basic ball handling fctionality

* u can walk with the ball
* u can use the 'tackle/shoot' button
* u can use the 'switch player/pass' button

all very 'shakey'

-- INSATALLATION --

* copy contents of 'repositoryUpdates' to same folders in repository (overwrite)
* [OPTIONAL] for fast Gfx Cards, copy contents of 'repositoryUpdatesOptional' to same folders in repository (overwrite)

-- TWEAKS / BUTTONS --

* button configuration: http://www.kimerastudios.com/wof_wiki/index.php/Button_config
* F2: enable debug info
* F3->F6: select debug info
* F7: footballer/ball states (includes ball control state)
* F8: wireframe
* F9: stop random AI -> all players idle

* C: switch between cameras (cam1/cam2/cam3) 
* V: detach CURRENTLY SELECTED CAM 
* B: attach CURRENTLY SELECTED CAM to ball
* M: attach CURRENTLY SELECTED CAM to controlled footballer, Press several times to cycle footballers

* H: attach the ball to camera
* G: drop ball from scene start position
* J: drop ball
* K: drop ball with some rotation
* ENTER: hold/release to shoot ball
* ENTER: hit key to drop ball from where camera is (works a bit weird if camera attached to ball)
* L: repeat last drop/shot (even if camera moved to other place) so u can drop, go somewhere else, and press L to watch the drop from another place


* u can tweak some things from deploy/bin/config.xml

* in _gameScene/WOF_matchTest_UbakaDribble.xml

	u can tweak this:
	
	<game dribbleVelHorizRatio="1.3" dribbleVelVert="1" 
		shotVelHoriz="40" shotVelVert="10" 
		passVelHoriz="12" passVelVert="2" 
		tackleVelHoriz="6" tackleVelVert="1" 
		COCRadius="1" sharedControlDelay="2" enableAI="true" 
		switchControllers="true">
	</game>	

	* enableAI="true" (see key F9)
	* switchControllers="true" control which teams has which controller (keyb / dualshock)
	* sharedControlDelay="2" (2 seconds) if ball has one owner, and another comes close, 
										the new owner can only perform a tackle on the ball, until 
										2 seconds elapse then he can control the ball fully 
										along with the previous owner.				
								
	* u can also tweak camera parameters		


-- UPDATES --

* bin_1: added player controller selection in scene file, need to update _gameScene/WOF_matchTest_UbakaDribble.xml
* bin3: added invisible picth walls, need to update _gameScene/WOF_matchTest_UbakaDribble.xml
* bin4: added AUDIO, need to update _gameScene/WOF_matchTest_UbakaDribble.xml and audio assets and install OpenAL
* bin5: added 'ogg' streaming audio support (for ambient sound per example). need to update assets/WOF.xml, _gameScene/WOF_matchTest_UbakaDribble.xml, assets/audio and assets/plugins 