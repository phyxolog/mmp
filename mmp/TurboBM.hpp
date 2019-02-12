#pragma once

#include <vector>
#include <cstdio>
#include <cstring>
#include <climits>
#include <algorithm>

namespace mmp {
    typedef std::vector<size_t> skiptableType;
    typedef std::vector<size_t> occtableType;

#if defined(_MSC_VER)
#include <BaseTsd.h>
    typedef SSIZE_T ssize_t;
#endif

    class TurboBM {
    private:
        static size_t backwardsMatchLen(const char*, const char*, size_t, size_t, size_t);

    public:
        static size_t search(const char*, size_t, const occtableType&, const skiptableType&, const char*, const size_t, const size_t = 0);
        static const skiptableType createSkipTable(const char*, size_t);
        static const occtableType createOccTable(const char*, size_t);
    };
}
