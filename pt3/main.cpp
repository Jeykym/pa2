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

	friend bool operator< (const Patch& lhs, size_t pos) {
		return lhs.globalOffset < pos;
	}
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


	CPatchStr& apppend(const CPatchStr& src) {
		// first resize the array if necessary
		if (size == maxSize) {
			maxSize *= 2;
			Patch* newArray = new Patch[maxSize];

			// copy over old Patches
			for (size_t i = 0; i < size; i++) {
				newArray[i] = array[i];
			}

			delete [] array;
			array = newArray;
		}

		length += src.length;
		// if inserting self
		auto originalSrcSize = src.size;

		// copy Patches from src to the end of the array
		for (size_t i = 0; i < originalSrcSize; i++) {
			array[size++] = src.array[i];
		}

		return *this;
	}


private:
	size_t length;
	size_t size;
	size_t maxSize;
	Patch* array;


	// returns the index of the patch at pos
	size_t find(Patch* arr, size_t arraySize, size_t pos) const {
		size_t low = 0;
		size_t high = arraySize;

		while (low < high) {
			size_t mid = low + (high - low) / 2;

			if (arr[mid] < pos) {
				low = mid + 1;
			} else {
				high = mid;
			}
		}

		return high - 1;
	}
};

#ifndef __PROGTEST__
int main() {
	CPatchStr a("abc");
	CPatchStr b("def");
	a.apppend(b);
	auto str = a.toStr();
	std::cout << a.toStr() << std::endl;
	delete str;
}


#endif /* __PROGTEST__ */
