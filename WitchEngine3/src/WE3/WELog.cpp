#include "WELog.h"

#include "WEString.h"

#include "tchar.h"
#include "strsafe.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "time.h"
#include "stdio.h"

//#include "win32/WEWin32DebugStream.h"

namespace WE {


#define LogType_Time_Level 0
#define LogType_Time 1
#define LogType_Level 2
#define LogType_None 3

//static w_windbgstream dbgstream;

Log::Log() {

	level = 0;
	file = NULL;
	options = 0;
}

Log::~Log() {

	destroy();
}


void Log::init(LogType mask, const TCHAR* name, const TCHAR* filepath, LogType loglevel, LogType optionbits)  {

	if (mask & LOG_MASK_LOGLEVEL) {
		level = loglevel;
	}
	if (mask & LOG_MASK_OPTIONBITS) {
		options = optionbits;
	}
	/*
	if (mask & LOG_MASK_NAME) {
		this->name.assign(name);
	} else {
		this->name.destroy();
	}
	*/

	if (!(options & LOG_LOG_TOFILE_BIT)) {
		if (file != NULL) {
			fclose (file);
			file = NULL;
		}
	} else {
		if (mask & LOG_MASK_FILEPATH) {
			if (file != NULL) {
				fclose (file);
				file = NULL;
			}
			if ((filepath != NULL) && (filepath[0])) {
				if (optionbits & LOG_FILE_APPEND_BIT) {
					file = _tfopen (filepath, L"a");
				} else {
					file = _tfopen (filepath, L"w");
				}
			}
		}
	}

	if (optionbits & LOG_LOG_LEVEL_BIT) {
		if (optionbits & LOG_LOG_TIME_BIT) {
			type = LogType_Time_Level;
		} else {
			type = LogType_Level;
		}
	} else {
		if (optionbits & LOG_LOG_TIME_BIT) {
			type = LogType_Time;
		} else {
			type = LogType_None;
		}
	}
	
}

bool Log::hasFile() const {

	return (file ? true : false);
}

void Log::destroy() {

	init (LOG_MASK_ALL, NULL, NULL, 0, 0);
}


void Log::flush() const {

	if (file != NULL) {
		fflush (file);
	}
}


TCHAR* Log::toString(LogType logLevel) const {


	switch (level) {
	  case LOG_ERROR:
		  return (L"Error");
	  case LOG_FORCE:
		  return (L"Force");
	  case LOG_WARNING:
		  return (L"Warning");
	  case LOG_INFO:
		  return (L"Info");
	  case LOG_DEBUG:
		  return (L"Debug");
	}

	return (L"Custom");
}

inline TCHAR* fixed_ctime (const time_t * t)
{
	TCHAR* ptr = _tctime(t);

	ptr[_tcslen(ptr) - 1] = L'0';
	return ptr;
}


#define Log_logToStream(stream, currtim, level, endline, newentry) {\
	if (stream != NULL) {\
		if (newentry) {\
			switch (type) {\
				case LogType_Time_Level:\
					_ftprintf (stream, L"[%s - %s] : ", currtime, level);\
					break;\
				case LogType_Time:\
					_ftprintf (stream, L"[%s] : ", currtime);\
					break;\
				case LogType_Level:\
					_ftprintf (stream, L"[%s] : ", level);\
					break;\
			}\
		}\
		va_start (ap, format);\
		_vftprintf (stream, format, ap);\
		va_end (ap);\
		if (endline)\
			_ftprintf (stream, L"\n");\
		if (options & LOG_LOG_AUTOFLUSH_BIT)\
			fflush (stream);\
	}\
}

#define Log_logToStream2(stream, currtim, level) {\
	if (stream != NULL) {\
		{\
			switch (type) {\
				case LogType_Time_Level:\
					_ftprintf (stream, L"[%s - %s] : ", currtime, level);\
					break;\
				case LogType_Time:\
					_ftprintf (stream, L"[%s] : ", currtime);\
					break;\
				case LogType_Level:\
					_ftprintf (stream, L"[%s] : ", level);\
					break;\
			}\
		}\
		va_start (ap, format);\
		_vftprintf (stream, format, ap);\
		va_end (ap);\
		_ftprintf (stream, L"\n");\
		if (options & LOG_LOG_AUTOFLUSH_BIT)\
			fflush (stream);\
	}\
}


void Log::log(LogType level, bool endline, bool newentry, const TCHAR *format, ...) const  {

	time_t currtime;
	const TCHAR* timestr;
	const TCHAR* levelstr;
	va_list ap;


	if ((level <= level) && (options & LOG_ENABLED_BIT)) {

		if (newentry) {
			if (options & LOG_LOG_TIME_BIT) {
				time (&currtime);
				timestr = fixed_ctime(&currtime);
			}
			if (options & LOG_LOG_LEVEL_BIT) {
				levelstr = toString(level);
			}
		}

		if (options & LOG_LOG_TOCONSOLE_BIT) {
			if (level < LOG_WARNING) {
				Log_logToStream (stderr, timestr, levelstr, endline, newentry);
			} else {
				Log_logToStream (stdout, timestr, levelstr, endline, newentry);
			}
		}
		if (options & LOG_LOG_TOFILE_BIT) {
			Log_logToStream (file, timestr, levelstr, endline, newentry);
		}
	}
}

void Log::log(const TCHAR *format, ...) const  {

	time_t currtime;
	const TCHAR* timestr;
	const TCHAR* levelstr;
	va_list ap;


	if (options & LOG_ENABLED_BIT) {

		{
			if (options & LOG_LOG_TIME_BIT) {
				time (&currtime);
				timestr = fixed_ctime(&currtime);
			}
			if (options & LOG_LOG_LEVEL_BIT) {
				levelstr = toString(level);
			}
		}

		if (options & LOG_LOG_TOCONSOLE_BIT) {
//#ifdef _DEBUG
//		Log_logToStream2 (dbgstream, timestr, levelstr);
//#elif
		if (level < LOG_WARNING) {
			Log_logToStream2 (stderr, timestr, levelstr);
		} else {
			Log_logToStream2 (stdout, timestr, levelstr);
		}
//#endif
		}
		if (options & LOG_LOG_TOFILE_BIT) {
			Log_logToStream2 (file, timestr, levelstr);
		}
	}

}


}
