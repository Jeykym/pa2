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


//class CPatchStr
//{
//public:
//	CPatchStr ();
//	CPatchStr ( const char * str );
//	// copy constructor
//	// destructor
//	// operator =
//	CPatchStr   subStr    ( size_t            from,
//	size_t            len ) const;
//	CPatchStr & append    ( const CPatchStr & src );
//
//	CPatchStr & insert    ( size_t            pos,
//	const CPatchStr & src );
//	CPatchStr & remove    ( size_t            from,
//	size_t            len );
//	char      * toStr     () const;
//private:
//	// todo
//};


struct Patch {
	size_t offset;
	size_t length;
	const char* str;
	size_t globalOffset;
};


class CPatchStr {
public:
	CPatchStr(const char* str)
		:	size(1),
			maxSize(10),
			array(new Patch[10]) {
		array[0] = {
			0,
			std::strlen(str),
			str,
			0
		};
	}


	CPatchStr(const CPatchStr& other) {
		if (this == &other) return;

		array = new Patch[other.size];
		for (size_t i = 0; i < other.size; i++) {
			array[i] = other.array[i];
		}

		size = other.size;
		maxSize = other.maxSize;
	}


	~CPatchStr() {
		delete [] array;
	}


	CPatchStr& operator=(const CPatchStr& other) {
		if (this == &other) return *this;

		delete [] array;

		// copying patches over
		array = new Patch[other.maxSize];
		for (size_t i = 0; i < other.size; i++) {
			array[i] = other.array[i];
		}

		size = other.size;
		maxSize = other.maxSize;

		return *this;
	}


	char* toStr() const {
		// one extra space for '\0'
		auto len = length();
		char* str = new char[len + 1];

		size_t strI = 0;
		for (size_t i = 0; i < size; i++) {
			for (size_t j = 0; j < array[i].length; j++) {
				str[strI] = array[i].str[j];
				strI++;
			}
		}

		// end the string
		str[len] = '\0';

		return str;
	}

private:
	size_t length() const {
		size_t total = 0;

		for (size_t i = 0; i < size; i++) {
			total += array[i].length;
		}

		return total;
	}

	size_t size;
	size_t maxSize;
	Patch* array;
};


#ifndef __PROGTEST__
void print(const CPatchStr& patchStr) {
	auto str = patchStr.toStr();
	std::cout << str << std::endl;
	delete [] str;
}


bool stringMatch(
	char* str,
	const char* expected
) {
	auto res = std::strcmp(str, expected) == 0;
	delete [] str;
	return res;
}



int main() {
	char tmpStr[100];

	CPatchStr a("Hello, world!");
	auto b = a;
	CPatchStr c(b);

	assert(stringMatch(a.toStr(), "Hello, world!"));
	assert(stringMatch(b.toStr(), "Hello, world!"));
	assert(stringMatch(c.toStr(), "Hello, world!"));

	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
