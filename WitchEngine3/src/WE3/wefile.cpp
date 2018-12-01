#include "WEFile.h"

#include "tchar.h"
#include "strsafe.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "time.h"
#include "stdio.h"
#include "WEAssert.h"

namespace WE {


File::File() {

	file = NULL;
}

File::File(const char* filepath, bool readOnly) {

	file = NULL;

	openFile(filepath, readOnly);
}

File::~File() {

	close();
}

bool File::openFile(const char* filepath, bool readOnly) {

	close();

	if (filepath) {

		file = fopen (filepath, readOnly ? "r+" : "w+");

		if (file == NULL) {

			return false;
		}

		return true;
	} 

	return false;
}

void File::close() {

	if (file) {
		fclose(file);
		file = NULL;
	}
}

void File::flush() {

	fflush(file);
}

void File::write(void* data, size_t bytes) {

	size_t doneCount = fwrite(data, bytes, 1, file);

	assrt(doneCount == 1);
}

size_t File::read(void* data, size_t bytes) {

	return fread(data, 1, bytes, file);

}


void File::rewind() {

	::rewind(file);
}

}