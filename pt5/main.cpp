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
	explicit CBase(
		const std::string& type
	)	:	isLast_(true),
			type_(type) {}


	friend std::ostream& operator<<(
		std::ostream& os,
		const CBase& obj
	) {
		obj.pad(os);
		obj.begin(os);
		obj.print(os);

		return os;
	}


	// first level printing
	virtual std::ostream& pad(std::ostream& os) const {
		os << "";
		return os;
	};


	// +- for all but last which is /-
	std::ostream& begin(std::ostream& os) const {
		os << (isLast_ ? "\\" : "+") << '-';
		return os;
	}


	virtual std::ostream& print(std::ostream& os) const = 0;


	void unsetLast() {
		isLast_ = false;
	}


protected:
	bool isLast_;
	bool hostIsLast;
	const std::string type_;
};



class CComponent: public CBase {
public:
	using CBase::CBase;


	std::ostream& print(std::ostream& os) const override {
		os << type_ << ", ";
		printProperties(os);
		return os;
	}


	virtual std::ostream& printProperties(std::ostream& os) const = 0;


	// second level print
	std::ostream& pad(std::ostream& os) const override {
		os << "| ";
		return os;
	}
};



class CCPU: public CComponent {
public:
	CCPU(
		size_t cores,
		size_t frequency
	)	:	CComponent("CPU"),
			cores_(cores),
			frequency_(frequency) {}


	std::ostream& printProperties(std::ostream& os) const override {
		os << cores_ << " cores @ " << frequency_ << "MHz";
		return os;
	}


private:
	size_t cores_;
	size_t frequency_;
};



class CMemory: public CComponent {
public:
	CMemory(size_t size)
		:	CComponent("Memory"),
			size_(size) {}


	std::ostream& printProperties(std::ostream& os) const override {
		os << size_ << " Mib";
		return os;
	}


private:
	size_t size_;
};


class CPartition: public CBase {
public:
	CPartition(
		size_t i,
		size_t size,
		const std::string& name
	)	:	CBase("[" + std::to_string(i) + "]"),
			size_(size),
			name_(name) {}


	std::ostream& pad(std::ostream& os) const override {
		os << "| | ";
		return os;
	}


	std::ostream& print(std::ostream& os) const override {
		os << type_ << " " << size_ << " GiB, " << name_;
		return os;
	}


private:
	size_t size_;
	const std::string name_;
};



class CDisk: public CComponent {
public:
	static const size_t MAGNETIC = 0;
	static const size_t SSD = 1;


	CDisk(
		size_t type,
		size_t size
	)	:	CComponent(type == MAGNETIC ? "HDD" : "SDD"),
			type_(type),
			size_(size) {}


	std::ostream& printProperties(std::ostream& os) const override {
		os << size_ << " GiB" << std::endl;

		for (size_t i = 0; i < partitions_.size() - 1; i++) {
			os << partitions_[i] << std::endl;
		}

		os << partitions_[partitions_.size() - 1];
		return os;
	}


	CDisk& addPartition(
		size_t size,
		const std::string& name
	) {
		if (!partitions_.empty()) partitions_[partitions_.size() - 1].unsetLast();

		partitions_.emplace_back(partitions_.size(), size, name);

		return *this;
	}


private:
	size_t size_;
	size_t type_;
	std::vector<CPartition> partitions_;
};



class CIP: public CBase {
public:
	CIP(const std::string& ip)
		:	CBase("IP"),
			ip_(ip) {}


	std::ostream& pad(std::ostream& os) const override {
		os << "| ";
		return os;
	}


	std::ostream& print(std::ostream& os) const override {
		os << ip_;
		return os;
	}


private:
	const std::string ip_;
};



class CComputer: public CBase {
public:
	explicit CComputer(const std::string& name)
		:	CBase("Host"),
			name_(name) {}


	std::ostream& print(std::ostream& os) const override {
		os << "Host: " << name_ << std::endl;


		for (auto& ip: ips_) {
			os << ip << std::endl;
		}

		for (auto& component: components_) {
			os << *component << std::endl;
		}

		return os;
	}


	CComputer& addAddress(const std::string& ip) {
		if (!ips_.empty()) ips_[ips_.size() - 1].unsetLast();

		ips_.push_back({ip});

		// first printing ips then components
		if (!components_.empty()) ips_[ips_.size() - 1].unsetLast();


		return *this;
	}


	CComputer& addComponent(const CCPU& cpu) {
		if (!components_.empty()) components_[components_.size() - 1]->unsetLast();

		// first printing ips then components
		if (components_.empty() && !ips_.empty()) ips_[ips_.size() - 1].unsetLast();

		components_.push_back(std::make_shared<CCPU>(cpu));

		return *this;
	}


	CComputer& addComponent(const CMemory& memory) {
		if (!components_.empty()) components_[components_.size() - 1]->unsetLast();

		// first printing ips then components
		if (components_.empty() && !ips_.empty()) ips_[ips_.size() - 1].unsetLast();

		components_.push_back(std::make_shared<CMemory>(memory));

		return *this;
	}


	CComputer& addComponent(const CDisk& disk) {
		if (!components_.empty()) components_[components_.size() - 1]->unsetLast();

		// first printing ips then components
		if (components_.empty() && !ips_.empty()) ips_[ips_.size() - 1].unsetLast();

		components_.push_back(std::make_shared<CDisk>(disk));

		return *this;
	}


	const std::string& name() const {
		return name_;
	}


private:
	const std::string name_;
	std::vector<CIP> ips_;
	std::vector<std::shared_ptr<CComponent>> components_;
};



class CNetwork {
public:
	CNetwork(const std::string& name)
	:	name_(name) {}


	CNetwork& addComputer(const CComputer& computer) {
		if (!computers_.empty()) computers_[computers_.size() - 1].unsetLast();

		computers_.push_back(computer);

		return *this;
	}


	friend std::ostream& operator<<(
		std::ostream& os,
		const CNetwork& network
	) {
		os << "Network: " << network.name_ << std::endl;

		for (auto& computer: network.computers_) {
			os << computer;
		}

		return os;
	}


	std::vector<CComputer>::iterator findComputer(const std::string& name) {
		auto it = computers_.begin();

		while (it != computers_.end()) {
			if (it->name() == name) return it;
		}

		return computers_.end();
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
	CNetwork n ( "FIT network" );
	n . addComputer (
	CComputer ( "progtest.fit.cvut.cz" ) .
	addAddress ( "147.32.232.142" ) .
	addComponent ( CCPU ( 8, 2400 ) ) .
	addComponent ( CCPU ( 8, 1200 ) ) .
	addComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
	addPartition ( 50, "/" ) .
	addPartition ( 5, "/boot" ).
	addPartition ( 1000, "/var" ) ) .
	addComponent ( CDisk ( CDisk::SSD, 60 ) .
	addPartition ( 60, "/data" )  ) .
	addComponent ( CMemory ( 2000 ) ).
	addComponent ( CMemory ( 2000 ) ) ) .
	addComputer (
	CComputer ( "courses.fit.cvut.cz" ) .
	addAddress ( "147.32.232.213" ) .
	addComponent ( CCPU ( 4, 1600 ) ) .
	addComponent ( CMemory ( 4000 ) ).
	addComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
	addPartition ( 100, "/" )   .
	addPartition ( 1900, "/data" ) ) ) .
	addComputer (
	CComputer ( "imap.fit.cvut.cz" ) .
	addAddress ( "147.32.232.238" ) .
	addComponent ( CCPU ( 4, 2500 ) ) .
	addAddress ( "2001:718:2:2901::238" ) .
	addComponent ( CMemory ( 8000 ) ) );
	std::cout << n << std::endl;
	assert ( toString ( n ) ==
			 "Network: FIT network\n"
			 "+-Host: progtest.fit.cvut.cz\n"
			 "| +-147.32.232.142\n"
			 "| +-CPU, 8 cores @ 2400MHz\n"
			 "| +-CPU, 8 cores @ 1200MHz\n"
			 "| +-HDD, 1500 GiB\n"
			 "| | +-[0]: 50 GiB, /\n"
			 "| | +-[1]: 5 GiB, /boot\n"
			 "| | \\-[2]: 1000 GiB, /var\n"
			 "| +-SSD, 60 GiB\n"
			 "| | \\-[0]: 60 GiB, /data\n"
			 "| +-Memory, 2000 MiB\n"
			 "| \\-Memory, 2000 MiB\n"
			 "+-Host: courses.fit.cvut.cz\n"
			 "| +-147.32.232.213\n"
			 "| +-CPU, 4 cores @ 1600MHz\n"
			 "| +-Memory, 4000 MiB\n"
			 "| \\-HDD, 2000 GiB\n"
			 "|   +-[0]: 100 GiB, /\n"
			 "|   \\-[1]: 1900 GiB, /data\n"
			 "\\-Host: imap.fit.cvut.cz\n"
			 "  +-147.32.232.238\n"
			 "  +-2001:718:2:2901::238\n"
			 "  +-CPU, 4 cores @ 2500MHz\n"
			 "  \\-Memory, 8000 MiB\n" );
	CNetwork x = n;
	auto c = x . findComputer ( "imap.fit.cvut.cz" );
	assert ( toString ( *c ) ==
			 "Host: imap.fit.cvut.cz\n"
			 "+-147.32.232.238\n"
			 "+-2001:718:2:2901::238\n"
			 "+-CPU, 4 cores @ 2500MHz\n"
			 "\\-Memory, 8000 MiB\n" );
	c -> addComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
	addPartition ( 100, "system" ) .
	addPartition ( 200, "WWW" ) .
	addPartition ( 700, "mail" ) );
	assert ( toString ( x ) ==
			 "Network: FIT network\n"
			 "+-Host: progtest.fit.cvut.cz\n"
			 "| +-147.32.232.142\n"
			 "| +-CPU, 8 cores @ 2400MHz\n"
			 "| +-CPU, 8 cores @ 1200MHz\n"
			 "| +-HDD, 1500 GiB\n"
			 "| | +-[0]: 50 GiB, /\n"
			 "| | +-[1]: 5 GiB, /boot\n"
			 "| | \\-[2]: 1000 GiB, /var\n"
			 "| +-SSD, 60 GiB\n"
			 "| | \\-[0]: 60 GiB, /data\n"
			 "| +-Memory, 2000 MiB\n"
			 "| \\-Memory, 2000 MiB\n"
			 "+-Host: courses.fit.cvut.cz\n"
			 "| +-147.32.232.213\n"
			 "| +-CPU, 4 cores @ 1600MHz\n"
			 "| +-Memory, 4000 MiB\n"
			 "| \\-HDD, 2000 GiB\n"
			 "|   +-[0]: 100 GiB, /\n"
			 "|   \\-[1]: 1900 GiB, /data\n"
			 "\\-Host: imap.fit.cvut.cz\n"
			 "  +-147.32.232.238\n"
			 "  +-2001:718:2:2901::238\n"
			 "  +-CPU, 4 cores @ 2500MHz\n"
			 "  +-Memory, 8000 MiB\n"
			 "  \\-HDD, 1000 GiB\n"
			 "    +-[0]: 100 GiB, system\n"
			 "    +-[1]: 200 GiB, WWW\n"
			 "    \\-[2]: 700 GiB, mail\n" );
	assert ( toString ( n ) ==
			 "Network: FIT network\n"
			 "+-Host: progtest.fit.cvut.cz\n"
			 "| +-147.32.232.142\n"
			 "| +-CPU, 8 cores @ 2400MHz\n"
			 "| +-CPU, 8 cores @ 1200MHz\n"
			 "| +-HDD, 1500 GiB\n"
			 "| | +-[0]: 50 GiB, /\n"
			 "| | +-[1]: 5 GiB, /boot\n"
			 "| | \\-[2]: 1000 GiB, /var\n"
			 "| +-SSD, 60 GiB\n"
			 "| | \\-[0]: 60 GiB, /data\n"
			 "| +-Memory, 2000 MiB\n"
			 "| \\-Memory, 2000 MiB\n"
			 "+-Host: courses.fit.cvut.cz\n"
			 "| +-147.32.232.213\n"
			 "| +-CPU, 4 cores @ 1600MHz\n"
			 "| +-Memory, 4000 MiB\n"
			 "| \\-HDD, 2000 GiB\n"
			 "|   +-[0]: 100 GiB, /\n"
			 "|   \\-[1]: 1900 GiB, /data\n"
			 "\\-Host: imap.fit.cvut.cz\n"
			 "  +-147.32.232.238\n"
			 "  +-2001:718:2:2901::238\n"
			 "  +-CPU, 4 cores @ 2500MHz\n"
			 "  \\-Memory, 8000 MiB\n" );
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */