// tests for revnu
#include "revnu.hpp"

int main(void)
{
    using namespace katahiromz;

    revnu_t a = 1;
    a.println();
    revnu_t b = 2;
    b.println();

    a += b;
    a.println();

    a = 100;
    b = 999;
    a *= b;
    a.println();

    a = 30;
    b = 20;
    a -= b;
    a.println();

    a = 30;
    b = 10;
    (a - b).println();
    ((a - b) * 2).println();

    a.clear();
    a.println();

#ifdef UNICODE
    a = L"999999999999999999999999999999";
#else
    a = "999999999999999999999999999999";
#endif
    a.println();
    a += 1;
    a.println();

    return 0;
}
