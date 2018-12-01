
function getModul_NoChange() { return "-modulate 100,100,100"; }
function getModul_RedToGreen() { return "-modulate 100,100,170"; }
function getModul_RedToBlue() { return "-modulate 100,100,35"; }
function getModul_RedToYellow() { return "-modulate 100,100,130"; }

function getModul_RefToP11() { return getModul_NoChange(); }
function getModul_RefToP12() { return getModul_RedToBlue(); }
function getModul_RefToP21() { return getModul_RedToYellow(); }
function getModul_RefToP22() { return getModul_RedToGreen(); }

function processResources(_game) {

	local game = _game.impl;

	local processCOCs = false;
	local processActiveCOCs = false;
	local processArrows = false;
	local processPitch = false;
	local processBoards = false;
	
	/*
		those should be organized into 'effects' thats take structures as parameters
		and input output files should also be more organized in lists with more support
		from file system scanning functions
	*/
	
	if (processCOCs)
	{
	
		local workingDirPath = game.resolveScriptRelativePath("../mesh/coc/");
		local srcFilePath = "coc_red_circle.*";
		
		local ret = 0;
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "coc_p11.png"))
			return false;						
		
		/*	
		local waterMark = game.resolveScriptRelativePath("../tex/wmark_image.*");
		
		if (0 != game.executePlugin("convert.exe", srcFilePath + " " + "-modulate 100,100,120" + " " + dirPath + "temp.png"))
			return false;							
			
		if (0 != game.executePlugin("composite.exe", "-dissolve 20% -gravity SouthEast " + waterMark + " " + dirPath + "temp.png" + " " + dirPath + "coc_p11.png"))
			return false;						
		*/	
									
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "coc_p12.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "coc_p21.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath  + " " + getModul_RefToP22() + " " + "coc_p22.png"))
			return false;							
	}
	
	if (processActiveCOCs)
	{
	
		local workingDirPath = game.resolveScriptRelativePath("../mesh/coc/");
		local srcFilePath = "coc_red_circle_active.*";
		
		local ret = 0;
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "coc_active_p11.png"))
			return false;						
		
		/*	
		local waterMark = game.resolveScriptRelativePath("../tex/wmark_image.*");
		
		if (0 != game.executePlugin("convert.exe", srcFilePath + " " + "-modulate 100,100,120" + " " + dirPath + "temp.png"))
			return false;							
			
		if (0 != game.executePlugin("composite.exe", "-dissolve 20% -gravity SouthEast " + waterMark + " " + dirPath + "temp.png" + " " + dirPath + "coc_p11.png"))
			return false;						
		*/	
									
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "coc_active_p12.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "coc_active_p21.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath  + " " + getModul_RefToP22() + " " + "coc_active_p22.png"))
			return false;							
	}
	
	
	
	if (processArrows)
	{
		local workingDirPath = game.resolveScriptRelativePath("../mesh/arrow/");
		local srcFilePath = "coc_red_arrow.*";
		
		local ret = 0;
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP11() + " " + "arrow_p11.png"))
			return false;						
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP12() + " " + "arrow_p12.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP21() + " " + "arrow_p21.png"))
			return false;											
							
		if (0 != game.executePlugin("convert.exe", workingDirPath, srcFilePath + " " + getModul_RefToP22() + " " + "arrow_p22.png"))
			return false;							
	}
	
		
	if (processPitch)
	{
		local modulatePitch = false;
		
		local workingDirPath = game.resolveScriptRelativePath("../mesh/pitch/");
		local modulStr = "";
		
		if (modulatePitch) {
		
			modulStr = "-modulate 100,150,110";
			//modulStr = "-modulate 100,130,150";
			//modulStr = "-charcoal 5";
		} 
				
		local ret = 0;
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_grass_center.*" + " " + modulStr + " " + "grass_center.png"))
			return false;						
			
		if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_grass_corner_2.*" + " " + modulStr + " " + "grass_corner_2.png"))
			return false;							
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_grass_generic.*" + " " + modulStr + " " + "grass_generic.png"))
			return false;									
		
		if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_grass_patt1.*" + " " + modulStr + " " + "grass_patt1.png"))
			return false;											
			
		if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_grass_penalty.*" + " " + modulStr + " " + "grass_penalty.png"))
			return false;												
							
	} 
	
		
	if (processBoards)
	{
	
		local workingDirPath = game.resolveScriptRelativePath("../mesh/boards/");
		local fontDirPath = game.resolveScriptRelativePath("../font/");
			
		local genBoardCount = 0;
		
		local size = "1024x256";
		local backColor = "xc:lightblue";
		local pointSize = "210";
		local gravity = "center";
		local font = "FontCenter.com/Vademecum.ttf";
		local fillColor = "red";
		local stroke = "black";
		local strokeWidth = "1";
		local drawCmd = "\"text 0,0 'W.O.F Rulz'\"";
		
		local sizes = [size, size, size, size, size, size, size, size, size, size];
		local backColors = [backColor, "xc:black", "xc:yellow", "xc:lightgreen", "xc:orange", "xc:white", "xc:brown", "xc:pink", "xc:pink", "xc:white"];
		local pointSizes = [pointSize, "170", "220", pointSize, "180", "160", "200", pointSize, "200", pointSize];
		local gravities = [gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity, gravity];
		local fonts = [font, font, font, "FontCenter.com/cuomotype.ttf", font, "FontCenter.com/cuomotype.ttf", font, font, "FontCenter.com/electroharmonix.ttf", "FontCenter.com/electroharmonix.ttf"];
		local fillColors = [fillColor, "blue", "blue", "red", "black", fillColor, fillColor, "white", fillColor, "green"];
		local strokes = [stroke, stroke, stroke, "white", stroke, "blue", stroke, "green", stroke, "red"];
		local strokeWidths = [strokeWidth, strokeWidth, "2", "4", "0", strokeWidth, strokeWidth, "3", strokeWidth, "2"];
		local drawCmds = [drawCmd, "\"text 0,0 'ImageMagick'\"", "\"text -40,-30 'Constant'\"", "\"text 0,20 'ProGrEsS'\"", 
							"\"text 0,0 'Kimera'\"", "\"text 0,0 'Name-In-Game'\"", "\"text 0,-10 'Release'\"", 
							"\"text 0,0 'Game'\"", "\"text 0,0 'Old Skool'\"", "\"text 0,-10 'ArCade'\""];
		
		local j = 1;
		for (local i = 0; i < genBoardCount; i+=1) {
		
			local cmdStr = " -size " + sizes[i]
						+ " " + backColors[i]
						+ " -pointsize " + pointSizes[i]
						+ " -gravity " + gravities[i]
						+ " -font " + fontDirPath + fonts[i]
						+ " -fill " + fillColors[i]
						+ " -stroke " + strokes[i]
						+ " -strokewidth " + strokeWidths[i]
						+ " -draw " + drawCmds[i];
			
			local j1 = j / 10;
			local j2 = j % 10;
						
			if (0 != game.executePlugin("convert.exe", workingDirPath, cmdStr + " " + "board_" + j1 + j2 + ".png" ))
				return false;
				
			j += 1;	
		}
		
		for (local i = genBoardCount; i < 10; i+=1) {
		
			local j1 = j / 10;
			local j2 = j % 10;
		
			if (0 != game.executePlugin("convert.exe", workingDirPath, "ref_board_" + j1 + j2 + ".png" + " " + "board_" + j1 + j2 + ".png" ))
				return false;
		
			j += 1;
		}
	
	}
	
	return true;
}