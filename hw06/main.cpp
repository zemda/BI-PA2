#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>
using namespace std;

class CRect {
public:
    CRect(double x,double y,double w,double h): m_X(x),m_Y(y),m_W(w),m_H(h) {}

    friend ostream &operator<<(ostream &os, const CRect &x) {
        return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
    }

    double m_X;
    double m_Y;
    double m_W;
    double m_H;
};
#endif /* __PROGTEST__ */

class CWindow;
class Panel{
public:
    Panel(int id, const CRect &relPos) : m_id(id), m_relPos(relPos), m_absPos(nullptr) {}
    virtual void print(ostream &os, const string &indent = "") const = 0;

    virtual ~Panel() = default;
    virtual Panel *clone() const = 0;
    friend ostream &operator<<(ostream &os, const Panel &panel){
        panel.print(os);
        return os;
    }

    int m_id;
    CRect m_relPos;
    CRect* m_absPos;
};

class CWindow{
public:
    CWindow(int id, const string &title, const CRect &absPos) : m_id(id), m_title(title), m_absPos(absPos) {}
    CWindow(const CWindow &other) : m_id(other.m_id), m_title(other.m_title), m_absPos(other.m_absPos), m_elements() {
        for (const auto &element : other.m_elements) {
            Panel *temp = element->clone();
            temp->m_absPos = &m_absPos;
            m_elements.push_back(temp);
        }
    }
    ~CWindow() {
        for (Panel *element : m_elements)
            delete element;
    }
    CWindow &add(const Panel &panel) {
        m_elements.push_back(panel.clone());
        m_elements.back()->m_absPos = &m_absPos;
        return *this;
    }
    Panel *search(int id) {
        for (Panel *panel : m_elements) if (panel->m_id == id) return panel;
        return nullptr;
    }
    void setPosition(const CRect &pos) { m_absPos = pos; }
    CWindow &operator=(const CWindow &other) {
        if (this == &other) return *this;

        m_id = other.m_id;
        m_title = other.m_title;
        m_absPos = other.m_absPos;

        for (auto &element : m_elements) delete element;
        m_elements.clear();

        for (const auto &element : other.m_elements) {
            Panel *temp = element->clone();
            temp->m_absPos = &m_absPos;
            m_elements.push_back(temp);
        }

        return *this;
    }


    friend ostream &operator<<(ostream &os, const CWindow &win) {
        os << "[" << win.m_id << "] Window \"" << win.m_title << "\" (" << win.m_absPos.m_X << "," << win.m_absPos.m_Y << "," << win.m_absPos.m_W << "," << win.m_absPos.m_H << ")\n";

        for (size_t i = 0; i < win.m_elements.size(); ++i) {
            os << "+- ";
            win.m_elements[i]->print(os, (i + 1 < win.m_elements.size()) ? "|  " : "   ");
        }
        return os;
    }

private:
    int m_id;
    string m_title;
    CRect m_absPos;
    vector<Panel*> m_elements;
};

class CButton : public Panel {
public:
    CButton(int id, const CRect &relPos, const string &name) : Panel(id, relPos), m_name(name) {}
    CButton(const CButton &other) : Panel(other.m_id, other.m_relPos), m_name(other.m_name) {}

    void print(ostream &os, const string &indent = "") const override {
        os << "[" << m_id << "] Button \"" << m_name << "\" (" << m_absPos->m_X + m_relPos.m_X * m_absPos->m_W << ","
           << m_absPos->m_Y + m_relPos.m_Y * m_absPos->m_H << "," << m_relPos.m_W * m_absPos->m_W << "," << m_relPos.m_H * m_absPos->m_H << ")\n";
    }
    Panel *clone() const override { return new CButton(*this); }

    CButton &operator=(const CButton &other) {
        if (this == &other) return *this;

        Panel::operator=(other);
        m_name = other.m_name;

        return *this;
    }


private:
    string m_name;
};

class CInput : public Panel {
public:
    CInput(int id, const CRect &relPos, const string &value) : Panel(id, relPos), m_value(value) {}
    CInput(const CInput &other) : Panel(other.m_id, other.m_relPos), m_value(other.m_value) {}

    void print(ostream &os, const string &indent = "") const override {
        os << "[" << m_id << "] Input \"" << m_value << "\" (" << m_absPos->m_X + m_relPos.m_X * m_absPos->m_W << ","
           << m_absPos->m_Y + m_relPos.m_Y * m_absPos->m_H << "," << m_relPos.m_W * m_absPos->m_W << "," << m_relPos.m_H * m_absPos->m_H << ")\n";
    }
    Panel *clone() const override { return new CInput(*this); }
    void setValue(const string &value) { m_value = value; }
    string getValue() const { return m_value; }

    CInput &operator=(const CInput &other) {
        if (this == &other) return *this;

        Panel::operator=(other);
        m_value = other.m_value;

        return *this;
    }


private:
    string m_value;
};

class CLabel : public Panel {
public:
    CLabel(int id, const CRect &relPos, const string &label) : Panel(id, relPos), m_label(label) {}
    CLabel(const CLabel &other) : Panel(other.m_id, other.m_relPos), m_label(other.m_label) {}

    void print(ostream &os, const string &indent = "") const override {
        os << "[" << m_id << "] Label \"" << m_label << "\" (" << m_absPos->m_X + m_relPos.m_X * m_absPos->m_W << ","
           << m_absPos->m_Y + m_relPos.m_Y * m_absPos->m_H << ","<< m_relPos.m_W * m_absPos->m_W << "," << m_relPos.m_H * m_absPos->m_H << ")\n";
    }
    Panel *clone() const override { return new CLabel(*this); }

    CLabel &operator=(const CLabel &other) {
        if (this == &other) return *this;

        Panel::operator=(other);
        m_label = other.m_label;

        return *this;
    }


private:
    string m_label;
};

class CComboBox : public Panel {
public:
    CComboBox(int id, const CRect &relPos) : Panel(id, relPos), m_selected(0) {}
    CComboBox(const CComboBox &other) : Panel(other.m_id, other.m_relPos), m_selected(other.m_selected), m_items(other.m_items) {}

    CComboBox &add(const string &item) {
        m_items.push_back(item);
        return *this;
    }
    void print(ostream &os, const string &indent) const override {
        os  << "[" << m_id << "] ComboBox ("
            << m_absPos->m_X + m_relPos.m_X * m_absPos->m_W << "," << m_absPos->m_Y + m_relPos.m_Y * m_absPos->m_H  << ","
            << m_relPos.m_W * m_absPos->m_W                 << "," << m_relPos.m_H * m_absPos->m_H                  << ")\n";
        for (int i = 0; i < (int)m_items.size(); ++i)
            os << (i == m_selected ? indent + "+->" : indent + "+- ") << m_items[i] << (i == m_selected ? "<\n" : "\n");

    }
    Panel *clone() const override { return new CComboBox(*this); }
    int getSelected() const { return m_selected; }
    void setSelected(int index) { if (index >= 0 && index < (int)m_items.size()) m_selected = index; }

    CComboBox &operator=(const CComboBox &other) {
        if (this == &other) return *this;

        Panel::operator=(other);
        m_selected = other.m_selected;
        m_items = other.m_items;

        return *this;
    }

private:
    int m_selected;
    vector<string> m_items;
};

#ifndef __PROGTEST__
template <typename _T>
string toString ( const _T & x )
{
    ostringstream oss;
    oss << x;
    return oss . str ();
}

int main ( void )
{
    assert ( sizeof ( CButton ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
    assert ( sizeof ( CInput ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
    assert ( sizeof ( CLabel ) - sizeof ( string ) < sizeof ( CComboBox ) - sizeof ( vector<string> ) );
    CWindow a ( 0, "Sample window", CRect ( 10, 10, 600, 480 ) );
    a . add ( CButton ( 1, CRect ( 0.1, 0.8, 0.3, 0.1 ), "Ok" ) ) . add ( CButton ( 2, CRect ( 0.6, 0.8, 0.3, 0.1 ), "Cancel" ) );
    a . add ( CLabel ( 10, CRect ( 0.1, 0.1, 0.2, 0.1 ), "Username:" ) );
    a . add ( CInput ( 11, CRect ( 0.4, 0.1, 0.5, 0.1 ), "chucknorris" ) );
    a . add ( CComboBox ( 20, CRect ( 0.1, 0.3, 0.8, 0.1 ) ) . add ( "Karate" ) . add ( "Judo" ) . add ( "Box" ) . add ( "Progtest" ) );
    assert ( toString ( a ) ==
             "[0] Window \"Sample window\" (10,10,600,480)\n"
             "+- [1] Button \"Ok\" (70,394,180,48)\n"
             "+- [2] Button \"Cancel\" (370,394,180,48)\n"
             "+- [10] Label \"Username:\" (70,58,120,48)\n"
             "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
             "+- [20] ComboBox (70,154,480,48)\n"
             "   +->Karate<\n"
             "   +- Judo\n"
             "   +- Box\n"
             "   +- Progtest\n" );
    CWindow b = a;
    assert ( toString ( *b . search ( 20 ) ) ==
             "[20] ComboBox (70,154,480,48)\n"
             "+->Karate<\n"
             "+- Judo\n"
             "+- Box\n"
             "+- Progtest\n" );
    assert ( dynamic_cast<CComboBox &> ( *b . search ( 20 ) ) . getSelected () == 0 );
    dynamic_cast<CComboBox &> ( *b . search ( 20 ) ) . setSelected ( 3 );
    assert ( dynamic_cast<CInput &> ( *b . search ( 11 ) ) . getValue () == "chucknorris" );
    dynamic_cast<CInput &> ( *b . search ( 11 ) ) . setValue ( "chucknorris@fit.cvut.cz" );
    b . add ( CComboBox ( 21, CRect ( 0.1, 0.5, 0.8, 0.1 ) ) . add ( "PA2" ) . add ( "OSY" ) . add ( "Both" ) );
    assert ( toString ( b ) ==
             "[0] Window \"Sample window\" (10,10,600,480)\n"
             "+- [1] Button \"Ok\" (70,394,180,48)\n"
             "+- [2] Button \"Cancel\" (370,394,180,48)\n"
             "+- [10] Label \"Username:\" (70,58,120,48)\n"
             "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
             "+- [20] ComboBox (70,154,480,48)\n"
             "|  +- Karate\n"
             "|  +- Judo\n"
             "|  +- Box\n"
             "|  +->Progtest<\n"
             "+- [21] ComboBox (70,250,480,48)\n"
             "   +->PA2<\n"
             "   +- OSY\n"
             "   +- Both\n" );
    assert ( toString ( a ) ==
             "[0] Window \"Sample window\" (10,10,600,480)\n"
             "+- [1] Button \"Ok\" (70,394,180,48)\n"
             "+- [2] Button \"Cancel\" (370,394,180,48)\n"
             "+- [10] Label \"Username:\" (70,58,120,48)\n"
             "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
             "+- [20] ComboBox (70,154,480,48)\n"
             "   +->Karate<\n"
             "   +- Judo\n"
             "   +- Box\n"
             "   +- Progtest\n" );
    b . setPosition ( CRect ( 20, 30, 640, 520 ) );
    assert ( toString ( b ) ==
             "[0] Window \"Sample window\" (20,30,640,520)\n"
             "+- [1] Button \"Ok\" (84,446,192,52)\n"
             "+- [2] Button \"Cancel\" (404,446,192,52)\n"
             "+- [10] Label \"Username:\" (84,82,128,52)\n"
             "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
             "+- [20] ComboBox (84,186,512,52)\n"
             "|  +- Karate\n"
             "|  +- Judo\n"
             "|  +- Box\n"
             "|  +->Progtest<\n"
             "+- [21] ComboBox (84,290,512,52)\n"
             "   +->PA2<\n"
             "   +- OSY\n"
             "   +- Both\n" );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
