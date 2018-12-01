#ifndef _WELog_h
#define _WELog_h

#include "stdio.h"
#include "tchar.h"

namespace WE {

	
	typedef unsigned int LogType;
	# define LOG_ALL_BIT 0xffff
	# define LOG_ENABLED_BIT 0x0001
	# define LOG_LOG_TOCONSOLE_BIT 0x0004
	# define LOG_LOG_TIME_BIT  0x0008
	# define LOG_LOG_AUTOFLUSH_BIT 0x0010
	# define LOG_LOG_LEVEL_BIT 0x0100
	# define LOG_FILE_APPEND_BIT 0x0040
	# define LOG_LOG_TOFILE_BIT 0x0080
	# define LOG_MASK_ALL 0xffff
	# define LOG_MASK_NAME 0x0001
	# define LOG_MASK_FILEPATH 0x0004
	# define LOG_MASK_LOGLEVEL 0x0008
	# define LOG_MASK_OPTIONBITS 0x0010
	# define LOG_DEBUG 15
	# define LOG_INFO 10
	# define LOG_WARNING 5
	# define LOG_WARN 5
	# define LOG_ERROR 0
	# define LOG_FORCE -1


	class Log {
	public:

		LogType level;

	protected:
	
		FILE *file;
		LogType options;
		
		int type;
	
	public:

		Log();
		~Log();

		void init(LogType mask, const TCHAR* name, const TCHAR* filepath, LogType loglevel, LogType optionbits);
		void destroy();

		bool hasFile() const;
		TCHAR* toString(LogType logLevel) const;
		
		void log(LogType loglevel, bool endline, bool newentry, const TCHAR* format, ...) const;
		void log(const TCHAR* format, ...) const;
		void flush() const;
	};
	
}

#endif