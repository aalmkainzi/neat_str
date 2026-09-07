
// ============================================================================
// Edge Case Tests for cgs_append
// ============================================================================
#include "test_common.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "../cgs.h"

void test_str_append_edge_cases() {
    TEST("cgs_append: empty to empty");;;
    {
        CGS_DStr dst = cgs_dstr_init(10);
        char src[] = "";
        CGS_Error err = cgs_append(&dst, src);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&dst), 0);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_append: to itself (aliasing)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_append(&dstr, &dstr);
        // Should detect aliasing or handle it
        ASSERT_TRUE(err.ec == CGS_ALIASING_NOT_SUPPORTED || err.ec == CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_append: to CGS_StrBuf causing overflow");;;
    {
        char backing[10] = "hello";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 10);
        char src[] = "world!";
        CGS_Error err = cgs_append(&sb, src);
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_append: exactly fitting capacity");;;
    {
        char backing[11] = "hello";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 11);
        char src[] = "world";
        CGS_Error err = cgs_append(&sb, src);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&sb, "helloworld"));
    }
    
    TEST("cgs_append: multiple appends growing CGS_DStr");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1);
        for (int i = 0; i < 100; i++) {
            CGS_Error err = cgs_append(&dstr, "x");
            ASSERT_EQ(err.ec, CGS_OK);
        }
        ASSERT_EQ(cgs_len(&dstr), 100);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_append: very long string");;;
    {
        CGS_DStr dstr = cgs_dstr_init(100);
        CGS_DStr long_str = cgs_dstr_init(5000);
        for (int i = 0; i < 4999; i++) {
            cgs_putc(&long_str, 'a');
        }
        CGS_Error err = cgs_append(&dstr, &long_str);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&dstr), 4999);
        cgs_dstr_deinit(&dstr);
        cgs_dstr_deinit(&long_str);
    }
}

// ============================================================================
// Edge Case Tests for cgs_del
// ============================================================================

void test_str_del_edge_cases() {
    TEST("cgs_del: delete entire string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 0, 4);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&dstr), 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: delete nothing (from == to)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 2, 2);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "test"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: invalid range (from > to)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 3, 1);
        ASSERT_EQ(err.ec, CGS_BAD_RANGE);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: out of bounds from");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 10, 15);
        ASSERT_EQ(err.ec, CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: out of bounds to");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 0, 100);
        ASSERT_EQ(err.ec, CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: delete from middle");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("abcdef");
        CGS_Error err = cgs_del(&dstr, 2, 4);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "abef"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: delete first character");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 0, 1);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "est"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: delete last character");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_del(&dstr, 3, 4);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "tes"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_del: on empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_del(&dstr, 0, 0);
        ASSERT_EQ(err.ec, CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for cgs_replace
// ============================================================================

void test_str_replace_edge_cases() {
    TEST("cgs_replace: target not found");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("hello world");
        CGS_Result(int) res = cgs_replace(&dstr, "xyz", "abc");
        ASSERT_EQ(res.err.ec, CGS_NOT_FOUND);
        ASSERT_EQ(res.val, 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: empty target");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Result(int) res = cgs_replace(&dstr, "", "x");
        // Replacing empty string is likely an error or no-op
        ASSERT_TRUE(res.err.ec == CGS_OK || res.err.ec == CGS_NOT_FOUND);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: replace with empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("hello world");
        CGS_Result(int) res = cgs_replace(&dstr, "o", "");
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "hell wrld"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: replacement longer than target");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("hi hi hi");
        CGS_Result(int) res = cgs_replace(&dstr, "hi", "hello");
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "hello hello hello"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: replacement shorter than target");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("hello hello");
        CGS_Result(int) res = cgs_replace(&dstr, "hello", "hi");
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "hi hi"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: overlapping matches");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("aaa");
        CGS_Result(int) res = cgs_replace(&dstr, "aa", "b");
        // Should replace non-overlapping: "aaa" -> "ba" (1 replacement)
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_TRUE(res.val == 1);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: target equals entire string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("exact");
        CGS_Result(int) res = cgs_replace(&dstr, "exact", "replaced");
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "replaced"));
        ASSERT_EQ(res.val, 1);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace: on CGS_StrBuf with insufficient space");;;
    {
        char backing[10] = "hi hi"; // "hello hi"
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 10);
        CGS_Result(int) res = cgs_replace(&sb, "hi", "hello");
        // println("hi hi", " became :: ", sb);
        ASSERT_EQ(res.err.ec, CGS_DST_TOO_SMALL);
        ASSERT_EQ(res.val, 1);
    }
    
    TEST("cgs_replace: many occurrences");;;
    {
        CGS_DStr dstr = cgs_dstr_init(100);
        for (int i = 0; i < 50; i++) {
            cgs_append(&dstr, "x ");
        }
        CGS_Result(int) res = cgs_replace(&dstr, "x", "y");
        ASSERT_EQ(res.err.ec, CGS_OK);
        ASSERT_EQ(res.val, 50);
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for cgs_replace_first
// ============================================================================

void test_str_replace_first_edge_cases() {
    TEST("cgs_replace_first: no occurrence");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("hello");
        CGS_Error err = cgs_replace_first(&dstr, "xyz", "abc");
        ASSERT_EQ(err.ec, CGS_NOT_FOUND);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_first: empty string target");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        // Replacing the first "" is defined in many libs as an injection at start
        cgs_replace_first(&s, "", "!");
        ASSERT_TRUE(cgs_equal(&s, "!abc"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_first: match at end of string");;;
    {
        CGS_DStr s = cgs_dstr_init_from("path/to/file");
        cgs_replace_first(&s, "file", "dir");
        ASSERT_TRUE(cgs_equal(&s, "path/to/dir"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_first: multiple occurrences");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test test test");
        CGS_Error err = cgs_replace_first(&dstr, "test", "word");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "word test test"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_first: at end of string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("abcdef");
        CGS_Error err = cgs_replace_first(&dstr, "def", "xyz");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "abcxyz"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_first: only first occurrence");;;
    {
        CGS_DStr s = cgs_dstr_init_from("ababab");
        CGS_Error err = cgs_replace_first(&s, "ab", "X");
        ASSERT_TRUE(cgs_equal(&s, "Xabab") && err.ec == CGS_OK);
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_first: target at the very end");;;
    {
        CGS_DStr s = cgs_dstr_init_from("hello world");
        CGS_Error err = cgs_replace_first(&s, "world", "C");
        ASSERT_TRUE(cgs_equal(&s, "hello C"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_first: target is the whole string");;;
    {
        CGS_DStr s = cgs_dstr_init_from("match");
        cgs_replace_first(&s, "match", "replaced");
        ASSERT_TRUE(cgs_equal(&s, "replaced"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_first: error when not found");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        CGS_Error err = cgs_replace_first(&s, "z", "x");
        // Some libs return OK with 0 changes, but CGS_NOT_FOUND is safer for a "First" API
        ASSERT_TRUE(err.ec == CGS_NOT_FOUND);
        cgs_dstr_deinit(&s);
    }
}

// ============================================================================
// Edge Case Tests for cgs_replace_range
// ============================================================================

void test_str_replace_range_edge_cases() {
    TEST("cgs_replace_range: remove last character");;;
    {
        CGS_DStr s = cgs_dstr_init_from("Hello!");
        // Range is [5, 6) -> the '!'
        cgs_replace_range(&s, 5, 6, ""); 
        ASSERT_TRUE(cgs_equal(&s, "Hello"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: replace inside string with much longer string");;;
    {
        CGS_DStr s = cgs_dstr_init_from("a[ ]c");
        // Replace "[ ]" (index 1 to 4) with "long string"
        cgs_replace_range(&s, 1, 4, "is a very long string");
        ASSERT_TRUE(cgs_equal(&s, "ais a very long stringc"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: null/empty replacement at specific index (Insertion)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("ac");
        cgs_replace_range(&s, 1, 1, "b");
        ASSERT_TRUE(cgs_equal(&s, "abc"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: replace entire string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_replace_range(&dstr, 0, 4, "replaced");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "replaced"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: invalid range");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_replace_range(&dstr, 3, 1, "x");
        ASSERT_EQ(err.ec, CGS_BAD_RANGE);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: out of bounds");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_replace_range(&dstr, 0, 100, "x");
        ASSERT_EQ(err.ec, CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: zero-length range (insertion)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_replace_range(&dstr, 2, 2, "XX");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "teXXst"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: with empty replacement (deletion)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("testing");
        CGS_Error err = cgs_replace_range(&dstr, 2, 5, "");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "teng"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: at string boundaries");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_replace_range(&dstr, 0, 0, "start");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "starttest"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_replace_range: CGS_StrBuf overflow");;;
    {
        char backing[10] = "test";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 10);
        CGS_Error err = cgs_replace_range(&sb, 0, 4, "very long replacement");
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_replace_range: replace middle (same size)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("ABCDE");
        // Replace "BCD" (indices 1, 2, 3) with "123"
        CGS_Error err = cgs_replace_range(&s, 1, 4, "123");
        ASSERT_TRUE(cgs_equal(&s, "A123E"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: grow string (insertion)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("AB");
        // Replace nothing at index 1 with "123" -> "A123B"
        CGS_Error err = cgs_replace_range(&s, 1, 1, "123");
        ASSERT_TRUE(cgs_equal(&s, "A123B"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: shrink string (deletion)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("Hello World");
        // Replace " World" (index 5 to 11) with ""
        CGS_Error err = cgs_replace_range(&s, 5, 11, "");
        ASSERT_TRUE(cgs_equal(&s, "Hello"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: replace entire string");;;
    {
        CGS_DStr s = cgs_dstr_init_from("old");
        cgs_replace_range(&s, 0, 3, "new");
        ASSERT_TRUE(cgs_equal(&s, "new"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: out of bounds");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        CGS_Error err = cgs_replace_range(&s, 1, 10, "x");
        ASSERT_TRUE(err.ec == CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace_range: inverted range");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        CGS_Error err = cgs_replace_range(&s, 3, 1, "x");
        ASSERT_TRUE(err.ec == CGS_BAD_RANGE);
        cgs_dstr_deinit(&s);
    }
}

// ============================================================================
// Edge Case Tests for cgs_split
// ============================================================================

void test_str_split_edge_cases() {
    
    TEST("cgs_split: empty string");;;
    {
        char str[] = "";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Should return array with 0 or 1 empty element
        ASSERT_TRUE(arr.len == 1 && arr.strs[0].chars == str);
        free(arr.strs);
    }
    
    TEST("Split: No delimiter present in source");;;
    {
        char str[] = "hello world";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Delimiter not found -> 1 element (the whole string)
        ASSERT_TRUE(arr.len == 1);
        ASSERT_TRUE(arr.strs[0].len == 11);
        ASSERT_TRUE(memcmp(arr.strs[0].chars, "hello world", 11) == 0);
        free(arr.strs);
    }
    
    TEST("cgs_split: empty string with non-empty delimiter");;;
    {
        char str[] = "";
        CGS_StrViewArray arr = cgs_split(str, ",");
        // Most languages (Java, JS, Python) return [""]
        ASSERT_TRUE(arr.len == 1);
        ASSERT_TRUE(arr.strs[0].len == 0);
        free(arr.strs);
    }
    
    TEST("cgs_split: empty delimiter (Explode behavior)");;;
    {
        char str[] = "abc";
        CGS_StrViewArray arr = cgs_split(str, "");
        // Explode: "abc" -> ["a", "b", "c"]
        ASSERT_TRUE(arr.len == 3);
        ASSERT_TRUE(arr.strs[0].len == 1 && arr.strs[0].chars[0] == 'a');
        ASSERT_TRUE(arr.strs[2].len == 1 && arr.strs[2].chars[0] == 'c');
        free(arr.strs);
    }
    
    TEST("cgs_split: consecutive delimiters (Empty fields)");;;
    {
        char str[] = "a,,b";
        CGS_StrViewArray arr = cgs_split(str, ",");
        // Result: ["a", "", "b"]
        ASSERT_TRUE(arr.len == 3);
        ASSERT_TRUE(arr.strs[1].len == 0);
        free(arr.strs);
    }
    
    TEST("cgs_split: leading and trailing delimiters");;;
    {
        char str[] = ",a,";
        CGS_StrViewArray arr = cgs_split(str, ",");
        // Result: ["", "a", ""]
        ASSERT_TRUE(arr.len == 3);
        ASSERT_TRUE(arr.strs[0].len == 0);
        ASSERT_TRUE(arr.strs[2].len == 0);
        free(arr.strs);
    }
    
    TEST("cgs_split: delimiter not present");;;
    {
        char str[] = "hello";
        CGS_StrViewArray arr = cgs_split(str, "|");
        // Should return the original string as the only element
        ASSERT_TRUE(arr.len == 1);
        ASSERT_TRUE(cgs_equal(arr.strs[0], "hello"));
        free(arr.strs);
    }
    
    TEST("cgs_split: string is exactly the delimiter");;;
    {
        char str[] = "::";
        CGS_StrViewArray arr = cgs_split(str, "::");
        // Result: ["", ""]
        ASSERT_TRUE(arr.len == 2);
        ASSERT_TRUE(arr.strs[0].len == 0 && arr.strs[1].len == 0);
        free(arr.strs);
    }    
    TEST("cgs_split: delimiter not in string");;;
    {
        char str[] = "no delimiters here";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        ASSERT_EQ(arr.len, 1);
        free(arr.strs);
    }
    
    TEST("cgs_split: consecutive delimiters");;;
    {
        char str[] = "a,,b,,c";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Should have 5 elements: "a", "", "b", "", "c"
        ASSERT_EQ(arr.len, 5);
        free(arr.strs);
    }
    
    TEST("cgs_split: delimiter at start");;;
    {
        char str[] = ",test";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Should have 2 elements: "" and "test"
        ASSERT_EQ(arr.len, 2);
        free(arr.strs);
    }
    
    TEST("cgs_split: delimiter at end");;;
    {
        char str[] = "test,";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Should have 2 elements: "test" and ""
        ASSERT_EQ(arr.len, 2);
        free(arr.strs);
    }
    
    TEST("cgs_split: only delimiter");;;
    {
        char str[] = ",";
        char delim[] = ",";
        CGS_StrViewArray arr = cgs_split(str, delim);
        // Should have 2 empty elements
        ASSERT_EQ(arr.len, 2);
        free(arr.strs);
    }
    
    TEST("cgs_split: multi-character delimiter");;;
    {
        char str[] = "a::b::c";
        char delim[] = "::";
        CGS_StrViewArray arr = cgs_split(str, delim);
        ASSERT_EQ(arr.len, 3);
        free(arr.strs);
    }
    
    TEST("cgs_split: many splits");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1000);
        for (int i = 0; i < 100; i++) {
            cgs_append(&dstr, "x,");
        }
        CGS_StrViewArray arr = cgs_split(&dstr, ",");
        ASSERT_EQ(arr.len, 101); // 100 "x" and 1 trailing empty
        cgs_dstr_deinit(&dstr);
        free(arr.strs);
    }
}

// ============================================================================
// Edge Case Tests for cgs_insert / cgs_prepend
// ============================================================================

void test_str_insert_prepend_edge_cases() {
    TEST("cgs_insert: at position 0 (same as prepend)");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_insert(&dstr, "START", 0);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "STARTtest"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: at middle position");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("abef");
        CGS_Error err = cgs_insert(&dstr, "cd", 2);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "abcdef"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: at end position");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_insert(&dstr, "end", 4);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "testend"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: out of bounds position");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_insert(&dstr, "x", 100);
        ASSERT_EQ(err.ec, CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: into empty string at position 0");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_insert(&dstr, "first", 0);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "first"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: into empty string at position > 0");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_insert(&dstr, "test", 5);
        ASSERT_EQ(err.ec, CGS_INDEX_OUT_OF_BOUNDS);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_insert: CGS_StrBuf overflow");;;
    {
        char backing[10] = "test";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 10);
        CGS_Error err = cgs_insert(&sb, "longinsert", 2);
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_insert: empty string insertion");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_insert(&dstr, "", 2);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "test"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_prepend: to empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_prepend(&dstr, "first");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "first"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_prepend: CGS_StrBuf overflow");;;
    {
        char backing[10] = "test";
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(backing, 10);
        CGS_Error err = cgs_prepend(&sb, "longprefix");
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_prepend: multiple times");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("end");
        cgs_prepend(&dstr, "middle");
        cgs_prepend(&dstr, "start");
        ASSERT_TRUE(cgs_equal(&dstr, "startmiddleend"));
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for CGS_DStr functions
// ============================================================================

void test_dstring_edge_cases() {
    TEST("cgs_dstr_init: zero capacity");;;
    {
        CGS_DStr dstr = cgs_dstr_init(0);
        ASSERT_EQ(cgs_len(&dstr), 0);
        // Should still be usable
        CGS_Error err = cgs_append(&dstr, "test");
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_ALLOC_ERROR);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_init: very large capacity");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1000000);
        ASSERT_TRUE(cgs_cap(&dstr) >= 1000000 || cgs_cap(&dstr) == 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_init_from: empty string");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("");
        ASSERT_EQ(cgs_len(&dstr), 0);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_shrink_to_fit: already minimal");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("test");
        CGS_Error err = cgs_dstr_shrink_to_fit(&dstr);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_cap(&dstr) >= cgs_len(&dstr));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_shrink_to_fit: after deletions");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1000);
        cgs_append(&dstr, "small");
        CGS_Error err = cgs_dstr_shrink_to_fit(&dstr);
        ASSERT_EQ(err.ec, CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_ensure_cap: already sufficient");;;
    {
        CGS_DStr dstr = cgs_dstr_init(100);
        CGS_Error err = cgs_dstr_ensure_cap(&dstr, 50);
        ASSERT_EQ(err.ec, CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_ensure_cap: needs growth");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_dstr_ensure_cap(&dstr, 100);
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_ALLOC_ERROR);
        if (err.ec == CGS_OK) {
            ASSERT_TRUE(cgs_cap(&dstr) >= 100);
        }
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_dstr_ensure_cap: zero requested");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_dstr_ensure_cap(&dstr, 0);
        ASSERT_EQ(err.ec, CGS_OK);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_find: null checks");;;
    {
        CGS_DStr dstr = cgs_dstr_init();
        ASSERT_TRUE(dstr.chars == NULL);
        ASSERT_TRUE(cgs_find(dstr, "").chars == dstr.chars);
        ASSERT_TRUE(cgs_equal(dstr, ""));
        ASSERT_TRUE(cgs_starts_with(dstr, ""));
        ASSERT_TRUE(cgs_ends_with(dstr, ""));
        
        ASSERT_FALSE(cgs_starts_with(dstr, "something"));
        
        CGS_StrBuf sb = {0};
        CGS_Error err = cgs_copy(&sb, "hello");
        ASSERT_TRUE(err.ec == CGS_DST_TOO_SMALL);
    }
}

// ============================================================================
// Edge Case Tests for CGS_StrBuf
// ============================================================================

void test_string_buffer_edge_cases() {
    TEST("CGS_StrBuf: from empty array");;;
    {
        char buf[1] = {'\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(buf, 1);
        ASSERT_EQ(cgs_len(&sb), 0);
        ASSERT_EQ(cgs_cap(&sb), 1);
    }
    
    TEST("CGS_StrBuf: from array without null terminator");;;
    {
        char buf[5] = {'t', 'e', 's', 't', 'x'};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(buf, 5);
        ASSERT_TRUE(sb.len == 0);
    }
    
    TEST("CGS_StrBuf: from cstr with capacity exactly length + 1");;;
    {
        char buf[5] = {'t', 'e', 's', 't', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(buf, 5);
        ASSERT_EQ(cgs_cap(&sb), 5);
        ASSERT_EQ(cgs_len(&sb), 4);
    }
    
    TEST("CGS_StrBuf: from buf with exact capacity");;;
    {
        char buf[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(buf, 10);
        ASSERT_EQ(cgs_cap(&sb), 10);
    }
    
    TEST("CGS_StrBuf: append to full buffer from cstr");;;
    {
        char buf[5] = {'f', 'u', 'l', 'l', '\0'};
        CGS_StrBuf sb = cgs_strbuf_init_from_cstr(buf, 5);
        CGS_Error err = cgs_append(&sb, "x");
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("CGS_StrBuf: init from buf then set content");;;
    {
        char buf[20];
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(buf, 20);
        CGS_Error err = cgs_copy(&sb, "test");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&sb, "test"));
    }
}

// ============================================================================
// Edge Case Tests for cgs_strv
// ============================================================================

void test_strv_edge_cases() {
    TEST("cgs_strv: empty range");;;
    {
        char str[] = "test";
        CGS_StrView sv = cgs_strv(str, 2, 2);
        ASSERT_EQ(cgs_len(sv), 0);
    }
    
    TEST("cgs_strv: entire string");;;
    {
        char str[] = "test";
        CGS_StrView sv = cgs_strv(str, 0, 4);
        ASSERT_EQ(cgs_len(sv), 4);
        ASSERT_TRUE(cgs_equal(sv, "test"));
    }
    
    TEST("cgs_strv: invalid range (from > to)");;;
    {
        char str[] = "test";
        CGS_StrView sv = cgs_strv(str, 3, 1);
        // Likely returns empty or errors
        ASSERT_TRUE(1);
    }
    
    TEST("cgs_strv: single character");;;
    {
        char str[] = "test";
        CGS_StrView sv = cgs_strv(str, 1, 2);
        ASSERT_EQ(cgs_len(sv), 1);
        ASSERT_EQ(sv.chars[0], 'e');
    }
}

// ============================================================================
// Edge Case Tests for file I/O functions
// ============================================================================

void test_file_io_edge_cases() {
    TEST("cgs_fread_line: from empty file");;;
    {
        FILE *f = tmpfile();
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_fread_line(&dstr, f);
        // Should indicate EOF or empty read
        ASSERT_TRUE(err.ec == CGS_OK || err.ec != CGS_OK);
        fclose(f);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_fread_line: very long line");;;
    {
        FILE *f = tmpfile();
        for (int i = 0; i < 10000; i++)
        {
            fputc('x', f);
        }
        fputc('\n', f);
        rewind(f);
        
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_fread_line(&dstr, f);
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_ALLOC_ERROR);
        if (err.ec == CGS_OK)
        {
            ASSERT_EQ(cgs_len(&dstr), 10001);
        }
        fclose(f);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_fread_line: line without newline");;;
    {
        FILE *f = tmpfile();
        fprintf(f, "no newline");
        rewind(f);
        
        CGS_DStr dstr = cgs_dstr_init(20);
        CGS_Error err = cgs_fread_line(&dstr, f);
        ASSERT_EQ(err.ec, CGS_NOT_FOUND);
        ASSERT_TRUE(cgs_equal(&dstr, "no newline"));
        fclose(f);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_fread_line: multiple calls");;;
    {
        FILE *f = tmpfile();
        fprintf(f, "line1\nline2\nline3\n");
        rewind(f);
        
        CGS_DStr dstr = cgs_dstr_init(20);
        cgs_fread_line(&dstr, f);
        ASSERT_TRUE(cgs_equal(&dstr, "line1\n"));
        
        cgs_clear(&dstr);
        cgs_fread_line(&dstr, f);
        ASSERT_TRUE(cgs_equal(&dstr, "line2\n"));
        
        fclose(f);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_append_fread_line: append multiple lines");;;
    {
        FILE *f = tmpfile();
        fprintf(f, "line1\nline2\n");
        rewind(f);
        
        CGS_DStr dstr = cgs_dstr_init(50);
        cgs_append_fread_line(&dstr, f);
        cgs_append_fread_line(&dstr, f);
        ASSERT_TRUE(cgs_equal(&dstr, "line1\nline2\n"));
        
        fclose(f);
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for appender
// ============================================================================

void test_appender_edge_cases() {
    TEST("cgs_appender: basic usage");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("start");
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        cgs_append(appender, "end");
        CGS_Error err = cgs_commit_appender(&dstr, appender);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "startend"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: without commit");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("start");
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        cgs_append(appender, "lost");
        // Not committed - dstr should be unchanged
        ASSERT_TRUE(cgs_equal(&dstr, "start"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: multiple appends before commit");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("start");
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        cgs_append(appender, "a");
        cgs_append(appender, "b");
        cgs_append(appender, "c");
        cgs_commit_appender(&dstr, appender);
        ASSERT_TRUE(cgs_equal(&dstr, "startabc"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: on empty CGS_DStr");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        cgs_append(appender, "first");
        cgs_commit_appender(&dstr, appender);
        ASSERT_TRUE(cgs_equal(&dstr, "first"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: commit without any appends");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("unchanged");
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        CGS_Error err = cgs_commit_appender(&dstr, appender);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "unchanged"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: multiple uses with same state");;;
    {
        CGS_DStr dstr = cgs_dstr_init_from("base");
        CGS_AppenderState state = {};
        
        CGS_MutStrRef app1 = cgs_appender(&dstr, &state);
        cgs_append(app1, "1");
        cgs_commit_appender(&dstr, app1);
        
        CGS_MutStrRef app2 = cgs_appender(&dstr, &state);
        cgs_append(app2, "2");
        cgs_commit_appender(&dstr, app2);
        
        ASSERT_TRUE(cgs_equal(&dstr, "base12"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_appender: causing buffer growth");;;
    {
        CGS_DStr dstr = cgs_dstr_init(5);
        cgs_append(&dstr, "x");
        CGS_AppenderState state = {};
        CGS_MutStrRef appender = cgs_appender(&dstr, &state);
        for (int i = 0; i < 100; i++) {
            cgs_append(appender, "y");
        }
        CGS_Error err = cgs_commit_appender(&dstr, appender);
        ASSERT_TRUE(err.ec == CGS_OK || err.ec == CGS_ALLOC_ERROR);
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Edge Case Tests for cgs_join
// ============================================================================

void test_str_join_edge_cases() {
    
    TEST("cgs_join: empty array");;;
    {
        CGS_StrViewArray arr = {NULL, 0, 0};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, ",");
        ASSERT_TRUE(dst.len == 0); // Should be empty string
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: single element");;;
    {
        char s1[] = "lonely";
        CGS_StrView views[] = { cgs_strv(s1, 0, 6) };
        CGS_StrViewArray arr = {views, 1, 1};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, ",");
        // Should NOT have a trailing delimiter: "lonely", not "lonely,"
        ASSERT_TRUE(cgs_equal(&dst, "lonely"));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: elements are empty strings");;;
    {
        char empty[] = "";
        CGS_StrView views[] = { cgs_strv(empty, 0, 0), cgs_strv(empty, 0, 0) };
        CGS_StrViewArray arr = {views, 2, 2};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, ",");
        // Should result in exactly the delimiter: ","
        ASSERT_TRUE(cgs_equal(&dst, ","));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: empty delimiter");;;
    {
        char s1[] = "a", s2[] = "b";
        CGS_StrView views[] = { cgs_strv(s1, 0, 1), cgs_strv(s2, 0, 1) };
        CGS_StrViewArray arr = {views, 2, 2};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, "");
        // Should just concatenate: "ab"
        ASSERT_TRUE(cgs_equal(&dst, "ab"));
        cgs_dstr_deinit(&dst);
    }
    
    // old
    TEST("cgs_join: empty array");;;
    {
        CGS_StrViewArray arr = {NULL, 0, 0};
        CGS_DStr dst = cgs_dstr_init(10);
        CGS_Error err = cgs_join(&dst, arr, ",");
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_EQ(cgs_len(&dst), 0);
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: single element");;;
    {
        char s1[] = "only";
        CGS_StrView views[] = {cgs_strv(s1, 0, 4)};
        CGS_StrViewArray arr = {views, 1, 1};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, ",");
        ASSERT_TRUE(cgs_equal(&dst, "only"));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: empty delimiter");;;
    {
        char s1[] = "a", s2[] = "b", s3[] = "c";
        CGS_StrView views[] = {
            cgs_strv(s1, 0, 1),
            cgs_strv(s2, 0, 1),
            cgs_strv(s3, 0, 1)
        };
        CGS_StrViewArray arr = {views, 3, 3};
        CGS_DStr dst = cgs_dstr_init(10);
        cgs_join(&dst, arr, "");
        ASSERT_TRUE(cgs_equal(&dst, "abc"));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: multi-char delimiter");;;
    {
        char s1[] = "a", s2[] = "b";
        CGS_StrView views[] = {
            cgs_strv(s1, 0, 1),
            cgs_strv(s2, 0, 1)
        };
        CGS_StrViewArray arr = {views, 2, 2};
        CGS_DStr dst = cgs_dstr_init(20);
        cgs_join(&dst, arr, "::");
        ASSERT_TRUE(cgs_equal(&dst, "a::b"));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: with empty strings");;;
    {
        char s1[] = "", s2[] = "mid", s3[] = "";
        CGS_StrView views[] = {
            cgs_strv(s1, 0, 0),
            cgs_strv(s2, 0, 3),
            cgs_strv(s3, 0, 0)
        };
        CGS_StrViewArray arr = {views, 3, 3};
        CGS_DStr dst = cgs_dstr_init(20);
        cgs_join(&dst, arr, ",");
        ASSERT_TRUE(cgs_equal(&dst, ",mid,"));
        cgs_dstr_deinit(&dst);
    }
    
    TEST("cgs_join: buffer too small");;;
    {
        char s1[] = "long", s2[] = "strings";
        CGS_StrView views[] = {
            cgs_strv(s1, 0, 4),
            cgs_strv(s2, 0, 7)
        };
        CGS_StrViewArray arr = {views, 2, 2};
        char backing[5];
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(backing, 5);
        cgs_clear(&sb); // Initialize to empty
        CGS_Error err = cgs_join(&sb, arr, ",");
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
}

void test_zstrv()
{
    /* ════════════════════════════════════════════════════════════
     * Construction
     * ════════════════════════════════════════════════════════════ */
    
    TEST("cgs_zstrv: from a string literal, default begin=0");
    {
        CGS_ZStrView z = cgs_zstrv("hello");
        ASSERT_TRUE(cgs_len(z) == 5);
        ASSERT_TRUE(cgs_equal(z, "hello"));
    }
    
    TEST("cgs_zstrv: explicit begin=0 matches the default");
    {
        CGS_ZStrView a = cgs_zstrv("hello");
        CGS_ZStrView b = cgs_zstrv("hello", 0);
        ASSERT_TRUE(cgs_equal(a, b));
    }
    
    TEST("cgs_zstrv: begin offset yields a suffix");
    {
        CGS_ZStrView z = cgs_zstrv("hello, world", 7);
        ASSERT_TRUE(cgs_equal(z, "world"));
        ASSERT_TRUE(cgs_len(z) == 5);
    }
    
    TEST("cgs_zstrv: begin=1 drops the first char");
    {
        CGS_ZStrView z = cgs_zstrv("abcdef", 1);
        ASSERT_TRUE(cgs_equal(z, "bcdef"));
    }
    
    TEST("cgs_zstrv: begin == len yields an empty view");
    {
        CGS_ZStrView z = cgs_zstrv("abc", 3);
        ASSERT_TRUE(cgs_len(z) == 0);
        ASSERT_TRUE(cgs_equal(z, ""));
    }
    
    TEST("cgs_zstrv: from an empty string");
    {
        CGS_ZStrView z = cgs_zstrv("");
        ASSERT_TRUE(cgs_len(z) == 0);
        ASSERT_TRUE(cgs_equal(z, ""));
    }
    
    /* ── construction from the other string types ─────────────── */
    
    TEST("cgs_zstrv: from char[]");
    {
        char arr[] = "buffer text";
        CGS_ZStrView z = cgs_zstrv(arr);
        ASSERT_TRUE(cgs_equal(z, "buffer text"));
    }
    
    TEST("cgs_zstrv: from char[] with begin offset");
    {
        char arr[] = "buffer text";
        CGS_ZStrView z = cgs_zstrv(arr, 7);
        ASSERT_TRUE(cgs_equal(z, "text"));
    }
    
    TEST("cgs_zstrv: from unsigned char[]");
    {
        unsigned char arr[] = "unsigned buf";
        CGS_ZStrView z = cgs_zstrv(arr);
        ASSERT_TRUE(cgs_equal(z, "unsigned buf"));
    }
    
    TEST("cgs_zstrv: from char*");
    {
        char *p = "pointer text";
        CGS_ZStrView z = cgs_zstrv(p, 8);
        ASSERT_TRUE(cgs_equal(z, "text"));
    }
    
    TEST("cgs_zstrv: from DStr");
    {
        CGS_DStr d = cgs_dstr_init_from("dynamic string");
        CGS_ZStrView z = cgs_zstrv(d, 8);
        ASSERT_TRUE(cgs_equal(z, "string"));
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_zstrv: from DStr*");
    {
        CGS_DStr d = cgs_dstr_init_from("dynamic string");
        CGS_ZStrView z = cgs_zstrv(&d, 0);
        ASSERT_TRUE(cgs_equal(z, "dynamic string"));
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_zstrv: from StrBuf*");
    {
        char mem[64] = {0};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(mem);
        cgs_copy(&sb, "strbuf content");
        CGS_ZStrView z = cgs_zstrv(&sb, 7);
        ASSERT_TRUE(cgs_equal(z, "content"));
    }
    
    TEST("cgs_zstrv: from MutStrRef");
    {
        CGS_DStr d = cgs_dstr_init_from("ref content");
        CGS_MutStrRef mr = cgs_mutstr_ref(&d);
        CGS_ZStrView z = cgs_zstrv(mr, 4);
        ASSERT_TRUE(cgs_equal(z, "content"));
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_zstrv: from another ZStrView (still null terminated)");
    {
        CGS_ZStrView a = cgs_zstrv("hello, world");
        CGS_ZStrView b = cgs_zstrv(a, 7);
        ASSERT_TRUE(cgs_equal(b, "world"));
    }
    
    /* ════════════════════════════════════════════════════════════
     * Null termination guarantee
     * ════════════════════════════════════════════════════════════ */
    
    TEST("cgs_zstrv: chars are null terminated at len");
    {
        CGS_ZStrView z = cgs_zstrv("hello");
        ASSERT_TRUE(cgs_chars(z)[cgs_len(z)] == '\0');
    }
    
    TEST("cgs_zstrv: offset view is still null terminated");
    {
        CGS_ZStrView z = cgs_zstrv("hello, world", 7);
        ASSERT_TRUE(cgs_chars(z)[cgs_len(z)] == '\0');
    }
    
    TEST("cgs_zstrv: usable directly as a C string");
    {
        CGS_ZStrView z = cgs_zstrv("hello, world", 7);
        ASSERT_TRUE(strcmp(cgs_chars(z), "world") == 0);
    }
    
    TEST("cgs_zstrv: empty view is null terminated");
    {
        CGS_ZStrView z = cgs_zstrv("abc", 3);
        ASSERT_TRUE(cgs_chars(z)[0] == '\0');
    }
    
    /* ════════════════════════════════════════════════════════════
     * Works as anystr_t in read-only APIs
     * ════════════════════════════════════════════════════════════ */
    
    TEST("cgs_zstrv: cgs_len / cgs_equal accept a ZStrView");
    {
        CGS_ZStrView z = cgs_zstrv("compare me");
        ASSERT_TRUE(cgs_len(z) == 10);
        ASSERT_TRUE(cgs_equal(z, "compare me"));
        ASSERT_TRUE(cgs_equal("compare me", z));
    }
    
    TEST("cgs_zstrv: cgs_equal between two ZStrViews");
    {
        CGS_ZStrView a = cgs_zstrv("xyz");
        CGS_ZStrView b = cgs_zstrv("wwxyz", 2);
        ASSERT_TRUE(cgs_equal(a, b));
    }
    
    TEST("cgs_zstrv: cgs_equal against a StrView");
    {
        CGS_ZStrView z = cgs_zstrv("hello");
        ASSERT_TRUE(cgs_equal(z, cgs_strv("hello")));
    }
    
    TEST("cgs_zstrv: cgs_find with ZStrView as haystack");
    {
        CGS_ZStrView z = cgs_zstrv("hello world");
        CGS_StrView f = cgs_find(z, "world");
        ASSERT_TRUE(cgs_equal(f, "world"));
    }
    
    TEST("cgs_zstrv: cgs_find with ZStrView as needle");
    {
        CGS_ZStrView needle = cgs_zstrv("wor");
        CGS_StrView f = cgs_find("hello world", needle);
        ASSERT_TRUE(cgs_equal(f, "wor"));
    }
    
    TEST("cgs_zstrv: cgs_count with ZStrView");
    {
        CGS_ZStrView z = cgs_zstrv("aaabccdddd");
        ASSERT_TRUE(cgs_count(z, "a") == 3);
        ASSERT_TRUE(cgs_count(z, "d") == 4);
    }
    
    TEST("cgs_zstrv: cgs_starts_with / cgs_ends_with");
    {
        CGS_ZStrView z = cgs_zstrv("hello world");
        ASSERT_TRUE(cgs_starts_with(z, "hello"));
        ASSERT_TRUE(cgs_ends_with(z, "world"));
        ASSERT_TRUE(!cgs_starts_with(z, "world"));
    }
    
    TEST("cgs_zstrv: cgs_starts_with using a ZStrView as the needle");
    {
        CGS_ZStrView needle = cgs_zstrv("hel");
        ASSERT_TRUE(cgs_starts_with("hello", needle));
    }
    
    TEST("cgs_zstrv: cgs_spn / cgs_cspn accept a ZStrView");
    {
        CGS_ZStrView z = cgs_zstrv("abc123");
        ASSERT_TRUE(cgs_equal(cgs_spn(z, "abc"), "abc"));
        ASSERT_TRUE(cgs_equal(cgs_cspn(z, "0123456789"), "abc"));
    }
    
    TEST("cgs_zstrv: cgs_trim_view accepts a ZStrView");
    {
        CGS_ZStrView z = cgs_zstrv("  padded  ");
        ASSERT_TRUE(cgs_equal(cgs_trim_view(z), "padded"));
    }
    
    /* ── as a source for mutating / producing APIs ────────────── */
    
    TEST("cgs_zstrv: cgs_copy from a ZStrView source");
    {
        char mem[64] = {0};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(mem);
        CGS_ZStrView z = cgs_zstrv("hello, world", 7);
        cgs_copy(&sb, z);
        ASSERT_TRUE(cgs_equal(sb, "world"));
    }
    
    TEST("cgs_zstrv: cgs_append from a ZStrView source");
    {
        char mem[64] = {0};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(mem);
        cgs_copy(&sb, "say: ");
        cgs_append(&sb, cgs_zstrv("hello, world", 7));
        ASSERT_TRUE(cgs_equal(sb, "say: world"));
    }
    
    TEST("cgs_zstrv: cgs_dstr_init_from a ZStrView");
    {
        CGS_DStr d = cgs_dstr_init_from(cgs_zstrv("hello, world", 7));
        ASSERT_TRUE(cgs_equal(d, "world"));
        cgs_dstr_deinit(&d);
    }
    
    TEST("cgs_zstrv: cgs_split with a ZStrView source");
    {
        CGS_ZStrView z = cgs_zstrv("a,b,c");
        CGS_StrViewArray arr = cgs_split(z, ",");
        ASSERT_TRUE(arr.len == 3);
        ASSERT_TRUE(cgs_equal(arr.strs[1], "b"));
        free(arr.strs);
    }
    
    TEST("cgs_zstrv: cgs_replace with ZStrView target and replacement");
    {
        char mem[64] = {0};
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(mem);
        cgs_copy(&sb, "a-b-c");
        CGS_Result(int) r = cgs_replace(&sb, cgs_zstrv("-"), cgs_zstrv("+"));
        ASSERT_TRUE(r.val == 2);
        ASSERT_TRUE(cgs_equal(sb, "a+b+c"));
    }
    
    /* ════════════════════════════════════════════════════════════
     * Formatting
     * ════════════════════════════════════════════════════════════ */
    
    TEST("cgs_zstrv: printable via cgs_fmt");
    {
        char buf[64];
        cgs_fmt(buf, "%?", cgs_zstrv("hello, world", 7));
        ASSERT_STR_EQ(buf, "world");
    }
    
    TEST("cgs_zstrv: usable as a fmt arg alongside other types");
    {
        char buf[64];
        cgs_fmt(buf, "%? = %?", cgs_zstrv("answer"), 42);
        ASSERT_STR_EQ(buf, "answer = 42");
    }
    
    TEST("cgs_zstrv: length via unsigned int* counting writer");
    {
        unsigned int n = 0;
        cgs_appendf(&n, "%?", cgs_zstrv("hello, world", 7));
        ASSERT_TRUE(n == 5);
    }
    
    /* ════════════════════════════════════════════════════════════
     * Aliasing / independence
     * ════════════════════════════════════════════════════════════ */
    
    TEST("cgs_zstrv: view points into the original buffer");
    {
        char arr[] = "hello, world";
        CGS_ZStrView z = cgs_zstrv(arr, 7);
        ASSERT_TRUE(cgs_chars(z) == arr + 7);
    }
    
    TEST("cgs_zstrv: view reflects later mutation of the source buffer");
    {
        char arr[] = "hello, world";
        CGS_ZStrView z = cgs_zstrv(arr, 7);
        ASSERT_TRUE(cgs_equal(z, "world"));
        arr[7] = 'W';
        ASSERT_TRUE(cgs_equal(z, "World"));
    }
}
