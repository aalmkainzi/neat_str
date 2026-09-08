#include "test_common.h"

#define CGS_debug_break()
#define CGS_SHORT_NAMES
#include "../cgs.c"
// Forward declaration for appender state

// ============================================================================
// Edge Case Tests for cgs_len
// ============================================================================

void test_str_len_edge_cases() {
    TEST("cgs_len: empty string");;;
    {
        char buf[] = "";
        ASSERT_EQ(cgs_len(buf), 0);
    }
    
    TEST("cgs_len: single null terminator");;;
    {
        char buf[1] = {'\0'};
        ASSERT_EQ(cgs_len(buf), 0);
    }
    
    TEST("cgs_len: CGS_DStr with zero capacity");;;
    {
        CGS_DStr dstr = cgs_dstr_init(0);
        ASSERT_EQ(cgs_len(&dstr), 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_len: very long string");;;
    {
        // This tests if length calculation handles large strings
        CGS_DStr dstr = cgs_dstr_init(10000);
        for (int i = 0; i < 9999; i++) {
            cgs_putc(&dstr, 'a');
        }
        ASSERT_EQ(cgs_len(&dstr), 9999);
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for cgs_cap
// ============================================================================

void test_str_cap_edge_cases() {
    TEST("cgs_cap: raw char array");;;
    {
        char buf[100];
        unsigned int cap = cgs_cap(buf);
        ASSERT_TRUE(cap == 100);
    }
    
    TEST("cgs_cap: CGS_StrBuf at exact capacity");;;
    {
        char backing[5] = {'t', 'e', 's', 't', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 5);
        ASSERT_EQ(cgs_cap(&sb), 5);
    }
}

// ============================================================================
// Edge Case Tests for cgs_equal
// ============================================================================

void test_str_equal_edge_cases() {
    TEST("cgs_equal: both empty");;;
    {
        char a[] = "";
        char b[] = "";
        ASSERT_TRUE(cgs_equal(a, b));
    }
    
    TEST("cgs_equal: one empty, one not");;;
    {
        char a[] = "";
        char b[] = "test";
        ASSERT_FALSE(cgs_equal(a, b));
        ASSERT_FALSE(cgs_equal(b, a));
    }
    
    TEST("cgs_equal: same pointer");;;
    {
        char a[] = "test";
        ASSERT_TRUE(cgs_equal(a, a));
    }
    
    TEST("cgs_equal: different types with same content");;;
    {
        char cstr[] = "hello";
        CGS_DStr dstr = cgs_dstr_init_from(cstr);
        ASSERT_TRUE(cgs_equal(cstr, &dstr));
        ASSERT_TRUE(cgs_equal(&dstr, cstr));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_equal: strings differing only in last char");;;
    {
        char a[] = "test1";
        char b[] = "test2";
        ASSERT_FALSE(cgs_equal(a, b));
    }
    
    TEST("cgs_equal: strings with embedded nulls");;;
    {
        char a[5] = {'a', 'b', '\0', 'c', 'd'};
        char b[5] = {'a', 'b', '\0', 'x', 'y'};
        // Should be equal up to first null
        ASSERT_TRUE(cgs_equal(a, b));
    }
    
    TEST("cgs_equal: very long identical strings");;;
    {
        CGS_DStr a = cgs_dstr_init(5000);
        CGS_DStr b = cgs_dstr_init(5000);
        for (int i = 0; i < 4999; i++) {
            cgs_putc(&a, 'x');
            cgs_putc(&b, 'x');
        }
        ASSERT_TRUE(cgs_equal(&a, &b));
        cgs_dstr_deinit(&a);
        cgs_dstr_deinit(&b);
    }
}

// ============================================================================
// Edge Case Tests for cgs_find
// ============================================================================

void test_str_find_edge_cases() {
    TEST("cgs_find: basic match");;;
    {
        char hay[] = "hello world";
        CGS_StrView v = cgs_find(hay, "world");
        // Should return view starting at index 6, length 5
        ASSERT_TRUE(v.chars == &hay[6]);
        ASSERT_TRUE(v.len == 5);
    }
    
    TEST("cgs_find: not found");;;
    {
        CGS_StrView v = cgs_find("apple", "orange");
        // Usually returns a null view (chars == NULL) or a view with len 0
        ASSERT_TRUE(v.chars == NULL || v.len == 0);
    }
    
    TEST("cgs_find: match at start and end");;;
    {
        char hay[] = "bracket";
        CGS_StrView start = cgs_find(hay, "b");
        CGS_StrView end = cgs_find(hay, "t");
        
        ASSERT_TRUE(start.chars == &hay[0]);
        ASSERT_TRUE(end.chars == &hay[6]);
    }
    
    TEST("cgs_find: empty needle");;;
    {
        char hay[] = "abc";
        CGS_StrView v = cgs_find(hay, "");
        // In most languages, finding "" returns the start of the string
        ASSERT_TRUE(v.chars == &hay[0]);
        ASSERT_TRUE(v.len == 0);
    }
    
    TEST("cgs_find: needle equals haystack");;;
    {
        char hay[] = "same";
        CGS_StrView v = cgs_find(hay, "same");
        ASSERT_TRUE(v.len == 4 && v.chars == &hay[0]);
    }
    
    TEST("cgs_find: needle longer than haystack");;;
    {
        CGS_StrView v = cgs_find("short", "much longer needle");
        ASSERT_TRUE(v.chars == NULL || v.len == 0);
    }
    
    TEST("cgs_find: empty needle in empty haystack");;;
    {
        char hay[] = "";
        char needle[] = "";
        CGS_StrView result = cgs_find(hay, needle);
        // Finding empty in empty might return start or invalid
        // Check it doesn't crash
        ASSERT_TRUE((char*)result.chars == hay);
    }
    
    TEST("cgs_find: empty needle in non-empty haystack");;;
    {
        char hay[] = "test";
        char needle[] = "";
        CGS_StrView result = cgs_find(hay, needle);
        // Should probably find at position 0 or be invalid
        ASSERT_TRUE(result.chars == hay);
    }
    
    TEST("cgs_find: needle longer than haystack");;;
    {
        char hay[] = "ab";
        char needle[] = "abcdef";
        CGS_StrView result = cgs_find(hay, needle);
        ASSERT_EQ(cgs_len(result), 0); // Not found
    }
    
    TEST("cgs_find: needle equals haystack");;;
    {
        char hay[] = "exact";
        char needle[] = "exact";
        CGS_StrView result = cgs_find(hay, needle);
        ASSERT_EQ(cgs_len(result), 5);
    }
    
    TEST("cgs_find: needle at very end");;;
    {
        char hay[] = "abcdefghij";
        char needle[] = "hij";
        CGS_StrView result = cgs_find(hay, needle);
        ASSERT_EQ(cgs_len(result), 3);
    }
    
    TEST("cgs_find: needle at very start");;;
    {
        char hay[] = "abcdefghij";
        char needle[] = "abc";
        CGS_StrView result = cgs_find(hay, needle);
        ASSERT_EQ(cgs_len(result), 3);
    }
    
    TEST("cgs_find: overlapping pattern");;;
    {
        char hay[] = "aaaa";
        char needle[] = "aa";
        CGS_StrView result = cgs_find(hay, needle);
        // Should find first occurrence
        ASSERT_TRUE(cgs_len(result) > 0);
    }
    
    TEST("cgs_find: pattern that almost matches");;;
    {
        char hay[] = "abcabcabd";
        char needle[] = "abcabd";
        CGS_StrView result = cgs_find(hay, needle);
        ASSERT_TRUE(cgs_len(result) > 0);
    }
    
    /* --- cgs_trim_view --- */
    
    TEST("cgs_trim_view: no whitespace");;;
    {
        char s[] = "hello";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[0]);
    }
    
    TEST("cgs_trim_view: leading spaces");;;
    {
        char s[] = "   hello";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[3]);
    }
    
    TEST("cgs_trim_view: trailing spaces");;;
    {
        char s[] = "hello   ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[0]);
    }
    
    TEST("cgs_trim_view: leading and trailing spaces");;;
    {
        char s[] = "  hello  ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[2]);
    }
    
    TEST("cgs_trim_view: interior whitespace is preserved");;;
    {
        char s[] = "  hello world  ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 11 && v.chars == &s[2]);
    }
    
    TEST("cgs_trim_view: only spaces");;;
    {
        char s[] = "     ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 0);
    }
    
    TEST("cgs_trim_view: empty string");;;
    {
        char s[] = "";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 0);
    }
    
    TEST("cgs_trim_view: empty string");;;
    {
        char s[] = " \t\n";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 0);
    }
    
    TEST("cgs_trim_view: single non-whitespace character");;;
    {
        char s[] = "x";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 1 && v.chars == &s[0]);
    }
    
    TEST("cgs_trim_view: single space");;;
    {
        char s[] = " ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 0 && v.chars == s + 1);
    }
    
    TEST("cgs_trim_view: leading tab");;;
    {
        char s[] = "\thello";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[1]);
    }
    
    TEST("cgs_trim_view: trailing newline");;;
    {
        char s[] = "hello\n";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[0]);
    }
    
    TEST("cgs_trim_view: mixed whitespace characters on both sides");;;
    {
        char s[] = " \t\n hello \r\n";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 5 && v.chars == &s[4]);
    }
    
    TEST("cgs_trim_view: accepts CGS_StrView");;;
    {
        CGS_StrView src = cgs_strv("  hi  ");
        CGS_StrView v = cgs_trim_view(src);
        ASSERT_TRUE(v.len == 2);
    }
    
    TEST("cgs_trim_view: accepts CGS_DStr");;;
    {
        CGS_DStr d = cgs_dstr_init_from("  hi  ");
        CGS_StrView v = cgs_trim_view(d);
        ASSERT_TRUE(v.len == 2);
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_trim_view: result points into original buffer, not a copy");;;
    {
        char s[] = "  abc  ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.chars == &s[2]);
    }
    
    TEST("cgs_trim_view: only spaces");;;
    {
        char s[] = "   ";
        CGS_StrView v = cgs_trim_view(s);
        ASSERT_TRUE(v.len == 0 && v.chars == s + 3);
    }
    
    /* --- cgs_trim --- */
    
    TEST("cgs_trim: no whitespace");;;
    {
        char s[64] = "hello";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: leading spaces");;;
    {
        char s[64] = "   hello";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0);
        ASSERT_TRUE(cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: trailing spaces");;;
    {
        char s[64] = "hello   ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: leading and trailing spaces");;;
    {
        char s[64] = "  hello  ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(s, "hello"));
        ASSERT_TRUE( cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: all whitespace");;;
    {
        // A string of 4 spaces. Length is 4.
        // Buffer is exactly large enough for 4 chars + null (5 bytes).
        char s[5] = "    "; 
        
        // Logic execution:
        // 1. begin will become 4 (it hits the end of the string).
        // 2. end will stay 4 (the loop 'end > begin' will never execute).
        // 3. len = 4 - 4 = 0.
        // 4. memmove(s, s + 4, 4);  <-- BUG HERE
        
        // This memmove will try to read 4 bytes starting from index 4.
        // Index 4 is '\0', but indices 5, 6, and 7 are OUTSIDE the array 's[5]'.
        
        CGS_Error e = cgs_trim(s);
        
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(s, ""));
        ASSERT_TRUE(cgs_len(s) == 0);
    }
    
    TEST("cgs_trim: interior whitespace is preserved");;;
    {
        char s[64] = "  hello world  ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello world") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: only spaces");;;
    {
        char s[64] = "     ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: empty string");;;
    {
        char s[64] = "";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: single non-whitespace character");;;
    {
        char s[64] = "x";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "x") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: single space");;;
    {
        char s[64] = " ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: leading tab");;;
    {
        char s[64] = "\thello";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: trailing newline");;;
    {
        char s[64] = "hello\n";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: mixed whitespace on both sides");;;
    {
        char s[64] = " \t\n hello \r\n";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(s, "hello") == 0 && cgs_chars(s) == p);
    }
    
    TEST("cgs_trim: CGS_DStr*");;;
    {
        CGS_DStr d = cgs_dstr_init_from("  hello  ");
        char *p = cgs_chars(d);
        CGS_Error e = cgs_trim(&d);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(d, "hello") && cgs_chars(d) == p);
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_trim: CGS_StrBuf*");;;
    {
        char buf[64] = "  hello  ";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(buf, sizeof(buf));
        char *p = cgs_chars(sb);
        CGS_Error e = cgs_trim(&sb);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(sb, "hello") && cgs_chars(sb) == p);
    }
    
    TEST("cgs_trim: CGS_MutStrRef");;;
    {
        char buf[64] = "  hello  ";
        CGS_MutStrRef ref = cgs_mutstr_ref(buf);
        char *p = buf;
        CGS_Error e = cgs_trim(ref);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(strcmp(buf, "hello") == 0 && buf == p);
    }
    
    TEST("cgs_trim: length decreases correctly after trim");;;
    {
        char s[64] = "  hi  ";
        char *p = s;
        CGS_Error e = cgs_trim(s);
        ASSERT_TRUE(e.ec == CGS_OK);
        ASSERT_TRUE(cgs_len(s) == 2 && cgs_chars(s) == p);
    }
}

void test_spn_cspn_tok()
{
    /* ════════════════════════════════════════════════════════════
     * cgs_spn — first chunk made up ONLY of chars in charset
     * ════════════════════════════════════════════════════════════ */

    TEST("cgs_spn: leading run of charset chars");
    {
        /* "aaabbb" with charset "a" → "aaa" */
        CGS_StrView v = cgs_spn(cgs_strv("aaabbb"), cgs_strv("a"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("aaa")));
    }

    TEST("cgs_spn: charset with multiple chars");
    {
        /* "abc123" charset "abc" → "abc" */
        CGS_StrView v = cgs_spn(cgs_strv("abc123"), cgs_strv("abc"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("abc")));
    }

    TEST("cgs_spn: whole string matches charset");
    {
        CGS_StrView v = cgs_spn(cgs_strv("12345"), cgs_strv("0123456789"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("12345")));
    }

    TEST("cgs_spn: first char not in charset → empty view");
    {
        CGS_StrView v = cgs_spn(cgs_strv("xabc"), cgs_strv("abc"));
        ASSERT_TRUE(v.len == 0);
    }

    TEST("cgs_spn: empty source → empty view");
    {
        CGS_StrView v = cgs_spn(cgs_strv(""), cgs_strv("abc"));
        ASSERT_TRUE(v.len == 0);
    }

    TEST("cgs_spn: empty charset → empty view");
    {
        CGS_StrView v = cgs_spn(cgs_strv("abc"), cgs_strv(""));
        ASSERT_TRUE(v.len == 0);
    }

    TEST("cgs_spn: view begins at start of source");
    {
        CGS_StrView src = cgs_strv("aabxx");
        CGS_StrView v = cgs_spn(src, cgs_strv("ab"));
        ASSERT_TRUE(v.chars == src.chars);
        ASSERT_TRUE(cgs_equal(v, cgs_strv("aab")));
    }

    TEST("cgs_spn: repeated charset chars are handled");
    {
        /* charset "aА" duplicate-free behaviour: "aaa bbb", charset "a " */
        CGS_StrView v = cgs_spn(cgs_strv("aaa bbb"), cgs_strv("a "));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("aaa ")));
    }

    /* ════════════════════════════════════════════════════════════
     * cgs_cspn — first chunk made up ONLY of chars NOT in charset
     * ════════════════════════════════════════════════════════════ */

    TEST("cgs_cspn: stop at first charset char");
    {
        /* "abc123" charset "0123456789" → "abc" */
        CGS_StrView v = cgs_cspn(cgs_strv("abc123"), cgs_strv("0123456789"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("abc")));
    }

    TEST("cgs_cspn: stop at first delimiter");
    {
        /* "hello world" charset " " → "hello" */
        CGS_StrView v = cgs_cspn(cgs_strv("hello world"), cgs_strv(" "));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("hello")));
    }

    TEST("cgs_cspn: no charset char present → whole string");
    {
        CGS_StrView v = cgs_cspn(cgs_strv("hello"), cgs_strv("xyz"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("hello")));
    }

    TEST("cgs_cspn: first char in charset → empty view");
    {
        CGS_StrView v = cgs_cspn(cgs_strv(",abc"), cgs_strv(","));
        ASSERT_TRUE(v.len == 0);
    }

    TEST("cgs_cspn: empty source → empty view");
    {
        CGS_StrView v = cgs_cspn(cgs_strv(""), cgs_strv(","));
        ASSERT_TRUE(v.len == 0);
    }

    TEST("cgs_cspn: empty charset → whole string");
    {
        /* nothing to stop at, so the whole string qualifies */
        CGS_StrView v = cgs_cspn(cgs_strv("abc"), cgs_strv(""));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("abc")));
    }

    TEST("cgs_cspn: multi-char charset, stop at earliest match");
    {
        /* "abc;def,ghi" charset ";," → "abc" (';' comes first) */
        CGS_StrView v = cgs_cspn(cgs_strv("abc;def,ghi"), cgs_strv(";,"));
        ASSERT_TRUE(cgs_equal(v, cgs_strv("abc")));
    }

    /* ════════════════════════════════════════════════════════════
     * cgs_next_tok — skips leading delims, returns next token,
     * advances base past it. Empty token (all delim / empty base)
     * returns CGS_NOT_FOUND.
     * ════════════════════════════════════════════════════════════ */

    TEST("cgs_next_tok: simple comma-separated tokens");
    {
        CGS_StrView base = cgs_strv("one,two,three");

        CGS_Result(CGS_StrView) r1 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r1.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("one")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r2.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r2.val, cgs_strv("two")));

        CGS_Result(CGS_StrView) r3 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r3.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r3.val, cgs_strv("three")));

        /* base now exhausted */
        CGS_Result(CGS_StrView) r4 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r4.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: skips leading delimiters");
    {
        /* leading commas are skipped, first token is "a" */
        CGS_StrView base = cgs_strv(",,,a,b");
        CGS_Result(CGS_StrView) r = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r.val, cgs_strv("a")));
    }

    TEST("cgs_next_tok: collapses consecutive delimiters between tokens");
    {
        /* "a,,,b" → "a" then "b" (no empty tokens) */
        CGS_StrView base = cgs_strv("a,,,b");

        CGS_Result(CGS_StrView) r1 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r1.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("a")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r2.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r2.val, cgs_strv("b")));

        CGS_Result(CGS_StrView) r3 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r3.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: trailing delimiters yield NOT_FOUND after last token");
    {
        CGS_StrView base = cgs_strv("a,b,,,");

        CGS_Result(CGS_StrView) r1 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("a")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(cgs_equal(r2.val, cgs_strv("b")));

        CGS_Result(CGS_StrView) r3 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r3.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: empty base → NOT_FOUND");
    {
        CGS_StrView base = cgs_strv("");
        CGS_Result(CGS_StrView) r = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: base entirely delimiters → NOT_FOUND");
    {
        CGS_StrView base = cgs_strv(",,,,");
        CGS_Result(CGS_StrView) r = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: no delimiter present → whole string is one token");
    {
        CGS_StrView base = cgs_strv("nodelim");
        CGS_Result(CGS_StrView) r1 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r1.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("nodelim")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok(&base, cgs_strv(","));
        ASSERT_TRUE(r2.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: multi-char delimiter");
    {
        /* delim "::" → "a", "b", "c" */
        CGS_StrView base = cgs_strv("a::b::c");

        CGS_Result(CGS_StrView) r1 = cgs_next_tok(&base, cgs_strv("::"));
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("a")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok(&base, cgs_strv("::"));
        ASSERT_TRUE(cgs_equal(r2.val, cgs_strv("b")));

        CGS_Result(CGS_StrView) r3 = cgs_next_tok(&base, cgs_strv("::"));
        ASSERT_TRUE(cgs_equal(r3.val, cgs_strv("c")));

        CGS_Result(CGS_StrView) r4 = cgs_next_tok(&base, cgs_strv("::"));
        ASSERT_TRUE(r4.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok: multi-char delimiter, partial prefix is not a match");
    {
        /* "key-value" contains '-' but not "->" so no split should occur */
        CGS_StrView base = cgs_strv("key-value");
        CGS_Result(CGS_StrView) r = cgs_next_tok(&base, cgs_strv("->"));
        ASSERT_TRUE(r.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r.val, cgs_strv("key-value")));
    }

    TEST("cgs_next_tok: drive a full loop to collect all tokens");
    {
        CGS_StrView base = cgs_strv("  the quick   brown fox  ");
        const char *expected[] = { "the", "quick", "brown", "fox" };
        int i = 0;

        for (;;)
        {
            CGS_Result(CGS_StrView) r = cgs_next_tok(&base, cgs_strv(" "));
            if (r.err.ec == CGS_NOT_FOUND) break;
            ASSERT_TRUE(cgs_equal(r.val, cgs_strv(expected[i])));
            i++;
        }
        ASSERT_TRUE(i == 4);
    }

    /* ════════════════════════════════════════════════════════════
     * cgs_next_tok_any — like cgs_next_tok but any char in the
     * delim_set is treated as a delimiter.
     * ════════════════════════════════════════════════════════════ */

    TEST("cgs_next_tok_any: mixed single-char delimiters");
    {
        CGS_StrView base = cgs_strv("a;b,c|d");
        const char *expected[] = { "a", "b", "c", "d" };
        int i = 0;

        for (;;)
        {
            CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(";,|"));
            if (r.err.ec == CGS_NOT_FOUND) break;
            ASSERT_TRUE(cgs_equal(r.val, cgs_strv(expected[i])));
            i++;
        }
        ASSERT_TRUE(i == 4);
    }

    TEST("cgs_next_tok_any: skips leading delimiters from the set");
    {
        /* leading mix of ' ' and '\t' skipped */
        CGS_StrView base = cgs_strv(" \t a b");
        CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(" \t"));
        ASSERT_TRUE(r.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r.val, cgs_strv("a")));
    }

    TEST("cgs_next_tok_any: collapses consecutive delimiters of differing kinds");
    {
        /* "a,;|b" with set ",;|" → "a" then "b" */
        CGS_StrView base = cgs_strv("a,;|b");

        CGS_Result(CGS_StrView) r1 = cgs_next_tok_any(&base, cgs_strv(",;|"));
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("a")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok_any(&base, cgs_strv(",;|"));
        ASSERT_TRUE(cgs_equal(r2.val, cgs_strv("b")));

        CGS_Result(CGS_StrView) r3 = cgs_next_tok_any(&base, cgs_strv(",;|"));
        ASSERT_TRUE(r3.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok_any: whitespace tokenization");
    {
        CGS_StrView base = cgs_strv("foo\tbar baz\nqux");
        const char *expected[] = { "foo", "bar", "baz", "qux" };
        int i = 0;

        for (;;)
        {
            CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(" \t\n"));
            if (r.err.ec == CGS_NOT_FOUND) break;
            ASSERT_TRUE(cgs_equal(r.val, cgs_strv(expected[i])));
            i++;
        }
        ASSERT_TRUE(i == 4);
    }

    TEST("cgs_next_tok_any: empty base → NOT_FOUND");
    {
        CGS_StrView base = cgs_strv("");
        CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(",;"));
        ASSERT_TRUE(r.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok_any: base entirely delimiters → NOT_FOUND");
    {
        CGS_StrView base = cgs_strv(";,;,");
        CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(",;"));
        ASSERT_TRUE(r.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok_any: no delimiter present → whole string");
    {
        CGS_StrView base = cgs_strv("single");
        CGS_Result(CGS_StrView) r1 = cgs_next_tok_any(&base, cgs_strv(",;|"));
        ASSERT_TRUE(r1.err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(r1.val, cgs_strv("single")));

        CGS_Result(CGS_StrView) r2 = cgs_next_tok_any(&base, cgs_strv(",;|"));
        ASSERT_TRUE(r2.err.ec == CGS_NOT_FOUND);
    }

    TEST("cgs_next_tok_any: single-element delim set behaves like next_tok");
    {
        CGS_StrView base = cgs_strv("a,b,c");
        const char *expected[] = { "a", "b", "c" };
        int i = 0;

        for (;;)
        {
            CGS_Result(CGS_StrView) r = cgs_next_tok_any(&base, cgs_strv(","));
            if (r.err.ec == CGS_NOT_FOUND) break;
            ASSERT_TRUE(cgs_equal(r.val, cgs_strv(expected[i])));
            i++;
        }
        ASSERT_TRUE(i == 3);
    }
}

// ============================================================================
// Edge Case Tests for cgs_count
// ============================================================================

void test_str_count_edge_cases() {
    TEST("cgs_count: basic count");;;
    {
        unsigned int n = cgs_count("banana", "a");
        ASSERT_TRUE(n == 3);
    }
    
    TEST("cgs_count: non-overlapping logic");;;
    {
        // "aa" appears in "aaaaa" at:
        // [aa] [aa] a  <- Non-overlapping (2)
        //  a [aa] [aa] <- Non-overlapping alternative
        // Standard behavior is to consume the first match and move on.
        unsigned int n = cgs_count("aaaaa", "aa");
        ASSERT_TRUE(n == 2);
    }
    
    TEST("cgs_count: no matches");;;
    {
        unsigned int n = cgs_count("abc", "z");
        ASSERT_TRUE(n == 0);
    }
    
    TEST("cgs_count: empty needle (Interstitial Gaps)");;;
    {
        // "abc" has gaps: ^ a ^ b ^ c ^ 
        // Index:        0 1 1 2 2 3 3
        // Most modern libs (Python, Go, etc.) count len + 1
        unsigned int n = cgs_count("abc", "");
        ASSERT_TRUE(n == 4);
    }
    
    TEST("cgs_count: empty haystack");;;
    {
        // "" contains one ""
        ASSERT_TRUE(cgs_count("", "") == 1);
        // "" contains zero "a"
        ASSERT_TRUE(cgs_count("", "a") == 0);
    }
    
    TEST("cgs_count: needle is haystack");;;
    {
        ASSERT_TRUE(cgs_count("test", "test") == 1);
    }
    
    TEST("cgs_count: needle overlaps but is not identical");;;
    {
        // count "ababa" in "abababa"
        // [ababa] ba -> 1 match
        unsigned int n = cgs_count("abababa", "ababa");
        ASSERT_TRUE(n == 1);
    }
    
    TEST("cgs_count: empty needle");;;
    {
        char hay[] = "test";
        char needle[] = "";
        unsigned int count = cgs_count(hay, needle);
        ASSERT_TRUE(count == cgs_len(hay) + 1);
    }
    
    TEST("cgs_count: empty haystack");;;
    {
        char hay[] = "";
        char needle[] = "x";
        ASSERT_EQ(cgs_count(hay, needle), 0);
    }
    
    TEST("cgs_count: overlapping occurrences");;;
    {
        char hay[] = "aaaa";
        char needle[] = "aa";
        unsigned int count = cgs_count(hay, needle);
        // Could be 2 (non-overlapping) or 3 (overlapping)
        ASSERT_TRUE(count == 2 || count == 3);
    }
    
    TEST("cgs_count: needle equals haystack");;;
    {
        char hay[] = "exact";
        char needle[] = "exact";
        ASSERT_EQ(cgs_count(hay, needle), 1);
    }
    
    TEST("cgs_count: needle longer than haystack");;;
    {
        char hay[] = "ab";
        char needle[] = "abcdef";
        ASSERT_EQ(cgs_count(hay, needle), 0);
    }
    
    TEST("cgs_count: many occurrences");;;
    {
        char hay[] = "a a a a a a a a a a";
        char needle[] = "a";
        ASSERT_EQ(cgs_count(hay, needle), 10);
    }
}

// ============================================================================
// Edge Case Tests for cgs_clear
// ============================================================================

void test_str_clear_edge_cases() {
    TEST("cgs_clear: already empty CGS_DStr");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_clear(&dstr);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&dstr), 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_clear: CGS_StrBuf at capacity");;;
    {
        char backing[5] = {'f', 'u', 'l', 'l', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 5);
        CGS_Error err = cgs_clear(&sb);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&sb), 0);
    }
    
    TEST("cgs_clear: raw char array (might be unsupported)");;;
    {
        char buf[10] = "test";
        // This might return an error or work
        CGS_Error err = cgs_clear(buf);
        // Just check it doesn't crash
        ASSERT_TRUE(1);
    }
    
    TEST("cgs_clear: then append");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("original");
        cgs_clear(&dstr);
        CGS_Error err = cgs_append(&dstr, "new");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "new"));
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for cgs_starts_with / cgs_ends_with
// ============================================================================

void test_str_starts_ends_with_edge_cases() {
    TEST("cgs_starts_with: empty prefix");;;
    {
        char hay[] = "test";
        char prefix[] = "";
        ASSERT_TRUE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: prefix equals string");;;
    {
        char hay[] = "exact";
        char prefix[] = "exact";
        ASSERT_TRUE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: prefix longer than string");;;
    {
        char hay[] = "ab";
        char prefix[] = "abcdef";
        ASSERT_FALSE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: both empty");;;
    {
        char hay[] = "";
        char prefix[] = "";
        ASSERT_TRUE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_ends_with: empty suffix");;;
    {
        char hay[] = "test";
        char suffix[] = "";
        ASSERT_TRUE(cgs_ends_with(hay, suffix));
    }
    
    TEST("cgs_ends_with: suffix equals string");;;
    {
        char hay[] = "exact";
        char suffix[] = "exact";
        ASSERT_TRUE(cgs_ends_with(hay, suffix));
    }
    
    TEST("cgs_ends_with: suffix longer than string");;;
    {
        char hay[] = "ab";
        char suffix[] = "zabcdef";
        ASSERT_FALSE(cgs_ends_with(hay, suffix));
    }
    
    TEST("cgs_ends_with: single character suffix");;;
    {
        char hay[] = "test";
        char suffix[] = "t";
        ASSERT_TRUE(cgs_ends_with(hay, suffix));
    }
    
    TEST("cgs_starts_with: case sensitive");;;
    {
        char hay[] = "Test";
        char prefix[] = "test";
        ASSERT_FALSE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: NULL hay");;;
    {
        CGS_StrView hay = {0};
        CGS_StrView prefix = cgs_strv("hello");
        ASSERT_FALSE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: NULL needle");;;
    {
        CGS_StrView hay = cgs_strv("hello");
        CGS_StrView prefix = {0};
        ASSERT_TRUE(cgs_starts_with(hay, prefix));
    }
    
    TEST("cgs_starts_with: NULL both");;;
    {
        CGS_StrView hay = {0};
        CGS_StrView prefix = {0};
        ASSERT_TRUE(cgs_starts_with(hay, prefix));
    }
}

// ============================================================================
// Edge Case Tests for cgs_tolower / cgs_toupper
// ============================================================================

void test_str_case_edge_cases() {
    TEST("cgs_tolower: empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("");
        cgs_tolower(&dstr);
        ASSERT_EQ(cgs_len(&dstr), 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tolower: already lowercase");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("alllower");
        cgs_tolower(&dstr);
        ASSERT_TRUE(cgs_equal(&dstr, "alllower"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tolower: with numbers and symbols");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("Test123!@#");
        cgs_tolower(&dstr);
        ASSERT_TRUE(cgs_equal(&dstr, "test123!@#"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_toupper: empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("");
        cgs_toupper(&dstr);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_toupper: already uppercase");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("ALLUPPER");
        cgs_toupper(&dstr);
        ASSERT_TRUE(cgs_equal(&dstr, "ALLUPPER"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tolower: on read-only buffer (should fail)");;;
    {
        char buf[] = "TEST";
        cgs_tolower(buf);
        ASSERT_TRUE(cgs_equal(buf, "test"));
    }
}

// ============================================================================
// Edge Case Tests for cgs_copy
// ============================================================================

void test_str_copy_edge_cases() {
    TEST("cgs_copy: empty to empty");;;
    {
        char dst[10] = "";
        char src[] = "";
        CGS_Error err = cgs_copy(dst, src);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(dst), 0);
    }
    
    TEST("cgs_copy: to buffer exactly sized");;;
    {
        char dst[5];
        char src[] = "test";
        CGS_Error err = cgs_copy(dst, src);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, "test"));
    }
    
    TEST("cgs_copy: to buffer one too small");;;
    {
        char dst[4];
        char src[] = "test";
        CGS_Error err = cgs_copy(dst, src);
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_copy: to CGS_StrBuf at capacity");;;
    {
        char backing[5];
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(backing, 5);
        char src[] = "test";
        CGS_Error err = cgs_copy(&sb, src);
        ASSERT_EQ(err.ec, CGS_OK);
    }
    
    TEST("cgs_copy: overlapping buffers (aliasing)");;;
    {
        char buf[10] = "test";
        // Copying to itself should fail or handle gracefully
        CGS_Error err = cgs_copy(buf, buf);
        // Might return CGS_ALIASING_NOT_SUPPORTED
        ASSERT_TRUE(err.ec == CGS_ALIASING_NOT_SUPPORTED || err.ec == CGS_OK);
    }
    
    TEST("cgs_copy: to CGS_DStr with insufficient capacity");;;
    {
        CGS_DStr dstr = cgs_dstr_init(2);
        char src[] = "this is a long string";
        CGS_Error err = cgs_copy(&dstr, src);
        // Should either reallocate or fail
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_DST_TOO_SMALL || err.ec == CGS_ALLOC_ERROR);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_copy: from CGS_DStr to char array");;;
    {
        CGS_DStr src = cgs_dstr_init_from("source");
        char dst[20];
        CGS_Error err = cgs_copy(dst, &src);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, "source"));
        cgs_dstr_deinit(&src);
    }
}

// ============================================================================
// Edge Case Tests for cgs_putc
// ============================================================================

void test_str_putc_edge_cases() {
    TEST("cgs_putc: to empty CGS_DStr");;;
    {
        CGS_DStr dstr = cgs_dstr_init(0);
        CGS_Error err = cgs_putc(&dstr, 'a');
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_ALLOC_ERROR);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_putc: null character");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_putc(&dstr, '\0');
        // Behavior may vary - might end string or add literal null
        (void)err;
        ASSERT_TRUE(1);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_putc: to CGS_StrBuf at capacity");;;
    {
        char backing[3] = {'a', 'b', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 3);
        CGS_Error err = cgs_putc(&sb, 'c');
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_putc: extended ASCII character");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_putc(&dstr, 0xFF);
        ASSERT_EQ(err.ec, CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_putc: many characters sequentially");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1);
        for (int i = 0; i < 1000; i++) {
            CGS_Error err = cgs_putc(&dstr, 'x');
            if (err.ec != CGS_OK) {
                printf("  Failed at iteration %d with error code %d\n", i, err.ec);
                break;
            }
        }
        ASSERT_EQ(cgs_len(&dstr), 1000);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("writer_t: cgs_putc with FILE*");;;
    {
        FILE* tmp = tmpfile();
        
        cgs_putc(tmp, 'A');
        cgs_putc(tmp, 'B');
        cgs_putc(tmp, 'C');
        
        rewind(tmp);
        char buf[4] = {0};
        fread(buf, 1, 3, tmp);
        
        ASSERT_EQ(strcmp(buf, "ABC"), 0);
        fclose(tmp);
    }
    
    TEST("writer_t: cgs_append with FILE*");;;
    {
        FILE* tmp = tmpfile();
        
        cgs_append(tmp, "Hello, ");
        cgs_append(tmp, "World");
        
        rewind(tmp);
        CGS_DStr result = cgs_dstr_init();
        cgs_fread_line(&result, tmp);
        
        ASSERT_TRUE(cgs_equal(result, "Hello, World"));
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("writer_t: cgs_append_tostr with FILE*");;;
    {
        FILE* tmp = tmpfile();
        
        // Testing integer conversion to file
        cgs_append_tostr(tmp, 1234);
        cgs_putc(tmp, ' ');
        // Testing float conversion to file
        cgs_append_tostr(tmp, cgs_nfmt(5.5, 'f', 2));
        
        rewind(tmp);
        CGS_DStr result = cgs_dstr_init();
        cgs_fread_line(&result, tmp);
        
        ASSERT_EQ(strcmp(cgs_chars(result), "1234 5.50"), 0);
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("writer_t: cgs_append_tostr_many with FILE*");;;
    {
        FILE* tmp = tmpfile();
        
        // Using the variadic printing macro directly to a FILE*
        cgs_append_tostr_many(tmp, "Hex: ", cgs_nfmt(255, 'X'), ", Score: ", 100);
        
        rewind(tmp);
        CGS_DStr result = cgs_dstr_init();
        cgs_fread_line(&result, tmp);
        
        ASSERT_EQ(strcmp(cgs_chars(result), "Hex: FF, Score: 100"), 0);
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_line: basic line reading");;;
    {
        FILE* tmp = tmpfile();
        fputs("line one\nline two", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        cgs_fread_line(&result, tmp);
        ASSERT_TRUE(cgs_equal(result, "line one\n"));
        
        cgs_clear(&result);
        cgs_fread_line(&result, tmp);
        ASSERT_TRUE(cgs_equal(result, "line two"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_append_fread_line: appending multiple lines");;;
    {
        FILE* tmp = tmpfile();
        fputs("first\nsecond", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init_from("init:");
        
        cgs_append_fread_line(&result, tmp); // appends "first\n"
        cgs_append_fread_line(&result, tmp); // appends "second"
        
        ASSERT_TRUE(cgs_equal(result, "init:first\nsecond"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: basic delimiter test");;;
    {
        FILE* tmp = tmpfile();
        fputs("user@hostname.com", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        cgs_fread_until(&result, tmp, '@');
        ASSERT_TRUE(cgs_equal(result, "user@"));
        
        cgs_clear(&result);
        cgs_fread_until(&result, tmp, '.');
        ASSERT_TRUE(cgs_equal(result, "hostname."));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: read until EOF (delimiter not found)");;;
    {
        FILE* tmp = tmpfile();
        fputs("hello world", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        // Search for '!' which doesn't exist
        cgs_fread_until(&result, tmp, '!');
        ASSERT_TRUE(cgs_equal(result, "hello world"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_append_fread_until: building a path");;;
    {
        FILE* tmp = tmpfile();
        fputs("home/user/docs", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init_from("/");
        
        cgs_append_fread_until(&result, tmp, '/'); // appends "home/"
        cgs_append_fread_until(&result, tmp, '/'); // appends "user/"
        
        ASSERT_TRUE(cgs_equal(result, "/home/user/"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_line: empty file and immediate EOF");;;
    {
        FILE* tmp = tmpfile(); // empty
        CGS_DStr result = cgs_dstr_init_from("untouched");
        
        CGS_Error err = cgs_fread_line(&result, tmp);
        
        ASSERT_TRUE(cgs_equal(result, ""));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: basic delimiter inclusion");;;
    {
        FILE* tmp = tmpfile();
        fputs("key:value", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        // Should include the ':'
        cgs_fread_until(&result, tmp, ':');
        ASSERT_TRUE(cgs_equal(result, "key:"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: EOF as delimiter (read whole file)");;;
    {
        FILE* tmp = tmpfile();
        fputs("line 1\nline 2", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        // Using EOF as the delimiter should read until the end
        // EOF itself is not a character, so it is never "included"
        cgs_fread_until(&result, tmp, EOF);
        ASSERT_TRUE(cgs_equal(result, "line 1\nline 2"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: reaching EOF before delim (delim not included)");;;
    {
        FILE* tmp = tmpfile();
        fputs("partial data", tmp); // No semicolon here
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init();
        
        // Searching for ';' but hitting EOF first
        cgs_fread_until(&result, tmp, ';');
        
        // Per requirements: if EOF is reached, the delim (;) won't be included
        ASSERT_TRUE(cgs_equal(result, "partial data"));
        ASSERT_FALSE(cgs_ends_with(result, ";"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_append_fread_until: logic with EOF and appends");;;
    {
        FILE* tmp = tmpfile();
        fputs("part1;part2", tmp);
        rewind(tmp);
        
        CGS_DStr result = cgs_dstr_init_from("Data: ");
        
        // 1. Read until ';' (found, so included)
        cgs_append_fread_until(&result, tmp, ';');
        ASSERT_TRUE(cgs_equal(result, "Data: part1;"));
        
        // 2. Read until ';' again (not found, hits EOF, so NOT included)
        cgs_append_fread_until(&result, tmp, ';');
        ASSERT_TRUE(cgs_equal(result, "Data: part1;part2"));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    TEST("cgs_fread_until: empty file immediately hitting EOF");;;
    {
        FILE* tmp = tmpfile(); // Empty
        CGS_DStr result = cgs_dstr_init();
        
        // Should handle immediate EOF gracefully (result empty, no delim)
        cgs_fread_until(&result, tmp, ':');
        ASSERT_TRUE(cgs_equal(result, ""));
        
        cgs_dstr_deinit(&result);
        fclose(tmp);
    }
    
    /* =========================================================================
     * cgs_fmt — basic copying (no specifiers)
     * ========================================================================= */
    
    TEST("cgs_fmt: no specifiers copies string as-is into CGS_DStr");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: no specifiers copies string as-is into CGS_StrBuf");
    {
        char buf[32];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
    }
    
    TEST("cgs_fmt: no specifiers copies string as-is into FILE*");
    {
        FILE *f = tmpfile();
        CGS_Error err = cgs_appendf(f, "hello world");
        ASSERT_TRUE(err.ec == CGS_OK);
        rewind(f);
        char buf[32] = {0};
        fread(buf, 1, sizeof(buf) - 1, f);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
        fclose(f);
    }
    
    /* =========================================================================
     * cgs_fmt — percent escape
     * ========================================================================= */
    
    TEST("cgs_fmt: %% writes a literal percent sign");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "100%%");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("100%")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: multiple %% escapes all produce literal percent signs");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%%+%%=%%");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("%+%=%")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — single substitution
     * ========================================================================= */
    
    TEST("cgs_fmt: single % substituted with string arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        const char *fmt = "hello %?";
        CGS_Error err = cgs_fmt(&dst, fmt, cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: % at start of format string");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? world", cgs_strv("hello"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: % at end of format string");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: % substituted with empty string arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello%?world", cgs_strv(""));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("helloworld")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — multiple substitutions
     * ========================================================================= */
    
    TEST("cgs_fmt: multiple % substituted in order");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? %? %?",
                                   cgs_strv("one"), cgs_strv("two"), cgs_strv("three"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("one two three")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: mixed %% escapes and % substitutions");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%% of %? is %?",
                                   cgs_strv("50"), cgs_strv("done"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("% of 50 is done")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — arg count errors
     * ========================================================================= */
    
    TEST("cgs_fmt: more % than args returns CGS_NOT_ENOUGH_ARGS");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? and %?", cgs_strv("only one"));
        ASSERT_TRUE(err.ec == CGS_NOT_ENOUGH_ARGS);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: more args than % returns CGS_TOO_MANY_ARGS");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello",
                                   cgs_strv("extra"), cgs_strv("args"));
        ASSERT_TRUE(err.ec == CGS_TOO_MANY_ARGS);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %% escape does not consume an arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%% and %?", cgs_strv("hello"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("% and hello")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — overflow / CGS_DST_TOO_SMALL
     * ========================================================================= */
    
    TEST("cgs_fmt: output exceeding CGS_StrBuf capacity returns CGS_DST_TOO_SMALL");
    {
        char buf[4];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_fmt: substitution exceeding CGS_StrBuf capacity returns CGS_DST_TOO_SMALL");
    {
        char buf[8];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hi %?", cgs_strv("this is way too long"));
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_fmt: output exactly filling CGS_StrBuf capacity succeeds");
    {
        char buf[6];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello");  /* 5 chars + null */
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello")));
    }
    
    /* =========================================================================
     * cgs_fmt — writer type variants
     * ========================================================================= */
    
    TEST("cgs_fmt: writer as char[]");
    {
        char buf[32] = {0};
        CGS_Error err = cgs_fmt(buf, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
    }
    
    TEST("cgs_fmt: writer as CGS_MutStrRef");
    {
        char buf[32] = {0};
        CGS_MutStrRef dst = cgs_mutstr_ref(buf);
        CGS_Error err = cgs_fmt(dst, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
    }
    
    TEST("cgs_fmt: writer as FILE*");
    {
        FILE *f = tmpfile();
        CGS_Error err = cgs_appendf(f, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        rewind(f);
        char buf[32] = {0};
        fread(buf, 1, sizeof(buf) - 1, f);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
        fclose(f);
    }
    
    /* =========================================================================
     * cgs_appendf — appending behaviour
     * ========================================================================= */
    
    TEST("cgs_appendf: appends to existing CGS_DStr content");
    {
        CGS_DStr dst = cgs_dstr_init_from(cgs_strv("hello "));
        CGS_Error err = cgs_appendf(&dst, "world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_appendf: appends with substitution to existing CGS_DStr content");
    {
        CGS_DStr dst = cgs_dstr_init_from(cgs_strv("hello "));
        CGS_Error err = cgs_appendf(&dst, "%?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_appendf: multiple appends accumulate correctly");
    {
        CGS_DStr dst = cgs_dstr_init();
        cgs_appendf(&dst, "one");
        cgs_appendf(&dst, ", two");
        CGS_Error err = cgs_appendf(&dst, ", three");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("one, two, three")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_appendf: appends to existing CGS_StrBuf content");
    {
        char buf[32] = {0};
        CGS_StrBuf dst = cgs_strbuf_init_from_cstr(buf);
        cgs_appendf(&dst, "hello ");
        CGS_Error err = cgs_appendf(&dst, "world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
    }
    
    TEST("cgs_appendf: appending to CGS_StrBuf that leaves no remaining capacity returns CGS_DST_TOO_SMALL");
    {
        char buf[8];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        cgs_appendf(&dst, "hello");   /* 5 chars, 2 remaining (+ null) */
        CGS_Error err = cgs_appendf(&dst, " world");
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_appendf: appends to FILE* after existing content");
    {
        FILE *f = tmpfile();
        fwrite("hello ", 1, 6, f);
        CGS_Error err = cgs_appendf(f, "world");
        ASSERT_TRUE(err.ec == CGS_OK);
        rewind(f);
        char buf[32] = {0};
        fread(buf, 1, sizeof(buf) - 1, f);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
        fclose(f);
    }
    
    TEST("cgs_appendf: appends with substitution to FILE* after existing content");
    {
        FILE *f = tmpfile();
        fwrite("hello ", 1, 6, f);
        CGS_Error err = cgs_appendf(f, "%?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        rewind(f);
        char buf[32] = {0};
        fread(buf, 1, sizeof(buf) - 1, f);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
        fclose(f);
    }
    
    TEST("cgs_appendf: appends to CGS_StrBuf with sufficient remaining capacity");
    {
        char buf[32];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        cgs_appendf(&dst, "hello ");
        CGS_Error err = cgs_appendf(&dst, "world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
    }
    
    TEST("cgs_appendf: appends with substitution to CGS_StrBuf");
    {
        char buf[32];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        cgs_appendf(&dst, "hello ");
        CGS_Error err = cgs_appendf(&dst, "%?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
    }
    
    /* =========================================================================
     * cgs_fmt — basic copying (no specifiers)
     * ========================================================================= */
    
    TEST("cgs_fmt: no specifiers copies string as-is into CGS_DStr");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: no specifiers copies string as-is into CGS_StrBuf");
    {
        char buf[32];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
    }
    
    /* =========================================================================
     * cgs_fmt — percent escape
     * ========================================================================= */
    
    TEST("cgs_fmt: %% writes a literal percent sign");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "100%%");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("100%")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: multiple %% escapes all produce literal percent signs");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%%+%%=%%");
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("%+%=%")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — auto-indexed substitution (%?)
     * ========================================================================= */
    
    TEST("cgs_fmt: single %? substituted with string arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: multiple %? substituted in order");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? %? %?",
                                   cgs_strv("one"), cgs_strv("two"), cgs_strv("three"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("one two three")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %? substituted with integer arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "value is %?", 42);
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("value is 42")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %? substituted with float arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "pi is %?", 3.14f);
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("pi is 3.14")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: mixed %% escape and %? substitution");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%% of %? is %?",
                                   cgs_strv("50"), cgs_strv("done"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("% of 50 is done")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %? at start of format string");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? world", cgs_strv("hello"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %? at end of format string");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %? substituted with empty string arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello%?world", cgs_strv(""));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("helloworld")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — explicit indexed substitution (%[0], %[1], ...)
     * ========================================================================= */
    
    TEST("cgs_fmt: explicit index %[0] selects first arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello %[0]", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello world")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: explicit indices in order");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%[0] %[1] %[2]",
                                   cgs_strv("one"), cgs_strv("two"), cgs_strv("three"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("one two three")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: explicit indices out of order");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%[2] %[0] %[1]",
                                   cgs_strv("one"), cgs_strv("two"), cgs_strv("three"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("three one two")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: explicit index can repeat an arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%[0] and %[0]", cgs_strv("hello"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello and hello")));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: explicit index out of bounds returns CGS_INDEX_OUT_OF_BOUNDS");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%[3]", cgs_strv("only one arg"));
        ASSERT_TRUE(err.ec == CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: mixed %% escape and explicit index substitution");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%% of %[0] is %[1]",
                                   cgs_strv("50"), cgs_strv("done"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("% of 50 is done")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — mixing auto and explicit indexing
     * ========================================================================= */
    
    TEST("cgs_fmt: mixing %? and explicit index returns CGS_BAD_FORMAT");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? and %[0]",
                                   cgs_strv("hello"), cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_BAD_FORMAT);
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — arg count errors
     * ========================================================================= */
    
    TEST("cgs_fmt: more %? than args returns CGS_NOT_ENOUGH_ARGS");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%? and %?", cgs_strv("only one"));
        ASSERT_TRUE(err.ec == CGS_NOT_ENOUGH_ARGS);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: more args than %? returns CGS_TOO_MANY_ARGS");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "hello",
                                   cgs_strv("extra"), cgs_strv("args"));
        ASSERT_TRUE(err.ec == CGS_TOO_MANY_ARGS);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_fmt: %% escape does not consume an arg");
    {
        CGS_DStr dst = cgs_dstr_init();
        CGS_Error err = cgs_fmt(&dst, "%% and %?", cgs_strv("hello"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("% and hello")));
        cgs_dstr_deinit(&dst);
    }
    
    /* =========================================================================
     * cgs_fmt — overflow / CGS_DST_TOO_SMALL
     * ========================================================================= */
    
    TEST("cgs_fmt: output exceeding CGS_StrBuf capacity returns CGS_DST_TOO_SMALL");
    {
        char buf[4];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello world");
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_fmt: substitution exceeding CGS_StrBuf capacity returns CGS_DST_TOO_SMALL");
    {
        char buf[8];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hi %?", cgs_strv("this is way too long"));
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_fmt: output exactly filling CGS_StrBuf capacity succeeds");
    {
        char buf[6];
        CGS_StrBuf dst = cgs_strbuf_init_from_buf(buf);
        CGS_Error err = cgs_fmt(&dst, "hello");  /* 5 chars + null */
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(cgs_equal(dst, cgs_strv("hello")));
    }
    
    /* =========================================================================
     * cgs_fmt — writer type variants
     * ========================================================================= */
    
    TEST("cgs_fmt: writer as char[]");
    {
        char buf[32] = {0};
        CGS_Error err = cgs_fmt(buf, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
    }
    
    TEST("cgs_fmt: writer as CGS_MutStrRef");
    {
        char buf[32] = {0};
        CGS_MutStrRef dst = cgs_mutstr_ref(buf);
        CGS_Error err = cgs_fmt(dst, "hello %?", cgs_strv("world"));
        ASSERT_TRUE(err.ec == CGS_OK);
        ASSERT_TRUE(strcmp(buf, "hello world") == 0);
    }
}

/* ── ASSUMPTIONS (adjust if the real API differs) ─────────────────
 * 1. CGS_Writer holds a write callback; here it's spelled
 *      CGS_Error (*write)(CGS_Writer *self, CGS_StrView data)
 *    and is set via a designated initializer on .base.
 * 2. Because CGS_CustomWriter has CGS_Writer as its FIRST member, a
 *    custom writer recovers its ctx by casting self back:
 *      CGS_CustomWriter *cw = (CGS_CustomWriter *)self;
 * 3. writer_t args take a pointer: &chain, &custom, &dstr, &counter.
 * ---------------------------------------------------------------- */

/* ── a custom writer that counts bytes and records the last chunk ── */

typedef struct CountCtx
{
    unsigned int total;
    unsigned int calls;
    char         last[64];
} CountCtx;

static CGS_Error count_write(CGS_Writer *self, CGS_StrView data)
{
    CGS_CustomWriter *cw  = (CGS_CustomWriter *)self;
    CountCtx         *ctx = cw->ctx;

    ctx->total += data.len;
    ctx->calls += 1;

    unsigned int n = data.len < sizeof(ctx->last) - 1 ? data.len : sizeof(ctx->last) - 1;
    memcpy(ctx->last, data.chars, n);
    ctx->last[n] = '\0';

    return (CGS_Error){CGS_OK};
}

/* ── a custom writer that uppercases into a fixed buffer ────────── */

typedef struct UpperCtx
{
    char         buf[128];
    unsigned int len;
} UpperCtx;

static CGS_Error upper_write(CGS_Writer *self, CGS_StrView data)
{
    CGS_CustomWriter *cw  = (CGS_CustomWriter *)self;
    UpperCtx         *ctx = cw->ctx;

    for (unsigned int i = 0; i < data.len; i++)
    {
        if (ctx->len + 1 >= sizeof(ctx->buf))
            return (CGS_Error){CGS_DST_TOO_SMALL};
        char c = data.chars[i];
        ctx->buf[ctx->len++] = (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
    }
    ctx->buf[ctx->len] = '\0';
    return (CGS_Error){CGS_OK};
}

/* ── a custom writer that always fails ──────────────────────────── */

static CGS_Error failing_write(CGS_Writer *self, CGS_StrView data)
{
    (void)self; (void)data;
    return (CGS_Error){CGS_IO_ERROR};
}


void test_custom_writer(void)
{
    /* ════════════════════════════════════════════════════════════
     * CGS_CustomWriter
     * ════════════════════════════════════════════════════════════ */

    TEST("CGS_CustomWriter: cgs_append routes through the callback");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };

        cgs_append(&cw, "hello");

        ASSERT_TRUE(ctx.total == 5);
        ASSERT_TRUE(ctx.calls >= 1);
        ASSERT_STR_EQ(ctx.last, "hello");
    }

    TEST("CGS_CustomWriter: multiple appends accumulate in ctx");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };

        cgs_append(&cw, "abc");
        cgs_append(&cw, "de");

        ASSERT_TRUE(ctx.total == 5);
        ASSERT_STR_EQ(ctx.last, "de");
    }

    TEST("CGS_CustomWriter: cgs_putc reaches the callback");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };

        cgs_putc(&cw, 'x');

        ASSERT_TRUE(ctx.total == 1);
        ASSERT_STR_EQ(ctx.last, "x");
    }

    TEST("CGS_CustomWriter: cgs_appendf routes formatted output");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };

        cgs_appendf(&cw, "%? + %? = %?", 1, 2, 3);

        /* "1 + 2 = 3" is 9 chars total, however many chunks it arrives in */
        ASSERT_TRUE(ctx.total == 9);
    }

    TEST("CGS_CustomWriter: cgs_append_tostr_many routes all args");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };

        cgs_append_tostr_many(&cw, "ab", 42, "!");

        /* "ab" + "42" + "!" = 5 chars */
        ASSERT_TRUE(ctx.total == 5);
    }

    TEST("CGS_CustomWriter: transforming writer uppercases appended text");
    {
        UpperCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = upper_write }, .ctx = &ctx };

        cgs_append(&cw, "hello, world");

        ASSERT_STR_EQ(ctx.buf, "HELLO, WORLD");
        ASSERT_TRUE(ctx.len == 12);
    }

    TEST("CGS_CustomWriter: transforming writer works with cgs_appendf");
    {
        UpperCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = upper_write }, .ctx = &ctx };

        cgs_appendf(&cw, "value is %?", "forty two");

        ASSERT_STR_EQ(ctx.buf, "VALUE IS FORTY TWO");
    }

    TEST("CGS_CustomWriter: empty append does not corrupt ctx");
    {
        UpperCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = upper_write }, .ctx = &ctx };

        cgs_append(&cw, "");

        ASSERT_TRUE(ctx.len == 0);
        ASSERT_STR_EQ(ctx.buf, "");
    }

    TEST("CGS_CustomWriter: two writers with separate ctx stay independent");
    {
        CountCtx c1 = {0}, c2 = {0};
        CGS_CustomWriter w1 = { .base = { .append = count_write }, .ctx = &c1 };
        CGS_CustomWriter w2 = { .base = { .append = count_write }, .ctx = &c2 };

        cgs_append(&w1, "aaa");
        cgs_append(&w2, "bb");

        ASSERT_TRUE(c1.total == 3);
        ASSERT_TRUE(c2.total == 2);
    }

    TEST("CGS_CustomWriter: error from the callback propagates to the caller");
    {
        CGS_CustomWriter cw = { .base = { .append = failing_write }, .ctx = NULL };

        CGS_Error e = cgs_append(&cw, "anything");
        ASSERT_TRUE(e.ec == CGS_IO_ERROR);
    }

    TEST("CGS_CustomWriter: NULL ctx is fine when the callback ignores it");
    {
        CGS_CustomWriter cw = { .base = { .append = failing_write }, .ctx = NULL };

        CGS_Error e = cgs_putc(&cw, 'z');
        ASSERT_TRUE(e.ec == CGS_IO_ERROR);
    }
}


void test_chain_writer(void)
{
    /* ════════════════════════════════════════════════════════════
     * CGS_ChainWriter — cgs_writer(a, b) writes to both
     * ════════════════════════════════════════════════════════════ */

    TEST("CGS_ChainWriter: append reaches both StrBuf destinations");
    {
        char m1[64] = {0}, m2[64] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        cgs_append(&chain, "hello");

        ASSERT_TRUE(cgs_equal(a, "hello"));
        ASSERT_TRUE(cgs_equal(b, "hello"));
    }

    TEST("CGS_ChainWriter: multiple appends accumulate in both");
    {
        char m1[64] = {0}, m2[64] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        cgs_append(&chain, "abc");
        cgs_append(&chain, "def");

        ASSERT_TRUE(cgs_equal(a, "abcdef"));
        ASSERT_TRUE(cgs_equal(b, "abcdef"));
    }

    TEST("CGS_ChainWriter: cgs_putc reaches both");
    {
        char m1[16] = {0}, m2[16] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        cgs_putc(&chain, 'x');
        cgs_putc(&chain, 'y');

        ASSERT_TRUE(cgs_equal(a, "xy"));
        ASSERT_TRUE(cgs_equal(b, "xy"));
    }

    TEST("CGS_ChainWriter: cgs_appendf reaches both");
    {
        char m1[64] = {0}, m2[64] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        cgs_appendf(&chain, "%? + %? = %?", 1, 2, 3);

        ASSERT_TRUE(cgs_equal(a, "1 + 2 = 3"));
        ASSERT_TRUE(cgs_equal(b, "1 + 2 = 3"));
    }

    TEST("CGS_ChainWriter: mixed destination types (DStr and StrBuf)");
    {
        CGS_DStr d = cgs_dstr_init();
        char m[64] = {0};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(m);

        CGS_ChainWriter chain = cgs_writer(&d, &sb);
        cgs_append(&chain, "mixed");

        ASSERT_TRUE(cgs_equal(d, "mixed"));
        ASSERT_TRUE(cgs_equal(sb, "mixed"));
        cgs_dstr_deinit(&d);
    }

    TEST("CGS_ChainWriter: chain a real sink with a counting writer");
    {
        CGS_DStr d = cgs_dstr_init();
        unsigned int n = 0;

        CGS_ChainWriter chain = cgs_writer(&d, &n);
        cgs_append(&chain, "hello");

        ASSERT_TRUE(cgs_equal(d, "hello"));
        ASSERT_TRUE(n == 5);
        cgs_dstr_deinit(&d);
    }

    TEST("CGS_ChainWriter: two counting writers both increment");
    {
        unsigned int n1 = 0, n2 = 0;

        CGS_ChainWriter chain = cgs_writer(&n1, &n2);
        cgs_appendf(&chain, "%?", "abcdef");

        ASSERT_TRUE(n1 == 6);
        ASSERT_TRUE(n2 == 6);
    }

    TEST("CGS_ChainWriter: chained with a CustomWriter destination");
    {
        CountCtx ctx = {0};
        CGS_CustomWriter cw = { .base = { .append = count_write }, .ctx = &ctx };
        CGS_DStr d = cgs_dstr_init();

        CGS_ChainWriter chain = cgs_writer(&d, &cw);
        cgs_append(&chain, "tapped");

        ASSERT_TRUE(cgs_equal(d, "tapped"));
        ASSERT_TRUE(ctx.total == 6);
        cgs_dstr_deinit(&d);
    }

    TEST("CGS_ChainWriter: two CustomWriters both receive the data");
    {
        UpperCtx u = {0};
        CountCtx c = {0};
        CGS_CustomWriter wu = { .base = { .append = upper_write }, .ctx = &u };
        CGS_CustomWriter wc = { .base = { .append = count_write }, .ctx = &c };

        CGS_ChainWriter chain = cgs_writer(&wu, &wc);
        cgs_append(&chain, "abc");

        ASSERT_STR_EQ(u.buf, "ABC");
        ASSERT_TRUE(c.total == 3);
    }

    TEST("CGS_ChainWriter: nested chains fan out to three sinks");
    {
        unsigned int n = 0;
        CGS_DStr d1 = cgs_dstr_init();
        CGS_DStr d2 = cgs_dstr_init();

        CGS_ChainWriter inner = cgs_writer(&d1, &d2);
        CGS_ChainWriter outer = cgs_writer(&inner, &n);

        cgs_append(&outer, "fanout");

        ASSERT_TRUE(cgs_equal(d1, "fanout"));
        ASSERT_TRUE(cgs_equal(d2, "fanout"));
        ASSERT_TRUE(n == 6);

        cgs_dstr_deinit(&d1);
        cgs_dstr_deinit(&d2);
    }

    TEST("CGS_ChainWriter: empty append leaves both destinations empty");
    {
        char m1[16] = {0}, m2[16] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        cgs_append(&chain, "");

        ASSERT_TRUE(cgs_len(a) == 0);
        ASSERT_TRUE(cgs_len(b) == 0);
    }

    TEST("CGS_ChainWriter: same content regardless of destination order");
    {
        char m1[64] = {0}, m2[64] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(m1);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(m2);

        CGS_ChainWriter ab = cgs_writer(&a, &b);
        cgs_append(&ab, "order");

        ASSERT_TRUE(cgs_equal(a, b));
    }

    TEST("CGS_ChainWriter: error from one destination propagates");
    {
        /* second sink always fails → the chain call reports the error */
        CGS_DStr d = cgs_dstr_init();
        CGS_CustomWriter bad = { .base = { .append = failing_write }, .ctx = NULL };

        CGS_ChainWriter chain = cgs_writer(&d, &bad);
        CGS_Error e = cgs_append(&chain, "data");

        ASSERT_TRUE(e.ec == CGS_IO_ERROR);
        cgs_dstr_deinit(&d);
    }

    TEST("CGS_ChainWriter: destination too small reports DST_TOO_SMALL");
    {
        char big[64] = {0};
        char tiny[4] = {0};
        CGS_StrBuf a = cgs_strbuf_init_from_buf(big);
        CGS_StrBuf b = cgs_strbuf_init_from_buf(tiny);

        CGS_ChainWriter chain = cgs_writer(&a, &b);
        CGS_Error e = cgs_append(&chain, "way too long for tiny");

        ASSERT_TRUE(e.ec == CGS_DST_TOO_SMALL);
    }

    TEST("CGS_ChainWriter: tee a value while measuring its length");
    {
        /* common use: write to the real sink and count in one pass */
        CGS_DStr out = cgs_dstr_init();
        unsigned int len = 0;

        CGS_ChainWriter tee = cgs_writer(&out, &len);
        cgs_appendf(&tee, "%?-%?", "id", 99);

        ASSERT_TRUE(cgs_equal(out, "id-99"));
        ASSERT_TRUE(len == cgs_len(out));
        cgs_dstr_deinit(&out);
    }
}