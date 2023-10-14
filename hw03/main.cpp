#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>

using namespace std;
#endif /* __PROGTEST__ */

class CRange{
public:
    CRange(long long lo, long long hi) : low(lo), high(hi){
        if (lo > hi) throw logic_error("bleble");
    }
    long long low, high;
};

class CRangeList{
public:
    bool includes(const long long num) const {
        auto it = lower_bound(ranges.begin(), ranges.end(), num, [](const CRange& r, long long num) { return r.high < num;});
        return (it != ranges.end() && num >= it->low && num <= it->high);
    }
    bool includes(const CRange& range) const {
        auto it = lower_bound(ranges.begin(), ranges.end(), range.high, [](const CRange& r, long long num) { return r.high < num;});
        return (it != ranges.end() && range.low >= it->low && range.high <= it->high);
    }

    bool operator==(const CRangeList& other) const{
        return equal(ranges.begin(), ranges.end(), other.ranges.begin(), other.ranges.end(), [](const CRange& r1, const CRange& r2) {return r1.low == r2.low && r1.high == r2.high;});
    }
    bool operator!=(const CRangeList& other) const{
        return !(*this == other);
    }

    CRangeList& operator=(const CRangeList& other){
        this->ranges.assign(other.ranges.begin(), other.ranges.end());
        return *this;
    }
    CRangeList& operator=(const CRange& other){
        ranges.assign(1, other);
        return *this;
    }

    CRangeList& operator+=(const CRange& new_range) {
        auto current_range = lower_bound(ranges.begin(), ranges.end(), new_range, [](const CRange& r1, const CRange& r2) {return r1.low < r2.low;});
        current_range = ranges.insert(current_range, new_range);
        if (current_range != ranges.begin()) --current_range; //predchozi range muze byt soused, proto jeste minus jedna
        while(current_range != ranges.end() - 1) {
            if (current_range->high >= (current_range + 1)->low) { //overlapy/include -> merge
                current_range->high = max(current_range->high, (current_range + 1)->high);
                current_range = ranges.erase(current_range + 1) - 2; //erase vraci iterator na dalsi prvek, proto -1 a -1 proto, protoze jeden prvek mazeme a v dalsi iteraci chceme zustat na stejnem prvku
            }else if (current_range->high + 1 == (current_range + 1)->low) { //sousedi -> merge
                current_range->high = (current_range + 1)->high;
                current_range = ranges.erase(current_range + 1) - 2;
            }
            ++current_range;
        }
        return *this;
    } //mohl bych pridat upper a pak to najednoud deletnout ty prekryvajici se intervaly, ale az se mi bude chtit a bude cas or else

    CRangeList& operator-=(const CRange& remove_range) {
        auto current_range = lower_bound(ranges.begin(), ranges.end(), remove_range, [](const CRange& r1, const CRange& r2) {return r1.low < r2.low;});
        if (current_range != ranges.begin()) --current_range;
        while (current_range != ranges.end()) {
            if (current_range->low > remove_range.high) break;
            else if (current_range->high < remove_range.low) ++current_range;
            else if (current_range->low < remove_range.low && current_range->high > remove_range.high) { //remove_range je uprostred aktualniho intervalu
                auto high = current_range->high;
                current_range->high = remove_range.low - 1;
                ranges.insert(++current_range, CRange(remove_range.high + 1, high));
                break;
            }else if (current_range->low < remove_range.low && current_range->high >= remove_range.low) { // remove_range overlapuje doprava (zhora)
                current_range->high = remove_range.low - 1;
                ++current_range;
            }else if (current_range->low <= remove_range.high && current_range->high > remove_range.high) {// remove_range overlapuje doleva (odspodu)
                current_range->low = remove_range.high + 1;
                break;
            }else current_range = ranges.erase(current_range);
        }
        return *this;
    }

    CRangeList& operator+=(const CRangeList& new_range_list) {
        for (const auto& range : new_range_list.ranges) *this += range;
        return *this;
    }
    CRangeList& operator-=(const CRangeList& new_range) {
        for (const auto& range : new_range.ranges) *this -= range;
        return *this;
    }

    CRangeList& operator+(CRangeList& new_range_list){
        for (const CRange& range : new_range_list.ranges) *this += range;
        return *this;
    }
    CRangeList& operator-(CRangeList& new_range_list){
        for (const CRange& range : new_range_list.ranges) *this -= range;
        return *this;
    }

    CRangeList& operator+(const CRange& new_range){
        *this += new_range;
        return *this;
    }
    CRangeList& operator-(const CRange& remove_range){
        *this -= remove_range;
        return *this;
    }

    friend ostream& operator<<(ostream& os, const CRangeList& list){
        os << "{";
        auto begin = list.ranges.begin();
        auto it = begin;
        for (; it != list.ranges.end(); ++it){
            if (it != begin) os << ",";
            if (it->low == it->high) os << it->low;
            else os << "<" << it->low << ".." << it->high << ">";
        }
        os << "}";
        return os;
    }
private:
    vector<CRange> ranges;
};

CRangeList operator-(const CRange& left, const CRange& right){
    CRangeList list;
    list=left;
    list-=right;
    return list;
}
CRangeList operator+(const CRange& left, const CRange& right) {
    CRangeList list;
    list=left;
    list+=right;
    return list;
}


#ifndef __PROGTEST__

string toString(const CRangeList& list) {
    ostringstream out;
    out << list;
    cout << out.str() << endl;
    return out.str();
}

void test0 ()
{
    CRangeList a;
    a += CRange(1,10);
    CRange b(10,10);
    assert(a.includes(b));
}
void test1 ()
{
    CRangeList a;
    a += CRange(-10000, 10000);
    a += CRange(10000000, 1000000000);
    a += CRange(LLONG_MAX - 1, LLONG_MAX);
    assert(toString(a) == "{<-10000..10000>,<10000000..1000000000>,<9223372036854775806..9223372036854775807>}");
    a += CRange(LLONG_MAX, LLONG_MAX) + CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a -= a;
    assert(toString(a) == "{}");
    a += CRange(-10000, 10000) + CRange(10000000, 1000000000) + CRange(LLONG_MIN, LLONG_MIN + 1);
    assert(toString(a) == "{<-9223372036854775808..-9223372036854775807>,<-10000..10000>,<10000000..1000000000>}");
    a += CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
}
void test2 ()
{
    CRangeList a;
    a = CRange(10, 10) + CRange(20, 20) + CRange(12, 12) + CRange(18, 18);
    assert ( toString ( a ) == "{10,12,18,20}" );
    a -= CRange(11, 19);
    assert ( toString ( a ) == "{10,20}" );
    a = CRange(10, 10) + CRange(20, 20) + CRange(12, 12) + CRange(18, 18);
    assert ( toString ( a ) == "{10,12,18,20}" );
    a -= CRange(10, 20);
    assert ( toString ( a ) == "{}" );
    a = CRange(10, 100);
    a -= CRange(20, 80);
    assert(toString(a) == "{<10..19>,<81..100>}");
    a = CRange(10, 100);
    a -= CRange(11, 99);
    assert(toString(a) == "{10,100}");
    a = CRange(10, 100);
    a -= CRange(11, 101);
    assert(toString(a) == "{10}");
    a = CRange(10, 100);
    a -= CRange(50, 150);
    assert(toString(a) == "{<10..49>}");
    a = CRange(10, 100);
    a -= CRange(0, 50);
    assert(toString(a) == "{<51..100>}");
    a = CRange(10, 100);
    a -= CRange(0, 99);
    assert(toString(a) == "{100}");
    a = CRange(10, 100);
    a -= CRange(10, 100);
    assert(toString(a) == "{}");
    a = CRange(10, 100);
    a -= CRange(0, 80);
    assert(toString(a) == "{<81..100>}");
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a -= CRange(11, 29);
    assert(toString(a) == "{<0..10>,<30..40>}");
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a -= CRange(10, 30);
    assert(toString(a) == "{<0..9>,<31..40>}");
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(0, 40);
    assert ( toString ( a ) == "{<-10..-5>,<50..60>}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(1, 39);
    assert ( toString ( a ) == "{<-10..-5>,0,40,<50..60>}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(2, 38);
    assert ( toString ( a ) == "{<-10..-5>,<0..1>,<39..40>,<50..60>}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(-5, 50);
    assert ( toString ( a ) == "{<-10..-6>,<51..60>}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(-8, 55);
    assert ( toString ( a ) == "{<-10..-9>,<56..60>}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(-9, 59);
    assert ( toString ( a ) == "{-10,60}" );
    a = CRange(0, 10);
    a += CRange(12, 28);
    a += CRange(30, 40);
    a += CRange(-10, -5);
    a += CRange(50, 60);
    a -= CRange(-10, 60);
    assert ( toString ( a ) == "{}" );
    a = CRange(2, 2);
    a += CRange(4, 4);
    a -= CRange(2, 2);
    a -= CRange(4, 4);
    assert ( toString ( a ) == "{}" );
    a = CRange(2, 2);
    a += CRange(4, 4);
    a += CRange(0, 0);
    a += CRange(6, 6);
    a -= CRange(2, 2);
    a -= CRange(4, 4);
    assert ( toString ( a ) == "{0,6}" );
    a = CRange(0, 0);
    a += CRange(10, 10);
    a += CRange(2, 8);
    a -= CRange(2, 2);
    a -= CRange(8, 8);
    assert ( toString ( a ) == "{0,<3..7>,10}" );
    a = CRange(0, 0);
    a += CRange(10, 10);
    a += CRange(5, 5);
    a -= CRange(5, 5);
    assert ( toString ( a ) == "{0,10}" );
    a = CRange(0, 10);
    a -= CRange(0, 0);
    a -= CRange(10, 10);
    assert ( toString ( a ) == "{<1..9>}" );
    a = CRange(0, 10);
    a += CRange(12, 20);
    a += CRange(22, 30);
    a -= CRange(11, 11);
    a -= CRange(21, 21);
    assert ( toString ( a ) == "{<0..10>,<12..20>,<22..30>}" );
    a = CRange(0, 10);
    a += CRange(12, 20);
    a += CRange(22, 30);
    a -= CRange(5, 5);
    a -= CRange(25, 25);
    assert ( toString ( a ) == "{<0..4>,<6..10>,<12..20>,<22..24>,<26..30>}" );
    a = CRange(0, 10);
    a += CRange(12, 20);
    a += CRange(22, 30);
    a -= CRange(-1, -1);
    a -= CRange(31, 31);
    assert ( toString ( a ) == "{<0..10>,<12..20>,<22..30>}" );
    a -= a;
    for (int sf = 0; sf <= 60; sf++) {
        if (sf % 2 == 0) {
            a -= CRange(sf, sf);
        } else {
            a += CRange(sf, sf);
        }
    }
    assert ( toString ( a ) == "{1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53,55,57,59}" );
    a -= CRange(2, 58);
    assert ( toString ( a ) == "{1,59}" );
    a = CRange(10, 50);
    a += CRange(0, 8);
    a += CRange(52, 60);
    a += CRange(9, 51);
    assert ( toString ( a ) == "{<0..60>}" );
}
void test3 ()
{
    CRangeList a;
    a = CRange(LLONG_MIN+1, LLONG_MAX-1);
    assert(toString(a) == "{<" + to_string(LLONG_MIN+1) + ".." + to_string(LLONG_MAX-1) + ">}");
    a += CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{<" + to_string(LLONG_MIN) + ".." + to_string(LLONG_MAX) + ">}");
    a -= CRange(LLONG_MIN, LLONG_MIN);
    a -= CRange(LLONG_MAX, LLONG_MAX);
    assert(toString(a) == "{<" + to_string(LLONG_MIN+1) + ".." + to_string(LLONG_MAX-1) + ">}");
    a += CRange(LLONG_MIN, LLONG_MIN);
    a += CRange(LLONG_MAX, LLONG_MAX);
    assert(toString(a) == "{<" + to_string(LLONG_MIN) + ".." + to_string(LLONG_MAX) + ">}");
    a -= CRange(0, 0);
    assert(toString(a) == "{<" + to_string(LLONG_MIN) + "..-1>,<1.." + to_string(LLONG_MAX) + ">}");
    a += CRange(0, 0);
    assert(toString(a) == "{<" + to_string(LLONG_MIN) + ".." + to_string(LLONG_MAX) + ">}");
    a -= CRange(LLONG_MIN+1, LLONG_MAX-1);
    assert(toString(a) == "{" + to_string(LLONG_MIN) + "," + to_string(LLONG_MAX) + "}");
}
void test5 ()
{
    CRangeList a;
    assert(toString(CRange(-1595,0) + CRange(0,LLONG_MAX) + CRange(LLONG_MAX,LLONG_MAX)) ==
           "{<-1595..9223372036854775807>}");
}
void test6 ()
{
    CRangeList a;
    a = CRange(0, 0) + CRange(6, 6);
    a -= CRange(-1, 1);
    a -= CRange(5, 7);
    assert(toString(a) == "{}");
    a = CRange(0, 0) + CRange(6, 6);
    assert(toString(a) == "{0,6}");
    a += CRange(1, 2) + CRange(3, 4) + CRange(5, 6) + CRange(7, 8) + CRange(9, 10);
    a -= a;
    assert(toString(a) == "{}");
    a = CRange(1, 1);
    a += CRange(2, 2);
    a -= CRange(2, 2);
    a -= CRange(2, 2) + CRange(1, 1);
    assert(toString(a) == "{}");
    a = CRange(10, 15);
    a += CRange(1, 5);
    assert(toString(a) == "{<1..5>,<10..15>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(1, 5);
    assert(toString(a) == "{<1..5>,<10..15>,<20..25>}");
    a = CRange(1, 5);
    a += CRange(10, 15);
    assert(toString(a) == "{<1..5>,<10..15>}");
    a = CRange(1, 5) + CRange(10, 15);
    a += CRange(20, 25);
    assert(toString(a) == "{<1..5>,<10..15>,<20..25>}");
    a = CRange(10, 15);
    a -= CRange(1, 5);
    assert(toString(a) == "{<10..15>}");
    a = CRange(10, 15) + CRange(20, 25);
    a -= CRange(1, 5);
    assert(toString(a) == "{<10..15>,<20..25>}");
    a = CRange(1, 5);
    a -= CRange(10, 15);
    assert(toString(a) == "{<1..5>}");
    a = CRange(1, 5) + CRange(10, 15);
    a -= CRange(20, 25);
    assert(toString(a) == "{<1..5>,<10..15>}");
    a = CRange(10, 15);
    a += CRange(1, 9);
    assert(toString(a) == "{<1..15>}");
    a = CRange(10, 15);
    a += CRange(1, 10);
    assert(toString(a) == "{<1..15>}");
    a = CRange(10, 15);
    a += CRange(1, 11);
    assert(toString(a) == "{<1..15>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(1, 9);
    assert(toString(a) == "{<1..15>,<20..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(1, 10);
    assert(toString(a) == "{<1..15>,<20..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(1, 11);
    assert(toString(a) == "{<1..15>,<20..25>}");
    a = CRange(1, 5);
    a += CRange(6, 15);
    assert(toString(a) == "{<1..15>}");
    a = CRange(1, 5);
    a += CRange(5, 15);
    assert(toString(a) == "{<1..15>}");
    a = CRange(1, 5);
    a += CRange(4, 15);
    assert(toString(a) == "{<1..15>}");
    a = CRange(1, 5) + CRange(10, 15);
    a += CRange(16, 25);
    assert(toString(a) == "{<1..5>,<10..25>}");
    a = CRange(1, 5) + CRange(10, 15);
    a += CRange(15, 25);
    assert(toString(a) == "{<1..5>,<10..25>}");
    a = CRange(1, 5) + CRange(10, 15);
    a += CRange(14, 25);
    assert(toString(a) == "{<1..5>,<10..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(16, 19);
    assert(toString(a) == "{<10..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(15, 20);
    assert(toString(a) == "{<10..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a += CRange(14, 21);
    assert(toString(a) == "{<10..25>}");
    a = CRange(10, 15);
    a -= CRange(1, 9);
    assert(toString(a) == "{<10..15>}");
    a = CRange(10, 15);
    a -= CRange(1, 10);
    assert(toString(a) == "{<11..15>}");
    a = CRange(10, 15);
    a -= CRange(1, 11);
    assert(toString(a) == "{<12..15>}");
    a = CRange(1, 5);
    a -= CRange(6, 15);
    assert(toString(a) == "{<1..5>}");
    a = CRange(1, 5);
    a -= CRange(5, 15);
    assert(toString(a) == "{<1..4>}");
    a = CRange(1, 5);
    a -= CRange(4, 15);
    assert(toString(a) == "{<1..3>}");
    a = CRange(10, 15) + CRange(20, 25);
    a -= CRange(16, 19);
    assert(toString(a) == "{<10..15>,<20..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a -= CRange(15, 20);
    assert(toString(a) == "{<10..14>,<21..25>}");
    a = CRange(10, 15) + CRange(20, 25);
    a -= CRange(14, 21);
    assert(toString(a) == "{<10..13>,<22..25>}");
    a = CRange(1, 10);
    a += CRange(2, 8);
    assert(toString(a) == "{<1..10>}");
    a = CRange(1, 10);
    a += CRange(1, 10);
    assert(toString(a) == "{<1..10>}");
    a = CRange(1, 10) + CRange(20, 30);
    a += CRange(2, 8);
    assert(toString(a) == "{<1..10>,<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a += CRange(1, 10);
    assert(toString(a) == "{<1..10>,<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a += CRange(2, 8) + CRange(22, 28);
    assert(toString(a) == "{<1..10>,<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a += CRange(1, 10) + CRange(20, 30);
    assert(toString(a) == "{<1..10>,<20..30>}");
    a = CRange(1, 10);
    a -= CRange(2, 9);
    assert(toString(a) == "{1,10}");
    a = CRange(1, 10);
    a -= CRange(3, 8);
    assert(toString(a) == "{<1..2>,<9..10>}");
    a = CRange(1, 10);
    a -= CRange(1, 10);
    assert(toString(a) == "{}");
    a = CRange(1, 10);
    a -= CRange(0, 11);
    assert(toString(a) == "{}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(1, 10);
    assert(toString(a) == "{<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(2, 9);
    assert(toString(a) == "{1,10,<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(3, 8);
    assert(toString(a) == "{<1..2>,<9..10>,<20..30>}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(1, 10) + CRange(20, 30);
    assert(toString(a) == "{}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(2, 9) + CRange(21, 29);
    assert(toString(a) == "{1,10,20,30}");
    a = CRange(1, 10) + CRange(20, 30);
    a -= CRange(3, 8) + CRange(22, 28);
    assert(toString(a) == "{<1..2>,<9..10>,<20..21>,<29..30>}");
}
void test7 ()
{
    CRangeList a;
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    assert(!a.includes(LLONG_MIN));
    assert(!a.includes(LLONG_MAX));
    assert(a.includes(0));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MIN) + CRange(0,0);
    assert(!a.includes(LLONG_MIN));
    assert(!a.includes(0));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MAX, LLONG_MAX) + CRange(0,0);
    assert(a.includes(LLONG_MIN));
    assert(!a.includes(0));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, 0);
    assert(!a.includes(LLONG_MIN));
    assert(!a.includes(0));
    assert(!a.includes(-100));
    assert(a.includes(100));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, 0);
    assert(!a.includes(LLONG_MIN));
    assert(!a.includes(0));
    assert(!a.includes(-100));
    assert(a.includes(100));
    assert(a.includes(LLONG_MAX));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(0, LLONG_MAX);
    assert(!a.includes(LLONG_MAX));
    assert(!a.includes(0));
    assert(a.includes(-100));
    assert(!a.includes(100));
    assert(a.includes(LLONG_MIN));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a += CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MIN);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MIN);
    a -= CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MIN);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a -= CRange(LLONG_MIN, LLONG_MIN);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MAX, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MAX, LLONG_MAX);
    assert(toString(a) == "{9223372036854775807}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a += CRange(LLONG_MIN, LLONG_MIN);
    assert(toString(a) == "{-9223372036854775808}");
    a = CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MIN, LLONG_MAX);
    a -= CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{-9223372036854775808,9223372036854775807}");
    a = CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MIN + 2, LLONG_MAX - 2);
    assert(toString(a) == "{-9223372036854775808,<-9223372036854775806..9223372036854775805>,9223372036854775807}");
    assert(a.includes(CRange(-9223372036854775806, 9223372036854775805)));
    a = CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX - 1);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a += CRange(LLONG_MIN + 1, LLONG_MAX);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN, LLONG_MAX);
    assert(toString(a) == "{}");
    a = CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{-9223372036854775808,9223372036854775807}");
    a = CRange(LLONG_MIN, LLONG_MIN) + CRange(LLONG_MAX, LLONG_MAX);
    a += CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{<-9223372036854775808..9223372036854775807>}");
    a = CRange(LLONG_MIN, LLONG_MIN);
    a -= CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{-9223372036854775808}");
    a = CRange(LLONG_MAX, LLONG_MAX);
    a -= CRange(LLONG_MIN + 1, LLONG_MAX - 1);
    assert(toString(a) == "{9223372036854775807}");
}
void test8 ()
{
    CRangeList a;
    assert(!a.includes(5));
    assert(!a.includes(CRange(5, 10)));
    a = CRange(5, 10);
    assert(a.includes(5));
    assert(a.includes(10));
    assert(!a.includes(4));
    assert(!a.includes(11));
    assert(a.includes(CRange(5, 10)));
    assert(!a.includes(CRange(4, 10)));
    assert(!a.includes(CRange(5, 11)));
}
void test9 ()
{
    CRangeList a = CRange(10, 20) + CRange(0, 9) + CRange(21, 30);
    assert ( toString ( a ) == "{<0..30>}" );
    a = CRange(10, 20) + CRange(0, 8) + CRange(22, 30);
    assert ( toString ( a ) == "{<0..8>,<10..20>,<22..30>}" );
    a = CRange(10, 20) + CRange(22, 30) + CRange(0, 50);
    assert ( toString ( a ) == "{<0..50>}" );
    a -= CRange(-5, 5) + CRange(25, 35) + CRange(45, 55);
    assert ( toString ( a ) == "{<6..24>,<36..44>}" );
    CRangeList b;
    b = CRange(0, 0);
    b -= CRange(0, 0);
    assert ( toString ( b ) == "{}" );
    b -= CRange(0, 10);
    assert ( toString ( b ) == "{}" );
    b += CRange ( 25, 100 );
    assert ( toString ( b ) == "{<25..100>}" );
    b -= CRange(25, 25);
    assert ( toString ( b ) == "{<26..100>}" );
    b += CRange(1000, 1200);
    b -= CRange(1000, 1000);
    assert ( toString ( b ) == "{<26..100>,<1001..1200>}" );
    b -= CRange(1200, 1200);
    assert ( toString ( b ) == "{<26..100>,<1001..1199>}" );
    b += CRange(30, 1100);
    assert ( toString ( b ) == "{<26..1199>}" );
    b -= CRange(30, 1100);
    assert ( toString ( b ) == "{<26..29>,<1101..1199>}" );
}
void test10 ()
{
    CRangeList a, b;
    a -= CRange(5, 10);
    assert(!a.includes(5));
    assert(!a.includes( CRange(5, 10) ));
    a += b;
    a = CRange(LLONG_MIN, LLONG_MAX);
    assert(a.includes(5));
    assert(a.includes(LLONG_MIN));
    assert(a.includes(LLONG_MAX));
    a -= CRange(LLONG_MIN, 0);
    assert(a.includes(5));
    assert(!a.includes(-5));
    a = CRange(LLONG_MIN, LLONG_MAX);
    a += CRange(LLONG_MIN, LLONG_MAX);
    assert(a.includes(LLONG_MIN));
    assert(a.includes(LLONG_MAX));
    a -= CRange(LLONG_MIN, LLONG_MIN);
    assert(!a.includes(LLONG_MIN));
    a += CRange(LLONG_MIN, LLONG_MIN);
    assert(a.includes(LLONG_MIN));
    a -= CRange(LLONG_MAX, LLONG_MAX);
    assert(!a.includes(LLONG_MAX));
    a += CRange(LLONG_MAX, LLONG_MAX);
    assert(a.includes(LLONG_MAX));
}
void test11()
{
    CRangeList a;
    a += CRange(LLONG_MIN, LLONG_MAX-1) + CRange(LLONG_MIN, LLONG_MAX);
    assert(toString( a ) == "{<-9223372036854775808..9223372036854775807>}");
}


int main() {

//    CRangeList d;
//    assert(!d.includes(5));
//    d += CRange(0, 10);
//    assert(d.includes(9));
//    d += CRange(20, 30);
//    d += CRange(60, 100);
//    d += CRange(110, 120);
//    toString(d);
//    d -= CRange(5, 115);
//    toString(d);
//
//    CRangeList e;
//    e += CRange(-400, -350);
//    e += CRange(-300, -250);
//    e += CRange(-200, -150);
//    e += CRange(-100, -50);
//    toString(e);
//    e -= CRange(-375, -98);
//    toString(e);
//
//    CRangeList f;
//    f += CRange(LLONG_MIN, LLONG_MAX-1) + CRange(LLONG_MIN, LLONG_MAX) - CRange(10,4165416);
//    toString(f);
//    f -= CRange(LLONG_MIN, LLONG_MIN);
//    f -= CRange(LLONG_MAX, LLONG_MAX);
//    toString(f);
//    f += CRange(LLONG_MIN, LLONG_MIN);
//    f += CRange(LLONG_MAX, LLONG_MAX);
//    toString(f);

    CRangeList a, b;
    assert(!a.includes(9));
    assert (sizeof(CRange) <= 2 * sizeof(long long));
    a = CRange(5, 10);
    a += CRange(25, 100);
    assert (toString(a) == "{<5..10>,<25..100>}");
    a += CRange(-5, 0);
    a += CRange(8, 50);
    assert (toString(a) == "{<-5..0>,<5..100>}");
    a += CRange(101, 105) + CRange(120, 150) + CRange(160, 180) + CRange(190, 210);
    assert (toString(a) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}");
    a += CRange(106, 119) + CRange(152, 158);
    assert (toString(a) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}");
    a += CRange(-3, 170);
    a += CRange(-30, 1000);
    assert (toString(a) == "{<-30..1000>}");
    b = CRange(-500, -300) + CRange(2000, 3000) + CRange(700, 1001);
    a += b;
    assert (toString(a) == "{<-500..-300>,<-30..1001>,<2000..3000>}");
    a -= CRange(-400, -400);
    assert (toString(a) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}");
    a -= CRange(10, 20) + CRange(900, 2500) + CRange(30, 40) + CRange(10000, 20000);
    assert (toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    try {
        a += CRange(15, 18) + CRange(10, 0) + CRange(35, 38);
        assert ("Exception not thrown" == nullptr);
    }
    catch (const std::logic_error &e) {
    }
    catch (...) {
        assert ("Invalid exception thrown" == nullptr);
    }
    assert (toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    b = a;
    assert (a == b);
    assert (!(a != b));
    b += CRange(2600, 2700);
    assert (toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
    assert (a == b);
    assert (!(a != b));
    b += CRange(15, 15);
    assert (toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}");
    assert (!(a == b));
    assert (a != b);
    assert (b.includes(15));
    assert (b.includes(2900));
    assert (b.includes(CRange(15, 15)));
    assert (b.includes(CRange(-350, -350)));
    assert (b.includes(CRange(100, 200)));
    assert (!b.includes(CRange(800, 900)));
    assert (!b.includes(CRange(-1000, -450)));
    assert (!b.includes(CRange(0, 500)));
    a += CRange(-10000, 10000) + CRange(10000000, 1000000000);
    assert (toString(a) == "{<-10000..10000>,<10000000..1000000000>}");
    b += a;
    assert (toString(b) == "{<-10000..10000>,<10000000..1000000000>}");
    b -= a;
    assert (toString(b) == "{}");
    b += CRange(0, 100) + CRange(200, 300) - CRange(150, 250) + CRange(160, 180) - CRange(170, 170);
    assert (toString(b) == "{<0..100>,<160..169>,<171..180>,<251..300>}");
    b -= CRange(10, 90) - CRange(20, 30) - CRange(40, 50) - CRange(60, 90) + CRange(70, 80);
    assert (toString(b) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}");

    test0();
    test1();
    test2();
    test3();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();

//    cout << x << endl;
//    cout << y << endl;

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
