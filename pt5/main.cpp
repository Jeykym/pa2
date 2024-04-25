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
	virtual void print(std::ostream& os, const std::string& prefix, const std::string& begin) const = 0;

	static bool isLast(const std::string& begin) {
		return begin == "\\-";
	}
};


class CNetwork;


class CComponent: public CBase {
public:
	virtual ~CComponent() = default;
	virtual std::unique_ptr<CComponent> clone() const = 0;
	virtual CNetwork* findNetwork(const std::string& name) = 0;
};


class CDisk: public CComponent {
public:
	static const size_t MAGNETIC = 0;
	static const size_t SSD = 1;

	CDisk(size_t type, size_t size)
		:	type_(type),
			size_(size) {}


	void print(std::ostream& os, const std::string& prefix, const std::string& begin) const override {
		os << prefix << begin << (type_ == MAGNETIC ? "HDD, " : "SSD, ") << size_ << " GiB" << std::endl;

		printPartitions(os, prefix + (isLast(begin) ? "  " : "| "));
	}


	std::unique_ptr<CComponent> clone() const override {
		return std::make_unique<CDisk>(*this);
	}


	CDisk& addPartition(size_t size, const std::string& name) {
		partitions_.emplace_back(size, name);
		return *this;
	}


	void printPartitions(std::ostream& os, const std::string& prefix) const {
		if (partitions_.empty()) return;

		for (size_t i = 0; i < partitions_.size() - 1; i++) {
			os << prefix << "+-[" << i << "]: ";
			os << partitions_[i].first << " GiB, " << partitions_[i].second << std::endl;
		}

		os << prefix << "\\-[" << partitions_.size() - 1 << "]: ";
		os << partitions_[partitions_.size() - 1].first << " GiB, " << partitions_[partitions_.size() - 1].second << std::endl;
	}


	CNetwork* findNetwork(const std::string& name) override {
		return nullptr;
	}


private:
	size_t type_;
	size_t size_;
	std::vector<std::pair<size_t, std::string>> partitions_;
};


class CCPU: public CComponent {
public:
	CCPU(size_t cores, size_t frequency)
		:	cores_(cores),
			frequency_(frequency) {}


	void print(std::ostream& os, const std::string& prefix, const std::string& begin) const override {
		os << prefix << begin << "CPU, " << cores_ << " cores @ " << frequency_ << "MHz" << std::endl;
	}


	std::unique_ptr<CComponent> clone() const override {
		return std::make_unique<CCPU>(*this);
	}


	CNetwork* findNetwork(const std::string& name) override {
		return nullptr;
	}


private:
	size_t cores_;
	size_t frequency_;
};



class CMemory: public CComponent {
public:
	CMemory(size_t size)
		:	size_(size) {}


	void print(std::ostream& os, const std::string& prefix, const std::string& begin) const override {
		os << prefix << begin << "Memory, " << size_ << " MiB" << std::endl;
	}


	std::unique_ptr<CComponent> clone() const override {
		return std::make_unique<CMemory>(*this);
	}


	CNetwork* findNetwork(const std::string& name) override {
		return nullptr;
	}


private:
	size_t size_;
};


class CNetwork;



class CComputer: public	CBase {
public:
	CComputer(const std::string& name)
		:	name_(name) {}


	CComputer(const CComputer& other)
		:	name_(other.name_),
			ips_(other.ips_) {
		for (auto& component: other.components_) {
			components_.push_back(component->clone());
		}
	}


	friend std::ostream& operator<<(std::ostream& os, const CComputer& computer) {
		os << "Host: " << computer.name_ << std::endl;
		computer.printIps(os, "");
		computer.printComponents(os, "");

		return os;
	}



	CComputer& operator=(const CComputer& other) {
		if (this == &other) return *this;

		name_ = other.name_;
		ips_ = other.ips_;

		components_.clear();


		for (auto& component: other.components_) {
			components_.push_back(component->clone());
		}

		return *this;
	}


	void print(std::ostream& os, const std::string& prefix, const std::string& begin) const override {
		os << prefix << begin << "Host: " << name_ << std::endl;
		const std::string childrenPrefix = prefix + (isLast(begin) ? "  " : "| ");
		printIps(os, childrenPrefix);
		printComponents(os, childrenPrefix);
	}


	void printIps(std::ostream& os, const std::string& prefix) const {
		if (ips_.empty()) return;

		for (size_t i = 0; i < ips_.size() - 1; i++) {
			os << prefix << "+-" << ips_[i] << std::endl;
		}

		os << prefix << (components_.empty() ? "\\-" : "+-") << ips_[ips_.size() - 1] << std::endl;
	}


	void printComponents(std::ostream& os, const std::string& prefix) const {
		if (components_.empty()) return;

		for (size_t i = 0; i < components_.size() - 1; i++) {
			components_[i]->print(os, prefix, "+-");
		}

		components_[components_.size() - 1]->print(os, prefix, "\\-");
	}


	CComputer& addAddress(const std::string& ip) {
		ips_.push_back(ip);
		return *this;
	}


	CComputer& addComponent(const CDisk& disk) {
		components_.push_back(disk.clone());
		return *this;
	}


	CComputer& addComponent(const CCPU& cpu) {
		components_.push_back(cpu.clone());
		return *this;
	}


	CComputer& addComponent(const CMemory& memory) {
		components_.push_back(memory.clone());
		return *this;
	}


	CComputer& addComponent(const CNetwork& network);


	CNetwork* findNetwork(const std::string& name) const;


	const std::string& name() const {
		return name_;
	}


private:
	std::string name_;
	std::vector<std::string> ips_;
	std::vector<std::unique_ptr<CComponent>> components_;
};




class CNetwork: public CComponent {
public:
	CNetwork(const std::string& name)
	:	name_(name) {}


	friend std::ostream& operator<<(std::ostream& os, const CNetwork& network) {
		os << "Network: " << network.name_ << std::endl;
		network.printComputers(os, "");

		return os;
	}


	void print(std::ostream& os, const std::string& prefix, const std::string& begin) const override {
		os << prefix << begin << "Network: " << name_ << std::endl;
		printComputers(os, prefix + (isLast(begin) ? "  " : "| "));
}


	void printComputers(std::ostream& os, const std::string& prefix) const {
		if (computers_.empty()) return;

		for (size_t i = 0; i < computers_.size() - 1; i++) {
			computers_[i].print(os, prefix, "+-");
		}

		computers_[computers_.size() - 1].print(os, prefix, "\\-");
	}


	CNetwork& addComputer(const CComputer& computer) {
		computers_.push_back(computer);
		return *this;
	}


	CComputer* findComputer(const std::string& name) {
		for (auto& computer: computers_) {
			if (computer.name() == name) return &computer;
		}

		return nullptr;
	}


	const std::string& name() const {
		return name_;
	}


	std::unique_ptr<CComponent> clone() const override {
		return std::make_unique<CNetwork>(*this);
	}


	CNetwork* findNetwork(const std::string& name) override {
		if (name == name_) return this;

		for (auto& computer: computers_) {
			auto* n = computer.findNetwork(name);
			if (n != nullptr) return n;
		}

		return nullptr;
	}


private:
	std::string name_;
	std::vector<CComputer> computers_;
};



CComputer& CComputer::addComponent(const CNetwork& network) {
	components_.push_back(network.clone());
	return *this;
}


CNetwork* CComputer::findNetwork(const std::string& name) const {
	for (auto& component: components_) {
		auto* n = component->findNetwork(name);
		if (n != nullptr) return n;
	}

	return nullptr;
}


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
	n.addComputer(
	CComputer("progtest.fit.cvut.cz")
	.addAddress("147.32.232.142")
	.addComponent(CCPU(8, 2400))
	.addComponent(CCPU(8, 1200))
	.addComponent(CDisk(CDisk::MAGNETIC, 1500)
		.addPartition(50, "/")
		.addPartition(5, "/boot")
		.addPartition(1000, "/var"))
	.addComponent(CDisk(CDisk::SSD, 60)
		.addPartition(60, "/data"))
	.addComponent(CMemory(2000))
	.addComponent(CMemory(2000)))
	.addComputer(
	CComputer("courses.fit.cvut.cz")
	.addAddress("147.32.232.213")
	.addComponent(CCPU(4, 1600))
	.addComponent(CMemory(4000))
	.addComponent(CDisk(CDisk::MAGNETIC, 2000)
		.addPartition(100, "/")
		.addPartition(1900, "/data")))
	.addComputer(
	CComputer("imap.fit.cvut.cz")
	.addAddress("147.32.232.238")
	.addComponent(CCPU(4, 2500))
	.addAddress("2001:718:2:2901::238")
	.addComponent(CMemory(8000)));
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
	auto c = x.findComputer("imap.fit.cvut.cz");
	assert ( toString ( *c ) ==
			 "Host: imap.fit.cvut.cz\n"
			 "+-147.32.232.238\n"
			 "+-2001:718:2:2901::238\n"
			 "+-CPU, 4 cores @ 2500MHz\n"
			 "\\-Memory, 8000 MiB\n" );

	c->addComponent(CDisk(CDisk::MAGNETIC, 1000)
		.addPartition(100, "system")
		.addPartition(200, "WWW")
		.addPartition(700, "mail"));
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

	n.addComputer(CComputer("vm01.fit.cvut.cz")
		.addAddress("147.32.232.232")
		.addComponent(CCPU(32, 4000))
		.addComponent(CMemory(32768))
		.addComponent(CDisk(CDisk::MAGNETIC, 3000)
			.addPartition(500, "SYSTEM")
			.addPartition(2000, "DATA")
			.addPartition(500, "BACKUP"))
		.addComponent(CNetwork("dummy00")
		.addComputer(CComputer("testing.fit.cvut.cz")
				 .addAddress("192.168.1.1")
				 .addComponent(CCPU(1, 300))
				 .addComponent(CMemory(256))))
		 .addComponent(CMemory(16384))
		 .addComponent(CNetwork("vnet00")));
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
			 "+-Host: imap.fit.cvut.cz\n"
			 "| +-147.32.232.238\n"
			 "| +-2001:718:2:2901::238\n"
			 "| +-CPU, 4 cores @ 2500MHz\n"
			 "| \\-Memory, 8000 MiB\n"
			 "\\-Host: vm01.fit.cvut.cz\n"
			 "  +-147.32.232.232\n"
			 "  +-CPU, 32 cores @ 4000MHz\n"
			 "  +-Memory, 32768 MiB\n"
			 "  +-HDD, 3000 GiB\n"
			 "  | +-[0]: 500 GiB, SYSTEM\n"
			 "  | +-[1]: 2000 GiB, DATA\n"
			 "  | \\-[2]: 500 GiB, BACKUP\n"
			 "  +-Network: dummy00\n"
			 "  | \\-Host: testing.fit.cvut.cz\n"
			 "  |   +-192.168.1.1\n"
			 "  |   +-CPU, 1 cores @ 300MHz\n"
			 "  |   \\-Memory, 256 MiB\n"
			 "  +-Memory, 16384 MiB\n"
			 "  \\-Network: vnet00\n" );
	auto vmnet = n . findNetwork ( "vnet00" );
	vmnet -> addComputer ( CComputer ( "monitor1.fit.cvut.cz" ) .
	addAddress ( "147.32.232.254" ) .
	addComponent ( CCPU ( 2, 1500 ) ) .
	addComponent ( CMemory ( 4096 ) ) .
	addComponent ( CDisk ( CDisk::MAGNETIC, 750 ) .
	addPartition ( 100, "root" ) .
	addPartition ( 600, "log" ) ) );
	std::cout << n << std::endl;
	assert ( toString ( *vmnet ) ==
			 "Network: vnet00\n"
			 "\\-Host: monitor1.fit.cvut.cz\n"
			 "  +-147.32.232.254\n"
			 "  +-CPU, 2 cores @ 1500MHz\n"
			 "  +-Memory, 4096 MiB\n"
			 "  \\-HDD, 750 GiB\n"
			 "    +-[0]: 100 GiB, root\n"
			 "    \\-[1]: 600 GiB, log\n" );
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
			 "+-Host: imap.fit.cvut.cz\n"
			 "| +-147.32.232.238\n"
			 "| +-2001:718:2:2901::238\n"
			 "| +-CPU, 4 cores @ 2500MHz\n"
			 "| \\-Memory, 8000 MiB\n"
			 "\\-Host: vm01.fit.cvut.cz\n"
			 "  +-147.32.232.232\n"
			 "  +-CPU, 32 cores @ 4000MHz\n"
			 "  +-Memory, 32768 MiB\n"
			 "  +-HDD, 3000 GiB\n"
			 "  | +-[0]: 500 GiB, SYSTEM\n"
			 "  | +-[1]: 2000 GiB, DATA\n"
			 "  | \\-[2]: 500 GiB, BACKUP\n"
			 "  +-Network: dummy00\n"
			 "  | \\-Host: testing.fit.cvut.cz\n"
			 "  |   +-192.168.1.1\n"
			 "  |   +-CPU, 1 cores @ 300MHz\n"
			 "  |   \\-Memory, 256 MiB\n"
			 "  +-Memory, 16384 MiB\n"
			 "  \\-Network: vnet00\n"
			 "    \\-Host: monitor1.fit.cvut.cz\n"
			 "      +-147.32.232.254\n"
			 "      +-CPU, 2 cores @ 1500MHz\n"
			 "      +-Memory, 4096 MiB\n"
			 "      \\-HDD, 750 GiB\n"
			 "        +-[0]: 100 GiB, root\n"
			 "        \\-[1]: 600 GiB, log\n" );
//	vmnet -> addComputer ( n . findComputer ( "monitor1.fit.cvut.cz" )
//						   -> duplicate ( { std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor2.fit.cvut.cz") } ) );
//	assert ( toString ( *vmnet ) ==
//			 "Network: vnet00\n"
//			 "+-Host: monitor1.fit.cvut.cz\n"
//			 "| +-147.32.232.254\n"
//			 "| +-CPU, 2 cores @ 1500MHz\n"
//			 "| +-Memory, 4096 MiB\n"
//			 "| \\-HDD, 750 GiB\n"
//			 "|   +-[0]: 100 GiB, root\n"
//			 "|   \\-[1]: 600 GiB, log\n"
//			 "\\-Host: monitor2.fit.cvut.cz\n"
//			 "  +-147.32.232.254\n"
//			 "  +-CPU, 2 cores @ 1500MHz\n"
//			 "  +-Memory, 4096 MiB\n"
//			 "  \\-HDD, 750 GiB\n"
//			 "    +-[0]: 100 GiB, root\n"
//			 "    \\-[1]: 600 GiB, log\n" );
//	n . addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
//					  -> duplicate (
//					  {
//					  std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor3.fit.cvut.cz"),
//					  std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor4.fit.cvut.cz"),
//					  std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm02.fit.cvut.cz")
//					  } ) );
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
//			 "+-Host: imap.fit.cvut.cz\n"
//			 "| +-147.32.232.238\n"
//			 "| +-2001:718:2:2901::238\n"
//			 "| +-CPU, 4 cores @ 2500MHz\n"
//			 "| \\-Memory, 8000 MiB\n"
//			 "+-Host: vm01.fit.cvut.cz\n"
//			 "| +-147.32.232.232\n"
//			 "| +-CPU, 32 cores @ 4000MHz\n"
//			 "| +-Memory, 32768 MiB\n"
//			 "| +-HDD, 3000 GiB\n"
//			 "| | +-[0]: 500 GiB, SYSTEM\n"
//			 "| | +-[1]: 2000 GiB, DATA\n"
//			 "| | \\-[2]: 500 GiB, BACKUP\n"
//			 "| +-Network: dummy00\n"
//			 "| | \\-Host: testing.fit.cvut.cz\n"
//			 "| |   +-192.168.1.1\n"
//			 "| |   +-CPU, 1 cores @ 300MHz\n"
//			 "| |   \\-Memory, 256 MiB\n"
//			 "| +-Memory, 16384 MiB\n"
//			 "| \\-Network: vnet00\n"
//			 "|   +-Host: monitor1.fit.cvut.cz\n"
//			 "|   | +-147.32.232.254\n"
//			 "|   | +-CPU, 2 cores @ 1500MHz\n"
//			 "|   | +-Memory, 4096 MiB\n"
//			 "|   | \\-HDD, 750 GiB\n"
//			 "|   |   +-[0]: 100 GiB, root\n"
//			 "|   |   \\-[1]: 600 GiB, log\n"
//			 "|   \\-Host: monitor2.fit.cvut.cz\n"
//			 "|     +-147.32.232.254\n"
//			 "|     +-CPU, 2 cores @ 1500MHz\n"
//			 "|     +-Memory, 4096 MiB\n"
//			 "|     \\-HDD, 750 GiB\n"
//			 "|       +-[0]: 100 GiB, root\n"
//			 "|       \\-[1]: 600 GiB, log\n"
//			 "\\-Host: vm02.fit.cvut.cz\n"
//			 "  +-147.32.232.232\n"
//			 "  +-CPU, 32 cores @ 4000MHz\n"
//			 "  +-Memory, 32768 MiB\n"
//			 "  +-HDD, 3000 GiB\n"
//			 "  | +-[0]: 500 GiB, SYSTEM\n"
//			 "  | +-[1]: 2000 GiB, DATA\n"
//			 "  | \\-[2]: 500 GiB, BACKUP\n"
//			 "  +-Network: dummy00\n"
//			 "  | \\-Host: testing.fit.cvut.cz\n"
//			 "  |   +-192.168.1.1\n"
//			 "  |   +-CPU, 1 cores @ 300MHz\n"
//			 "  |   \\-Memory, 256 MiB\n"
//			 "  +-Memory, 16384 MiB\n"
//			 "  \\-Network: vnet00\n"
//			 "    +-Host: monitor3.fit.cvut.cz\n"
//			 "    | +-147.32.232.254\n"
//			 "    | +-CPU, 2 cores @ 1500MHz\n"
//			 "    | +-Memory, 4096 MiB\n"
//			 "    | \\-HDD, 750 GiB\n"
//			 "    |   +-[0]: 100 GiB, root\n"
//			 "    |   \\-[1]: 600 GiB, log\n"
//			 "    \\-Host: monitor4.fit.cvut.cz\n"
//			 "      +-147.32.232.254\n"
//			 "      +-CPU, 2 cores @ 1500MHz\n"
//			 "      +-Memory, 4096 MiB\n"
//			 "      \\-HDD, 750 GiB\n"
//			 "        +-[0]: 100 GiB, root\n"
//			 "        \\-[1]: 600 GiB, log\n" );
//	vmnet -> addComputer ( n . findComputer ( "vm02.fit.cvut.cz" )
//						   -> duplicate (
//						   {
//						   std::pair<std::string,std::string>("monitor3.fit.cvut.cz","monitor5.fit.cvut.cz"),
//						   std::pair<std::string,std::string>("monitor4.fit.cvut.cz","monitor6.fit.cvut.cz"),
//						   std::pair<std::string,std::string>("vm02.fit.cvut.cz","vm03.fit.cvut.cz"),
//						   std::pair<std::string,std::string>("vnet00","vnet01")
//						   } ) );
//	assert ( toString ( *vmnet ) ==
//			 "Network: vnet00\n"
//			 "+-Host: monitor1.fit.cvut.cz\n"
//			 "| +-147.32.232.254\n"
//			 "| +-CPU, 2 cores @ 1500MHz\n"
//			 "| +-Memory, 4096 MiB\n"
//			 "| \\-HDD, 750 GiB\n"
//			 "|   +-[0]: 100 GiB, root\n"
//			 "|   \\-[1]: 600 GiB, log\n"
//			 "+-Host: monitor2.fit.cvut.cz\n"
//			 "| +-147.32.232.254\n"
//			 "| +-CPU, 2 cores @ 1500MHz\n"
//			 "| +-Memory, 4096 MiB\n"
//			 "| \\-HDD, 750 GiB\n"
//			 "|   +-[0]: 100 GiB, root\n"
//			 "|   \\-[1]: 600 GiB, log\n"
//			 "\\-Host: vm03.fit.cvut.cz\n"
//			 "  +-147.32.232.232\n"
//			 "  +-CPU, 32 cores @ 4000MHz\n"
//			 "  +-Memory, 32768 MiB\n"
//			 "  +-HDD, 3000 GiB\n"
//			 "  | +-[0]: 500 GiB, SYSTEM\n"
//			 "  | +-[1]: 2000 GiB, DATA\n"
//			 "  | \\-[2]: 500 GiB, BACKUP\n"
//			 "  +-Network: dummy00\n"
//			 "  | \\-Host: testing.fit.cvut.cz\n"
//			 "  |   +-192.168.1.1\n"
//			 "  |   +-CPU, 1 cores @ 300MHz\n"
//			 "  |   \\-Memory, 256 MiB\n"
//			 "  +-Memory, 16384 MiB\n"
//			 "  \\-Network: vnet01\n"
//			 "    +-Host: monitor5.fit.cvut.cz\n"
//			 "    | +-147.32.232.254\n"
//			 "    | +-CPU, 2 cores @ 1500MHz\n"
//			 "    | +-Memory, 4096 MiB\n"
//			 "    | \\-HDD, 750 GiB\n"
//			 "    |   +-[0]: 100 GiB, root\n"
//			 "    |   \\-[1]: 600 GiB, log\n"
//			 "    \\-Host: monitor6.fit.cvut.cz\n"
//			 "      +-147.32.232.254\n"
//			 "      +-CPU, 2 cores @ 1500MHz\n"
//			 "      +-Memory, 4096 MiB\n"
//			 "      \\-HDD, 750 GiB\n"
//			 "        +-[0]: 100 GiB, root\n"
//			 "        \\-[1]: 600 GiB, log\n" );
//	n . findComputer ( "vm02.fit.cvut.cz" )
//	-> findNetwork ( "vnet00" )
//	-> addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
//					 -> duplicate (
//					 {
//					 std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor11.fit.cvut.cz"),
//					 std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor12.fit.cvut.cz"),
//					 std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm11.fit.cvut.cz"),
//					 std::pair<std::string,std::string>("vm03.fit.cvut.cz","vm13.fit.cvut.cz"),
//					 std::pair<std::string,std::string>("vnet00","vnet10"),
//					 std::pair<std::string,std::string>("vnet01","vnet11")
//					 } ) );
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
//			 "+-Host: imap.fit.cvut.cz\n"
//			 "| +-147.32.232.238\n"
//			 "| +-2001:718:2:2901::238\n"
//			 "| +-CPU, 4 cores @ 2500MHz\n"
//			 "| \\-Memory, 8000 MiB\n"
//			 "+-Host: vm01.fit.cvut.cz\n"
//			 "| +-147.32.232.232\n"
//			 "| +-CPU, 32 cores @ 4000MHz\n"
//			 "| +-Memory, 32768 MiB\n"
//			 "| +-HDD, 3000 GiB\n"
//			 "| | +-[0]: 500 GiB, SYSTEM\n"
//			 "| | +-[1]: 2000 GiB, DATA\n"
//			 "| | \\-[2]: 500 GiB, BACKUP\n"
//			 "| +-Network: dummy00\n"
//			 "| | \\-Host: testing.fit.cvut.cz\n"
//			 "| |   +-192.168.1.1\n"
//			 "| |   +-CPU, 1 cores @ 300MHz\n"
//			 "| |   \\-Memory, 256 MiB\n"
//			 "| +-Memory, 16384 MiB\n"
//			 "| \\-Network: vnet00\n"
//			 "|   +-Host: monitor1.fit.cvut.cz\n"
//			 "|   | +-147.32.232.254\n"
//			 "|   | +-CPU, 2 cores @ 1500MHz\n"
//			 "|   | +-Memory, 4096 MiB\n"
//			 "|   | \\-HDD, 750 GiB\n"
//			 "|   |   +-[0]: 100 GiB, root\n"
//			 "|   |   \\-[1]: 600 GiB, log\n"
//			 "|   +-Host: monitor2.fit.cvut.cz\n"
//			 "|   | +-147.32.232.254\n"
//			 "|   | +-CPU, 2 cores @ 1500MHz\n"
//			 "|   | +-Memory, 4096 MiB\n"
//			 "|   | \\-HDD, 750 GiB\n"
//			 "|   |   +-[0]: 100 GiB, root\n"
//			 "|   |   \\-[1]: 600 GiB, log\n"
//			 "|   \\-Host: vm03.fit.cvut.cz\n"
//			 "|     +-147.32.232.232\n"
//			 "|     +-CPU, 32 cores @ 4000MHz\n"
//			 "|     +-Memory, 32768 MiB\n"
//			 "|     +-HDD, 3000 GiB\n"
//			 "|     | +-[0]: 500 GiB, SYSTEM\n"
//			 "|     | +-[1]: 2000 GiB, DATA\n"
//			 "|     | \\-[2]: 500 GiB, BACKUP\n"
//			 "|     +-Network: dummy00\n"
//			 "|     | \\-Host: testing.fit.cvut.cz\n"
//			 "|     |   +-192.168.1.1\n"
//			 "|     |   +-CPU, 1 cores @ 300MHz\n"
//			 "|     |   \\-Memory, 256 MiB\n"
//			 "|     +-Memory, 16384 MiB\n"
//			 "|     \\-Network: vnet01\n"
//			 "|       +-Host: monitor5.fit.cvut.cz\n"
//			 "|       | +-147.32.232.254\n"
//			 "|       | +-CPU, 2 cores @ 1500MHz\n"
//			 "|       | +-Memory, 4096 MiB\n"
//			 "|       | \\-HDD, 750 GiB\n"
//			 "|       |   +-[0]: 100 GiB, root\n"
//			 "|       |   \\-[1]: 600 GiB, log\n"
//			 "|       \\-Host: monitor6.fit.cvut.cz\n"
//			 "|         +-147.32.232.254\n"
//			 "|         +-CPU, 2 cores @ 1500MHz\n"
//			 "|         +-Memory, 4096 MiB\n"
//			 "|         \\-HDD, 750 GiB\n"
//			 "|           +-[0]: 100 GiB, root\n"
//			 "|           \\-[1]: 600 GiB, log\n"
//			 "\\-Host: vm02.fit.cvut.cz\n"
//			 "  +-147.32.232.232\n"
//			 "  +-CPU, 32 cores @ 4000MHz\n"
//			 "  +-Memory, 32768 MiB\n"
//			 "  +-HDD, 3000 GiB\n"
//			 "  | +-[0]: 500 GiB, SYSTEM\n"
//			 "  | +-[1]: 2000 GiB, DATA\n"
//			 "  | \\-[2]: 500 GiB, BACKUP\n"
//			 "  +-Network: dummy00\n"
//			 "  | \\-Host: testing.fit.cvut.cz\n"
//			 "  |   +-192.168.1.1\n"
//			 "  |   +-CPU, 1 cores @ 300MHz\n"
//			 "  |   \\-Memory, 256 MiB\n"
//			 "  +-Memory, 16384 MiB\n"
//			 "  \\-Network: vnet00\n"
//			 "    +-Host: monitor3.fit.cvut.cz\n"
//			 "    | +-147.32.232.254\n"
//			 "    | +-CPU, 2 cores @ 1500MHz\n"
//			 "    | +-Memory, 4096 MiB\n"
//			 "    | \\-HDD, 750 GiB\n"
//			 "    |   +-[0]: 100 GiB, root\n"
//			 "    |   \\-[1]: 600 GiB, log\n"
//			 "    +-Host: monitor4.fit.cvut.cz\n"
//			 "    | +-147.32.232.254\n"
//			 "    | +-CPU, 2 cores @ 1500MHz\n"
//			 "    | +-Memory, 4096 MiB\n"
//			 "    | \\-HDD, 750 GiB\n"
//			 "    |   +-[0]: 100 GiB, root\n"
//			 "    |   \\-[1]: 600 GiB, log\n"
//			 "    \\-Host: vm11.fit.cvut.cz\n"
//			 "      +-147.32.232.232\n"
//			 "      +-CPU, 32 cores @ 4000MHz\n"
//			 "      +-Memory, 32768 MiB\n"
//			 "      +-HDD, 3000 GiB\n"
//			 "      | +-[0]: 500 GiB, SYSTEM\n"
//			 "      | +-[1]: 2000 GiB, DATA\n"
//			 "      | \\-[2]: 500 GiB, BACKUP\n"
//			 "      +-Network: dummy00\n"
//			 "      | \\-Host: testing.fit.cvut.cz\n"
//			 "      |   +-192.168.1.1\n"
//			 "      |   +-CPU, 1 cores @ 300MHz\n"
//			 "      |   \\-Memory, 256 MiB\n"
//			 "      +-Memory, 16384 MiB\n"
//			 "      \\-Network: vnet10\n"
//			 "        +-Host: monitor11.fit.cvut.cz\n"
//			 "        | +-147.32.232.254\n"
//			 "        | +-CPU, 2 cores @ 1500MHz\n"
//			 "        | +-Memory, 4096 MiB\n"
//			 "        | \\-HDD, 750 GiB\n"
//			 "        |   +-[0]: 100 GiB, root\n"
//			 "        |   \\-[1]: 600 GiB, log\n"
//			 "        +-Host: monitor12.fit.cvut.cz\n"
//			 "        | +-147.32.232.254\n"
//			 "        | +-CPU, 2 cores @ 1500MHz\n"
//			 "        | +-Memory, 4096 MiB\n"
//			 "        | \\-HDD, 750 GiB\n"
//			 "        |   +-[0]: 100 GiB, root\n"
//			 "        |   \\-[1]: 600 GiB, log\n"
//			 "        \\-Host: vm13.fit.cvut.cz\n"
//			 "          +-147.32.232.232\n"
//			 "          +-CPU, 32 cores @ 4000MHz\n"
//			 "          +-Memory, 32768 MiB\n"
//			 "          +-HDD, 3000 GiB\n"
//			 "          | +-[0]: 500 GiB, SYSTEM\n"
//			 "          | +-[1]: 2000 GiB, DATA\n"
//			 "          | \\-[2]: 500 GiB, BACKUP\n"
//			 "          +-Network: dummy00\n"
//			 "          | \\-Host: testing.fit.cvut.cz\n"
//			 "          |   +-192.168.1.1\n"
//			 "          |   +-CPU, 1 cores @ 300MHz\n"
//			 "          |   \\-Memory, 256 MiB\n"
//			 "          +-Memory, 16384 MiB\n"
//			 "          \\-Network: vnet11\n"
//			 "            +-Host: monitor5.fit.cvut.cz\n"
//			 "            | +-147.32.232.254\n"
//			 "            | +-CPU, 2 cores @ 1500MHz\n"
//			 "            | +-Memory, 4096 MiB\n"
//			 "            | \\-HDD, 750 GiB\n"
//			 "            |   +-[0]: 100 GiB, root\n"
//			 "            |   \\-[1]: 600 GiB, log\n"
//			 "            \\-Host: monitor6.fit.cvut.cz\n"
//			 "              +-147.32.232.254\n"
//			 "              +-CPU, 2 cores @ 1500MHz\n"
//			 "              +-Memory, 4096 MiB\n"
//			 "              \\-HDD, 750 GiB\n"
//			 "                +-[0]: 100 GiB, root\n"
//			 "                \\-[1]: 600 GiB, log\n" );
	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */