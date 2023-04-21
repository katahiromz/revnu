//////////////////////////////////////////////////////////////////////////////
// revnu --- reversible numeric for C++11

#pragma once

#include <string>
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <stdexcept>

namespace katahiromz
{

//////////////////////////////////////////////////////////////////////////////
// to_str_t --- glue between std::to_string and std::to_wstring

template <typename T_VALUE, typename T_CHAR>
struct to_str_t
{
    static_assert(sizeof(T_CHAR) == 0, "specialization is failed");
};

template <typename T_VALUE>
struct to_str_t<T_VALUE, char>
{
    std::basic_string<char> operator()(const T_VALUE& value)
    {
        return std::to_string(value);
    }
};

template <typename T_VALUE>
struct to_str_t<T_VALUE, wchar_t>
{
    std::basic_string<wchar_t> operator()(const T_VALUE& value)
    {
        return std::to_wstring(value);
    }
};

//////////////////////////////////////////////////////////////////////////////
// print_t

template <typename T_CHAR>
struct print_t
{
    static_assert(sizeof(T_CHAR) == 0, "specialization is failed");
};

template <>
struct print_t<char>
{
    void operator()(const std::string& str, FILE *fout)
    {
        std::fputs(str.c_str(), fout);
    }
};

template <>
struct print_t<wchar_t>
{
    void operator()(const std::wstring& str, FILE *fout)
    {
        using namespace std;
        fputws(str.c_str(), fout);
    }
};

//////////////////////////////////////////////////////////////////////////////
// revnu<T_CHAR>

template <typename T_CHAR>
class revnu
{
public:
    typedef T_CHAR char_type;
    typedef std::basic_string<T_CHAR> string_type;
    typedef size_t value_type;

protected:
    string_type m_digits;
    bool m_rev = false;

public:
    revnu()
    {
    }

    revnu(value_type value)
        : m_digits(to_str_t<value_type, char_type>()(value))
        , m_rev(false)
    {
        trim();
    }

    revnu(const char_type *str, bool rev = false)
        : m_digits(str)
        , m_rev(rev)
    {
        trim();
    }

    revnu(const string_type& str, bool rev = false)
        : m_digits(str)
        , m_rev(rev)
    {
        trim();
    }

    revnu(string_type&& str, bool rev = false)
        : m_digits(std::move(str))
        , m_rev(rev)
    {
        trim();
    }

    revnu(const revnu& value)
        : m_digits(value.m_digits)
        , m_rev(value.m_rev)
    {
    }

    revnu(revnu&& value)
        : m_digits(std::move(value.m_digits))
        , m_rev(value.m_rev)
    {
    }

    revnu& operator=(value_type value)
    {
        m_digits = to_str_t<value_type, char_type>()(value);
        m_rev = false;
        trim();
        return *this;
    }

    revnu& operator=(const char_type *str)
    {
        m_digits = str;
        m_rev = false;
        trim();
        return *this;
    }

    revnu& operator=(const string_type& str)
    {
        m_digits = str;
        m_rev = false;
        trim();
        return *this;
    }

    revnu& operator=(string_type&& str)
    {
        m_digits = std::move(str);
        m_rev = false;
        trim();
        return *this;
    }

    revnu& operator=(const revnu<T_CHAR>& value)
    {
        m_digits = value.m_digits;
        m_rev = value.m_rev;
        return *this;
    }

    revnu& operator=(revnu<T_CHAR>&& value)
    {
        m_digits = std::move(value.m_digits);
        m_rev = value.m_rev;
        return *this;
    }

    void clear()
    {
        m_digits.clear();
        m_rev = false;
    }

    void reverse()
    {
        std::reverse(m_digits.begin(), m_digits.end());
        m_rev = !m_rev;
    }

    void trim();

    string_type str() const
    {
        if (m_digits.empty())
            return { char_type('0') };
        if (!m_rev)
            return m_digits;
        auto d = m_digits;
        if (m_rev)
            std::reverse(d.begin(), d.end());
        return d;
    }

    void print() const
    {
        print_t<T_CHAR>()(str(), stdout);
    }

    void print(FILE *fout) const
    {
        print_t<T_CHAR>()(str(), fout);
    }

    void println() const
    {
        print();
        print_t<T_CHAR>()({ '\n' }, stdout);
    }

    void println(FILE *fout) const
    {
        print(fout);
        print_t<T_CHAR>()({ '\n' }, fout);
    }

    revnu& operator+=(const revnu& other);
    revnu& operator-=(const revnu& other);
    revnu& operator*=(const revnu& other);

    bool is_zero() const
    {
        if (m_digits.empty())
            return true;
        for (auto ch : m_digits)
        {
            if (ch != char_type('0'))
                return false;
        }
        return true;
    }

    explicit operator bool() const
    {
        return !is_zero();
    }

    bool operator!() const
    {
        return is_zero();
    }

    bool is_one() const
    {
        revnu copy = *this;
        --copy;
        return copy.is_zero();
    }

    int compare(const revnu& other) const;

    revnu operator++(int)
    {
        revnu copy = *this;
        ++(*this);
        return copy;
    }

    revnu operator--(int)
    {
        revnu copy = *this;
        --(*this);
        return copy;
    }

    revnu& operator++()
    {
        if (!m_rev)
            reverse();

        size_t index;
        for (index = 0; index < size(); ++index)
        {
            if (m_digits[index] != char_type('9'))
            {
                ++m_digits[index];
                break;
            }
            else
            {
                m_digits[index] = char_type('0');
            }
        }
        if (index == size())
            m_digits += char_type('1');
        return *this;
    }

    revnu& operator--()
    {
        if (!m_rev)
            reverse();

        size_t index;
        for (index = 0; index < size(); ++index)
        {
            if (m_digits[index] != char_type('0'))
            {
                --m_digits[index];
                break;
            }
            else
            {
                m_digits[index] = char_type('9');
            }
        }
        if (index == size())
            throw std::overflow_error("revnu::operator--");
        return *this;
    }

    void operator<<=(value_type value)
    {
        if (m_rev)
            m_digits.insert(0, string_type(value, char_type('0')));
        else
            m_digits += string_type(value, char_type('0'));
    }

    void operator>>=(value_type value)
    {
        if (value > size())
        {
            clear();
            return;
        }

        if (m_rev)
            m_digits.erase(0, value);
        else
            m_digits.erase(size() - value, value);
    }

protected:
    size_t size() const
    {
        return m_digits.size();
    }

    void resize(size_t new_size)
    {
        if (size() == new_size)
            return;

        if (!m_rev)
            reverse();

        assert(new_size > size());

        m_digits += string_type(new_size - size(), char_type('0'));
    }
};

//////////////////////////////////////////////////////////////////////////////
// revnu_a, revnu_w, revnu_t

typedef revnu<char> revnu_a;
typedef revnu<wchar_t> revnu_w;

#ifdef UNICODE
    typedef revnu_w revnu_t;
#else
    typedef revnu_a revnu_t;
#endif

//////////////////////////////////////////////////////////////////////////////
// inline functions

template <typename T_CHAR>
inline revnu<T_CHAR> operator+(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret += y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator-(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret -= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator*(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret *= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator+(const revnu<T_CHAR>& x, const typename revnu<T_CHAR>::value_type& y)
{
    revnu<T_CHAR> ret = x;
    ret += y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator-(const revnu<T_CHAR>& x, const typename revnu<T_CHAR>::value_type& y)
{
    revnu<T_CHAR> ret = x;
    ret -= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator*(const revnu<T_CHAR>& x, const typename revnu<T_CHAR>::value_type& y)
{
    revnu<T_CHAR> ret = x;
    ret *= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator+(const typename revnu<T_CHAR>::value_type& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret += y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator-(const typename revnu<T_CHAR>::value_type& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret -= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator*(const typename revnu<T_CHAR>::value_type& x, const revnu<T_CHAR>& y)
{
    revnu<T_CHAR> ret = x;
    ret *= y;
    return ret;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator<(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) < 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator>(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) > 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator<=(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) <= 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator>=(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) >= 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator==(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) == 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR> operator!=(const revnu<T_CHAR>& x, const revnu<T_CHAR>& y)
{
    return x.compare(y) != 0;
}

template <typename T_CHAR>
inline int revnu<T_CHAR>::compare(const revnu<T_CHAR>& other) const
{
    if (size() < other.size())
        return -1;
    if (size() > other.size())
        return +1;

    if (m_rev != other.m_rev)
    {
        revnu<T_CHAR> copy = *this;
        copy.reverse();
        return copy.compare(other);
    }

    if (!m_rev)
    {
        for (size_t i = 0; i < size(); ++i)
        {
            if (m_digits[i] < other.m_digits[i])
                return -1;
            if (m_digits[i] > other.m_digits[i])
                return +1;
        }
    }
    else
    {
        for (size_t i = size(); i > 0; )
        {
            --i;
            if (m_digits[i] < other.m_digits[i])
                return -1;
            if (m_digits[i] > other.m_digits[i])
                return +1;
        }
    }

    return 0;
}

template <typename T_CHAR>
inline revnu<T_CHAR>& revnu<T_CHAR>::operator+=(const revnu<T_CHAR>& other)
{
    if (size() < other.size())
    {
        resize(other.size());
    }

    if (size() == other.size())
    {
        if (!m_rev)
            reverse();
        if (other.m_rev)
        {
            bool carry = false;
            for (size_t i = 0; i < size(); ++i)
            {
                char_type ch1 = m_digits[i];
                char_type ch2 = other.m_digits[i];
                int n1 = ch1 - char_type('0');
                int n2 = ch2 - char_type('0');
                int m = (n1 + n2 + carry);
                m_digits[i] = char_type('0') + (m % 10);
                carry = (m >= 10);
            }
            if (carry)
                m_digits += char_type('1');
            trim();
            return *this;
        }
    }

    revnu<T_CHAR> copy = other;
    copy += *this;
    *this = std::move(copy);
    return *this;
}

template <typename T_CHAR>
inline revnu<T_CHAR>& revnu<T_CHAR>::operator-=(const revnu<T_CHAR>& other)
{
    if (size() < other.size())
    {
        resize(other.size());
    }

    if (size() == other.size())
    {
        if (!m_rev)
            reverse();
        if (other.m_rev)
        {
            bool carry = false;
            for (size_t i = 0; i < size(); ++i)
            {
                char_type ch1 = m_digits[i];
                char_type ch2 = other.m_digits[i];
                int n1 = ch1 - char_type('0');
                int n2 = ch2 - char_type('0');
                int m = (n1 - n2 - carry);
                m_digits[i] = char_type('0') + ((m + 10) % 10);
                carry = (m < 0);
            }
            if (carry)
                throw std::overflow_error("revnu::operator-=");
            trim();
            return *this;
        }
        else
        {
            revnu<T_CHAR> copy = other;
            copy.reverse();
            *this -= copy;
        }
    }
    else
    {
        revnu<T_CHAR> copy = other;
        copy.resize(size());
        if (!copy.m_rev)
            copy.reverse();
        *this -= copy;
    }

    return *this;
}

template <typename T_CHAR>
inline revnu<T_CHAR>& revnu<T_CHAR>::operator*=(const revnu<T_CHAR>& other)
{
    revnu<T_CHAR> copy = other;
    if (*this < other)
    {
        copy *= *this;
        *this = std::move(copy);
        return *this;
    }

    if (!copy.m_rev)
        copy.reverse();

    revnu<T_CHAR> sum;
    while (copy)
    {
        sum += *this;
        --copy;
    }

    *this = std::move(sum);
    return *this;
}

template <typename T_CHAR>
inline void str_trim_left(std::basic_string<T_CHAR>& str, T_CHAR ch)
{
    typedef std::basic_string<T_CHAR> string_type;
    size_t j = str.find_first_not_of(ch);
    if (j == string_type::npos)
        str.clear();
    else
        str = str.substr(j);
}

template <typename T_CHAR>
inline void str_trim_right(std::basic_string<T_CHAR>& str, T_CHAR ch)
{
    typedef std::basic_string<T_CHAR> string_type;
    size_t j = str.find_last_not_of(ch);
    if (j == string_type::npos)
        str.clear();
    else
        str = str.substr(0, j + 1);
}

template <typename T_CHAR>
inline void revnu<T_CHAR>::trim()
{
    if (!m_rev)
        str_trim_left(m_digits, char_type('0'));
    else
        str_trim_right(m_digits, char_type('0'));
}

} // namespace katahiromz
