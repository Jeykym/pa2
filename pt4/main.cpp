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
#endif


//class CDate
//{
//public:
//	CDate                         ( int               y,
//	int               m,
//	int               d )
//	: m_Y ( y ),
//	  m_M ( m ),
//	  m_D ( d )
//	{
//	}
//
//	std::strong_ordering     operator <=>                  ( const CDate     & other ) const = default;
//
//	friend std::ostream    & operator <<                   ( std::ostream    & os,
//	const CDate     & d )
//	{
//		return os << d . m_Y << '-' << d . m_M << '-' << d . m_D;
//	}
//
//private:
//	int                      m_Y;
//	int                      m_M;
//	int                      m_D;
//};

class CDate {
public:
	CDate(
		int y_,
		int m_,
		int d_
	)	:	y(y_),
			m(m_),
			d(d_) {}


	friend bool operator==(
		const CDate& lhs,
		const CDate& rhs
	) {
		return lhs.y == rhs.y && lhs.m == rhs.m && lhs.d == rhs.d;
	}


	friend bool operator!=(
		const CDate& lhs,
		const CDate& rhs
	) {
		return !(lhs == rhs);
	}


	std::strong_ordering operator<=>(const CDate& other) const = default;

	friend std::ostream& operator<<(
		std::ostream& os,
		const CDate& date
	) {
		return os << date.y << '-' << date.m << '-' << date.d;
	}


private:
	const int y;
	const int m;
	const int d;
};



class CStudent {
public:
	CStudent(
		const std::string& name_,
		const CDate& born_,
		int enrolled
	)	:	name(normalize(name_)),
			born(born_),
			enrolled(enrolled) {}


private:
	std::vector<std::string> name;
	const CDate born;
	int enrolled;


	static std::vector<std::string> normalize(const std::string& name) {
		std::vector<std::string> normalizedName;

		std::istringstream iss(name);

		std::string namePart;

		while(iss >> namePart) {
			normalizedName.push_back(namePart);
		}

		return normalizedName;
	}


	bool operator==(const CStudent& other) const {
		return (
			enrolled != other.enrolled
			&& born != other.born
			&& name != other.name
		);
	}


	bool operator!=(const CStudent& other) const {
		return !(*this == other);
	}
};


enum class ESortKey {
	NAME,
	BIRTH_DATE,
	ENROLLED_YEAR
};



//enum class ESortKey
//{
//	NAME,
//	BIRTH_DATE,
//	ENROLL_YEAR
//};
//#endif /* __PROGTEST__ */
//class CStudent
//{
//public:
//	CStudent                      ( const std::string & name,
//	const CDate     & born,
//	int               enrolled );
//
//	bool                     operator ==                   ( const CStudent  & other ) const;
//	bool                     operator !=                   ( const CStudent  & other ) const;
//private:
//};
//
//class CFilter
//{
//public:
//	CFilter                       ();
//	CFilter                & name                          ( const std::string & name );
//	CFilter                & bornBefore                    ( const CDate     & date );
//	CFilter                & bornAfter                     ( const CDate     & date );
//	CFilter                & enrolledBefore                ( int               year );
//	CFilter                & enrolledAfter                 ( int               year );
//private:
//};
//
//class CSort
//{
//public:
//	CSort                         ();
//	CSort                  & addKey                        ( ESortKey          key,
//	bool              ascending );
//
//private:
//};
//
//class CStudyDept
//{
//public:
//	CStudyDept                    ();
//	bool                     addStudent                    ( const CStudent  & x );
//	bool                     delStudent                    ( const CStudent  & x );
//	std::list<CStudent>      search                        ( const CFilter   & flt,
//	const CSort     & sortOpt ) const;
//	std::set<std::string>    suggest                       ( const std::string & name ) const;
//private:
#ifndef __PROGTEST__
int main ( void )
{
	assert(CDate(1980, 4, 11) == CDate(1980, 4, 11));
	assert(CDate(1980, 4, 11) != CDate(1980, 4, 10));
	assert(CDate(1980, 4, 11) != CDate(1980, 5, 11));
	assert(CDate(1980, 4, 11) != CDate(1981, 4, 11));
//	CStudyDept x0;
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) ) );
//	assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) );
//	assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ) ) );
//	assert ( x0 . addStudent ( CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ) ) );
//	assert ( x0 . search ( CFilter (), CSort () ) == (std::list<CStudent>
//	{
//	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
//	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
//	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
//	}) );
//	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
//	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
//	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
//	}) );
//	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, false ) ) == (std::list<CStudent>
//	{
//	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
//	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
//	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
//	}) );
//	assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
//	CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
//	CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
//	}) );
//	assert ( x0 . search ( CFilter () . name ( "james bond" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 )
//	}) );
//	assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
//	}) );
//	assert ( x0 . search ( CFilter () . name ( "james" ), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	}) );
//	assert ( x0 . suggest ( "peter" ) == (std::set<std::string>
//	{
//	"John Peter Taylor",
//	"Peter John Taylor",
//	"Peter Taylor"
//	}) );
//	assert ( x0 . suggest ( "bond" ) == (std::set<std::string>
//	{
//	"Bond James",
//	"James Bond"
//	}) );
//	assert ( x0 . suggest ( "peter joHn" ) == (std::set<std::string>
//	{
//	"John Peter Taylor",
//	"Peter John Taylor"
//	}) );
//	assert ( x0 . suggest ( "peter joHn bond" ) == (std::set<std::string>
//	{
//	}) );
//	assert ( x0 . suggest ( "pete" ) == (std::set<std::string>
//	{
//	}) );
//	assert ( x0 . suggest ( "peter joHn PETER" ) == (std::set<std::string>
//	{
//	"John Peter Taylor",
//	"Peter John Taylor"
//	}) );
//	assert ( ! x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
//	assert ( x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
//	assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
//	{
//	CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
//	CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
//	CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
//	CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
//	}) );
//	assert ( ! x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */