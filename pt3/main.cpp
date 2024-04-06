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


	CPatchStr(const CPatchStr& other)
			:	size(other.size),
				maxSize(other.maxSize) {
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

private:
	void appendSelf() {
		// just copy the array to the end
		for (size_t i = 0; i < size; i++) {
			array[size + i] = array[i];
		}
		size *= 2;
		updateOffsets();
		printOffsets();
	}


	void appendOther(const CPatchStr& src) {
		// copy the other patchstr' array
		for (size_t i = 0; i < src.size; i++) {
			array[size] = src.array[i];
			size++;
		}

		updateOffsets();
		printOffsets();
	}

	void updateOffsets() {
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

	size_t length() const {
		size_t total = 0;

		for (size_t i = 0; i < size; i++) {
			total += array[i].length;
		}

		return total;
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
	CPatchStr a("abc");
	a.append(a);
	assert(stringMatch(a.toStr(), "abcabc"));
	CPatchStr b("def");
	a.append(b);
	assert(stringMatch(a.toStr(), "abcabcdef"));

	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
