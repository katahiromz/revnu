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

class revnu
{
public:
    typedef char char_type;
    typedef std::string string_type;
    typedef size_t value_type;

protected:
    string_type m_digits;
    bool m_rev = false;

public:
    revnu()
    {
    }

    revnu(value_type value)
        : m_digits(std::to_string(value))
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

    revnu& operator=(value_type value)
    {
        m_digits = std::to_string(value);
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
        std::fputs(str().c_str(), stdout);
    }

    void print(FILE *fp) const
    {
        std::fputs(str().c_str(), fp);
    }

    void println() const
    {
        std::printf("%s\n", str().c_str());
    }

    void println(FILE *fp) const
    {
        std::fprintf(fp, "%s\n", str().c_str());
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
};

//////////////////////////////////////////////////////////////////////////////
// inline functions

inline revnu operator+(const revnu& x, const revnu& y)
{
    revnu ret = x;
    ret += y;
    return ret;
}

inline revnu operator-(const revnu& x, const revnu& y)
{
    revnu ret = x;
    ret -= y;
    return ret;
}

inline revnu operator*(const revnu& x, const revnu& y)
{
    revnu ret = x;
    ret *= y;
    return ret;
}

inline revnu operator+(const revnu& x, const revnu::value_type& y)
{
    revnu ret = x;
    ret += y;
    return ret;
}

inline revnu operator-(const revnu& x, const revnu::value_type& y)
{
    revnu ret = x;
    ret -= y;
    return ret;
}

inline revnu operator*(const revnu& x, const revnu::value_type& y)
{
    revnu ret = x;
    ret *= y;
    return ret;
}


inline revnu operator+(const revnu::value_type& x, const revnu& y)
{
    revnu ret = x;
    ret += y;
    return ret;
}

inline revnu operator-(const revnu::value_type& x, const revnu& y)
{
    revnu ret = x;
    ret -= y;
    return ret;
}

inline revnu operator*(const revnu::value_type& x, const revnu& y)
{
    revnu ret = x;
    ret *= y;
    return ret;
}

inline revnu operator<(const revnu& x, const revnu& y)
{
    return x.compare(y) < 0;
}

inline revnu operator>(const revnu& x, const revnu& y)
{
    return x.compare(y) > 0;
}

inline revnu operator<=(const revnu& x, const revnu& y)
{
    return x.compare(y) <= 0;
}

inline revnu operator>=(const revnu& x, const revnu& y)
{
    return x.compare(y) >= 0;
}

inline revnu operator==(const revnu& x, const revnu& y)
{
    return x.compare(y) == 0;
}

inline revnu operator!=(const revnu& x, const revnu& y)
{
    return x.compare(y) != 0;
}

inline int revnu::compare(const revnu& other) const
{
    if (size() < other.size())
        return -1;
    if (size() > other.size())
        return +1;

    if (m_rev != other.m_rev)
    {
        revnu copy = *this;
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

inline revnu& revnu::operator+=(const revnu& other)
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

    revnu copy = other;
    copy += *this;
    *this = std::move(copy);
    return *this;
}

inline revnu& revnu::operator-=(const revnu& other)
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
            revnu copy = other;
            copy.reverse();
            *this -= copy;
        }
    }
    else
    {
        revnu copy = other;
        copy.resize(size());
        if (!copy.m_rev)
            copy.reverse();
        *this -= copy;
    }

    return *this;
}

inline revnu& revnu::operator*=(const revnu& other)
{
    revnu copy = other;
    if (*this < other)
    {
        copy *= *this;
        *this = std::move(copy);
        return *this;
    }

    revnu sum;
    while (copy)
    {
        sum += *this;
        --copy;
    }

    *this = std::move(sum);
    return *this;
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

inline void revnu::trim()
{
    if (!m_rev)
        reverse();
    str_trim_right(m_digits, char_type('0'));
}

} // namespace katahiromz
