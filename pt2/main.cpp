#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <compare>
#include <stdexcept>
#endif /* __PROGTEST__ */

//class CBigInt
//{
//public:
    // default constructor
    // copying/assignment/destruction
    // int constructor
    // string constructor
    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    // operator +=, any of {CBigInt/int/string}
    // operator *=, any of {CBigInt/int/string}
    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    // output operator <<
    // input operator >>
//private:
//};


class CBigInt {
public:
    CBigInt()
        : sign(false),
          value("0") {}

    CBigInt(int value_)
        : sign(value_ < 0),
          value(std::to_string(abs(value_))) {}

    CBigInt(const char* val) {
        // empty string
        if (*val == '\0') throw std::invalid_argument("Value is not a valid number");

        bool tempSign = false;
        std::string tempValue;

        // negative value
        if (*val == '-') {
            tempSign = true;
            ++val;
        }


        bool leadingZeros = true;
        size_t start_i = tempSign ? 1 : 0;

        for (; *val != '\0'; ++val) {
            // invalid character
            if (!isdigit(*val)) throw std::invalid_argument("Value is not a valid number");

            // first nonzero digit
            if (leadingZeros && *val != '0') leadingZeros = false;

            // skip leading zeros
            if (leadingZeros) continue;

            tempValue += *val;
        }


        // string is only 0's
        if (leadingZeros) {
            sign = false;
            value = "0";
        } else {
            sign = tempSign;
            value = tempValue;
        }

    }


    friend std::istream& operator>>(std::istream& is, CBigInt& num) {
        // empty string is 0
        if (is.peek() == EOF or is.peek() == 0) {
            is.setstate(std::ios::failbit);
            return is;
        }

        bool tempSign = false;
        std::string tempValue;
        char c;

        // negative number
        if (is.peek() == '-') {
            tempSign = true;
            is >> c;
        }


        bool leadingZeros = true;

        while (is.get(c)) {
            // invalid character
            if (!isdigit(c)) {
                // fails is no digit read
                if (tempValue.empty()) {
                    is.setstate(std::ios::failbit);
                    return is;
                }

                // success if at least one digit read
                break;
            }


            // first nonzero character
            if (leadingZeros && c != '0') leadingZeros = false;

            // skip leading zeros
            if (leadingZeros) continue;

            tempValue += c;
        }


        // only 0's on input
        if (leadingZeros) {
            num.sign = false;
            num.value = "0";
        } else {
            num.sign = tempSign;
            num.value = tempValue;
        }

        is.clear();
        return is;
    }


    friend std::ostream& operator<<(std::ostream& os, const CBigInt& num) {
        // negative value
        if (num.sign) {
            os << '-';
        }

        os << num.value;
        return os;
    }

    CBigInt& operator= (const CBigInt& other) {
        if (this != &other) {
            this->sign = other.sign;
            this->value = other.value;
        }

        return *this;
    }


    bool operator<(const CBigInt& other) const {
        // negative is smaller than positive
        if (sign && !other.sign) return true;
        if (!sign && other.sign) return false;

        // signs are the same

        // different sizes means different exponent

        // negatives
        if (sign) {
            // bigger exponent means smaller number
            if (value.size() > other.value.size()) return true;
            if (value.size() < other.value.size()) return false;
        // positives
        } else {
            // bigger exponent means bigger number
            if (value.size() < other.value.size()) return true;
            if (value.size() > other.value.size()) return false;
        }

        // same magnitude
        // compare coefficients for each exponent
        for (size_t i = 0; i < value.size(); i++) {
            // negatives
            if (sign) {
                // bigger coefficient means smaller number
                if (value[i] > other.value[i]) return true;
                if (value[i] < other.value[i]) return false;
            // positives
            } else {
                // smaller coefficient means smaller number
                if (value[i] < other.value[i]) return true;
                if (value[i] > other.value[i]) return false;
            }
        }

        return false;
    }


    bool operator== (const CBigInt& other) const {
        return !(*this < other) && !(other < *this);
    }


    bool operator> (const CBigInt& other) const {
        return !(*this < other) && !(*this == other);
    }


    bool operator <= (const CBigInt& other) const {
        return (*this < other) || (*this == other);
    }


    bool operator >= (const CBigInt& other) const {
        return (*this == other) || (*this > other);
    }


private:
    // if sign the value is negative
    bool sign;
    std::string value;
};

#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char val [] )
{
    std::ostringstream oss;
    oss << x;
    return oss . str () == val;
}
//static bool equalHex ( const CBigInt & x, const char value [] )
//{
//    return true; // hex output is needed for bonus tests only
//    // std::ostringstream oss;
//    // oss << std::hex << x;
//    // return oss . str () == value;
//}
int main ()
{
//    CBigInt a, b; std::istringstream is; a = 10; a += 20; assert ( equal ( a, "30" ) ); a *= 5; assert ( equal ( a, "150" ) ); b = a + 3;
//    assert ( equal ( b, "153" ) );
//    b = a * 7;
//    assert ( equal ( b, "1050" ) );
//    assert ( equal ( a, "150" ) );
//    assert ( equalHex ( a, "96" ) );
//
//    a = 10;
//    a += -20;
//    assert ( equal ( a, "-10" ) );
//    a *= 5;
//    assert ( equal ( a, "-50" ) );
//    b = a + 73;
//    assert ( equal ( b, "23" ) );
//    b = a * -7;
//    assert ( equal ( b, "350" ) );
//    assert ( equal ( a, "-50" ) );
//    assert ( equalHex ( a, "-32" ) );
//
//    a = "12345678901234567890";
//    a += "-99999999999999999999";
//    assert ( equal ( a, "-87654321098765432109" ) );
//    a *= "54321987654321987654";
//    assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
//    a *= 0;
//    assert ( equal ( a, "0" ) );
//    a = 10;
//    b = a + "400";
//    assert ( equal ( b, "410" ) );
//    b = a * "15";
//    assert ( equal ( b, "150" ) );
//    assert ( equal ( a, "10" ) );
//    assert ( equalHex ( a, "a" ) );
//
//    is . clear ();
//    is . str ( " 1234" );
//    assert ( is >> b );
//    assert ( equal ( b, "1234" ) );
//    is . clear ();
//    is . str ( " 12 34" );
//    assert ( is >> b );
//    assert ( equal ( b, "12" ) );
//    is . clear ();
//    is . str ( "999z" );
//    assert ( is >> b );
//    assert ( equal ( b, "999" ) );
//    is . clear ();
//    is . str ( "abcd" );
//    assert ( ! ( is >> b ) );
//    is . clear ();
//    is . str ( "- 758" );
//    assert ( ! ( is >> b ) );
//    a = 42;
//    try
//    {
//        a = "-xyz";
//        assert ( "missing an exception" == nullptr );
//    }
//    catch ( const std::invalid_argument & e )
//    {
//        assert ( equal ( a, "42" ) );
//    }
//
//    a = "73786976294838206464";
//    assert ( equal ( a, "73786976294838206464" ) );
//    assert ( equalHex ( a, "40000000000000000" ) );
//    assert ( a < "1361129467683753853853498429727072845824" );
//    assert ( a <= "1361129467683753853853498429727072845824" );
//    assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
//    assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
//    assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
//    assert ( a != "1361129467683753853853498429727072845824" );
//    assert ( ! ( a < "73786976294838206464" ) );
//    assert ( a <= "73786976294838206464" );
//    assert ( ! ( a > "73786976294838206464" ) );
//    assert ( a >= "73786976294838206464" );
//    assert ( a == "73786976294838206464" );
//    assert ( ! ( a != "73786976294838206464" ) );
//    assert ( a < "73786976294838206465" );
//    assert ( a <= "73786976294838206465" );
//    assert ( ! ( a > "73786976294838206465" ) );
//    assert ( ! ( a >= "73786976294838206465" ) );
//    assert ( ! ( a == "73786976294838206465" ) );
//    assert ( a != "73786976294838206465" );
//    a = "2147483648";
//    assert ( ! ( a < -2147483648 ) );
//    assert ( ! ( a <= -2147483648 ) );
//    assert ( a > -2147483648 );
//    assert ( a >= -2147483648 );
//    assert ( ! ( a == -2147483648 ) );
//    assert ( a != -2147483648 );
//    a = "-12345678";
//    assert ( ! ( a < -87654321 ) );
//    assert ( ! ( a <= -87654321 ) );
//    assert ( a > -87654321 );
//    assert ( a >= -87654321 );
//    assert ( ! ( a == -87654321 ) );
//    assert ( a != -87654321 );
//
//    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */