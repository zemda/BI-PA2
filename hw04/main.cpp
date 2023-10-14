#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
#endif /* __PROGTEST__ */

class s_string{
public:
    s_string(){
        size = 0;
        str = nullptr;
    }
    s_string(const char* src){
        size = strlen(src) + 1;
        str = new char[size];
        strcpy(str, src);
    }
    ~s_string(){
        delete[] str;
    }

    bool compare(const s_string &x)const{
        return strcmp(str, x.str) == 0;
    }

    s_string& operator=(const s_string& x) {
        if (this != &x) {
            size = x.size + 1;
            delete[] str;
            str = new char[size];
            strcpy(str, x.str);
        }
        return *this;
    }

    size_t size;
    char *str;
};

class CMail {
public:
    CMail() = default;
    CMail(const char *from, const char *to, const char *body) : m_from(from), m_to(to), m_body(body){
    }

    bool operator==(const CMail &x) const{
        return m_from.compare(x.m_from) && m_to.compare(x.m_to) && m_body.compare(x.m_body);
    }

    friend ostream &operator<<(ostream &os, const CMail &m){
        os << "From: " << m.m_from.str << ", To: " << m.m_to.str << ", Body: " << m.m_body.str;
        return os;
    }

    CMail& operator=(const CMail &x){
        if (this != &x) {
            m_from = x.m_from;
            m_to = x.m_to;
            m_body = x.m_body;
        }
        return *this;
    }

    s_string m_from;
    s_string m_to;
    s_string m_body;
};

class arr{
public:
    arr(){
        size = 0;
        capacity = 0;
        data = nullptr;
    }

    ~arr(){
        delete[] data;
    }

    arr &operator=(const arr &src){
        if (this != &src) {
            size = src.size;
            capacity = src.capacity;
            delete[] data;
            data = new CMail[capacity];
            for (int i = 0; i < size; ++i) data[i] = src.data[i];
        }
        return *this;
    }

    void push_back(const CMail &x){
        if (size == capacity) {
            capacity = capacity * 2 + 1;
            auto *new_data = new CMail[capacity];
            for (int i = 0; i < size; ++i) new_data[i] = data[i];
            delete[] data;
            data = new_data;
        }
        data[size] = x;
        size++;
    }

    CMail &operator[](int i) const{
        return data[i];
    }

    int size;
    int capacity;
    CMail *data;
};

class CMailIterator {
public:
    friend class CMailServer;

    CMailIterator(const arr &mails, int index){
        m_mails = mails;
        m_index = index;
    }

    explicit operator bool() const{
        return m_index < m_mails.size;
    }

    bool operator!() const{
        return !(bool (*this));
    }

    const CMail &operator*() const{
        return m_mails[m_index];
    }

    CMailIterator &operator++(){
        if (m_index == m_mails.size - 1) m_index = m_mails.size;
        else m_index++;
        return *this;
    }

private:
    arr m_mails;
    int m_index;
};

class CMailServer {
public:
    CMailServer() = default;
    CMailServer(const CMailServer &src){
        m_mails = src.m_mails;
    }

    CMailServer &operator=(const CMailServer &src){
        if (this == &src) return *this;
        m_mails = src.m_mails;
        return *this;
    }

    ~CMailServer()= default;

    void sendMail(const CMail &m){
        m_mails.push_back(m);
    }

    CMailIterator outbox(const char *email) const{
        arr out;
        for (int i = 0; i < m_mails.size; ++i)
            if (m_mails[i].m_from.compare(email))
                out.push_back(m_mails[i]);

        return CMailIterator(out, 0);
    }

    CMailIterator inbox(const char *email) const{
        arr inbox;
        for (int i = 0; i < m_mails.size; ++i)
            if (m_mails[i].m_to.compare(email))
                inbox.push_back(m_mails[i]);

        return CMailIterator(inbox, 0);
    }

private:
    arr m_mails;
};



#ifndef __PROGTEST__

bool matchOutput(const CMail &m, const char *str) {
    ostringstream oss;
    oss << m;
    return oss.str() == str;
}

int main ()
{
    char from[100], to[100], body[1024];

    assert ( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "john", "peter", "progtest deadline" ) );
    assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "john", "progtest deadline", "peter" ) ) );
    assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "peter", "john", "progtest deadline" ) ) );
    assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "peter", "progtest deadline", "john" ) ) );
    assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "progtest deadline", "john", "peter" ) ) );
    assert ( !( CMail ( "john", "peter", "progtest deadline" ) == CMail ( "progtest deadline", "peter", "john" ) ) );
    CMailServer s0;
    s0 . sendMail ( CMail ( "john", "peter", "some important mail" ) );
    strncpy ( from, "john", sizeof ( from ) );
    strncpy ( to, "thomas", sizeof ( to ) );
    strncpy ( body, "another important mail", sizeof ( body ) );
    s0 . sendMail ( CMail ( from, to, body ) );
    strncpy ( from, "john", sizeof ( from ) );
    strncpy ( to, "alice", sizeof ( to ) );
    strncpy ( body, "deadline notice", sizeof ( body ) );
    s0 . sendMail ( CMail ( from, to, body ) );
    s0 . sendMail ( CMail ( "alice", "john", "deadline confirmation" ) );
    s0 . sendMail ( CMail ( "peter", "alice", "PR bullshit" ) );
    CMailIterator i0 = s0 . inbox ( "alice" );
    assert ( i0 && *i0 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i0,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i0 && *i0 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i0,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ! ++i0 );

    CMailIterator i1 = s0 . inbox ( "john" );
    assert ( i1 && *i1 == CMail ( "alice", "john", "deadline confirmation" ) );
    assert ( matchOutput ( *i1,  "From: alice, To: john, Body: deadline confirmation" ) );
    assert ( ! ++i1 );

    CMailIterator i2 = s0 . outbox ( "john" );
    assert ( i2 && *i2 == CMail ( "john", "peter", "some important mail" ) );
    assert ( matchOutput ( *i2,  "From: john, To: peter, Body: some important mail" ) );
    assert ( ++i2 && *i2 == CMail ( "john", "thomas", "another important mail" ) );
    assert ( matchOutput ( *i2,  "From: john, To: thomas, Body: another important mail" ) );
    assert ( ++i2 && *i2 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i2,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ! ++i2 );

    CMailIterator i3 = s0 . outbox ( "thomas" );
    assert ( ! i3 );

    CMailIterator i4 = s0 . outbox ( "steve" );
    assert ( ! i4 );

    CMailIterator i5 = s0 . outbox ( "thomas" );
    s0 . sendMail ( CMail ( "thomas", "boss", "daily report" ) );
    assert ( ! i5 );

    CMailIterator i6 = s0 . outbox ( "thomas" );
    assert ( i6 && *i6 == CMail ( "thomas", "boss", "daily report" ) );
    assert ( matchOutput ( *i6,  "From: thomas, To: boss, Body: daily report" ) );
    assert ( ! ++i6 );

    CMailIterator i7 = s0 . inbox ( "alice" );
    s0 . sendMail ( CMail ( "thomas", "alice", "meeting details" ) );
    assert ( i7 && *i7 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i7,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i7 && *i7 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i7,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ! ++i7 );

    CMailIterator i8 = s0 . inbox ( "alice" );
    assert ( i8 && *i8 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i8,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i8 && *i8 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i8,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ++i8 && *i8 == CMail ( "thomas", "alice", "meeting details" ) );
    assert ( matchOutput ( *i8,  "From: thomas, To: alice, Body: meeting details" ) );
    assert ( ! ++i8 );

    CMailServer s1 ( s0 );
    s0 . sendMail ( CMail ( "joe", "alice", "delivery details" ) );
    s1 . sendMail ( CMail ( "sam", "alice", "order confirmation" ) );
    CMailIterator i9 = s0 . inbox ( "alice" );
    assert ( i9 && *i9 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i9,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i9 && *i9 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i9,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ++i9 && *i9 == CMail ( "thomas", "alice", "meeting details" ) );
    assert ( matchOutput ( *i9,  "From: thomas, To: alice, Body: meeting details" ) );
    assert ( ++i9 && *i9 == CMail ( "joe", "alice", "delivery details" ) );
    assert ( matchOutput ( *i9,  "From: joe, To: alice, Body: delivery details" ) );
    assert ( ! ++i9 );

    CMailIterator i10 = s1 . inbox ( "alice" );
    assert ( i10 && *i10 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i10,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i10 && *i10 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i10,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ++i10 && *i10 == CMail ( "thomas", "alice", "meeting details" ) );
    assert ( matchOutput ( *i10,  "From: thomas, To: alice, Body: meeting details" ) );
    assert ( ++i10 && *i10 == CMail ( "sam", "alice", "order confirmation" ) );
    assert ( matchOutput ( *i10,  "From: sam, To: alice, Body: order confirmation" ) );
    assert ( ! ++i10 );

    CMailServer s2;
    s2 . sendMail ( CMail ( "alice", "alice", "mailbox test" ) );
    CMailIterator i11 = s2 . inbox ( "alice" );
    assert ( i11 && *i11 == CMail ( "alice", "alice", "mailbox test" ) );
    assert ( matchOutput ( *i11,  "From: alice, To: alice, Body: mailbox test" ) );
    assert ( ! ++i11 );

    s2 = s0;
    s0 . sendMail ( CMail ( "steve", "alice", "newsletter" ) );
    s2 . sendMail ( CMail ( "paul", "alice", "invalid invoice" ) );
    CMailIterator i12 = s0 . inbox ( "alice" );
    assert ( i12 && *i12 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i12,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i12 && *i12 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i12,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ++i12 && *i12 == CMail ( "thomas", "alice", "meeting details" ) );
    assert ( matchOutput ( *i12,  "From: thomas, To: alice, Body: meeting details" ) );
    assert ( ++i12 && *i12 == CMail ( "joe", "alice", "delivery details" ) );
    assert ( matchOutput ( *i12,  "From: joe, To: alice, Body: delivery details" ) );
    assert ( ++i12 && *i12 == CMail ( "steve", "alice", "newsletter" ) );
    assert ( matchOutput ( *i12,  "From: steve, To: alice, Body: newsletter" ) );
    assert ( ! ++i12 );

    CMailIterator i13 = s2 . inbox ( "alice" );
    assert ( i13 && *i13 == CMail ( "john", "alice", "deadline notice" ) );
    assert ( matchOutput ( *i13,  "From: john, To: alice, Body: deadline notice" ) );
    assert ( ++i13 && *i13 == CMail ( "peter", "alice", "PR bullshit" ) );
    assert ( matchOutput ( *i13,  "From: peter, To: alice, Body: PR bullshit" ) );
    assert ( ++i13 && *i13 == CMail ( "thomas", "alice", "meeting details" ) );
    assert ( matchOutput ( *i13,  "From: thomas, To: alice, Body: meeting details" ) );
    assert ( ++i13 && *i13 == CMail ( "joe", "alice", "delivery details" ) );
    assert ( matchOutput ( *i13,  "From: joe, To: alice, Body: delivery details" ) );
    assert ( ++i13 && *i13 == CMail ( "paul", "alice", "invalid invoice" ) );
    assert ( matchOutput ( *i13,  "From: paul, To: alice, Body: invalid invoice" ) );
    assert ( ! ++i13 );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */