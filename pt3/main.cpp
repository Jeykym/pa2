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
	CPatchStr()
		:	size(1),
			maxSize(10),
			array(new Patch[maxSize]),
			length(0) {
		array[0] = {
			0,
			0,
			std::make_shared<String>(""),
			0
		};
	}

	CPatchStr(const char* str)
	: size(1),
	  maxSize(10),
	  array(new Patch[maxSize]),
	  length(strlen(str)) {
		array[0] = {
			0,
			length,
			std::make_shared<String>(str),
			0
		};
		int i = 0;
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

		auto& originalStart = src.array[startI];
		auto& originalEnd = src.array[endI];

		length = len;
		size = endI - startI + 1;
		maxSize = size * 2;
		array = new Patch[maxSize];

		for (size_t i = 0; i < size; i++) {
			array[i] = src.array[startI + i];
		}

		auto& thisStart = array[0];
		auto& thisEnd = array[size - 1];

		array[0].offset = from - src.array[startI].globalOffset;
		array[0].length = src.array[startI].length - array[0].offset;

		array[size - 1].length = from + len - src.array[endI].globalOffset;

		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
		}
	}


	CPatchStr& operator=(const CPatchStr& src) {
		if (this == &src) return *this;

		delete [] array;

		size = src.size;
		maxSize = src.maxSize;
		length = src.length;
		array = new Patch[maxSize];

		for (size_t i = 0; i < size; i++) {
			array[i] = src.array[i];
		}

		return *this;
	}


	~CPatchStr() {
		delete [] array;
	}


	CPatchStr& append(const CPatchStr& src) {
		if (src.length == 0) return *this;

		if (length == 0) {
			*this = src;
			return *this;
		}


		if (size + src.size >= maxSize) realloc(size + src.size);

		if (length == 0) {
			*this = src;
			return *this;
		}

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


	char* toStr() const {
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
		if (from + len > length) throw std::out_of_range("Out of range");

		if (len == 0) return {""};

		return {from, len, *this};
	}


	CPatchStr& insert(
		size_t pos,
		const CPatchStr& src
	) {
		if (pos > length) throw std::out_of_range("Out of range");

		if (pos == 0) insertFront(src);
		else if (pos == length) append(src);
		else insertAt(pos, src);

		return *this;
	}


	CPatchStr& remove(
		size_t from,
		size_t len
	) {
		if (from + len > length) throw std::out_of_range("Out of range");

		if (len == 0) return *this;

		if (from == 0) removeFirstChars(len);
		else removeChars(from, len);

		return *this;
	}


	size_t Length() {
		return length;
	}


private:
	void insertFront(const CPatchStr& src) {
		auto res = src;
		res.append(*this);
		*this = res;
	}


	void insertAt(
	size_t pos,
	const CPatchStr& src
	) {
		auto res = subStr(0, pos);

		auto s = res.toStr();
		std::cout << '"' << s << '"' << std::endl;
		delete [] s;


		res.append(src);

		auto sub = subStr(pos, length - pos);
		auto str = sub.toStr();
		std::cout << '"' << str << '"' << std::endl;
		delete [] str;
		res.append(subStr(pos, length - pos));

		*this = res;
	}


	void removeFirstChars(size_t count) {
		*this = subStr(count, length - count);
	}


	void removeChars(
		size_t from,
		size_t len
	) {
		auto res = subStr(0, from);
		res.append(subStr(from + len, length - from - len));
		*this = res;
	}


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

#include <string>
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
	 */    /* testing appending
     */
}


void insertTest() {
	/* testing insertion
	*/

	CPatchStr x;
	std::string s;

	x . append ( "testtest" );
	s . append ( "testtest" );
	x . insert ( 4, "ing" );
	s . insert ( 4, "ing" );
	x . insert ( 0, "Too" );
	s . insert ( 0, "Too" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . insert ( 5, "" );
	s . insert ( 5, "" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . insert ( 0, "a" );
	s . insert ( 0, "a" );
	x . insert ( 1, "b" );
	s . insert ( 1, "b" );
	x . insert ( 2, "c" );
	s . insert ( 2, "c" );
	x . insert ( 3, "d" );
	s . insert ( 3, "d" );
	x . insert ( 4, "e" );
	s . insert ( 4, "e" );
	x . insert ( 5, "f" );
	s . insert ( 5, "f" );
	x . insert ( 6, "g" );
	s . insert ( 6, "g" );
	x . insert ( 7, "h" );
	s . insert ( 7, "h" );
	x . insert ( x . Length (), "hello" );
	s . insert ( s . length (), "hello" );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	try
	{
		x . insert ( x . Length () + 1, "hello" );
		assert ( "No exception thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown" == nullptr );
	}
	CPatchStr x2 ( "cool" );
	std::string s2 ( "cool" );
	x2 . insert ( 2, "FOO" );
	s2 . insert ( 2, "FOO" );
	x2 . insert ( 4, x );
	s2 . insert ( 4, s );
	x2 . insert ( 12, x2 );
	s2 . insert ( 12, s2 );
	x2 . insert ( x2 . Length (), x2 );
	s2 . insert ( s2 . length (), s2 );
	x2 . insert ( x2 . Length (), x2 );
	s2 . insert ( s2 . length (), s2 );
	x2 . insert ( x2 . Length (), x2 );
	s2 . insert ( s2 . length (), s2 );
	x2 . insert ( x2 . Length (), x2 );
	s2 . insert ( s2 . length (), s2 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );

	/* 2 260 allocs
	 */
}


void appendInsertRemoveTest() {
	/* testing deletion, insertion, appending
	*/

	CPatchStr x ( "fubar" );
	std::string s ( "fubar" );
	x . remove ( 2, 3 );
	s . erase ( 2, 3 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x = "fubar";
	s = "fubar";
	x . remove ( 0, 5 );
	s . erase ( 0, 5 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x = "fubar";
	s = "fubar";
	x . remove ( 0, 3 );
	s . erase ( 0, 3 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x = "fubar";
	s = "fubar";
	x . remove ( 3, 2 );
	s . erase ( 3, 2 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x = "fubar";
	s = "fubar";
	x . append ( "hello" );
	s . append ( "hello" );
	x . append ( "bleble" );
	s . append ( "bleble" );
	x . append ( "rumbaba" );
	s . append ( "rumbaba" );
	x . remove ( 5, 15 );
	s . erase ( 5, 15 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . insert ( 0, "abcdefghijkl" );
	s . insert ( 0, "abcdefghijkl" );
	x . insert ( 5, "lalala" );
	s . insert ( 5, "lalala" );
	x . remove ( 0, 4 );
	s . erase ( 0, 4 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . append ( "abc" );
	s . append ( "abc" );
	x . remove ( 0, x . Length () );
	s . erase ( 0, s . length () );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	x . remove ( 0, 0 );
	s . erase ( 0, 0 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );
	try
	{
		x . remove ( 0, 1 );
		assert ( "No exception thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown" == nullptr );
	}
	x . append ( "abcde" );
	s . append ( "abcde" );
	x . append ( x );
	s . append ( s );
	x . append ( "abc" );
	s . append ( "abc" );
	try
	{
		x . remove ( 10, 15 );
		assert ( "No exception thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown" == nullptr );
	}
	try
	{
		x . remove ( x . Length (), 0 );
		assert ( "No exception thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown" == nullptr );
	}
	x . remove ( 12, 1 );
	s . erase ( 12, 1 );
	assert ( stringMatch ( x . toStr (), s . c_str () ) );

	/* 2 345 allocs
	 */
}


void removeTests() {
	CPatchStr a ("aaaa");
	a.insert(0, "bbbb ");
	assert (stringMatch(a.toStr(), "bbbb aaaa"));
	a.insert(2, " cccc ");
	assert (stringMatch(a.toStr(), "bb cccc bb aaaa"));
	a.insert(15, " dddd");
	assert (stringMatch(a.toStr(), "bb cccc bb aaaa dddd"));
	a.insert(0, "bbbb ");
	assert (stringMatch(a.toStr(), "bbbb bb cccc bb aaaa dddd"));
	a.insert(2, " cccc ");
	assert (stringMatch(a.toStr(), "bb cccc bb bb cccc bb aaaa dddd"));
	a.insert(31, " dddd");
	assert (stringMatch(a.toStr(), "bb cccc bb bb cccc bb aaaa dddd dddd"));
	a.insert(0, "xxxx ").insert(41, " yyyy");
	assert (stringMatch(a.toStr(), "xxxx bb cccc bb bb cccc bb aaaa dddd dddd yyyy"));
	a.insert(1, "1111");
	assert (stringMatch(a.toStr(), "x1111xxx bb cccc bb bb cccc bb aaaa dddd dddd yyyy"));
	a.insert(8, "8888");
	char* str = a.toStr();
	std::cout << '"' << str << '"' << std::endl;
	delete [] str;
	assert (stringMatch(a.toStr(), "x1111xxx8888 bb cccc bb bb cccc bb aaaa dddd dddd yyyy"));
	a.insert(13, "1313");
	assert (stringMatch(a.toStr(), "x1111xxx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyy"));
	a.insert(7, a.subStr(0, 23));
	assert (stringMatch(a.toStr(), "x1111xxx1111xxx8888 1313bb cccx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyy"));
	a.append("asdfasdf65654564asf");
	a.append(a);
	a.append(a);

	std::string s = "x1111xxx1111xxx8888 1313bb cccx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyyasdfasdf65654564asfx1111xxx1111xxx8888 1313bb cccx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyyasdfasdf65654564asfx1111xxx1111xxx8888 1313bb cccx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyyasdfasdf65654564asfx1111xxx1111xxx8888 1313bb cccx8888 1313bb cccc bb bb cccc bb aaaa dddd dddd yyyyasdfasdf65654564asf";

	a.remove(25,1);
	s.erase(25,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(26,1);
	s.erase(26,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(34,1);
	s.erase(34,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,1);
	s.erase(12,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(34,1);
	s.erase(34,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(6,1);
	s.erase(6,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(18,1);
	s.erase(18,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(36,1);
	s.erase(36,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(17,1);
	s.erase(17,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(5,1);
	s.erase(5,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(35,1);
	s.erase(35,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(22,1);
	s.erase(22,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(1,1);
	s.erase(1,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(14,1);
	s.erase(14,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(6,1);
	s.erase(6,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(0,1);
	s.erase(0,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(9,1);
	s.erase(9,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(36,1);
	s.erase(36,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(37,1);
	s.erase(37,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(11,1);
	s.erase(11,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(37,1);
	s.erase(37,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(35,1);
	s.erase(35,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(2,1);
	s.erase(2,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(38,1);
	s.erase(38,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(25,1);
	s.erase(25,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(5,1);
	s.erase(5,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(15,1);
	s.erase(15,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(40,1);
	s.erase(40,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(31,1);
	s.erase(31,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(37,1);
	s.erase(37,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(21,1);
	s.erase(21,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,1);
	s.erase(12,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(25,1);
	s.erase(25,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(4,1);
	s.erase(4,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(39,1);
	s.erase(39,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(38,1);
	s.erase(38,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(31,1);
	s.erase(31,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(22,1);
	s.erase(22,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(31,1);
	s.erase(31,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(10,1);
	s.erase(10,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(19,1);
	s.erase(19,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(23,1);
	s.erase(23,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(37,1);
	s.erase(37,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(24,1);
	s.erase(24,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(13,1);
	s.erase(13,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(25,1);
	s.erase(25,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(19,1);
	s.erase(19,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(36,1);
	s.erase(36,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(36,1);
	s.erase(36,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(5,1);
	s.erase(5,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(25,1);
	s.erase(25,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(32,1);
	s.erase(32,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(24,1);
	s.erase(24,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(26,1);
	s.erase(26,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(0,1);
	s.erase(0,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(31,1);
	s.erase(31,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(21,1);
	s.erase(21,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(10,1);
	s.erase(10,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(4,1);
	s.erase(4,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(15,1);
	s.erase(15,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(29,1);
	s.erase(29,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(2,1);
	s.erase(2,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(16,1);
	s.erase(16,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(20,1);
	s.erase(20,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(28,1);
	s.erase(28,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(7,1);
	s.erase(7,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(23,1);
	s.erase(23,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(10,1);
	s.erase(10,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,1);
	s.erase(12,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(21,1);
	s.erase(21,1);
	assert(stringMatch(a.toStr(), s.c_str()));

	a.remove(13,6);
	s.erase(13,6);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(4,16);
	s.erase(4,16);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(20,4);
	s.erase(20,4);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(15,4);
	s.erase(15,4);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(19,11);
	s.erase(19,11);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(0,17);
	s.erase(0,17);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,3);
	s.erase(12,3);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(18,14);
	s.erase(18,14);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(20,12);
	s.erase(20,12);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,6);
	s.erase(12,6);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(1,1);
	s.erase(1,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(18,17);
	s.erase(18,17);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(8,9);
	s.erase(8,9);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(6,1);
	s.erase(6,1);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(13,4);
	s.erase(13,4);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(8,15);
	s.erase(8,15);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(15,12);
	s.erase(15,12);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(18,18);
	s.erase(18,18);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(4,20);
	s.erase(4,20);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(11,5);
	s.erase(11,5);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(9,5);
	s.erase(9,5);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(18,19);
	s.erase(18,19);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(10,3);
	s.erase(10,3);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(9,2);
	s.erase(9,2);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(4,9);
	s.erase(4,9);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(15,2);
	s.erase(15,2);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(12,2);
	s.erase(12,2);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(17,4);
	s.erase(17,4);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(19,11);
	s.erase(19,11);
	assert(stringMatch(a.toStr(), s.c_str()));
	a.remove(6,18);
	s.erase(6,18);
	assert(stringMatch(a.toStr(), s.c_str()));
}



int main () {
//	appendTest();
//	insertTest();
	removeTests();
//	appendInsertRemoveTest();
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */