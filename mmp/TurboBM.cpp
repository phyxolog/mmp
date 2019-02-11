#include "pch.hpp"
#include "TurboBM.hpp"

namespace mmp {
    size_t TurboBM::backwardsMatchLen(const char* ptr1, const char* ptr2, size_t strlen, size_t maxlen, size_t minlen) {
        size_t result = minlen;
        while (result < maxlen && ptr1[strlen - 1 - result] == ptr2[strlen - 1 - result]) {
            ++result;
        }
        return result;
    }

    const skiptableType TurboBM::CreateSkipTable(const char* needle, size_t needle_length) {
        skiptableType skip(needle_length, needle_length);

        if (needle_length <= 1) {
            return skip;
        }

        const size_t needle_length_minus_1 = needle_length - 1;

        std::vector<ssize_t> suff(needle_length);

        suff[needle_length_minus_1] = needle_length;

        ssize_t f = 0;
        ssize_t g = needle_length_minus_1;
        size_t j = 0;
        for (ssize_t i = needle_length - 2; i >= 0; i--) {
            if (i > g) {
                const ssize_t tmp = suff[i + needle_length_minus_1 - f];
                if (tmp < i - g) {
                    suff[i] = tmp;
                    goto i_done;
                }
            } else {
                g = i;
            }

            f = i;

            g -= backwardsMatchLen(
                needle,
                needle + needle_length_minus_1 - f,
                g + 1,
                g + 1,
                0
            );

            suff[i] = f - g;

        i_done:;
            if (suff[i] == i + 1) {
                size_t jlimit = needle_length_minus_1 - i;
                while (j < jlimit)
                    skip[j++] = jlimit;
            }
        }

        for (size_t i = 0; i < needle_length_minus_1; i++) {
            skip[needle_length_minus_1 - suff[i]] = needle_length_minus_1 - i;
        }

        return skip;
    }

    const occtableType TurboBM::CreateOccTable(const char* needle, size_t needle_length) {
        occtableType occ(UCHAR_MAX + 1, needle_length);

        if (needle_length >= 1) {
            const size_t needle_length_minus_1 = needle_length - 1;
            for (size_t a = 0; a < needle_length_minus_1; a++) {
                occ[needle[a]] = needle_length_minus_1 - a;
            }
        }
        return occ;
    }

    size_t TurboBM::Search(const char* haystack, size_t haystack_length,
        const occtableType& occ,
        const skiptableType& skip,
        const char* needle,
        const size_t needle_length,
        const size_t start_index) {
        if (needle_length > haystack_length) {
            return -1;
        }

        if (needle_length == 1) {
            const char* result = (const char*)std::memchr(haystack + start_index, *needle, haystack_length);
            return result ? size_t(result - haystack) : -1;
        }

        const size_t needle_length_minus_1 = needle_length - 1;

        size_t haystack_position = start_index;
        size_t ignore_num = 0, shift = needle_length;
        while (haystack_position <= haystack_length - needle_length) {
            size_t match_len;
            if (ignore_num == 0) {
                match_len = backwardsMatchLen(
                    needle,
                    haystack + haystack_position,
                    needle_length,
                    needle_length,
                    0
                );

                if (match_len == needle_length)
                    return haystack_position;
            } else {
                match_len =
                    backwardsMatchLen(needle, haystack + haystack_position,
                        needle_length,
                        shift,
                        0);

                if (match_len == shift) {
                    match_len =
                        backwardsMatchLen(needle, haystack + haystack_position,
                            needle_length,
                            needle_length,
                            shift + ignore_num);
                }

                if (match_len >= needle_length) {
                    return haystack_position;
                }
            }

            const size_t mismatch_position = needle_length_minus_1 - match_len;

            const unsigned char occ_char = haystack[haystack_position + mismatch_position];

            const ssize_t bcShift = occ[occ_char] - match_len;
            const size_t gcShift = skip[mismatch_position];
            const ssize_t turboShift = ignore_num - match_len;

            shift = std::max(std::max((ssize_t)gcShift, bcShift), turboShift);

            if (shift == gcShift) {
                ignore_num = std::min(needle_length - shift, match_len);
            } else {
                if (turboShift < bcShift && ignore_num >= shift) {
                    shift = ignore_num + 1;
                }
                ignore_num = 0;
            }

            haystack_position += shift;
        }

        return -1;
    }
}
