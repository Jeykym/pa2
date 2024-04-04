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


	friend bool operator< (size_t pos, const Patch& rhs) {
		return pos < rhs.globalOffset;
	}
};


class CPatchStr {
public:
	CPatchStr(const char* str)
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

	CPatchStr(
		size_t size,
		Patch* arr,
		size_t len
	)	:	size(size),
			maxSize(size),
			array(arr),
			length(len) {}


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
			auto& patch = array[i];
			for (size_t j = 0; j < patch.length; j++) {
				res[pos++] = patch.string[patch.offset + j];
			}
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

		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
		}

		return *this;
	}


	CPatchStr subStr(size_t from, size_t len) {
		if (len == 0) return {""};

		if (from + len > length) throw std::out_of_range("Invalid indexes");

		auto startPatchI = findStartPatch(array, size, from);
		auto endPatchI = findEndPatch(startPatchI, from, len);

		std::cout << "start: " << array[startPatchI].string << " end: " << array[endPatchI].string << std::endl;

		// allocate new array for the substring
		size_t count = endPatchI - startPatchI + 1;
		std::cout << "count: " << count << std::endl;
		Patch* newArray = new Patch[count];

		// creating the substring of patches
		size_t newArrayI = 0;
		for (size_t i = startPatchI; i <= endPatchI; i++) {
			newArray[newArrayI++] = array[i];
		}

		newArray[0].offset = from - array[startPatchI].globalOffset;
		newArray[0].length = array[startPatchI].length - newArray[0].offset;

		newArray[count - 1].length = from + length - array[endPatchI].globalOffset;

		return {count, newArray, len};
	}


private:
	size_t length;
	size_t size;
	size_t maxSize;
	Patch* array;


	// returns the index of the patch in which is pos
	size_t findStartPatch(Patch* arr, size_t arraySize, size_t pos) const {
		size_t i = 0;
		while (i <= arraySize - 1) {
			if (pos >= arr[i].globalOffset && pos < arr[i].globalOffset + arr[i].length) {
				break;
			}
			i++;
		}

		return i;
	}


	// returns the index of the patch in which is pos + len
	size_t findEndPatch(size_t startPatchI, size_t from, size_t len) {
		// from - globalOffset to find the local offset
		// -1 because indexing from zero
		while (len >= array[startPatchI].length - (from - array[startPatchI].globalOffset - 1)) {
			startPatchI++;
		}

		return startPatchI;
	}
};

#ifndef __PROGTEST__
int main() {
	CPatchStr a("abc");
	CPatchStr b("def");
	CPatchStr c("ghi");
	a.apppend(b);
	a.apppend(c);
	std::cout << a.subStr(2, 3).toStr() << std::endl;
}


#endif /* __PROGTEST__ */
