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

/*
#ifndef includeLuaBind_H
#define includeLuaBind_H

// Include the lua headers (the extern "C" is a requirement because we're
// using C++ and lua has been compiled as C code)
extern "C" {
  //#include "lua/lua.h"
  //#include "lua/lauxlib.h"
  #include "lua.h"
  #include "lauxlib.h"
}

#pragma warning( push )
#pragma warning( disable : 4996 )

// This is the only header we need to include for LuaBind to work
#include "luabind/luabind.hpp"

#pragma warning( pop )

//#define LUABIND_AUTOLIB

#ifdef DEBUG
	#define LUABIND_LIBNAME_lua "lua-d.msvc8.lib"
	#define LUABIND_LIBNAME_luabind "luabind-d.msvc8.lib"
#else
	#define LUABIND_LIBNAME_lua "lua.msvc8.lib"
	#define LUABIND_LIBNAME_luabind "luabind.msvc8.lib"
#endif

#ifdef LUABIND_AUTOLIB
	#pragma comment(lib, LUABIND_LIBNAME_lua)
	#pragma comment(lib, LUABIND_LIBNAME_luabind)
#endif

#endif
	*/