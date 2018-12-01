#ifndef _WEFile_h
#define _WEFile_h

#include "WEString.h"

namespace WE {

	class File {
	protected:

		FILE *file;

	public:

		File();
		File(const char* filepath, bool readOnly);
		~File();

		bool openFile(const char* filepath, bool readOnly);
		void close();

		void write(void* data, size_t bytes);
		size_t read(void* data, size_t bytes);

		void flush(); 
		void rewind(); //might assert if not supported
	};
}

#endif