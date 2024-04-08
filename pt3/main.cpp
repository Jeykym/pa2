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

	friend std::ostream& operator<<(std::ostream& os, const Patch& patch) {
		os << "offset: " << patch.offset << std::endl;
		os << "length: " << patch.length << std::endl;
		os << "str: " << '"' << patch.str->chars << '"' << std::endl;
		os << "globalOffset: " << patch.globalOffset << std::endl;

		return os;
	}
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

		for (size_t i = 0; i < size; i++) {
			array[i] = src.array[startI + i];
		}

		array[0].offset = from - src.array[startI].globalOffset + src.array[startI].offset;
		array[0].length = array[0].length - array[0].offset + src.array[startI].offset;

		updateGlobalOffsets();

		if (len <= array[0].length) {
			array[0].length = len;
		} else {
			for (size_t i = 0; i < size - 1; i++) {
				len -= array[i].length;
			}

			array[size - 1].length = len;
		}

	}

	friend std::ostream& operator<<(std::ostream& os, const CPatchStr& patchStr) {
		for (size_t i = 0; i < patchStr.size; i++) {
			os << patchStr.array[i] << std::endl;
		}

		return os;
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


	char* toStr() const {
		char* str = new char[length + 1];

		size_t strI = 0;
		for (size_t i = 0; i < size; i++) {
			size_t charI = array[i].offset;
			for (size_t j = 0; j < array[i].length; j++) {
				str[strI++] = array[i].str->chars[charI++];
			}
		}

		str[length] = '\0';

		return str;
	}


	CPatchStr& append(const CPatchStr& src) {
		if (size + src.size >= maxSize) realloc(size + src.size);

		if (this == &src) appendSelf();
		else appendOther(src);

		updateGlobalOffsets();

		return *this;
	}


	CPatchStr subStr(
		size_t from,
		size_t len
	) const {
		if (from + len > length) throw std::out_of_range("Out of range");

		if (len == 0) return {""};

		if (length == 0) return {""};

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

		auto res = subStr(0, from);
		res.append(subStr(from + len, length - (from + len)));
		*this = res;
		return *this;
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

		res.append(src);

		res.append(subStr(pos, length - pos));

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


	void updateGlobalOffsets() {
		size_t totalOffset = 0;
		for (size_t i = 0; i < size; i++) {
			array[i].globalOffset = totalOffset;
			totalOffset += array[i].length;
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
bool stringMatch ( char       * str,
const char * expected )
{
	char tmpStr[100];

	CPatchStr a ( "test" );
	assert ( stringMatch ( a . toStr (), "test" ) );
	std::strncpy ( tmpStr, " da", sizeof ( tmpStr ) - 1 );
	a . append ( tmpStr );
	assert ( stringMatch ( a . toStr (), "test da" ) );
	std::strncpy ( tmpStr, "ta", sizeof ( tmpStr ) - 1 );
	a . append ( tmpStr );
	assert ( stringMatch ( a . toStr (), "test data" ) );
	std::strncpy ( tmpStr, "foo text", sizeof ( tmpStr ) - 1 );
	CPatchStr b ( tmpStr );
	assert ( stringMatch ( b . toStr (), "foo text" ) );
	CPatchStr c ( a );
	assert ( stringMatch ( c . toStr (), "test data" ) );
	CPatchStr d ( a . subStr ( 3, 5 ) );
	assert ( stringMatch ( d . toStr (), "t dat" ) );
	d . append ( b );
	assert ( stringMatch ( d . toStr (), "t datfoo text" ) );
	d . append ( b . subStr ( 3, 4 ) );
	assert ( stringMatch ( d . toStr (), "t datfoo text tex" ) );
	c . append ( d );
	assert ( stringMatch ( c . toStr (), "test datat datfoo text tex" ) );
	c . append ( c );
	assert ( stringMatch ( c . toStr (), "test datat datfoo text textest datat datfoo text tex" ) );
	auto subStr = c.subStr(6, 9);
	d . insert ( 2, c . subStr ( 6, 9 ) );
	assert ( stringMatch ( d . toStr (), "t atat datfdatfoo text tex" ) );
	b = "abcdefgh";
	assert ( stringMatch ( b . toStr (), "abcdefgh" ) );
	assert ( stringMatch ( d . toStr (), "t atat datfdatfoo text tex" ) );
	assert ( stringMatch ( d . subStr ( 4, 8 ) . toStr (), "at datfd" ) );
	assert ( stringMatch ( b . subStr ( 2, 6 ) . toStr (), "cdefgh" ) );
	try
	{
		b . subStr ( 2, 7 ) . toStr ();
		assert ( "Exception not thrown" == nullptr );
	}
	catch ( const std::out_of_range & e )
	{
	}
	catch ( ... )
	{
		assert ( "Invalid exception thrown" == nullptr );
	}
	a . remove ( 3, 5 );
	assert ( stringMatch ( a . toStr (), "tesa" ) );
	return EXIT_SUCCESS;
}

int main () {
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */