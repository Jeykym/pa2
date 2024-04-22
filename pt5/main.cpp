#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#endif /* __PROGTEST__ */

//class CComputer ...
//class CNetwork ...
//class CCPU ...
//class CMemory ...
//class CDisk ...


class CBase {
public:
	virtual std::ostream& print(
		std::ostream& os,
		const std::string& padString
	) const = 0;
};


class CComputer: public CBase {
public:
	CComputer(const std::string& name)
		:	name_(name) {}


	const std::string& name() const {
		return name_;
	}


	std::ostream& print(
		std::ostream& os,
		const std::string& padString
	) const override {
		return os;
	}


private:
	const std::string name_;
};




class CNetwork: public CBase {
public:
	CNetwork(const std::string& name)
		:	name_(name) {}


	friend std::ostream& operator<<(
		std::ostream& os,
		const CNetwork& network
	) {
		network.print(os, "");
		return os;
	}


	std::ostream& print(
		std::ostream& os,
		const std::string& padString
	) const override {
		os << "Network: " << name_ << std::endl;

		if (computers_.size() >= 2) {
			for (size_t i = 0; i < computers_.size() - 1; i++) {
				os << padString << "+-" << "Host: " << computers_[i].name() << std::endl;
			}
		}

		if (!computers_.empty()) {
			os << padString << "\\-" << "Host: " << computers_[computers_.size() - 1].name() << std::endl;
		}

		return os;
	}


	CNetwork& addComputer(const CComputer& computer) {
		computers_.push_back(computer);
		return *this;
	}


private:
	const std::string name_;
	std::vector<CComputer> computers_;
};



#ifndef __PROGTEST__
template<typename T_>
std::string toString ( const T_ & x )
{
	std::ostringstream oss;
	oss << x;
	return oss . str ();
}


int main ()
{
	CNetwork n("FIT network");
	n.addComputer(CComputer("progtest.fit.cvut.cz"));
	n.addComputer(CComputer("google.com"));
	std::cout << n << std::endl;
	assert(toString(n) ==
		"Network: FIT network\n"
		"+-Host: progtest.fit.cvut.cz\n"
		"\\-Host: google.com\n"
	);
//	CNetwork n ( "FIT network" );
//	n . addComputer (
//	CComputer ( "progtest.fit.cvut.cz" ) .
//	addAddress ( "147.32.232.142" ) .
//	addComponent ( CCPU ( 8, 2400 ) ) .
//	addComponent ( CCPU ( 8, 1200 ) ) .
//	addComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
//	addPartition ( 50, "/" ) .
//	addPartition ( 5, "/boot" ).
//	addPartition ( 1000, "/var" ) ) .
//	addComponent ( CDisk ( CDisk::SSD, 60 ) .
//	addPartition ( 60, "/data" )  ) .
//	addComponent ( CMemory ( 2000 ) ).
//	addComponent ( CMemory ( 2000 ) ) ) .
//	addComputer (
//	CComputer ( "courses.fit.cvut.cz" ) .
//	addAddress ( "147.32.232.213" ) .
//	addComponent ( CCPU ( 4, 1600 ) ) .
//	addComponent ( CMemory ( 4000 ) ).
//	addComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
//	addPartition ( 100, "/" )   .
//	addPartition ( 1900, "/data" ) ) ) .
//	addComputer (
//	CComputer ( "imap.fit.cvut.cz" ) .
//	addAddress ( "147.32.232.238" ) .
//	addComponent ( CCPU ( 4, 2500 ) ) .
//	addAddress ( "2001:718:2:2901::238" ) .
//	addComponent ( CMemory ( 8000 ) ) );
//	assert ( toString ( n ) ==
//			 "Network: FIT network\n"
//			 "+-Host: progtest.fit.cvut.cz\n"
//			 "| +-147.32.232.142\n"
//			 "| +-CPU, 8 cores @ 2400MHz\n"
//			 "| +-CPU, 8 cores @ 1200MHz\n"
//			 "| +-HDD, 1500 GiB\n"
//			 "| | +-[0]: 50 GiB, /\n"
//			 "| | +-[1]: 5 GiB, /boot\n"
//			 "| | \\-[2]: 1000 GiB, /var\n"
//			 "| +-SSD, 60 GiB\n"
//			 "| | \\-[0]: 60 GiB, /data\n"
//			 "| +-Memory, 2000 MiB\n"
//			 "| \\-Memory, 2000 MiB\n"
//			 "+-Host: courses.fit.cvut.cz\n"
//			 "| +-147.32.232.213\n"
//			 "| +-CPU, 4 cores @ 1600MHz\n"
//			 "| +-Memory, 4000 MiB\n"
//			 "| \\-HDD, 2000 GiB\n"
//			 "|   +-[0]: 100 GiB, /\n"
//			 "|   \\-[1]: 1900 GiB, /data\n"
//			 "\\-Host: imap.fit.cvut.cz\n"
//			 "  +-147.32.232.238\n"
//			 "  +-2001:718:2:2901::238\n"
//			 "  +-CPU, 4 cores @ 2500MHz\n"
//			 "  \\-Memory, 8000 MiB\n" );
//	CNetwork x = n;
//	auto c = x . findComputer ( "imap.fit.cvut.cz" );
//	assert ( toString ( *c ) ==
//			 "Host: imap.fit.cvut.cz\n"
//			 "+-147.32.232.238\n"
//			 "+-2001:718:2:2901::238\n"
//			 "+-CPU, 4 cores @ 2500MHz\n"
//			 "\\-Memory, 8000 MiB\n" );
//	c -> addComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
//	addPartition ( 100, "system" ) .
//	addPartition ( 200, "WWW" ) .
//	addPartition ( 700, "mail" ) );
//	assert ( toString ( x ) ==
//			 "Network: FIT network\n"
//			 "+-Host: progtest.fit.cvut.cz\n"
//			 "| +-147.32.232.142\n"
//			 "| +-CPU, 8 cores @ 2400MHz\n"
//			 "| +-CPU, 8 cores @ 1200MHz\n"
//			 "| +-HDD, 1500 GiB\n"
//			 "| | +-[0]: 50 GiB, /\n"
//			 "| | +-[1]: 5 GiB, /boot\n"
//			 "| | \\-[2]: 1000 GiB, /var\n"
//			 "| +-SSD, 60 GiB\n"
//			 "| | \\-[0]: 60 GiB, /data\n"
//			 "| +-Memory, 2000 MiB\n"
//			 "| \\-Memory, 2000 MiB\n"
//			 "+-Host: courses.fit.cvut.cz\n"
//			 "| +-147.32.232.213\n"
//			 "| +-CPU, 4 cores @ 1600MHz\n"
//			 "| +-Memory, 4000 MiB\n"
//			 "| \\-HDD, 2000 GiB\n"
//			 "|   +-[0]: 100 GiB, /\n"
//			 "|   \\-[1]: 1900 GiB, /data\n"
//			 "\\-Host: imap.fit.cvut.cz\n"
//			 "  +-147.32.232.238\n"
//			 "  +-2001:718:2:2901::238\n"
//			 "  +-CPU, 4 cores @ 2500MHz\n"
//			 "  +-Memory, 8000 MiB\n"
//			 "  \\-HDD, 1000 GiB\n"
//			 "    +-[0]: 100 GiB, system\n"
//			 "    +-[1]: 200 GiB, WWW\n"
//			 "    \\-[2]: 700 GiB, mail\n" );
//	assert ( toString ( n ) ==
//			 "Network: FIT network\n"
//			 "+-Host: progtest.fit.cvut.cz\n"
//			 "| +-147.32.232.142\n"
//			 "| +-CPU, 8 cores @ 2400MHz\n"
//			 "| +-CPU, 8 cores @ 1200MHz\n"
//			 "| +-HDD, 1500 GiB\n"
//			 "| | +-[0]: 50 GiB, /\n"
//			 "| | +-[1]: 5 GiB, /boot\n"
//			 "| | \\-[2]: 1000 GiB, /var\n"
//			 "| +-SSD, 60 GiB\n"
//			 "| | \\-[0]: 60 GiB, /data\n"
//			 "| +-Memory, 2000 MiB\n"
//			 "| \\-Memory, 2000 MiB\n"
//			 "+-Host: courses.fit.cvut.cz\n"
//			 "| +-147.32.232.213\n"
//			 "| +-CPU, 4 cores @ 1600MHz\n"
//			 "| +-Memory, 4000 MiB\n"
//			 "| \\-HDD, 2000 GiB\n"
//			 "|   +-[0]: 100 GiB, /\n"
//			 "|   \\-[1]: 1900 GiB, /data\n"
//			 "\\-Host: imap.fit.cvut.cz\n"
//			 "  +-147.32.232.238\n"
//			 "  +-2001:718:2:2901::238\n"
//			 "  +-CPU, 4 cores @ 2500MHz\n"
//			 "  \\-Memory, 8000 MiB\n" );
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */