#ifndef includeScriptingAPI_H
#define includeScriptingAPI_H

#include "sqplus/sqplus.h"

#ifdef _DEBUG
	#define SQUIREL_LIBNAME_sqdbglib "sqdbglibDU.lib"
	#define SQUIREL_LIBNAME_sqplus "sqplusDU.lib"
	#define SQUIREL_LIBNAME_sqstdlib "sqstdlibDU.lib"
	#define SQUIREL_LIBNAME_squirrel "squirrelDU.lib"
#else
	#define SQUIREL_LIBNAME_sqdbglib "sqdbglibU.lib"
	#define SQUIREL_LIBNAME_sqplus "sqplusU.lib"
	#define SQUIREL_LIBNAME_sqstdlib "sqstdlibU.lib"
	#define SQUIREL_LIBNAME_squirrel "squirrelU.lib"
#endif

//#pragma comment(lib, SQUIREL_LIBNAME_sqdbglib)
#pragma comment(lib, SQUIREL_LIBNAME_sqplus)
#pragma comment(lib, SQUIREL_LIBNAME_sqstdlib)
#pragma comment(lib, SQUIREL_LIBNAME_squirrel)

#endif