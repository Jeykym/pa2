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
			array(new Patch[10]),
			length(std::strlen(str)) {
		array[0] = {
			0,
			std::strlen(str),
			str,
			0
		};
	}


	CPatchStr(const CPatchStr& other)
			:	size(other.size),
				maxSize(other.maxSize),
				length(other.length),
				array(new Patch[other.size]){
		if (this == &other) return;

		for (size_t i = 0; i < other.size; i++) {
			array[i] = other.array[i];
		}

		size = other.size;
		maxSize = other.maxSize;
	}


	// substring constructor
	CPatchStr(
		size_t from,
		size_t len,
		const CPatchStr& other
	) {
		auto startI = find(from, other.array, other.size);
		auto endI = find(from + len - 1, other.array, other.size);

		length = len + 1;
		size = endI - startI + 1;
		maxSize = size * 2;
		array = new Patch[maxSize];

		for (size_t i = 0; i < size; i++) {
			array[i] = other.array[startI + i];
		}


		auto& otherStart = other.array[startI];
		auto& otherEnd = other.array[endI];
		auto& thisStart = array[0];
		auto& thisEnd = array[size - 1];
		array[0].offset = from - other.array[startI].globalOffset;
		array[0].length -= array[0].offset;
		array[size - 1].length = from + len - other.array[endI].globalOffset;
		int i = 0;

		updateGlobalOffsets();

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
		char* str = new char[length + 1];

		size_t strI = 0;
		for (size_t i = 0; i < size; i++) {
			for (size_t j = 0; j < array[i].length; j++) {
				str[strI] = array[i].str[array[i].offset + j];
				strI++;
			}
		}

		// end the string
		str[length] = '\0';

		return str;
	}


	CPatchStr& append(const CPatchStr& src) {
		if (size + src.size > maxSize) realloc(size + src.size);

		// appending self
		if (this == &src) {
			appendSelf();
		} else {
			appendOther(src);
		}

		return *this;
	}


	CPatchStr subStr (
		size_t from,
		size_t len
	) const {
		if (from + len > length) throw std::out_of_range("Out of range");

		if (len == 0) {
			return {""};
		}

		return {from, len, *this};
	}


private:
	void appendSelf() {
		// just copy the array to the end
		for (size_t i = 0; i < size; i++) {
			array[size + i] = array[i];
		}

		size *= 2;
		length *= 2;

		updateGlobalOffsets();
		printOffsets();
	}


	void appendOther(const CPatchStr& src) {
		// copy the other patchstr' array
		for (size_t i = 0; i < src.size; i++) {
			array[size] = src.array[i];
			size++;
		}

		length += src.length;

		updateGlobalOffsets();
		printOffsets();
	}


	void updateGlobalOffsets() {
		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
		}
	}


	void printOffsets() const {
		for (size_t i = 0; i < size; i++) {
			std::cout << array[i].globalOffset << std::endl;
		}
	}

	void realloc(size_t newSize) {
		maxSize = newSize * 2;
		auto* newArray = new Patch[maxSize];

		for (size_t i = 0; i < size; i++) {
			newArray[i] = array[i];
		}

		delete [] array;
		array = newArray;
	}


	// returns the index of a patch containing the pos
	static size_t find(size_t pos, Patch* arr, size_t arrSize) {
		size_t left = 0;
		size_t right = arrSize - 1;

		while (left <= right) {
			size_t mid = left + (right - left) / 2;

			if (pos < arr[mid].globalOffset) {
				right = mid - 1;
			} else if (pos >= arr[mid].globalOffset + arr[mid].length) {
				left = mid + 1;
			} else {
				// Position found within this patch
				return mid;
			}
		}

		throw std::out_of_range("Position not found within any patch");
	}


	size_t size;
	size_t maxSize;
	Patch* array;
	size_t length;
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

bool stringMatchSub(
	char* str,
	const char* expected,
	size_t start,
	size_t len
) {
	expected += start;

	for (size_t i = 0; i < len; i++) {
		if (expected[i] != str[i]) return false;
	}

	delete [] str;

	return true;
}



int main() {
	CPatchStr a("abc");
	CPatchStr b("def");
	CPatchStr c("ghi");
	a.append(b);
	a.append(c);

	assert(stringMatch(a.toStr(), "abcdefghi"));

	for (size_t start = 0; start < 9; start++) {
		for (size_t len = 1; len < 9 - start + 1; len++) {
			auto substr = a.subStr(start, len);
			auto* str = substr.toStr();
			assert(stringMatchSub(str, "abcdefghi", start, len));
		}
	}

	for (size_t start = 0; start < 9; start++) {
		assert(stringMatch(a.subStr(start, 0).toStr(), ""));
	}

	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
