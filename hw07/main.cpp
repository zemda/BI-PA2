#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template <typename M_>
class CContest
{
public:
    CContest &addMatch(const string &contestant1, const string &contestant2, const M_ &result)
    {
        if (matches.count({contestant1, contestant2}) || matches.count({contestant2, contestant1}) || contestant1 == contestant2)
            throw logic_error("duplicate match");
        matches.emplace(make_pair(contestant1, contestant2), result);
        return *this;
    }

    bool isOrdered(function<int(M_)> comparator) const
    {
        try
        {
            results(comparator);
        }
        catch (const logic_error &e)
        {
            return false;
        }
        return true;
    }

    list<string> results(function<int(M_)> comparator) const
    {
        map<string, unordered_set<string>> graph;
        for (auto &&[key, result] : matches)
        {
            int res = comparator(result);
            if (res > 0)
                graph[key.first].insert(key.second);
            else if (res < 0)
                graph[key.second].insert(key.first);
            else
                throw logic_error("er");
        }
        
        map<string, int> in_degree;
        for (const auto &vertex : graph)
            in_degree[vertex.first] = 0;

        for (const auto &vertex : graph)
            for (const auto &successor : vertex.second)
                ++in_degree[successor];


        queue<string> sources;
        for (const auto &vertex : in_degree)
            if (vertex.second == 0)
                sources.push(vertex.first);

        list<string> list_result;
        while (!sources.empty())
        {
            if (sources.size() > 1)
                throw logic_error("er");

            const string &vertex = sources.front();
            sources.pop();
            list_result.push_back(vertex);

            for (const auto &successor : graph[vertex])
                if (--in_degree[successor] == 0)
                    sources.push(successor);
        }
        if (list_result.size() != graph.size())
            throw logic_error("er");
        return list_result;
    }

private:
    map<pair<string, string>, M_> matches;
};

#ifndef __PROGTEST__
struct CMatch
{
public:
    CMatch(int a,
           int b)
        : m_A(a),
          m_B(b)
    {
    }

    int m_A;
    int m_B;
};
class HigherScoreThreshold
{
public:
    HigherScoreThreshold(int diffAtLeast)
        : m_DiffAtLeast(diffAtLeast)
    {
    }
    int operator()(const CMatch &x) const
    {
        return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
    }

private:
    int m_DiffAtLeast;
};
int HigherScore(const CMatch &x)
{
    return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}
int main()
{
    CContest<CMatch> x;

    x.addMatch("C++", "Pascal", CMatch(10, 3))
        .addMatch("C++", "Java", CMatch(8, 1))
        .addMatch("Pascal", "Basic", CMatch(40, 0))
        .addMatch("Java", "PHP", CMatch(6, 2))
        .addMatch("Java", "Pascal", CMatch(7, 3))
        .addMatch("PHP", "Basic", CMatch(10, 0));

    assert(!x.isOrdered(HigherScore));
    try
    {
        list<string> res = x.results(HigherScore);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }

    x.addMatch("PHP", "Pascal", CMatch(3, 6));

    assert(x.isOrdered(HigherScore));
    try
    {
        list<string> res = x.results(HigherScore);
        assert((res == list<string>{"C++", "Java", "Pascal", "PHP", "Basic"}));
    }
    catch (...)
    {
        assert("Unexpected exception!" == nullptr);
    }

    assert(!x.isOrdered(HigherScoreThreshold(3)));
    try
    {
        list<string> res = x.results(HigherScoreThreshold(3));
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }

    assert(x.isOrdered([](const CMatch &x)
                       { return (x.m_A < x.m_B) - (x.m_B < x.m_A); }));
    try
    {
        list<string> res = x.results([](const CMatch &x)
                                     { return (x.m_A < x.m_B) - (x.m_B < x.m_A); });
        assert((res == list<string>{"Basic", "PHP", "Pascal", "Java", "C++"}));
    }
    catch (...)
    {
        assert("Unexpected exception!" == nullptr);
    }

    CContest<bool> y;

    y.addMatch("Python", "PHP", true)
        .addMatch("PHP", "Perl", true)
        .addMatch("Perl", "Bash", true)
        .addMatch("Bash", "JavaScript", true)
        .addMatch("JavaScript", "VBScript", true);

    assert(y.isOrdered([](bool v)
                       { return v ? 10 : -10; }));
    try
    {
        list<string> res = y.results([](bool v)
                                     { return v ? 10 : -10; });
        assert((res == list<string>{"Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript"}));
    }
    catch (...)
    {
        assert("Unexpected exception!" == nullptr);
    }

    y.addMatch("PHP", "JavaScript", false);
    assert(!y.isOrdered([](bool v)
                        { return v ? 10 : -10; }));
    try
    {
        list<string> res = y.results([](bool v)
                                     { return v ? 10 : -10; });
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }

    try
    {
        y.addMatch("PHP", "JavaScript", false);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }

    try
    {
        y.addMatch("JavaScript", "PHP", true);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
