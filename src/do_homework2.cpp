#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <tuple>

using std::string;
using std::cout;
using std::vector;
using std::ostream;
using std::tuple;
using std::multiset;

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
vector<string> split(const string& str, char d)
{
    vector<string> r;
    string::size_type start = 0;
    string::size_type stop = str.find_first_of(d);
    while (stop != string::npos)
    {
        r.push_back(str.substr(start, stop - start));
        start = stop + 1;
        stop = str.find_first_of(d, start);
    }
    r.push_back(str.substr(start));
    return r;
}

struct ip_t : public tuple<unsigned char, unsigned char, unsigned char, unsigned char>
{
    string to_string() const
    {
        string s;
        s = std::to_string(std::get<0>(*this)); s += ".";
        s += std::to_string(std::get<1>(*this)); s += ".";
        s += std::to_string(std::get<2>(*this)); s += ".";
        s += std::to_string(std::get<3>(*this));
        return s;
    }

    ip_t(string ip)
    {
        vector<string> vs = split(ip, '.');
        unsigned ix = 0;
        int i;
        unsigned char uch;
        for (auto n : vs)
        {
            try
            {
                i = std::stoi(n);
                uch = static_cast<unsigned char>(i);
            }
            catch (...)
            {
                // -1 indicate wrong input
                i = 0;
            }
            switch (ix)
            {
            case 0:
                std::get<0>(*this) = uch;
                break;
            case 1:
                std::get<1>(*this) = uch;
                break;
            case 2:
                std::get<2>(*this) = uch;
                break;
            case 3:
                std::get<3>(*this) = uch;
                break;
            }
            ix++;
        }
    }

    ip_t(vector<unsigned char> v)
    {
        if (v.size() > 0) std::get<0>(*this) = v[0];
        if (v.size() > 1) std::get<1>(*this) = v[1];
        if (v.size() > 2) std::get<2>(*this) = v[2];
        if (v.size() > 3) std::get<3>(*this) = v[3];
    }

    bool start_with(vector<unsigned char> v) const
    {
        unsigned ix = 0;
        auto it = v.cbegin();
        while (it != v.end())
        {
           switch (ix)
            {
            case 0:
                if ( std::get<0>(*this ) != *it) return false;
                break;
            case 1:
                if (std::get<1>(*this) != *it) return false;
               break;
            case 2:
                if (std::get<2>(*this) != *it) return false;
                break;
            case 3:
                if (std::get<3>(*this) != *it) return false;
            }
            ix++;
            it++;
        }
        return true;
    }

    bool find(unsigned char uch)
    {
        if (uch == std::get<0>(*this)
            or uch == std::get<1>(*this)
            or uch == std::get<2>(*this)
            or uch == std::get<3>(*this))
        {
            return true;
        }
        return false;
    }
    friend ostream& operator<< (std::ostream& os, const ip_t& ip);
};

ostream& operator<< (std::ostream& os, const ip_t& ip)
{
    os << ip.to_string();
    return os;
}

struct ip_set_t : public multiset<ip_t>
{
    friend ostream& operator<< (ostream&, const ip_set_t&);

    ip_set_t filter_any(unsigned char i)
    {
        ip_set_t subset;
        std::for_each(begin(), end(), [&](auto ip) {if (ip.find(i)) subset.insert(ip); });
        return subset;
    }

    ip_set_t filter(unsigned char uch)
    {
        // last link in recursive chain
        ip_set_t subset;
        search_v.push_back(uch);
        ip_t search_ip(search_v);
        auto it = this->lower_bound(search_ip);
        while (it != this->end())
        {
            if (it->start_with(search_v) )
            {
                subset.insert(*it);
            }
            else
            {
                break;
            }
            it++;
        }
        // clear search_v to be prepared for the next filter call:
        search_v.clear();
        return subset;
    }

    template<typename... Types>
    ip_set_t filter(unsigned char uch, Types... Other)
    {
        search_v.push_back(uch);
        return filter(Other...);
    }
private:
    vector<unsigned char> search_v;
};

ostream& operator<< (ostream& os, const ip_set_t& ip_set)
{
    std::for_each(ip_set.rbegin(), ip_set.rend(), [&](decltype(*ip_set.rend())& ip)
        {
         cout << ip; cout << std::endl;
        });
    return os;
}

void do_homework2()
{
    try
    {
        ip_set_t ip_pool;
        for(std::string line; std::getline(std::cin, line);)
        {
            vector<string> vs = split(line, '\t');
            if (vs.size() > 0)
            {
                ip_t ip(vs[0]);
                ip_pool.insert(ip);
            }
        }
        cout << ip_pool;
        auto ip_set_1 = ip_pool.filter(1);
        cout << ip_set_1;
        auto ip_set_2 = ip_pool.filter(46,70);
        cout << ip_set_2;
        auto ip_set_3 = ip_pool.filter_any(46);
        cout << ip_set_3;
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
