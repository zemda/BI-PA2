#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>
using namespace std;
class CDate{
public:
    CDate(int y, int m, int d) : m_Year(y), m_Month(m), m_Day(d) {
    }

    CDate() {}

    int compare(const CDate &x) const {
        if (m_Year != x.m_Year) return m_Year - x.m_Year;
        if (m_Month != x.m_Month) return m_Month - x.m_Month;
        return m_Day - x.m_Day;
    }
    int year() const {
        return m_Year;
    }
    int month() const {
        return m_Month;
    }
    int day() const {
        return m_Day;
    }
    friend ostream &operator<<(ostream &os, const CDate &x) {
        char oldFill = os.fill();
        return os << setfill('0') << setw(4) << x.m_Year << "-"
                  << setw(2) << static_cast<int> ( x.m_Month ) << "-"
                  << setw(2) << static_cast<int> ( x.m_Day )
                  << setfill(oldFill);
    }
private:
    int16_t m_Year;
    int8_t m_Month;
    int8_t m_Day;
};
#endif /* __PROGTEST__ */

long long order_num = 1;
/**
 * @brief This function removes all spaces from the beginning and the end of the string, and also removes all duplicate spaces from the string.
 * @param str string to be trimmed
 * @return trimmed string
 */
string trimString(const string &str) {
    if (str.empty()) return str;
    string res = str;
    while (res.front() == ' ') res.erase(res.begin());
    while (res.back() == ' ') res.pop_back();

    auto newEnd = unique(res.begin(), res.end(),[](char a, char b) {return (a == ' ' && b == ' ');});
    res.erase(newEnd, res.end());
    transform(res.begin(), res.end(), res.begin(), [](unsigned char c){ return tolower(c); });
    return res;
}
string toLower(const string& str){
    string res = str;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

class CInvoice{
public:
    CInvoice(const CDate &date, string seller, string buyer, unsigned int amount, double vat) : m_date(date), m_seller(std::move(seller)), m_buyer(std::move(buyer)), m_amount(amount), m_vat(vat){
        normalized_seller = trimString(m_seller);
        normalized_buyer = trimString(m_buyer);
        order = 0;
    }
    CInvoice(const CDate &date, string seller, string buyer, unsigned int amount, double vat, long long order) : order(order), m_date(date), m_seller(std::move(seller)), m_buyer(std::move(buyer)), m_amount(amount), m_vat(vat){}

    bool operator<(const CInvoice &x) const{
        if (this->date().compare(x.date()) != 0 )
            return this->date().compare(x.date()) > 0;
        if (this->seller() != x.seller())
            return this->seller() < x.seller();
        if (this->buyer() != x.buyer())
            return this->buyer() < x.buyer();
        if (this->amount() != x.amount())
            return this->amount() < x.amount();
        if (this->vat() != x.vat())
            return this->vat() < x.vat();
        return false;
    }

    CDate date() const{
        return m_date;
    }
    string buyer() const{
        return m_buyer;
    }
    string seller() const{
        return m_seller;
    }
    unsigned int amount() const{
        return m_amount;
    }
    double vat() const{
        return m_vat;
    }

    /**
     * @brief Getter for normalized seller
     * @return The trimmer name of the seller
     */
    string getNormalizedSeller() const{
        return normalized_seller;
    }

    /**
     * @brief Getter for normalized buyer
     * @return The trimmer name of the buyer
     */
    string getNormalizedBuyer() const{
        return normalized_buyer;
    }

    long long order;
private:
    CDate m_date;
    string m_seller;
    string m_buyer;
    unsigned int m_amount;
    double m_vat;
    string normalized_seller;
    string normalized_buyer;
};

/**
 * @brief This class is used to sort invoices by given keys.
 */
class CSortOpt{
public:
    static const int BY_DATE = 0;
    static const int BY_BUYER = 1;
    static const int BY_SELLER = 2;
    static const int BY_AMOUNT = 3;
    static const int BY_VAT = 4;

    CSortOpt()= default;
    bool operator()(const CInvoice &a, const CInvoice &b) const{
        string lower_buyer1, lower_buyer2, lower_seller1, lower_seller2;
        for (const auto& key : m_keys) {
            switch (key.first) {
                case 0:
                    if (a.date().compare(b.date()) != 0) return key.second ? a.date().compare(b.date()) < 0 : a.date().compare(b.date()) > 0;
                    continue;
                case 1:
                    lower_buyer1 = toLower(a.buyer());
                    lower_buyer2 = toLower(b.buyer());
                    if (lower_buyer1 != lower_buyer2) return key.second ? lower_buyer1 < lower_buyer2 : lower_buyer1 > lower_buyer2;
                    continue;
                case 2:
                    lower_seller1 = toLower(a.seller());
                    lower_seller2 = toLower(b.seller());
                    if (lower_seller1 != lower_seller2) return key.second ? lower_seller1 < lower_seller2 : lower_seller1 > lower_seller2;
                    continue;
                case 3:
                    if (a.amount() != b.amount()) return key.second ? a.amount() < b.amount() : a.amount() > b.amount();
                    continue;
                case 4:
                    if (a.vat() != b.vat()) return key.second ? a.vat() < b.vat() : a.vat() > b.vat();
                    continue;
            }
        }
        return a.order < b.order;
    }


    CSortOpt &addKey(int key, bool ascending = true){
        for (auto &i : m_keys)
            if (i.first == key)
                return *this;
        m_keys.emplace_back(key, ascending);
        return *this;
    }

private:
    vector<pair<int, bool>> m_keys;
};

class CVATRegister{
public:
    bool registerCompany(const string &name){
        string newString = trimString(name);
        if (isRegistered(newString)) return false;
        original_name.emplace(newString, name);
        unpairedInvoices.emplace(newString, set<CInvoice>());
        return true;
    }

    bool isRegistered(const string&name)const{
        return original_name.find(name) != original_name.end();
    }

    bool isValidInvoice(const CInvoice &x)const{
        return isRegistered(x.getNormalizedBuyer()) && isRegistered(x.getNormalizedSeller()) && x.getNormalizedBuyer() != x.getNormalizedSeller();
    }


    /**
     * @brief Adds an issued invoice to the register.
     *
     * This method adds an issued invoice to the register. The invoice is added to
     * the list of invoices issued by the seller and is associated with the buyer.
     *
     * @param x Invoice to be added
     * @return True if invoice was added, false otherwise
     *
     * @note This method uses the allInvoices map to store all invoices and the
     * unpairedInvoices map to store those invoices that weren't added by a buyer yet (addAccepted method).
     */
    bool addIssued(const CInvoice &x){
        CInvoice newInv = CInvoice(x.date(), x.getNormalizedSeller(), x.getNormalizedBuyer(), x.amount(), x.vat());
        if (!isValidInvoice(newInv)) return false;

        auto [iter, inserted] = allInvoices.emplace(newInv, make_pair(true, false));
        auto it1 = unpairedInvoices.find(newInv.getNormalizedBuyer());
        auto it2 = unpairedInvoices.find(newInv.getNormalizedSeller());

        /**
         * If invoice already exists and was already added by a seller, return false as it is not possible to add it again
         * if invoice already exists and was already added by a buyer, erase it from unpairedInvoices, because it is now paired
         */
        if (!inserted){
            if (iter->second.first) return false;
            iter->second.first = true;
            it1->second.erase(newInv);
            it2->second.erase(newInv);
        }else {
            newInv.order = order_num++;
            it1->second.emplace(newInv);
            it2->second.emplace(newInv);
        }
        return true;
    }


    /**
     * @brief Adds an accepted invoice to the register.
     *
     * This method adds an accepted invoice to the register. The invoice is added to
     * the list of invoices accepted by the buyer and is associated with the seller.
     *
     * @param x Invoice to be added
     * @return True if invoice was added, false otherwise
     *
     * @note This method uses the allInvoices map to store all invoices and the
     * unpairedInvoices map to store those invoices that weren't added by a seller yet (addIssued method).
     */
    bool addAccepted(const CInvoice &x){
        CInvoice newInv = CInvoice(x.date(), x.getNormalizedSeller(), x.getNormalizedBuyer(), x.amount(), x.vat());
        if (!isValidInvoice(newInv)) return false;

        auto [iter, inserted] = allInvoices.emplace(newInv, make_pair(false, true));
        auto it1 = unpairedInvoices.find(newInv.getNormalizedBuyer());
        auto it2 = unpairedInvoices.find(newInv.getNormalizedSeller());
        /**
         * If invoice already exists and was already added by a buyer, return false as it is not possible to add it again
         * if invoice already exists and was already added by a seller, erase it from unpairedInvoices, because it is now paired
         */
        if (!inserted){
            if (iter->second.second) return false;
            iter->second.second = true;
            it1->second.erase(newInv);
            it2->second.erase(newInv);
        }else {
            newInv.order = order_num++;
            it1->second.emplace(newInv);
            it2->second.emplace(newInv);
        }

        return true;
    }


    /**
     * @brief Removes an issued invoice from the register.
     * @param x Invoice to be removed
     * @return True if invoice was removed, false otherwise
     *
     * @note In case we are supposed to remove an invoice that has been added by a buyer as well as by a seller,
     * we have to insert it back to unpairedInvoices, because it is now unpaired again.
     */
    bool delIssued(const CInvoice &x){
        CInvoice newInv = CInvoice(x.date(), x.getNormalizedSeller(), x.getNormalizedBuyer(), x.amount(), x.vat());
        if (!isValidInvoice(newInv)) return false;

        auto iter = allInvoices.find(newInv);
        if (iter != allInvoices.end()){
            auto it1 = unpairedInvoices.find(newInv.getNormalizedBuyer());
            auto it2 = unpairedInvoices.find(newInv.getNormalizedSeller());

            if (!iter->second.first) return false;
            iter->second.first = false;
            if (!iter->second.second) {
                allInvoices.erase(iter);
                it1->second.erase(newInv);
                it2->second.erase(newInv);
            }else {
                newInv.order = order_num++;
                it1->second.emplace(newInv);
                it2->second.emplace(newInv);
            }
        }else
            return false;

        return true;
    }


    /**
     * @brief Removes an accepted invoice from the register.
     * @param x Invoice to be removed
     * @return True if invoice was removed, false otherwise
     *
     * @note In case we are supposed to remove an invoice that has been added by a seller as well as by a buyer,
     * we have to insert it back to unpairedInvoices, because it is now unpaired again.
     */
    bool delAccepted(const CInvoice &x){
        CInvoice newInv = CInvoice(x.date(), x.getNormalizedSeller(), x.getNormalizedBuyer(), x.amount(), x.vat());
        if (!isValidInvoice(newInv)) return false;

        auto iter = allInvoices.find(newInv);
        if (iter != allInvoices.end()){
            auto it1 = unpairedInvoices.find(newInv.getNormalizedBuyer());
            auto it2 = unpairedInvoices.find(newInv.getNormalizedSeller());

            if (!iter->second.second) return false;
            iter->second.second = false;
            if (!iter->second.first) {
                allInvoices.erase(iter);
                it1->second.erase(newInv);
                it2->second.erase(newInv);
            }else {
                newInv.order = order_num++;
                it1->second.emplace(newInv);
                it2->second.emplace(newInv);
            }
        }else
            return false;

        return true;
    }


    /**
     * @brief Finds all unmatched invoices for a given company.
     * @param company The name of the company to find invoices for.
     * @param sortBy The list is to be sorted by this criterion
     * @return List of all invoices that weren't added by either the seller or the buyer yet
     */
    list<CInvoice> unmatched(const string &company, const CSortOpt &sortBy) const{
        list<CInvoice> result;
        string companyTrimmed = trimString(company);

        auto it = unpairedInvoices.find(companyTrimmed);
        if (it != unpairedInvoices.end()){
            for (const auto& x : it->second){
                string orig_sell = getOriginalName(x.seller());
                string orig_buy = getOriginalName(x.buyer());
                result.emplace_back(x.date(), orig_sell, orig_buy, x.amount(), x.vat(), x.order);
            }
        }
        result.sort(sortBy);
        return result;
    }

    string getOriginalName(const string &company) const{
        return original_name.at(company);
    }

private:

    map<string, string> original_name;            /**< Map to store original names of companies (those names were entered during registration) */
    map<string, set<CInvoice>> unpairedInvoices;  /**< Map to store unpaired invoices for each company */
    map<CInvoice, pair<bool, bool>> allInvoices;  /**< Map to store all invoices (issued and accepted)
                                                   * @details pair<bool, bool> - first bool represents if invoice was issued, second bool represents if invoice was accepted
                                                   * */
};

#ifndef __PROGTEST__

/**
 * @brief Checks if two lists of invoices are equal
 * @param a First list
 * @param b Second list
 * @return True if lists are equal, false otherwise
 */
bool equalLists(const list<CInvoice> &a, const list<CInvoice> &b) {
    if (a.size() != b.size()) return false;
    auto itA = a.begin();
    auto itB = b.begin();
    while (itA != a.end()) {
        if (itA->date().compare(itB->date()) != 0) return false;
        if (itA->seller() != itB->seller()) return false;
        if (itA->buyer() != itB->buyer()) return false;
        if (itA->amount() != itB->amount()) return false;
        if (itA->vat() != itB->vat()) return false;
        itA++;
        itB++;
    }
    return true;
}

int main() {
//    CVATRegister p;
//    assert (p.registerCompany("first Company"));
//    assert (p.registerCompany("Second     Company"));
//    assert (p.registerCompany("ThirdCompany, Ltd."));
//    assert (p.registerCompany("Third Company, Ltd."));
//
//    for (int i = 2000; i < 100000000; i++) p.addIssued(CInvoice(CDate(i, i, i), "First Company", "Second Company ", i, 20));
//
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));
//    p.unmatched("ThirdCompany, Ltd.", CSortOpt().addKey(0, true).addKey(1, false).addKey(2, true).addKey(3, false).addKey(4, true));

    CVATRegister o;
    assert(o.registerCompany(""));
    assert(!o.registerCompany(" "));
    assert(!o.registerCompany("  "));




    CVATRegister r;
    assert (r.registerCompany("first Company"));
    assert (r.registerCompany("Second     Company"));
    assert (r.registerCompany("ThirdCompany, Ltd."));
    assert (r.registerCompany("Third Company, Ltd."));
    assert (!r.registerCompany("Third Company, Ltd."));
    assert (!r.registerCompany(" Third  Company,  Ltd.  "));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 20)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 2), "FirSt Company", "Second Company ", 200, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "Second Company ", "First Company", 300, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 4), "First Company", "First   Company", 200, 30)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 4), "Another Company", "First   Company", 200, 30)));

    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER,false)
                        .addKey(CSortOpt::BY_DATE,false)),list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true)
                        .addKey(CSortOpt::BY_BUYER, true)),list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_VAT, true).addKey(CSortOpt::BY_AMOUNT, true)
                        .addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company", CSortOpt()),list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000)
                               }));
    assert (equalLists(r.unmatched("second company", CSortOpt().addKey(CSortOpt::BY_DATE, false)),list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Third Company, Ltd.", "Second     Company", 400,34.000000)
                               }));
    assert (equalLists(r.unmatched("last company", CSortOpt().addKey(CSortOpt::BY_VAT, true)),list<CInvoice>{}));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company ", 200, 30)));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 1), "Second company ", "First Company", 300, 32)));
    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                        list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,32.000000)
                               }));
    assert (!r.delIssued(CInvoice(CDate(2001, 1, 1), "First Company", "Second Company ", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "A First Company", "Second Company ", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Hand", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 1200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 200, 130)));
    assert (r.delIssued(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company", 200, 30)));
    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));
    assert (r.delAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(
                                           CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));
    assert (r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (equalLists(r.unmatched("First Company",CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,32.000000)
                               }));
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */