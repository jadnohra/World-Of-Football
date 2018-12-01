#include "WEAppPath.h"

#include "WEAssert.h"

namespace WE {


bool AppPath::mIsProcessed;
String AppPath::mPath;

bool AppPath::process() {

	String cmdLine(GetCommandLine());
	
	StringPart cmdFilePart(&cmdLine);
	String cmdExe;
	bool found;
	int index = 0;

	if ((index = cmdLine.findNext(L'"', index, found, cmdFilePart.startIndex, cmdFilePart.count, false)) < 0) 
		return false;
	index++;

	if ((index = cmdLine.findNext(L'"', index, found, cmdFilePart.startIndex, cmdFilePart.count, false)) < 0) 
		return false;
	index++;

	cmdFilePart.assignTo(cmdExe);
	mPath.assign(cmdExe.c_tstr());

	return true;
}

const TCHAR* AppPath::getPath() {

	if (mIsProcessed == false) {
		
		mIsProcessed = true;

		if (process() == false) {

			assrt(false);
			return NULL;
		}
	}

	return mPath.c_tstr();
}

}