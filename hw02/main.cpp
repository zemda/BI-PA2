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
using namespace std;
#endif /* __PROGTEST__ */

class CPersonalAgenda{
public:
    CPersonalAgenda(void) = default;
    ~CPersonalAgenda(void) = default;

    bool add(const string &name, const string &surname, const string &email, unsigned int salary){
        Employee employee = {name, surname, email, salary};
        if (bin_s(employee, 1) || bin_s(employee, 2)) return false; //if already eixsts

        auto surname_iterator = lower_b(employee, 1);
        orderedAgendaBySurname.insert(surname_iterator, employee);

        auto email_iterator = lower_b(employee, 2);
        orderedAgendaByEmail.insert(email_iterator, employee);

        auto salary_iterator = lower_bound(salaries.begin(), salaries.end(), salary);
        salaries.insert(salary_iterator, salary);

        return true;
    }

    bool del(const string &name, const string &surname){
        Employee employee = {name, surname, "", 0};

        auto surname_iterator = lower_b(employee, 1);
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->name != name || surname_iterator->surname != surname) return false;
        employee = {name, surname, surname_iterator->email, surname_iterator->salary};
        orderedAgendaBySurname.erase(surname_iterator);

        auto email_iterator = lower_b(employee, 2);
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->name != name || email_iterator->surname != surname) return false;
        else orderedAgendaByEmail.erase(email_iterator);

        auto salary_iterator = lower_bound(salaries.begin(), salaries.end(), employee.salary);
        salaries.erase(salary_iterator);
        return true;
    }

    bool del(const string &email){
        Employee employee = {"", "", email, 0};

        auto email_iterator = lower_b(employee, 2);
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->email != email) return false;
        employee = {email_iterator->name, email_iterator->surname, email, email_iterator->salary};
        orderedAgendaByEmail.erase(email_iterator);

        auto surname_iterator = lower_b(employee, 1);
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->email != email) return false;
        else orderedAgendaBySurname.erase(surname_iterator);

        auto r = lower_bound(salaries.begin(), salaries.end(), employee.salary);
        salaries.erase(r);
        return true;
    }

    bool changeName(const string &email, const string &newName, const string &newSurname){
        Employee newEmployee = {newName, newSurname, email, 0};
        Employee oldEmployee = {"", "", email, 0};
        bool cantBeChanged = bin_s(newEmployee, 1); //if already exists
        if (cantBeChanged) return false;

        auto email_iterator = lower_b(oldEmployee, 2);
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->email != email) return false;
        oldEmployee.salary = email_iterator->salary;

        if(!del(email)) return false;
        if(!add(newName, newSurname, email, oldEmployee.salary)) return false;

        return true;
    }

    bool changeEmail(const string &name, const string &surname, const string &newEmail){
        Employee newEmployee = {name, surname, newEmail, 0};
        Employee oldEmployee = {name, surname, "", 0};
        bool cantChangeEmail = bin_s(newEmployee, 2);
        if (cantChangeEmail) return false;
        //SMAZAT TY BIN_SEARCHE A DÁT JEN if iterator.begin() then false

        auto surname_iterator = lower_b(oldEmployee, 1);
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->surname != surname || surname_iterator->name != name) return false;
        oldEmployee.salary = surname_iterator->salary;

        if(!del(name, surname)) return false;
        if(!add(name, surname, newEmail, oldEmployee.salary)) return false;

        return true;
    }

    bool setSalary(const string &name, const string &surname, unsigned int salary){
        Employee employee = {name, surname, "", 0};
        auto surname_iterator = lower_b(employee, 1);
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->surname != surname || surname_iterator->name != name) return false;
        employee = {name, surname, surname_iterator->email, surname_iterator->salary};
        surname_iterator->salary = salary;

        auto email_iterator = lower_b(employee, 2);
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->surname != surname || email_iterator->name != name) return false;
        else email_iterator->salary = salary;

        updateVectSalaries(employee.salary, salary);

        return true;
    }

    bool setSalary(const string &email, unsigned int salary){
        Employee employee = {"", "", email, 0};
        auto email_iterator = lower_b(employee, 2);
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->email != email) return false;
        employee = {email_iterator->name, email_iterator->surname, email, email_iterator->salary};
        email_iterator->salary = salary;

        auto surname_iterator = lower_b(employee, 1);
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->email != email) return false;
        else surname_iterator->salary = salary;

        updateVectSalaries(employee.salary, salary);

        return true;
    }

    unsigned int getSalary(const string &name, const string &surname) const{
        auto surname_iterator = lower_bound(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), Employee{name, surname, "", 0}, Employee::cmpSurname());
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->surname != surname || surname_iterator->name != name) return 0;
        else return surname_iterator->salary;
    }

    unsigned int getSalary(const string &email) const{
        auto email_iterator = lower_bound(orderedAgendaByEmail.begin(), orderedAgendaByEmail.end(), Employee{"", "", email, 0}, Employee::cmpEmail());
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->email != email) return 0;
        else return email_iterator->salary;
    }

    bool getRank(const string &name, const string &surname, int &rankMin, int &rankMax) const{
        auto surname_iterator = lower_bound(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), Employee{name, surname, "", 0}, Employee::cmpSurname());
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->surname != surname || surname_iterator->name != name) return false;
        updateRank(surname_iterator->salary, rankMin, rankMax);
        return true;
    }

    bool getRank(const string &email, int &rankMin, int &rankMax) const{
        auto email_iterator = lower_bound(orderedAgendaByEmail.begin(), orderedAgendaByEmail.end(), Employee{"", "", email, 0}, Employee::cmpEmail());
        if (email_iterator == orderedAgendaByEmail.end() || email_iterator->email != email) return false;
        updateRank(email_iterator->salary, rankMin, rankMax);
        return true;
    }

    bool getFirst(string &outName, string &outSurname) const{
        if (orderedAgendaBySurname.empty()) return false;
        outName = orderedAgendaBySurname[0].name;
        outSurname = orderedAgendaBySurname[0].surname;
        return true;
    }

    bool getNext(const string &name, const string &surname, string &outName, string &outSurname ) const{
//        auto surname_iterator = upper_bound(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), Employee{name, surname, "", 0}, Employee::cmpSurname());
//        int i = distance(orderedAgendaBySurname.begin(), surname_iterator - 1); //index of the given employee
//        if (surname_iterator == orderedAgendaBySurname.end() || (orderedAgendaBySurname[i].name != name && orderedAgendaBySurname[i].surname != surname)) return false;
//        outName = surname_iterator->name;
//        outSurname = surname_iterator->surname;
//        return true;

        auto surname_iterator = lower_bound(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), Employee{name, surname, "", 0}, Employee::cmpSurname());
        if (surname_iterator == orderedAgendaBySurname.end() || surname_iterator->surname != surname || surname_iterator->name != name) return false;
        if (surname_iterator == orderedAgendaBySurname.end() - 1) return false;
        outName = (surname_iterator + 1)->name;
        outSurname = (surname_iterator + 1)->surname;
        return true;
    }

private:
    struct Employee{
        string name;
        string surname;
        string email;
        unsigned int salary;

        struct cmpSurname{
            bool operator()(const Employee &a, const Employee &b){
                if (a.surname == b.surname) return a.name < b.name;
                return a.surname < b.surname;
            }
        };
        struct cmpEmail{
            bool operator()(const Employee &a, const Employee &b){
                return a.email < b.email;
            }
        };
    };

    bool bin_s(const Employee &employee, const int option){ //1-surname, 2-email
        bool res;
        switch (option) {
            case 1: res = binary_search(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), employee, Employee::cmpSurname()); break;
            case 2: res = binary_search(orderedAgendaByEmail.begin(), orderedAgendaByEmail.end(), employee, Employee::cmpEmail()); break;
        }
        return res;
    }
    vector<Employee>::iterator lower_b(const Employee &employee, const int option){ //1-surname, 2-email
        vector<Employee>::iterator res;
        switch(option){
            case 1:res = lower_bound(orderedAgendaBySurname.begin(), orderedAgendaBySurname.end(), employee, Employee::cmpSurname()); break;
            case 2: res = lower_bound(orderedAgendaByEmail.begin(), orderedAgendaByEmail.end(), employee, Employee::cmpEmail()); break;
        }
        return res;
    }
    void updateRank(unsigned int salary, int &rankMin, int &rankMax)const{
        auto r = lower_bound(salaries.begin(), salaries.end(), salary);
        rankMin = r - salaries.begin();
        r = upper_bound(salaries.begin(), salaries.end(), salary);
        rankMax = r - salaries.begin() - 1;
    }
    void updateVectSalaries(unsigned int old_salary, unsigned int new_salary){
        auto r = lower_bound(salaries.begin(), salaries.end(), old_salary);
        salaries.erase(r);
        r = lower_bound(salaries.begin(), salaries.end(), new_salary);
        salaries.insert(r, new_salary);
    }

    vector<Employee> orderedAgendaBySurname;
    vector<Employee> orderedAgendaByEmail;
    vector<unsigned int> salaries;
};

#ifndef __PROGTEST__
int main ( void ) //pardon, nevšiml jsem si, že to máme posílat i s testy (až jak to teď zmínili na disu), hlavně když jsem vesměs žádné nepsal a napoprvé to měl až na jeden if správně
{
    string outName, outSurname;
    int lo, hi;
    CPersonalAgenda b6;
    assert ( b6 . add ( "", "", "", 0 ) );
    assert (! b6 . add ( "", "", "", 0 ) );

    CPersonalAgenda b1;
    assert ( b1 . add ( "John", "Smith", "john", 30000 ) );
    assert ( b1 . add ( "John", "Miller", "johnm", 35000 ) );
    assert ( b1 . add ( "Peter", "Smith", "peter", 23000 ) );
    assert ( b1 . getFirst ( outName, outSurname )
             && outName == "John"
             && outSurname == "Miller" );
    assert ( b1 . getNext ( "John", "Miller", outName, outSurname )
             && outName == "John"
             && outSurname == "Smith" );
    assert ( b1 . getNext ( "John", "Smith", outName, outSurname )
             && outName == "Peter"
             && outSurname == "Smith" );
    assert ( ! b1 . getNext ( "Peter", "Smith", outName, outSurname ) );
    assert ( b1 . setSalary ( "john", 32000 ) );
    assert ( b1 . getSalary ( "john" ) ==  32000 );
    assert ( b1 . getSalary ( "John", "Smith" ) ==  32000 );
    assert ( b1 . getRank ( "John", "Smith", lo, hi )
             && lo == 1
             && hi == 1 );
    assert ( b1 . getRank ( "john", lo, hi )
             && lo == 1
             && hi == 1 );
    assert ( b1 . getRank ( "peter", lo, hi )
             && lo == 0
             && hi == 0 );
    assert ( b1 . getRank ( "johnm", lo, hi )
             && lo == 2
             && hi == 2 );
    assert ( b1 . setSalary ( "John", "Smith", 35000 ) );
    assert ( b1 . getSalary ( "John", "Smith" ) ==  35000 );
    assert ( b1 . getSalary ( "john" ) ==  35000 );
    assert ( b1 . getRank ( "John", "Smith", lo, hi )
             && lo == 1
             && hi == 2 );
    assert ( b1 . getRank ( "john", lo, hi )
             && lo == 1
             && hi == 2 );
    assert ( b1 . getRank ( "peter", lo, hi )
             && lo == 0
             && hi == 0 );
    assert ( b1 . getRank ( "johnm", lo, hi )
             && lo == 1
             && hi == 2 );
    assert ( b1 . changeName ( "peter", "James", "Bond" ) );
    assert ( b1 . getSalary ( "peter" ) ==  23000 );
    assert ( b1 . getSalary ( "James", "Bond" ) ==  23000 );
    assert ( b1 . getSalary ( "Peter", "Smith" ) ==  0 );
    assert ( b1 . getFirst ( outName, outSurname )
             && outName == "James"
             && outSurname == "Bond" );
    assert ( b1 . getNext ( "James", "Bond", outName, outSurname )
             && outName == "John"
             && outSurname == "Miller" );
    assert ( b1 . getNext ( "John", "Miller", outName, outSurname )
             && outName == "John"
             && outSurname == "Smith" );
    assert ( ! b1 . getNext ( "John", "Smith", outName, outSurname ) );
    assert ( b1 . changeEmail ( "James", "Bond", "james" ) );
    assert ( b1 . getSalary ( "James", "Bond" ) ==  23000 );
    assert ( b1 . getSalary ( "james" ) ==  23000 );
    assert ( b1 . getSalary ( "peter" ) ==  0 );
    assert ( b1 . del ( "james" ) );
    assert ( b1 . getRank ( "john", lo, hi )
             && lo == 0
             && hi == 1 );
    assert ( b1 . del ( "John", "Miller" ) );
    assert ( b1 . getRank ( "john", lo, hi )
             && lo == 0
             && hi == 0 );
    assert ( b1 . getFirst ( outName, outSurname )
             && outName == "John"
             && outSurname == "Smith" );
    assert ( ! b1 . getNext ( "John", "Smith", outName, outSurname ) );
    assert ( b1 . del ( "john" ) );
    assert ( ! b1 . getFirst ( outName, outSurname ) );
    assert ( b1 . add ( "John", "Smith", "john", 31000 ) );
    assert ( b1 . add ( "john", "Smith", "joHn", 31000 ) );
    assert ( b1 . add ( "John", "smith", "jOhn", 31000 ) );
    CPersonalAgenda b2;
    assert ( ! b2 . getFirst ( outName, outSurname ) );
    assert ( b2 . add ( "James", "Bond", "james", 70000 ) );
    assert ( b2 . add ( "James", "Smith", "james2", 30000 ) );
    assert ( b2 . add ( "Peter", "Smith", "peter", 40000 ) );
    assert ( ! b2 . add ( "James", "Bond", "james3", 60000 ) );
    assert ( ! b2 . add ( "Peter", "Bond", "peter", 50000 ) );
    assert ( ! b2 . changeName ( "joe", "Joe", "Black" ) );
    assert ( ! b2 . changeEmail ( "Joe", "Black", "joe" ) );
    assert ( ! b2 . setSalary ( "Joe", "Black", 90000 ) );
    assert ( ! b2 . setSalary ( "joe", 90000 ) );
    assert ( b2 . getSalary ( "Joe", "Black" ) ==  0 );
    assert ( b2 . getSalary ( "joe" ) ==  0 );
    assert ( ! b2 . getRank ( "Joe", "Black", lo, hi ) );
    assert ( ! b2 . getRank ( "joe", lo, hi ) );
    assert ( ! b2 . changeName ( "joe", "Joe", "Black" ) );
    assert ( ! b2 . changeEmail ( "Joe", "Black", "joe" ) );
    assert ( ! b2 . del ( "Joe", "Black" ) );
    assert ( ! b2 . del ( "joe" ) );
    assert ( ! b2 . changeName ( "james2", "James", "Bond" ) );
    assert ( ! b2 . changeEmail ( "Peter", "Smith", "james" ) );
    assert ( ! b2 . changeName ( "peter", "Peter", "Smith" ) );
    assert ( ! b2 . changeEmail ( "Peter", "Smith", "peter" ) );
    assert ( b2 . del ( "Peter", "Smith" ) );
    assert ( ! b2 . changeEmail ( "Peter", "Smith", "peter2" ) );
    assert ( ! b2 . setSalary ( "Peter", "Smith", 35000 ) );
    assert ( b2 . getSalary ( "Peter", "Smith" ) ==  0 );
    assert ( ! b2 . getRank ( "Peter", "Smith", lo, hi ) );
    assert ( ! b2 . changeName ( "peter", "Peter", "Falcon" ) );
    assert ( ! b2 . setSalary ( "peter", 37000 ) );
    assert ( b2 . getSalary ( "peter" ) ==  0 );
    assert ( ! b2 . getRank ( "peter", lo, hi ) );
    assert ( ! b2 . del ( "Peter", "Smith" ) );
    assert ( ! b2 . del ( "peter" ) );
    assert ( b2 . add ( "Peter", "Smith", "peter", 40000 ) );
    assert ( b2 . getSalary ( "peter" ) ==  40000 );

    CPersonalAgenda b3;
    assert(b3.add ("PPPPPPPP", "OOOOOOOO", "IIIIIIII", 30));
    assert(b3.add ("PPPPPPP", "OOOOOOO", "IIIIIII", 30));

    CPersonalAgenda b4;
    assert(b4.add("Jaja", "Pajvcgha", "jmnbojo", 574));
    assert(b4.add("Jadja", "Pacvja", "jhgfojo", 6541));
    assert(b4.add("Jajqa", "yAX", "jhgvjo", 852));
    assert(b4.add("Jafja", "Pazitgvuja", "jogfvjo", 6874));
    assert(b4.add("Jaxja", "Paja", "jofvjo", 987));
    assert(b4.add("Jajpa", "Pavcbja", "jfbcojo", 852));
    assert(b4.add("Jajoa", "Pavbnja", "jopojo", 654));
    assert(b4.add("Juaja", "Pajdvhbjna", "jlkjnnojo", 15426));
    assert(b4.add("Jaija", "Pajjsdxja", "jolkjhjo", 65655));
    assert(b4.add("Jajja", "Pajvbna", "jopoikjhugfjo", 51624));
    assert(b4.add("Janja", "Pahzgja", "jonbvcjo", 455445));
    assert(b4.add("Jajma", "Paxnjija", "jogfdjo", 98654));
    assert(b4.add("Javja", "Pajbnmkba", "johgfvcjo", 652145));
    assert(b4.add("Jabja", "Pajhgja", "jogfcjo", 555524));
    assert(b4.add("Jaxjca", "Pajrfa", "jofvdxjo", 52452));
    assert(b4.add("Jajesa", "Padcja", "jooiuhzgjo", 524145));
    assert(b4.add("Jajrfa", "Phgvaja", "jojjhgfco", 55214));
    assert(b4.add("Jajjzda", "Pgbvcaja", "jpoijujhgojo", 551747));
    assert(b4.add("Jajdda", "Pahjgfja", "joqwerjo", 93147));
    assert(b4.add("Jajqqqa", "Pjhgaja", "jadfggojo", 789411));
    assert(b4.add("Jajaff", "Pajjha", "joadfghjo", 3125));
    assert(b4.add("Jajjuza", "Pjhgfaja", "jadfghojo", 67846));
    assert(b4.add("Jajvdxea", "Phjgaja", "jhjoqertzujo", 6853413));
    assert(b4.add("Jajakhgd", "Pajhgfa", "jojhgfjo", 56741));

    assert(b4.setSalary("jmnbojo", 57744));
    assert(b4.setSalary("jhgfojo", 65441));
    assert(b4.setSalary("jhgvjo", 8552));
    assert(b4.setSalary("jogfvjo", 684874));
    assert(b4.setSalary("jofvjo", 91187));
    assert(b4.setSalary("jfbcojo", 84452));
    assert(b4.setSalary("jopojo", 65114));
    assert(b4.setSalary("jlkjnnojo", 1426));
    assert(b4.setSalary("jolkjhjo", 6565));
    assert(b4.setSalary("jopoikjhugfjo", 5124));
    assert(b4.setSalary("jonbvcjo", 45445));
    assert(b4.setSalary("jogfdjo", 9854));
    assert(b4.setSalary("johgfvcjo", 6545));
    assert(b4.setSalary("jogfcjo", 5524));
    assert(b4.setSalary("jofvdxjo", 522));
    assert(b4.setSalary("jooiuhzgjo", 5245));
    assert(b4.setSalary("jojjhgfco", 5214));
    assert(b4.setSalary("jpoijujhgojo", 5747));
    assert(b4.setSalary("joqwerjo", 937));
    assert(b4.setSalary("jadfggojo", 7411));
    assert(b4.setSalary("joadfghjo", 325));
    assert(b4.setSalary("jadfghojo", 6746));
    assert(b4.setSalary("jhjoqertzujo", 6813));
    assert(b4.setSalary("jojhgfjo", 561));

    assert(b4.getSalary("jmnbojo") == 57744);
    assert(b4.getSalary("jhgfojo") == 65441);
    assert(b4.getSalary("jhgvjo") == 8552);
    assert(b4.getSalary("jogfvjo") == 684874);
    assert(b4.getSalary("jofvjo") == 91187);
    assert(b4.getSalary("jfbcojo") == 84452);
    assert(b4.getSalary("jopojo") == 65114);
    assert(b4.getSalary("jlkjnnojo") == 1426);
    assert(b4.getSalary("jolkjhjo") == 6565);
    assert(b4.getSalary("jopoikjhugfjo") == 5124);
    assert(b4.getSalary("jonbvcjo") == 45445);
    assert(b4.getSalary("jogfdjo") == 9854);
    assert(b4.getSalary("johgfvcjo") == 6545);
    assert(b4.getSalary("jogfcjo") == 5524);
    assert(b4.getSalary("jofvdxjo") == 522);
    assert(b4.getSalary("jooiuhzgjo") == 5245);
    assert(b4.getSalary("jojjhgfco") == 5214);
    assert(b4.getSalary("jpoijujhgojo") == 5747);
    assert(b4.getSalary("joqwerjo") == 937);
    assert(b4.getSalary("jadfggojo") == 7411);
    assert(b4.getSalary("joadfghjo") == 325);
    assert(b4.getSalary("jadfghojo") == 6746);
    assert(b4.getSalary("jhjoqertzujo") == 6813);
    assert(b4.getSalary("jojhgfjo") == 561);
    assert(b4.getSalary("jmnbojo") == 57744);
    assert(b4.getSalary("jhgfojo") == 65441);
    assert(b4.getSalary("jhgvjo") == 8552);
    assert(b4.getSalary("jogfvjo") == 684874);
    assert(b4.getSalary("jofvjo") == 91187);
    assert(b4.getSalary("jfbcojo") == 84452);
    assert(b4.getSalary("jopojo") == 65114);

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */