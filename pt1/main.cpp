#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#endif /* __PROGTEST__ */
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>


class Land;



class Owner {
public:
    explicit Owner(const std::string &name) : name_(name) {}

    const std::string& getName() const {
        return name_;
    }


    const std::vector<std::shared_ptr<Land>> &getLands() const {
        return lands_;
    }


    size_t count() const {
        return lands_.size();
    }


    void add(const std::shared_ptr<Land>& land) {
        lands_.push_back(land);
    }


    void del(const std::shared_ptr<Land>& land) {
        auto it = std::find(lands_.begin(), lands_.end(), land);

        if (it != lands_.end()) lands_.erase(it);
    }


    bool operator == (const std::string& name) const {
        if (name_.size() != name.size()) return false;

        // case-insensitive char by char comparison
        for (size_t i = 0; i < name_.size(); i++) {
            if (tolower(name_[i]) != tolower(name[i])) return false;
        }

        return true;
    }


private:
    const std::string name_;
    std::vector<std::shared_ptr<Land>> lands_;
};



class Land {
public:
    Land(
            const std::string &city,
            const std::string &addr,
            const std::string &region,
            unsigned int id,
            const std::string& ownerName
    )   :   city_(city),
            addr_(addr),
            region_(region),
            id_(id),
            ownerName_(ownerName) {}

    const std::string &getCity() const {
        return city_;
    }


    const std::string &getAddr() const {
        return addr_;
    }


    const std::string &getRegion() const {
        return region_;
    }


    unsigned int getId() const {
        return id_;
    }


    const std::string &getOwnerName() const {
        return ownerName_;
    }

    void setOwnerName(const std::string &ownerName) {
        Land::ownerName_ = ownerName;
    }


private:
    const std::string city_;
    const std::string addr_;
    const std::string region_;
    unsigned int id_;
    std::string ownerName_;
};


class CIterator {
public:
    CIterator()
            :   index_(0) {}


    CIterator(const std::vector<std::shared_ptr<Land>> lands)
            : lands_(lands), index_(0) {}

    bool atEnd() const {
        return index_ == lands_.size();
    }

    void next() {
        ++index_;
    }

    std::string city() const {
        return lands_[index_]->getCity();
    }

    std::string addr() const {
        return lands_[index_]->getAddr();
    }

    std::string region() const {
        return lands_[index_]->getRegion();
    }

    unsigned id() const {
        return lands_[index_]->getId();
    }

    std::string owner() const {
        return lands_[index_]->getOwnerName();
    }

private:
    const std::vector<std::shared_ptr<Land>> lands_;
    size_t index_;
};


class CLandRegister {
public:
    // lands have no owner ("") when they are created
    CLandRegister() {
        noOwner_ = owners_.emplace_back(std::make_shared<Owner>(""));
    }

    bool add(
            const std::string& city,
            const std::string& addr,
            const std::string& region,
            unsigned id
    ) {
        // lands must be unique (city, addr)
        auto p = std::make_pair(city, addr);
        auto it = std::lower_bound(landsCa_.begin(), landsCa_.end(), p, comparePairCa);

        if (it != landsCa_.end() && (*it)->getCity() == city && (*it)->getAddr() == addr) return false;

        // also (region, id)
        auto p2 = std::make_pair(region, id);
        auto it2 = std::lower_bound(landsRi_.begin(), landsRi_.end(), p2, comparePairRi);

        if (it2 != landsRi_.end() && (*it2)->getRegion() == region && (*it2)->getId() == id) return false;

        // new land -> no owner ("")
        auto land = std::make_shared<Land>(city, addr, region, id, "");
        noOwner_->add(land);

        // adding the land to vectors sorted by (city, addr) and (region, id)
        landsCa_.insert(it, land);
        landsRi_.insert(it2, land);

        return true;
    }


    bool del(
            const std::string& city,
            const std::string& addr
    ) {
        // finding the land by city and addr
        auto p = std::make_pair(city, addr);
        auto itCa = std::lower_bound(landsCa_.begin(), landsCa_.end(), p, comparePairCa);

        // there is no such land
        if (itCa == landsCa_.end()) return false;

        // there is no such land
        auto land = *itCa;
        if (land->getCity() != city || land->getAddr() != addr) return false;

        auto it = findOwner(land->getOwnerName());
        (*it)->del(land);
        landsCa_.erase(itCa);

        // now removing it from the other vector
        auto p2 = std::make_pair(land->getRegion(), land->getId());
        auto itRi = std::lower_bound(landsRi_.begin(), landsRi_.end(), p2, comparePairRi);

        if (itRi == landsRi_.end()) return false;
        landsRi_.erase(itRi);


        return true;
    }


    bool del(
            const std::string& region,
            unsigned id
    ) {
        auto p = std::make_pair(region, id);
        auto itRi = std::lower_bound(landsRi_.begin(), landsRi_.end(), p, comparePairRi);

        if (itRi == landsRi_.end()) return false;


        auto land = *itRi;

        if (land->getRegion() != region || land->getId() != id) return false;

        auto it = findOwner(land->getOwnerName());
        (*it)->del(land);

        landsRi_.erase(itRi);
        auto p2 = std::make_pair(land->getCity(), land->getAddr());
        auto itCa = std::lower_bound(landsCa_.begin(), landsCa_.end(), p2, comparePairCa);

        if (itCa == landsCa_.end()) return false;
        landsCa_.erase(itCa);


        return true;
    }


    bool newOwner(
            const std::string& city,
            const std::string& addr,
            const std::string& owner
    ) {
        // there is no such land
        auto p = std::make_pair(city, addr);
        auto it = std::lower_bound(landsCa_.begin(), landsCa_.end(), p, comparePairCa);

        if (it == landsCa_.end()) return false;

        auto land = *it;

        if (land->getCity() != city || land->getAddr() != addr) return false;

        if (land->getOwnerName() == owner) return false;

        auto itO = findOwner(land->getOwnerName());
        (*itO)->del(land);

        auto ownerIt = findOwner(owner);
        std::shared_ptr<Owner> targetOwner;

        if (ownerIt == owners_.end()) {
            targetOwner = owners_.emplace_back(std::make_shared<Owner>(owner));
        } else {
            targetOwner = *ownerIt;
        }

        targetOwner->add(land);
        land->setOwnerName(owner);

        return true;
    }


    bool newOwner(
            const std::string& region,
            unsigned id,
            const std::string& owner
    ) {
        auto p = std::make_pair(region, id);
        auto it = std::lower_bound(landsRi_.begin(), landsRi_.end(), p, comparePairRi);

        if (it == landsRi_.end()) return false;

        auto land = *it;

        if (land->getRegion() != region && land->getId() != id) return false;

        if (land->getOwnerName() == owner) return false;

        auto itO = findOwner(land->getOwnerName());
        (*itO)->del(land);

        auto ownerIt = findOwner(owner);
        std::shared_ptr<Owner> targetOwner;

        if (ownerIt == owners_.end()) {
            // there is no such owner (doesn't own any lands)
            // creating a new owner
            targetOwner = owners_.emplace_back(std::make_shared<Owner>(owner));
        } else {
            targetOwner = *ownerIt;
        }

        targetOwner->add(land);
        land->setOwnerName(owner);

        return true;
    }


    bool getOwner(
            const std::string& city,
            const std::string& addr,
            std::string& owner
    ) const {

        auto p = std::make_pair(city, addr);
        auto it = std::lower_bound(landsCa_.begin(), landsCa_.end(), p, comparePairCa);

        if (it == landsCa_.end()) return false;

        auto land = *it;

        if (land->getCity() != city || land->getAddr() != addr) return false;

        owner = land->getOwnerName();

        return true;
    }


    bool getOwner(
            const std::string& region,
            unsigned id,
            std::string& owner
    ) const {
        auto p = std::make_pair(region, id);
        auto it = std::lower_bound(landsRi_.begin(), landsRi_.end(), p, comparePairRi);

        if (it == landsRi_.end()) return false;

        auto land = *it;

        if (land->getRegion() != region || land->getId() != id) return false;

        owner = land->getOwnerName();

        return true;
    }


    size_t count(const std::string& owner) const {
        for (auto& o: owners_) {
            if (*o == owner) return o->count();
        }

        return 0;
    }


    CIterator listByAddr() const {
        return landsCa_;
    }


    CIterator listByOwner(const std::string& owner) const {
        for (auto& o: owners_) {
            if (*o == owner) return o->getLands();
        }

        return {};
    }


private:
    static bool compareCa(
            const std::shared_ptr<Land>& lhs,
            const std::shared_ptr<Land>& rhs
    ) {
        if (lhs->getCity() < rhs->getCity()) return true;
        if (lhs->getCity() > rhs->getCity()) return false;

        if (lhs->getAddr() < rhs->getAddr()) return true;
        if (lhs->getAddr() > rhs->getAddr()) return false;

        return false;
    }


    static bool compareRi(
            const std::shared_ptr<Land>& lhs,
            const std::shared_ptr<Land>& rhs
    ) {
        if (lhs->getRegion() < rhs->getRegion()) return true;
        if (lhs->getRegion() > rhs->getRegion()) return false;

        if (lhs->getId() < rhs->getId()) return true;
        if (lhs->getId() > rhs->getId()) return false;

        return false;
    }


    static bool comparePairCa(
            const std::shared_ptr<Land>& land,
            const std::pair<std::string, std::string> cityAddr
    ) {
        if (land->getCity() < cityAddr.first) return true;
        if (land->getCity() > cityAddr.first) return false;

        if (land->getAddr() < cityAddr.second) return true;
        if (land->getAddr() > cityAddr.second) return false;

        return false;
    }


    static bool comparePairRi(
            const std::shared_ptr<Land>& land,
            const std::pair<std::string, unsigned> regionId
    ) {
        if (land->getRegion() < regionId.first) return true;
        if (land->getRegion() > regionId.first) return false;

        if (land->getId() < regionId.second) return true;
        if (land->getId() > regionId.second) return false;

        return false;
    }


    std::shared_ptr<Land> find(
            const std::string& city,
            const std::string& addr
    ) const {
        auto p = std::make_pair(city, addr);
        auto it = std::lower_bound(landsCa_.begin(), landsCa_.end(), p, comparePairCa);
        return (it != landsCa_.end()) ? *it : nullptr;
    }


    std::shared_ptr<Land> find(
            const std::string& region,
            unsigned id
    ) const {
        auto p = std::make_pair(region, id);
        auto it = std::lower_bound(landsRi_.begin(), landsRi_.end(), p, comparePairRi);
        return (it != landsRi_.end()) ? *it : nullptr;
    }

    std::vector<std::shared_ptr<Owner>>::iterator findOwner(const std::string& owner) {
        auto it = owners_.begin();

        for (auto& o: owners_) {
            if (*o == owner) break;
            it++;
        }

        return it;
    }

    std::vector<std::shared_ptr<Land>> landsCa_;
    std::vector<std::shared_ptr<Land>> landsRi_;
    std::vector<std::shared_ptr<Owner>> owners_;
    std::shared_ptr<Owner> noOwner_;
};



#ifndef __PROGTEST__
static void test0 ()
{
    CLandRegister x;
    std::string owner;

    assert ( x . add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( x . add ( "Plzen", "Evropska", "Plzen mesto", 78901 ) );
    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
    CIterator i0 = x . listByAddr ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Liberec"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Librec"
             && i0 . id () == 4552
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Plzen"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Plzen mesto"
             && i0 . id () == 78901
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Vokovice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Technicka"
             && i0 . region () == "Dejvice"
             && i0 . id () == 9873
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Thakurova"
             && i0 . region () == "Dejvice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( i0 . atEnd () );

    assert ( x . count ( "" ) == 5 );
    CIterator i1 = x . listByOwner ( "" );
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Thakurova"
             && i1 . region () == "Dejvice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Vokovice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Technicka"
             && i1 . region () == "Dejvice"
             && i1 . id () == 9873
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Plzen"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Plzen mesto"
             && i1 . id () == 78901
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Liberec"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Librec"
             && i1 . id () == 4552
             && i1 . owner () == "" );
    i1 . next ();
    assert ( i1 . atEnd () );

    assert ( x . count ( "CVUT" ) == 0 );
    CIterator i2 = x . listByOwner ( "CVUT" );
    assert ( i2 . atEnd () );

    assert ( x . newOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( x . newOwner ( "Dejvice", 9873, "CVUT" ) );
    assert ( x . newOwner ( "Plzen", "Evropska", "Anton Hrabis" ) );
    assert ( x . newOwner ( "Librec", 4552, "Cvut" ) );
    assert ( x . getOwner ( "Prague", "Thakurova", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 12345, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Prague", "Evropska", owner ) && owner == "" );
    assert ( x . getOwner ( "Vokovice", 12345, owner ) && owner == "" );
    assert ( x . getOwner ( "Prague", "Technicka", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 9873, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Plzen", "Evropska", owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Plzen mesto", 78901, owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Liberec", "Evropska", owner ) && owner == "Cvut" );
    assert ( x . getOwner ( "Librec", 4552, owner ) && owner == "Cvut" );
    CIterator i3 = x . listByAddr ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Liberec"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Librec"
             && i3 . id () == 4552
             && i3 . owner () == "Cvut" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Plzen"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Plzen mesto"
             && i3 . id () == 78901
             && i3 . owner () == "Anton Hrabis" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Vokovice"
             && i3 . id () == 12345
             && i3 . owner () == "" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Technicka"
             && i3 . region () == "Dejvice"
             && i3 . id () == 9873
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Thakurova"
             && i3 . region () == "Dejvice"
             && i3 . id () == 12345
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( i3 . atEnd () );

    assert ( x . count ( "cvut" ) == 3 );
    CIterator i4 = x . listByOwner ( "cVuT" );
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Thakurova"
             && i4 . region () == "Dejvice"
             && i4 . id () == 12345
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Technicka"
             && i4 . region () == "Dejvice"
             && i4 . id () == 9873
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Liberec"
             && i4 . addr () == "Evropska"
             && i4 . region () == "Librec"
             && i4 . id () == 4552
             && i4 . owner () == "Cvut" );
    i4 . next ();
    assert ( i4 . atEnd () );

    assert ( x . newOwner ( "Plzen mesto", 78901, "CVut" ) );
    assert ( x . count ( "CVUT" ) == 4 );
    CIterator i5 = x . listByOwner ( "CVUT" );
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Thakurova"
             && i5 . region () == "Dejvice"
             && i5 . id () == 12345
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Technicka"
             && i5 . region () == "Dejvice"
             && i5 . id () == 9873
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Liberec"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Librec"
             && i5 . id () == 4552
             && i5 . owner () == "Cvut" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Plzen"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Plzen mesto"
             && i5 . id () == 78901
             && i5 . owner () == "CVut" );
    i5 . next ();
    assert ( i5 . atEnd () );

    assert ( x . del ( "Liberec", "Evropska" ) );
    assert ( x . del ( "Plzen mesto", 78901 ) );
    assert ( x . count ( "cvut" ) == 2 );
    CIterator i6 = x . listByOwner ( "cVuT" );
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Thakurova"
             && i6 . region () == "Dejvice"
             && i6 . id () == 12345
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Technicka"
             && i6 . region () == "Dejvice"
             && i6 . id () == 9873
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( i6 . atEnd () );

    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
}

static void test1 ()
{
    CLandRegister x;
    std::string owner;

    assert ( x . add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( ! x . add ( "Prague", "Technicka", "Hradcany", 7344 ) );
    assert ( ! x . add ( "Brno", "Bozetechova", "Dejvice", 9873 ) );
    assert ( !x . getOwner ( "Prague", "THAKUROVA", owner ) );
    assert ( !x . getOwner ( "Hradcany", 7343, owner ) );
    CIterator i0 = x . listByAddr ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Vokovice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Technicka"
             && i0 . region () == "Dejvice"
             && i0 . id () == 9873
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Thakurova"
             && i0 . region () == "Dejvice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( i0 . atEnd () );

    assert ( x . newOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( ! x . newOwner ( "Prague", "technicka", "CVUT" ) );
    assert ( ! x . newOwner ( "prague", "Technicka", "CVUT" ) );
    assert ( ! x . newOwner ( "dejvice", 9873, "CVUT" ) );
    assert ( ! x . newOwner ( "Dejvice", 9973, "CVUT" ) );
    assert ( ! x . newOwner ( "Dejvice", 12345, "CVUT" ) );
    assert ( x . count ( "CVUT" ) == 1 );
    CIterator i1 = x . listByOwner ( "CVUT" );
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Thakurova"
             && i1 . region () == "Dejvice"
             && i1 . id () == 12345
             && i1 . owner () == "CVUT" );
    i1 . next ();
    assert ( i1 . atEnd () );

    assert ( ! x . del ( "Brno", "Technicka" ) );
    assert ( ! x . del ( "Karlin", 9873 ) );
    assert ( x . del ( "Prague", "Technicka" ) );
    assert ( ! x . del ( "Prague", "Technicka" ) );
    assert ( ! x . del ( "Dejvice", 9873 ) );
}


static void myTest() {
    CLandRegister x;
    std::string owner;

    assert(x.add("Prague", "Thakurova", "Dejvice", 5432));
    assert(!x.add("Prague", "Thakurova", "Dejvice", 5432));
    assert(x.add("Prague", "Na Dolinach", "Podoli", 5432));
    assert(!x.newOwner("Prague", "Thakurova", ""));
}

int main ( void )
{
    myTest();
    test0 ();
    test1 ();
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */