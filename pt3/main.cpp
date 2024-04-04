#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#endif /* __PROGTEST__ */

struct Patch {
	size_t globalOffset;
	size_t offset;
	size_t length;
	const char* ptr;
};


class CPatchStr {
public:
	explicit CPatchStr(const char* str)
		:	size(0),
			maxSize(10),
			array(new Patch[size]) {
		// first occurrence of the string
		array[0] = {
			0,
			0,
			strlen(str),
			str
		};
	}


	~CPatchStr() {
		delete [] array;
	}


private:
	size_t size;
	size_t maxSize;
	Patch* array;
};

#ifndef __PROGTEST__
int main() {
	CPatchStr a("test");
}
#endif /* __PROGTEST__ */
