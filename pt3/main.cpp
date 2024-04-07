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
#include <string>


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


struct String {
	char* chars;

	String(const char* str) {
		chars = new char[strlen(str) + 1];
		std::strcpy(chars, str);
	}


	~String() {
		delete [] chars;
	}
};


struct Patch {
	size_t offset;
	size_t length;
	std::shared_ptr<String> str;
	size_t globalOffset;
};


class CPatchStr {
public:
	CPatchStr(const char* str)
	: size(1),
	  maxSize(10),
	  length(strlen(str)),
	  array(new Patch[maxSize]) {
		array[0] = {
			0,
			length,
			std::make_shared<String>(str),
			0
		};
	}


	CPatchStr(const CPatchStr& src)
		:	size(src.size),
			maxSize(src.maxSize),
			length(src.length) {
		array = new Patch[maxSize];
		for (size_t i = 0; i < size; i++) {
			array[i] = src.array[i];
		}
	}


	CPatchStr(
		size_t from,
		size_t len,
		const CPatchStr& src
	) {
		size_t startI = find(from, src.array, src.size);
		size_t endI = find(from + len - 1, src.array, src.size);

		length = len;
		size = endI - startI + 1;
		maxSize = size * 2;
		array = new Patch[maxSize];

		auto& start = src.array[startI];
		auto& end = src.array[endI];

		for (size_t i = 0; i < size; i++) {
			array[i] = src.array[startI + i];
		}

		array[0].offset = from - src.array[startI].globalOffset;
		array[0].length = src.array[startI].length - array[0].offset;

		if (startI == endI) {
			array[0].length = len;
		} else {
			array[size - 1].length = from + len - array[endI].globalOffset;
		}

		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
		}
	}


	~CPatchStr() {
		delete [] array;
	}


	CPatchStr& append(const CPatchStr& src) {
		if (size + src.size >= maxSize) realloc(size + src.size);

		if (this == &src) {
			appendSelf();
		} else {
			appendOther(src);
		}

		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
		}

		return *this;
	}


	char* toStr() {
		char* str = new char[length + 1];

		size_t strI = 0;
		for (size_t i = 0; i < size; i++) {
			for (size_t j = 0; j < array[i].length; j++) {
				auto patchStr = array[i].str.get();
				str[strI++] = patchStr->chars[array[i].offset + j];
			}
		}

		str[length] = '\0';

		return str;
	}


	CPatchStr subStr(
		size_t from,
		size_t len
	) const {
		if (from + len >= length) throw std::out_of_range("Out of range");

		if (len == 0) return {""};

		return {from, len, *this};
	}


	size_t Length() {
		return length;
	}


private:
	void appendSelf() {
		for (size_t i = 0; i < size; i++) {
			array[size + i] = array[i];
		}

		size *= 2;
		length *= 2;
	}


	void appendOther(const CPatchStr& src) {
		length += src.length;

		for (size_t i = 0; i < src.size; i++) {
			array[size + i] = src.array[i];
		}

		size += src.size;
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
		auto& p0 = arr[0];
		auto& p1 = arr[1];
		auto& p2 = arr[2];
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
bool stringMatch ( char       * str,
const char * expected )
{
	bool res = std::strcmp ( str, expected ) == 0;
	delete [] str;
	return res;
}



void appendTest() {
	/* testing appending
 */
	CPatchStr x ( "test" );
	std::string s = "test";
	x . append ( "ing" );
	s . append ( "ing" );
	x . append ( " this" );
	s . append ( " this" );
	x . append ( " append function" );
	s . append ( " append function" );
	x . append ( ", hopefully" );
	s . append ( ", hopefully" );
	x . append ( " its going okay" );
	s . append ( " its going okay" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );

	CPatchStr x2 ( "another" );
	std::string s2 ( "another" );
	x2 . append ( " string" );
	s2 . append ( " string" );
	x2 . append ( x );
	s2 . append ( s );
	x2 . append ( x2 );
	s2 . append ( s2 );
	assert ( stringMatch ( x2 . toStr (), s2 . c_str () ) );
	x . append ( "a" );
	s . append ( "a" );
	x . append ( "b" );
	s . append ( "b" );
	x . append ( "c" );
	s . append ( "c" );
	x . append ( "d" );
	s . append ( "d" );
	x . append ( "e" );
	s . append ( "e" );
	x . append ( "f" );
	s . append ( "f" );
	x . append ( "g" );
	s . append ( "g" );
	x . append ( "h" );
	s . append ( "h" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . append ( "" );
	s . append ( "" );
	x . append ( "finaltest" );
	s . append ( "finaltest" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );

	/* 147 allocs
	 */
}


void subStringTest() {
	/* testing (SubStr), chaining of commands
	*/

	CPatchStr x1 ( "substrtest" );
	std::string s1 ( "substrtest" );
	CPatchStr x2 ( x1 . subStr ( 3, 3 ) );
	std::string    s2 ( s1 . substr ( 3, 3 ) );
	assert ( stringMatch ( x2 . toStr (), s2 . c_str () ) );
	x2 . append ( "moretesting" );
	s2 . append ( "moretesting" );
	CPatchStr x3 ( x2 . subStr ( 5, 5 ) );
	std::string    s3 ( s2 . substr ( 5, 5 ) );
	assert ( stringMatch ( x3 . toStr (), s3 . c_str () ) );
	CPatchStr x4 ( x1 . subStr ( 0, 0 ) );
	std::string    s4 ( s1 . substr ( 0, 0 ) );
	assert ( stringMatch ( x4 . toStr (), s4 . c_str () ) );
	try
	{
		CPatchStr x5 ( x4 . subStr ( 0, 1 ) );
		assert ( "No exception thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown." == nullptr );
	}
}

int main ()
{
	appendTest();
	subStringTest();
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */