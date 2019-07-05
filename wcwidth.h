#include <wcwidth/table_wide.h>
#include <wcwidth/table_zero.h>

// Given one unicode character, return its printable length on a terminal.
//
// The wcwidth() function returns 0 if the wc argument has no printable effect
// on a terminal (such as NUL '\0'), -1 if wc is not printable, or has an
// indeterminate effect on the terminal, such as a control character.
// Otherwise, the number of column positions the character occupies on a
// graphic terminal (1 or 2) is returned.
//
// The following have a column width of -1:
//
//     - C0 control characters (U+001 through U+01F).
//
//     - C1 control characters and DEL (U+07F through U+0A0).
//
// The following have a column width of 0:
//
//     - Non-spacing and enclosing combining characters (general
//       category code Mn or Me in the Unicode database).
//
//     - NULL (U+0000, 0).
//
//     - COMBINING GRAPHEME JOINER (U+034F).
//
//     - ZERO WIDTH SPACE (U+200B) through
//       RIGHT-TO-LEFT MARK (U+200F).
//
//     - LINE SEPERATOR (U+2028) and
//       PARAGRAPH SEPERATOR (U+2029).
//
//     - LEFT-TO-RIGHT EMBEDDING (U+202A) through
//       RIGHT-TO-LEFT OVERRIDE (U+202E).
//
//     - WORD JOINER (U+2060) through
//       INVISIBLE SEPARATOR (U+2063).
//
// The following have a column width of 1:
//
//     - SOFT HYPHEN (U+00AD) has a column width of 1.
//
//     - All remaining characters (including all printable
//       ISO 8859-1 and WGL4 characters, Unicode control characters,
//       etc.) have a column width of 1.
//
// The following have a column width of 2:
//
//     - Spacing characters in the East Asian Wide (W) or East Asian
//       Full-width (F) category as defined in Unicode Technical
//       Report #11 have a column width of 2.

int jquast_wcwidth (wchar_t wc);

// Given a unicode string, return its printable length on a terminal.
//
// Return the width, in cells, necessary to display the first ``n``
// characters of the unicode string ``pwcs``.  When ``n`` is None (default),
// return the length of the entire string.
//
// Returns ``-1`` if a non-printable character is encountered.

int jquast_wcswidth (wchar_t const * pwcs, size_t n);


// INLINE DEFINITIONS

int
_bisearch_zero (wchar_t wc)
{
    static_assert(sizeof(wchar_t) == sizeof(uint32_t));
    auto ucs = static_cast<uint32_t>(wc);
    int lbound = 0;
    int ubound = sizeof(ZERO_WIDTH) - 1;

    if (ucs < ZERO_WIDTH[0][0] || ucs > ZERO_WIDTH[ubound][1])
        return 0;
    while (ubound >= lbound) {
        int mid = (lbound + ubound) / 2;
        if (ucs > ZERO_WIDTH[mid][1])
            lbound = mid + 1;
        else if (ucs < ZERO_WIDTH[mid][0])
            ubound = mid - 1;
        else
            return 1;
    }

    return 0;
}

int
_bisearch_wide (wchar_t wc)
{
    static_assert(sizeof(wchar_t) == sizeof(uint32_t));
    auto ucs = static_cast<uint32_t>(wc);
    int lbound = 0;
    int ubound = sizeof(WIDE_EASTASIAN) - 1;

    if (ucs < WIDE_EASTASIAN[0][0] || ucs > WIDE_EASTASIAN[ubound][1])
        return 0;
    while (ubound >= lbound) {
        int mid = (lbound + ubound) / 2;
        if (ucs > WIDE_EASTASIAN[mid][1])
            lbound = mid + 1;
        else if (ucs < WIDE_EASTASIAN[mid][0])
            ubound = mid - 1;
        else
            return 1;
    }

    return 0;
}


int
jquast_wcwidth (wchar_t wc)
{
    // NOTE: created by hand, there isn't anything identifiable other than
    // general Cf category code to identify these, and some characters in Cf
    // category code are of non-zero width.
    if (wc == 0 ||
        wc == 0x034F ||
        (0x200B <= wc && wc <= 0x200F) ||
        wc == 0x2028 ||
        wc == 0x2029 ||
        (0x202A <= wc && wc <= 0x202E) ||
        (0x2060 <= wc && wc <= 0x2063))
        return 0;

    // C0/C1 control characters
    if (wc < 32 || (0x07F <= wc && wc < 0x0A0))
        return -1;

    if (_bisearch_zero(wc))
        return 0;

    return 1 + _bisearch_wide(wc);
}

int
jquast_wcswidth (wchar_t const * pwcs, size_t n)
{
    int width = 0;
    int wcw = 0;
    for (int i = 0; i < n; ++i)
    {
        wcw = jquast_wcwidth(pwcs[i]);
        if (wcw < 0)
            return -1;
        else
            width += wcw;
    }
    return width;
}
