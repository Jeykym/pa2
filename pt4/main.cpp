#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <compare>

class CDate {
public:
	CDate(
		int y,
		int m,
		int d
	)	:	y_(y),
			m_(m),
			d_(d) {}


	std::strong_ordering operator<=>(const CDate& other) const = default;


	// puts the date as y-m-d into the stream
	friend std::ostream& operator<<(
		std::ostream& os,
		const CDate& d
	) {
		return os << d.y_ << '-' << d.m_ << '-' << d.d_;
	}


private:
	int y_;
	int m_;
	int d_;
};



enum ESortKey {
	NAME,
	BIRTH_DATE,
	ENROLL_YEAR
};


#endif /* __PROGTEST__ */


class CFilter;

class CStudent {
public:
	CStudent(
		const std::string& name,
		const CDate& born,
		int enrolled
	)	:	enrollYear_(enrolled),
			birthDate_(born),
			name_(name),
			registrationIndex_(0) {
		std::istringstream iss(name);
		std::string namePart;

		while (iss >> namePart) {
			// normalize for CFilter
			for (auto& c: namePart) c = std::tolower(c);
			normalizedName_.push_back(namePart);
		}


		std::sort(normalizedName_.begin(), normalizedName_.end());
	}


	bool operator==(const CStudent& other) const {
		if (enrollYear_ != other.enrollYear_) return false;

		if ((birthDate_ <=> other.birthDate_) != std::strong_ordering::equal) return false;

		if (name_.size() != other.name_.size()) return false;

		// comparison depends on the word order and is case-sensitive
		return name_ == other.name_;

		return true;
	}


	// compares by enrollYear_, birthDate_ and name (alphabetically, then by size)
	bool operator<(const CStudent& other) const {
		if (enrollYear_ < other.enrollYear_) return true;
		if (enrollYear_ > other.enrollYear_) return false;

		if (birthDate_ < other.birthDate_) return true;
		if (birthDate_ > other.birthDate_) return false;

		size_t smallerSize = std::min(name_.size(), other.name_.size());

		// compare names alphabetically
		for (size_t i = 0; i < smallerSize; i++) {
			if (name_[i] < other.name_[i]) return true;
			if (name_[i] > other.name_[i]) return true;
		}

		// if names are equal but different size, smaller is the shorter one
		if (name_.size() < other.name_.size()) return true;
		if (name_.size() > other.name_.size()) return true;

		// they're equal, return false for std::set's lower_bound
		return false;
	}


	size_t registrationIndex() const {
		return registrationIndex_;
	}


	void setRegistrationIndex(size_t i) const {
		registrationIndex_ = i;
	}


	int enrollYear() const {
		return enrollYear_;
	}


	const CDate& birthDate() const {
		return birthDate_;
	}


	const std::vector<std::string>& normalizedName() const {
		return normalizedName_;
	}


	const std::string& name() const {
		return name_;
	}


	friend std::ostream& operator<< (std::ostream& os, const CStudent& student) {
		os << "name: " << '"' << student.name_ << '"' << " born: " << student.birthDate_ << " enrolled: " << student.enrollYear_;
		return os;
	}


private:
	int enrollYear_;
	const CDate birthDate_;
	std::string name_;
	std::vector<std::string> normalizedName_;
	mutable size_t registrationIndex_;


	// returns a vector where each element is a word in the student's name
	// preserves word order and letter case
	static std::vector<std::string>  splitName(const std::string& name) {
		std::vector<std::string> res;

		std::istringstream iss(name);
		std::string namePart;

		// skipping whitespaces
		while (iss >> namePart) res.push_back(namePart);

		return res;
	}
};



class CFilter {
public:
	CFilter()
		:	enrolledBefore_(INT_MAX),
			enrolledAfter_(INT_MIN),
			bornBefore_(CDate(INT_MAX, INT_MAX, INT_MAX)),
			bornAfter_(CDate(INT_MIN, INT_MIN, INT_MIN)) {}


	CFilter& enrolledBefore(int year) {
		enrolledBefore_ = year;
		return *this;
	}


	CFilter& enrolledAfter(int year) {
		enrolledAfter_ = year;
		return *this;
	}


	CFilter& bornBefore(const CDate& date) {
		bornBefore_ = date;
		return *this;
	}


	CFilter& bornAfter(const CDate& date) {
		bornAfter_ = date;
		return *this;
	}


	CFilter& name(const std::string& name) {
		std::istringstream iss(name);
		std::vector<std::string> splitName;
		std::string namePart;

		while (iss >> namePart) {
			for (auto& c: namePart) c = std::tolower(c);
			splitName.push_back(namePart);
		};

		std::sort(splitName.begin(), splitName.end());

		names_.push_back(splitName);

		return *this;
	}


	// checks whether student's enrollYear is within range specified in the filter
	bool isEnrollRange(int enrollYear) const {
		return enrollYear < enrolledBefore_ && enrollYear > enrolledAfter_;
	}


	// checks whether student's bornYear is within range specified in the filter
	bool isBornRange(const CDate& bornYear) const {
		return (bornYear <=> bornBefore_) == std::strong_ordering::less
				&& bornYear <=> bornAfter_ == std::strong_ordering::greater;
	}


	// check if student's name matches some name in the filter
	bool isNameRange(const std::vector<std::string> normalizedName) const {
		// no restriction for name
		if (names_.empty()) return true;

		for (auto& name: names_) {
			// different lengths can't match
			if (namesMatch(name, normalizedName)) return true;
		}

		// no names match
		return false;
	}


	bool hasName() const {
		return !names_.empty();
	}



private:
	int enrolledBefore_;
	int enrolledAfter_;
	CDate bornBefore_;
	CDate bornAfter_;
	std::vector<std::vector<std::string>> names_;


	bool namesMatch(
		const std::vector<std::string>& a,
		const std::vector<std::string>& b
	) const {
		if (a.size() != b.size()) return false;

		for (size_t i = 0; i < a.size(); i++) {
			if (a[i] != b[i]) return false;
		}

		return true;
	}
};



class CSort {
public:
	CSort& addKey(
		ESortKey key,
		bool ascending
	) {
		keys_.emplace_back(key, ascending);
		return *this;
	}


	bool empty() const {
		return keys_.empty();
	}


	const std::vector<std::pair<ESortKey, bool>>& keys() const {
		return keys_;
	}


private:
	// vector of criteria to be sorted by and whether ascending or descending
	std::vector<std::pair<ESortKey, bool>> keys_;
};



class CStudyDept {
public:
	CStudyDept()
		:	studentIndex(0) {}


	bool addStudent(const CStudent& student) {
		auto res = students_.emplace(student);

		// students is already in the db
		if (!res.second) return false;

		// update student registration index as a last resort for CFilter
		res.first->setRegistrationIndex(studentIndex++);

		return true;
	}


	bool delStudent(const CStudent& student) {
		auto it = students_.find(student);

		// student is not in the db
		if (it == students_.end()) return false;

		students_.erase(it);

		return true;
	}


	std::list<CStudent> search(
		const CFilter& flt,
		const CSort& sortOpt
	) const {
		std::list<CStudent> res;

		for (auto& student: students_) {
			if (
				flt.isEnrollRange(student.enrollYear())
				&& flt.isBornRange(student.birthDate())
			) {
				// student does not match any name in the filter
				if (flt.hasName() && !flt.isNameRange(student.normalizedName())) continue;

				// there is no name filter or student matches some name
				res.push_back(student);
			}
		}

		res.sort([&](const CStudent& a, const CStudent& b)-> bool {
			for (auto& keyPair: sortOpt.keys()) {
				if (keyPair.first == ESortKey::NAME) {
					if (a.name() == b.name()) continue;

					if (keyPair.second) return a.name() < b.name();
					return b.name() < a.name();
				}

				if (keyPair.first == ESortKey::ENROLL_YEAR) {
					if (a.enrollYear() == b.enrollYear()) continue;

					if (keyPair.second) return a.enrollYear() < b.enrollYear();
					return a.enrollYear() > b.enrollYear();
				}

				if (keyPair.first == ESortKey::BIRTH_DATE) {
					auto cmpRes = a.birthDate() <=> b.birthDate();

					if (cmpRes == std::strong_ordering::equal) continue;

					if (keyPair.second) return cmpRes == std::strong_ordering::less;
					return cmpRes == std::strong_ordering::greater;
				}
			}

			return a.registrationIndex() < b.registrationIndex();
		});

//		for (auto& student: res) {
//			std::cout << student << std::endl;
//		}
//		std::cout << std::endl;

		return res;
	}


	std::set<std::string> suggest(const std::string& name) const {
		std::vector<std::string> normalizedName;
		std::string namePart;
		std::istringstream iss(name);


		std::set<std::string> res;

		while (iss >> namePart) {
			for (auto& c: namePart) c = std::tolower(c);
			normalizedName.push_back(namePart);
		}

		for (auto& student: students_) {
			if (!hasNames(student.normalizedName(), normalizedName)) continue;

			res.insert(student.name());
		}

//		for (auto& n: res) std::cout << n << std::endl;
//		std::cout << std::endl;

		return res;
	}


private:
	std::set<CStudent> students_;
	size_t studentIndex;


	static bool hasNames(
		const std::vector<std::string>& studentName,
		const std::vector<std::string>& normalizedName
	) {
		for (auto& namePart: normalizedName) {
			// doesn't have some part of the name
			if (std::find(studentName.begin(), studentName.end(), namePart) == studentName.end()) return false;
		}

		return true;
	}
};



#ifndef __PROGTEST__
int main ( void )
{
	assert(CDate(1980, 4, 11) <=> CDate(1980, 4, 11) == std::strong_ordering::equal);
	assert(CDate(1980, 4, 10) <=> CDate(1980, 4, 11) == std::strong_ordering::less);
	assert(CDate(1980, 4, 12) <=> CDate(1980, 4, 11) == std::strong_ordering::greater);
	assert(CDate(1980, 3, 11) <=> CDate(1980, 4, 11) == std::strong_ordering::less);
	assert(CDate(1980, 5, 11) <=> CDate(1980, 4, 11) == std::strong_ordering::greater);
	assert(CDate(1979, 4, 11) <=> CDate(1980, 4, 11) == std::strong_ordering::less);
	assert(CDate(1981, 4, 11) <=> CDate(1980, 4, 11) == std::strong_ordering::greater);



	CStudyDept x0;
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) ) );
	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) );
	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) ) );
	assert ( x0 . addStudent ( CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ) ) );
	assert ( !x0 . addStudent ( CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ) ) );
	assert ( x0 . addStudent ( CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ) ) );
	assert ( x0 . addStudent ( CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ) ) );
	assert ( x0 . addStudent ( CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ) ) );
	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ) ) );
	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ) ) );
	assert ( !x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ) ) );
	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ) ) );
	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ) ) );
	assert ( x0 . addStudent ( CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ) ) );
	assert ( x0 . search ( CFilter (), CSort () ) == (std::list<CStudent>
	{
	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
	}) );
	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
	}) );
	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, false ) ) == (std::list<CStudent>
	{
	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
	}) );
	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
	}) );
	assert ( x0 . search ( CFilter () . name ( "james bond" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 )
	}) );
	assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
	}) );
	assert ( x0 . search ( CFilter () . name ( "james" ), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	}) );
	assert ( x0 . suggest ( "peter" ) == (std::set<std::string>
	{
	"John Peter Taylor",
	"Peter John Taylor",
	"Peter Taylor"
	}) );
	assert ( x0 . suggest ( "bond" ) == (std::set<std::string>
	{
	"Bond James",
	"James Bond"
	}) );
	assert ( x0 . suggest ( "peter joHn" ) == (std::set<std::string>
	{
	"John Peter Taylor",
	"Peter John Taylor"
	}) );
	assert ( x0 . suggest ( "peter joHn bond" ) == (std::set<std::string>
	{
	}) );
	assert ( x0 . suggest ( "pete" ) == (std::set<std::string>
	{
	}) );
	assert ( x0 . suggest ( "peter joHn PETER" ) == (std::set<std::string>
	{
	"John Peter Taylor",
	"Peter John Taylor"
	}) );
	assert ( ! x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
	assert ( x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
	assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
	{
	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
	}) );
	assert ( ! x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */