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
	size_t globalOffset;
	size_t offset;
	size_t length;
	const char* string;
};


class CPatchStr {
public:
	explicit CPatchStr(const char* str)
		:	size(1),
			maxSize(10),
			array(new Patch[maxSize]) {
		// the patch is only the string
		array[0] = {
			0,
			0,
			strlen(str),
			str
		};
//		length = array[0].length;
		length = array[0].length;
	}


	~CPatchStr() {
		delete [] array;
	}


	char* toStr() const {
		// one extra char for '\0'
		char* res = new char[length + 1];

		// current patch
		size_t pos = 0;
		// for each patch
		for (size_t i = 0; i < size; i++) {
			// for each character in the patch
			for (size_t j = 0; j < array[i].length; j++) {
				res[pos + j] = array[i].string[j];
			}

			// next patch
			pos += array[i].length;
		}

		res[length] = '\0';

		return res;
	}


private:
	size_t length;
	size_t size;
	size_t maxSize;
	Patch* array;


	bool full() const {
		return size == maxSize;
	}


	void resize() {
		// creating a new bigger array
		maxSize *= 2;
		auto* newArray = new Patch[maxSize];

		// copying the elements to the bigger array
		for (size_t i = 0; i < size; i++) {
			newArray[i] = array[i];
		}

		delete [] array;

		array = newArray;
	}


};

#ifndef __PROGTEST__
int main() {
	CPatchStr a("abc");
	auto string = a.toStr();
	std::cout << a.toStr() << std::endl;
	delete string;
}
#endif /* __PROGTEST__ */
