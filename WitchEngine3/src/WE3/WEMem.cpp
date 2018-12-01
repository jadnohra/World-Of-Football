#include "WEMem.h"

#include "WEAssert.h"

namespace WE {

void* _memCheckAddress = 0;

void _memCheckSetAddress(void* address) {

	_memCheckAddress = address;
}

void _memCheck(void* address) {

	if (_memCheckAddress == address) {

		debugBreak();
	}
}

}