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

struct ip_t
{
    union ipv4_t
    {
        tuple<unsigned char, unsigned char, unsigned char, unsigned char> bytes_value;
        unsigned int uint_value;

        ipv4_t() : bytes_value(0,0,0,0)
        {

        }

        string to_string() const
        {
            string s;
            s = std::to_string(std::get<0>(bytes_value)); s += ".";
            s += std::to_string(std::get<1>(bytes_value)); s += ".";
            s += std::to_string(std::get<2>(bytes_value)); s += ".";
            s += std::to_string(std::get<3>(bytes_value));
            return s;
        }

        ipv4_t(const ipv4_t& bro) : uint_value(bro.uint_value)
        {

        }
    };

    ipv4_t m_value;

    ip_t(const ip_t& bro) : m_value(bro.m_value)
    {
    }

    ip_t(string ip)
    {
        vector<string> vs = split(ip, '.');
        unsigned ix = 0;
        int i;
        for (auto n : vs)
        {
            try
            {
                i = std::stoi(n);
            }
            catch (...)
            {
                // -1 indicate wrong input
                i = 0;
            }
            switch (ix)
            {
            case 0:
                std::get<0>(m_value.bytes_value) = i;
                break;
            case 1:
                std::get<1>(m_value.bytes_value) = i;
                break;
            case 2:
                std::get<2>(m_value.bytes_value) = i;
                break;
            case 3:
                std::get<3>(m_value.bytes_value) = i;
                break;
            }
            ix++;
        }
    }

    ip_t(int c0, int c1, int c2, int c3)
    {
        std::get<0>(m_value.bytes_value) = c0;
        std::get<1>(m_value.bytes_value) = c1;
        std::get<2>(m_value.bytes_value) = c2;
        std::get<3>(m_value.bytes_value) = c3;
    }

    ip_t(vector<int> v)
    {
        m_value.uint_value = 0;
        if (v.size() > 0) std::get<0>(m_value.bytes_value) = v[0];
        if (v.size() > 1) std::get<1>(m_value.bytes_value) = v[1];
        if (v.size() > 2) std::get<2>(m_value.bytes_value) = v[2];
        if (v.size() > 3) std::get<3>(m_value.bytes_value) = v[3];
    }

    bool start_with(vector<int> v) const
    {
        unsigned ix = 0;
        auto it = v.cbegin();
        while (it != v.cend())
        {
            switch (ix)
            {
            case 0:
                if ( std::get<0>(m_value.bytes_value ) != *it) return false;
                break;
            case 1:
                if (std::get<1>(m_value.bytes_value) != *it) return false;
                break;
            case 2:
                if (std::get<2>(m_value.bytes_value) != *it) return false;
                break;
            case 3:
                if (std::get<3>(m_value.bytes_value) != *it) return false;
                break;
            }
            ix++;
            it++;
        }
        return true;
    }

    bool operator<(const ip_t& bro) const
    {
        return m_value.uint_value < bro.m_value.uint_value;
    }

    bool find(int uch)
    {
        if (uch == std::get<0>(m_value.bytes_value)
            or uch == std::get<1>(m_value.bytes_value)
            or uch == std::get<2>(m_value.bytes_value)
            or uch == std::get<3>(m_value.bytes_value))
        {
            return true;
        }
        return false;
    }
    friend ostream& operator<< (std::ostream& os, const ip_t& ip);
};

ostream& operator<< (std::ostream& os, const ip_t& ip)
{
    os << ip.m_value.to_string();
    return os;
}

struct ip_set_t : public multiset<ip_t>
{
public:

    friend ostream& operator<< (ostream&, const ip_set_t&);

    ip_set_t filter_any(int i)
    {
        ip_set_t subset;
        std::for_each(begin(), end(), [&](auto ip) {if (ip.find(i)) subset.insert(ip); });
        return subset;
    }

    ip_set_t filter(int uch)
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
    ip_set_t filter(int uch, Types... Other)
    {
        search_v.push_back(uch);
        return filter(Other...);
    }
private:
    vector<int> search_v;
};

ostream& operator<< (ostream& os, const ip_set_t& ip_set)
{
    std::for_each(ip_set.rbegin(), ip_set.rend(), [&](decltype(*ip_set.rend())& ip)
        {
         cout << ip; cout << std::endl;
        });
    return os;
}

int main(int argc, char const *argv[])
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

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.1n98.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        auto ip_set_1 = ip_pool.filter(1);
        cout << ip_set_1;

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        auto ip_set_2 = ip_pool.filter(46,70);
        cout << ip_set_2;

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        auto ip_set_3 = ip_pool.filter_any(46);
        cout << ip_set_3;

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
