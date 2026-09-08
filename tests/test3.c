#include "test_common.h"

// ============================================================================
// Edge Case Tests for cgs_tostr and print functions
// ============================================================================

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


static bool cb_toupper(char *c, void *arg) {
    (void)arg;
    if (*c >= 'a' && *c <= 'z') *c -= 32;
    return true;
}

static bool cb_exit_on_bang(char *c, void *arg) {
    int *count = (int*)arg;
    if (*c == '!') return false;
    (*count)++;
    return true;
}

void test_tostr_edge_cases() {
    TEST("cgs_tostr_len: scalar types");;;
    {
        // Integers
        ASSERT_EQ(cgs_tostr_len(0), 1);      // "0"
        ASSERT_EQ(cgs_tostr_len(123), 3);    // "123"
        ASSERT_EQ(cgs_tostr_len(-45), 3);    // "-45"
        
        // Floats (default precision 6)
        // 3.140000 -> 8 chars
        ASSERT_EQ(cgs_tostr_len(3.14), (unsigned int) snprintf(0,0,"%g",3.14)); 
        
        // Booleans
        ASSERT_EQ(cgs_tostr_len(true), 4);   // "true"
    }
    
    TEST("cgs_tostr_p_len: string and pointer types");;;
    {
        int val = 1024;
        ASSERT_EQ(cgs_tostr_p_len(&val), 4); // "1024"
        
        CGS_StrView sv = cgs_strv("hello world");
        ASSERT_EQ(cgs_tostr_p_len(&sv), 11);
        
        CGS_DStr ds = cgs_dstr_init_from("test");
        ASSERT_EQ(cgs_tostr_p_len(&ds), 4);
        cgs_dstr_deinit(&ds);
    }
    
    TEST("cgs_tostr_len: formatters (cgs_nfmt/arrfmt)");;;
    {
        // cgs_nfmt(255, 'X') -> "FF"
        ASSERT_EQ(cgs_tostr_len(cgs_nfmt(255, 'X')), 2);
        
        int arr[] = {1, 2, 3};
        
        ASSERT_EQ(cgs_tostr_len(cgs_arrfmt(arr, 3)), strlen("{1, 2, 3}"));
    }
    
    TEST("cgs_map_chars: successful full iteration");;;
    {
        char buf[] = "hello";
        CGS_Error err = cgs_map_chars(buf, cb_toupper, NULL);
        
        ASSERT_EQ(err.ec, 0); // Success
        ASSERT_TRUE(cgs_equal(buf, "HELLO"));
    }
    
    TEST("cgs_map_chars: early exit with CGS_CALLBACK_EXIT");;;
    {
        char buf[] = "abc!def";
        int processed_count = 0;
        
        CGS_Error err = cgs_map_chars(buf, cb_exit_on_bang, &processed_count);
        
        // Verify early exit error code
        ASSERT_EQ(err.ec, CGS_CALLBACK_EXIT);
        // Verify we stopped at '!'
        ASSERT_EQ(processed_count, 3); 
    }
    
    TEST("cgs_map_chars: edge cases (empty strings)");;;
    {
        char buf[] = "";
        int processed_count = 0;
        
        CGS_Error err = cgs_map_chars(buf, cb_exit_on_bang, &processed_count);
        
        ASSERT_EQ(err.ec, 0);
        ASSERT_EQ(processed_count, 0);
    }
    
    TEST("cgs_map_chars: with CGS_DStr* and pointer types");;;
    {
        CGS_DStr ds = cgs_dstr_init_from("123!456");
        int count = 0;
        
        CGS_Error err = cgs_map_chars(&ds, cb_exit_on_bang, &count);
        
        ASSERT_EQ(err.ec, CGS_CALLBACK_EXIT);
        ASSERT_EQ(count, 3);
        
        cgs_dstr_deinit(&ds);
    }
    
    TEST("cgs_tostr_len: large values");;;
    {
        // 18446744073709551615 is 20 digits
        unsigned long long ull = 18446744073709551615ULL;
        ASSERT_EQ(cgs_tostr_len(ull), 20);
    }
    
    TEST("cgs_tostr: integer boundary values (INT_MAX)");;;
    {
        CGS_DStr dstr = cgs_dstr_init();
        CGS_Error err = cgs_tostr(&dstr, INT_MAX);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_int_max[32]; // Max 2^31-1 is 10 digits, plus sign and null. 32 is safe.
        sprintf(expected_int_max, "%d", INT_MAX);
        ASSERT_TRUE(cgs_equal(&dstr, expected_int_max));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: integer boundary values (INT_MIN)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        CGS_Error err = cgs_tostr(&dstr, INT_MIN);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_int_min[32]; // Max -2^31 is 10 digits, plus sign and null. 32 is safe.
        sprintf(expected_int_min, "%d", INT_MIN);
        ASSERT_TRUE(cgs_equal(&dstr, expected_int_min));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_tostr(&dstr, 0);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "0"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: positive integer");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_tostr(&dstr, 12345);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "12345"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: negative integer");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        CGS_Error err = cgs_tostr(&dstr, -6789);
        ASSERT_EQ(err.ec, CGS_OK);
        ASSERT_TRUE(cgs_equal(&dstr, "-6789"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: long long boundary values (LLONG_MAX)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        CGS_Error err = cgs_tostr(&dstr, LLONG_MAX);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_llong_max[64]; // Long long can be up to 19 digits + sign. 64 is very safe.
        sprintf(expected_llong_max, "%lld", LLONG_MAX);
        ASSERT_TRUE(cgs_equal(&dstr, expected_llong_max));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: unsigned int (UINT_MAX)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        CGS_Error err = cgs_tostr(&dstr, UINT_MAX);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_uint_max[32];
        sprintf(expected_uint_max, "%u", UINT_MAX);
        ASSERT_TRUE(cgs_equal(&dstr, expected_uint_max));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: unsigned long long (ULLONG_MAX)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        CGS_Error err = cgs_tostr(&dstr, ULLONG_MAX);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_ullong_max[64];
        sprintf(expected_ullong_max, "%llu", ULLONG_MAX);
        ASSERT_TRUE(cgs_equal(&dstr, expected_ullong_max));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: float (positive)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        float val = 123.456f;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_float[30];
        sprintf(expected_float, "%.6g", val); // Use appropriate precision for floats
        ASSERT_TRUE(cgs_equal(&dstr, expected_float));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: float (negative)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        float val = -987.65f;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_float[30];
        sprintf(expected_float, "%.6g", val);
        ASSERT_TRUE(cgs_equal(&dstr, expected_float));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: float zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        float val = 0.0f;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_float[30];
        sprintf(expected_float, "%.6g", val);
        ASSERT_TRUE(cgs_equal(&dstr, expected_float));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: double (positive)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        double val = 12345.67890123;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_double[50];
        sprintf(expected_double, "%g", val); // Use appropriate precision for doubles
        ASSERT_TRUE(cgs_equal(&dstr, expected_double));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: double (negative)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        double val = -98765.43210987;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_double[50];
        sprintf(expected_double, "%g", val);
        ASSERT_TRUE(cgs_equal(&dstr, expected_double));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: double zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        double val = 0.0;
        CGS_Error err = cgs_tostr(&dstr, val);
        ASSERT_EQ(err.ec, CGS_OK);
        char expected_double[30];
        sprintf(expected_double, "%.12g", val);
        ASSERT_TRUE(cgs_equal(&dstr, expected_double));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr: buffer too small for integer");;;
    {
        char backing[3]; // Can hold "12" + null terminator
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(backing, 3);
        cgs_clear(&sb);
        CGS_Error err = cgs_tostr(&sb, 123456); // Requires more than 2 characters + null
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_tostr: buffer too small for double");;;
    {
        char backing[5]; // Can hold "0.0" + null. Try to fit something bigger.
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(backing, 5);
        cgs_clear(&sb);
        CGS_Error err = cgs_tostr(&sb, 123.456);
        ASSERT_EQ(err.ec, CGS_DST_TOO_SMALL);
    }
    
    // You could add tests for:
    // - Scientific notation for very large/small floats/doubles if your cgs_tostr supports it.
    // - Special float values: INFINITY, -INFINITY, NaN (if cgs_tostr handles them gracefully).
    
    TEST("cgs_print: multiple arguments (concatenation)");;;
    {
        CGS_StrBuf sb = {0};
        cgs_tostr_many(&sb, 123, " ", "test", " ", cgs_nfmt(456.78f, 'f', 2), "!", -99);
        ASSERT_TRUE(cgs_equal(sb, ""));
    }
    
    TEST("cgs_print: multiple arguments (concatenation)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        cgs_tostr_many(&dstr, 123, " ", "test", " ", cgs_nfmt(456.78f, 'f', 2), "!", -99);
        ASSERT_TRUE(cgs_equal(&dstr, "123 test 456.78!-99"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_print: multiple arguments, buffer too small");;;
    {
        char backing[10];
        CGS_StrBuf sb = cgs_strbuf_init_from_buf(backing, 10);
        cgs_clear(&sb);
        cgs_tostr_many(&sb, 1, " ", "long_string_here");
        ASSERT_TRUE(cgs_equal(sb, "1 long_st"));
    }
    
    TEST("cgs_nfmt: various format characters");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        // Test hex, octal, etc.
        cgs_tostr(&dstr, cgs_nfmt(255, 'x'));
        ASSERT_TRUE(cgs_equal(dstr, "ff"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: integer decimal (signed/unsigned)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        cgs_tostr_many(&dstr, cgs_nfmt(0, 'd'), " ", cgs_nfmt(-123, 'd'), " ", cgs_nfmt(2147483647, 'd'));
        ASSERT_TRUE(cgs_equal(&dstr, "0 -123 2147483647"));
        cgs_dstr_deinit(&dstr);
    }

    TEST("cgs_nfmt: integer hexadecimal (x/X)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(20);
        cgs_tostr(&dstr, cgs_nfmt(255, 'x'));
        ASSERT_TRUE(cgs_equal(&dstr, "ff"));
        
        cgs_clear(&dstr);
        cgs_tostr(&dstr, cgs_nfmt(4095, 'X'));
        ASSERT_TRUE(cgs_equal(&dstr, "FFF"));
        cgs_dstr_deinit(&dstr);
    }

    TEST("cgs_nfmt: integer octal and binary");;;
    {
        CGS_DStr dstr = cgs_dstr_init(20);
        cgs_tostr_many(&dstr, cgs_nfmt(8, 'o'), " ", cgs_nfmt(5, 'b'));
        // 8 in octal is 10, 5 in binary is 101
        ASSERT_TRUE(cgs_equal(&dstr, "10 101"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: floating point fixed-point (f/F)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        // Test precision and rounding
        cgs_tostr_many(&dstr, cgs_nfmt(3.14159f, 'f', 2), " ", cgs_nfmt(-0.555f, 'f', 1));
        ASSERT_TRUE(cgs_equal(&dstr, "3.14 -0.6"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: scientific notation (e/E)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        cgs_tostr_many(&dstr, cgs_nfmt(1000.0, 'e', 2));
        ASSERT_TRUE(cgs_equal(&dstr, "1.00e+03"));
        
        cgs_clear(&dstr);
        cgs_tostr_many(&dstr, cgs_nfmt(0.001, 'E', 1));
        ASSERT_TRUE(cgs_equal(&dstr, "1.0E-03"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: general format (g/G)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        // 'g' chooses between fixed and scientific based on magnitude
        cgs_tostr_many(&dstr, cgs_nfmt(123.456, 'g', 4));
        ASSERT_TRUE(cgs_equal(&dstr, "123.5")); // 4 significant digits
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: hex-float (a/A)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(30);
        cgs_tostr(&dstr, cgs_nfmt(1.0, 'a'));
        // Usually 0x1.0p+0 or similar depending on implementation
        ASSERT_TRUE(cgs_equal(&dstr, "0x1p+0")); 
        
        cgs_clear(&dstr);
        cgs_tostr(&dstr, cgs_nfmt(15.5, 'A'));
        ASSERT_TRUE(cgs_equal(&dstr, "0X1.FP+3"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: integer limits and zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        long long max_val = 9223372036854775807LL;
        cgs_tostr_many(&dstr, cgs_nfmt(0, 'x'), " ", cgs_nfmt(max_val, 'd'));
        ASSERT_TRUE(cgs_equal(&dstr, "0 9223372036854775807"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: high precision and rounding");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        // Test very small float with high precision
        double val = 0.0000123456;
        cgs_tostr_many(&dstr, cgs_nfmt(val, 'f', 8));
        ASSERT_TRUE(cgs_equal(&dstr, "0.00001235"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_tostr_many: complex mixed types");;;
    {
        CGS_DStr dstr = cgs_dstr_init(100);
        int hex_val = 0xDEAD;
        float pi = 3.14159f;
        cgs_tostr_many(&dstr, "Hex: ", cgs_nfmt(hex_val, 'X'), ", Pi: ", cgs_nfmt(pi, 'g', 3), ", Bin: ", cgs_nfmt(7, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "Hex: DEAD, Pi: 3.14, Bin: 111"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt (binary): basic and zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(20);
        // Zero is a common edge case
        cgs_tostr(&dstr, cgs_nfmt(0, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "0"));
        
        cgs_clear(&dstr);
        // Small powers of 2
        cgs_tostr_many(&dstr, cgs_nfmt(1, 'b'), ",", cgs_nfmt(2, 'b'), ",", cgs_nfmt(4, 'b'), ",", cgs_nfmt(8, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "1,10,100,1000"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt (octal): basic and zero");;;
    {
        CGS_DStr dstr = cgs_dstr_init(20);
        cgs_tostr(&dstr, cgs_nfmt(0, 'o'));
        ASSERT_TRUE(cgs_equal(&dstr, "0"));
        
        cgs_clear(&dstr);
        // Standard octal transitions
        cgs_tostr_many(&dstr, cgs_nfmt(7, 'o'), ",", cgs_nfmt(8, 'o'), ",", cgs_nfmt(16, 'o'));
        ASSERT_TRUE(cgs_equal(&dstr, "7,10,20"));
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: unsigned integer limits (binary)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(128);
        
        // uint8_t max (255)
        unsigned char u8_max = 255;
        cgs_tostr(&dstr, cgs_nfmt(u8_max, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "11111111"));
        
        cgs_clear(&dstr);
        // uint64_t max (18,446,744,073,709,551,615)
        unsigned long long u64_max = 0xFFFFFFFFFFFFFFFFULL;
        cgs_tostr(&dstr, cgs_nfmt(u64_max, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "1111111111111111111111111111111111111111111111111111111111111111"));
        
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: unsigned integer limits (octal)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(64);
        
        // uint16_t max (65535)
        unsigned short u16_max = 65535;
        cgs_tostr(&dstr, cgs_nfmt(u16_max, 'o'));
        ASSERT_TRUE(cgs_equal(&dstr, "177777"));
        
        cgs_clear(&dstr);
        // uint32_t max (4294967295)
        unsigned int u32_max = 4294967295U;
        cgs_tostr(&dstr, cgs_nfmt(u32_max, 'o'));
        ASSERT_TRUE(cgs_equal(&dstr, "37777777777"));
        
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: signed negative integers (binary/octal)");;;
    {
        // Note: Standard C behavior for %o and %x is to treat the bits as unsigned.
        // These tests assume your library follows that convention.
        CGS_DStr dstr = cgs_dstr_init(128);
        
        // int8_t -1 (Two's complement: 11111111)
        signed char s8_neg = -1;
        cgs_tostr(&dstr, cgs_nfmt(s8_neg, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "11111111"));
        
        cgs_clear(&dstr);
        // int16_t -1 in octal (Two's complement: 177777)
        short s16_neg = -1;
        cgs_tostr(&dstr, cgs_nfmt(s16_neg, 'o'));
        ASSERT_TRUE(cgs_equal(&dstr, "177777"));
        
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: signed integer limits (decimal vs others)");;;
    {
        CGS_DStr dstr = cgs_dstr_init(128);
        
        // 64-bit Signed Minimum: -9,223,372,036,854,775,808
        long long s64_min = -9223372036854775807LL - 1LL;
        
        // Decimal should show the minus sign
        cgs_tostr(&dstr, cgs_nfmt(s64_min, 'd'));
        ASSERT_TRUE(cgs_equal(&dstr, "-9223372036854775808"));
        
        cgs_clear(&dstr);
        // Binary for signed min (usually 1 followed by 63 zeros)
        cgs_tostr(&dstr, cgs_nfmt(s64_min, 'b'));
        ASSERT_TRUE(cgs_equal(&dstr, "1000000000000000000000000000000000000000000000000000000000000000"));
        
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("cgs_nfmt: type genericity check");;;
    {
        CGS_DStr dstr = cgs_dstr_init(100);
        
        unsigned char  v1 = 10;
        unsigned short v2 = 20;
        unsigned int   v3 = 30;
        
        // Testing that the macro/generic correctly handles different sizes in one call
        cgs_tostr_many(&dstr, cgs_nfmt(v1, 'b'), " ", cgs_nfmt(v2, 'o'), " ", cgs_nfmt(v3, 'x'));
        // 10=1010(b), 20=24(o), 30=1e(x)
        ASSERT_TRUE(cgs_equal(&dstr, "1010 24 1e"));
        
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Stress Tests
// ============================================================================

void test_stress_cases() {
    TEST("cgs_replace: fixed buffer overflow check");;;
    {
        char buf[10] = "apple"; // capacity 10
        // Result would be "appleapple", length 10 + 1 for null = 11.
        // This should return CGS_DST_TOO_SMALL
        CGS_Result(int) res = cgs_replace(buf, "apple", "appleapple");
        ASSERT_TRUE(res.err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_replace_range: invalid indices");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        // Start > End
        CGS_Error err1 = cgs_replace_range(&s, 2, 1, "x");
        ASSERT_TRUE(err1.ec == CGS_BAD_RANGE);
        
        // Start out of bounds
        CGS_Error err2 = cgs_replace_range(&s, 5, 6, "x");
        ASSERT_TRUE(err2.ec == CGS_INDEX_OUT_OF_BOUNDS);
        
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: fixed buffer overflow check");;;
    {
        char buf[10] = "apple"; // capacity 10
        // Result would be "appleapple", length 10 + 1 for null = 11.
        // This should return CGS_DST_TOO_SMALL
        CGS_Result(int) res = cgs_replace(buf, "apple", "appleapple");
        ASSERT_TRUE(res.err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_replace_range: invalid indices");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        // Start > End
        CGS_Error err1 = cgs_replace_range(&s, 2, 1, "x");
        ASSERT_TRUE(err1.ec == CGS_BAD_RANGE);
        
        // Start out of bounds
        CGS_Error err2 = cgs_replace_range(&s, 5, 6, "x");
        ASSERT_TRUE(err2.ec == CGS_INDEX_OUT_OF_BOUNDS);
        
        cgs_dstr_deinit(&s);
    }
    
    TEST("stress: many allocations and deallocations");;;
    {
        for (int i = 0; i < 100; i++) {
            CGS_DStr dstr = cgs_dstr_init(i * 10);
            for (int j = 0; j < i; j++) {
                cgs_append(&dstr, "x");
            }
            cgs_dstr_deinit(&dstr);
        }
        ASSERT_TRUE(1);
    }
    
    TEST("stress: alternating growth and shrinkage");;;
    {
        CGS_DStr dstr = cgs_dstr_init(10);
        for (int i = 0; i < 50; i++) {
            for (int j = 0; j < 100; j++) {
                cgs_putc(&dstr, 'x');
            }
            char x[101];
            memset(x, 'x', sizeof(x));
            x[sizeof(x) - 1] = 0;
            assert(cgs_equal(dstr, x));
            cgs_clear(&dstr);
        }
        cgs_dstr_deinit(&dstr);
        ASSERT_TRUE(1);
    }
    
    TEST("stress: very deep nesting of operations");;;
    {
        CGS_DStr dstr = cgs_dstr_init(1000);
        cgs_append(&dstr, "start");
        for (int i = 0; i < 100; i++) {
            cgs_replace(&dstr, "start", "middle");
            cgs_replace(&dstr, "middle", "end");
            cgs_replace(&dstr, "end", "start");
        }
        ASSERT_TRUE(cgs_equal(&dstr, "start"));
        cgs_dstr_deinit(&dstr);
    }
}

// ============================================================================
// Special Character Tests
// ============================================================================

void test_special_characters() {
    TEST("special chars: all printable ASCII");;;
    {
        CGS_DStr dstr = cgs_dstr_init(200);
        for (char c = 32; c < 127; c++) {
            cgs_putc(&dstr, c);
        }
        ASSERT_EQ(cgs_len(&dstr), 127 - 32);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("special chars: control characters");;;
    {
        CGS_DStr dstr = cgs_dstr_init(50);
        cgs_putc(&dstr, '\t');
        cgs_putc(&dstr, '\n');
        cgs_putc(&dstr, '\r');
        ASSERT_EQ(cgs_len(&dstr), 3);
        cgs_dstr_deinit(&dstr);
    }
    
    TEST("special chars: high ASCII");;;
    {
        CGS_DStr dstr = cgs_dstr_init(200);
        for (unsigned char c = 128; c < 255; c++) {
            cgs_putc(&dstr, c);
        }
        cgs_dstr_deinit(&dstr);
        ASSERT_TRUE(1);
    }
}

void test_replace_all()
{
    TEST("cgs_replace: standard multiple replacement");;;
    {
        CGS_DStr s = cgs_dstr_init_from("banana");
        CGS_Result(int) res = cgs_replace(&s, "a", "o");
        // Result: "bonono", count: 3
        ASSERT_TRUE(res.val == 3 && cgs_equal(&s, "bonono"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: empty target (Injection behavior)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        // Standard behavior (Python/JS): Replace "" with "-"
        // This usually results in "-a-b-c-"
        CGS_Result(int) res = cgs_replace(&s, "", "-");
        ASSERT_TRUE(cgs_equal(&s, "-a-b-c-"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: replacement contains target (No infinite loop)");;;
    {
        CGS_DStr s = cgs_dstr_init_from("a");
        // Replacing "a" with "aa" should only happen once per original match
        CGS_Result(int) res = cgs_replace(&s, "a", "aa");
        ASSERT_TRUE(res.val == 1 && cgs_equal(&s, "aa"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: non-overlapping counts");;;
    {
        CGS_DStr s = cgs_dstr_init_from("aaaaa"); 
        CGS_Result(int) res = cgs_replace(&s, "aa", "b");
        // Result must be "bba", count must be 2
        ASSERT_TRUE(res.val == 2);
        ASSERT_TRUE(cgs_equal(&s, "bba"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: target longer than source");;;
    {
        CGS_DStr s = cgs_dstr_init_from("abc");
        CGS_Result(int) res = cgs_replace(&s, "abcd", "x");
        ASSERT_TRUE(res.val == 0 && cgs_equal(&s, "abc"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: multiple replacements with different lengths");;;
    {
        CGS_DStr s = cgs_dstr_init_from("1-2-3");
        // Growing the string: "1" -> "one"
        CGS_Result(int) res = cgs_replace(&s, "-", "---");
        ASSERT_TRUE(cgs_equal(&s, "1---2---3"));
        cgs_dstr_deinit(&s);
    }
    
    TEST("cgs_replace: fixed buffer too small");;;
    {
        char buf[5] = "abc"; // Small fixed buffer
        CGS_Result(int) res = cgs_replace(buf, "b", "verylongstring");
        ASSERT_TRUE(res.err.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_replace_first: fixed buffer too small");;;
    {
        char buf[5] = "abc"; // Small fixed buffer
        CGS_Error res = cgs_replace_first(buf, "b", "verylongstring");
        ASSERT_TRUE(res.ec == CGS_DST_TOO_SMALL);
    }
    
    TEST("cgs_replace: target not found");;;
    {
        CGS_DStr s = cgs_dstr_init_from("hello");
        CGS_Result(int) res = cgs_replace(&s, "world", "earth");
        ASSERT_TRUE(res.val == 0 && res.err.ec == CGS_NOT_FOUND);
        cgs_dstr_deinit(&s);
    }
}

void test_alignfmt(void)
{
    char buf[64];

    /* ── LEFT: content shorter than width ─────────────────────── */

    TEST("cgs_alignfmt: left-align 'hello' width=10 space fill");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 10, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello     "));
    }

    TEST("cgs_alignfmt: left-align 'hello' width=10 dash fill");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 10, '-'));
        ASSERT_TRUE(cgs_equal (buf, "hello-----"));
    }

    TEST("cgs_alignfmt: left-align default fill (space)");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 8));
        ASSERT_TRUE(cgs_equal (buf, "hello   "));
    }

    /* ── RIGHT: content shorter than width ────────────────────── */

    TEST("cgs_alignfmt: right-align 'hello' width=10 space fill");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 10, ' '));
        ASSERT_TRUE(cgs_equal (buf, "     hello"));
    }

    TEST("cgs_alignfmt: right-align 'hello' width=10 dash fill");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 10, '-'));
        ASSERT_TRUE(cgs_equal (buf, "-----hello"));
    }

    TEST("cgs_alignfmt: right-align default fill (space)");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 8));
        ASSERT_TRUE(cgs_equal (buf, "   hello"));
    }

    /* ── CENTER: even padding ─────────────────────────────────── */

    TEST("cgs_alignfmt: center 'hi' width=6 even padding → 2 left 2 right");
    {
        char *s = "hi";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 6, ' '));
        ASSERT_TRUE(cgs_equal (buf, "  hi  "));
    }

    TEST("cgs_alignfmt: center 'hi' width=6 star fill even padding");
    {
        char *s = "hi";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 6, '*'));
        ASSERT_TRUE(cgs_equal (buf, "**hi**"));
    }

    TEST("cgs_alignfmt: center 'hello' width=9 even padding → 2 left 2 right");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 9, ' '));
        ASSERT_TRUE(cgs_equal (buf, "  hello  "));
    }

    /* ── CENTER: odd padding (extra goes RIGHT) ───────────────── */

    TEST("cgs_alignfmt: center 'hi' width=7 odd padding → 2 left 3 right");
    {
        char *s = "hi";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 7, ' '));
        ASSERT_TRUE(cgs_equal (buf, "  hi   "));
    }

    TEST("cgs_alignfmt: center 'hi' width=7 star fill odd → extra right");
    {
        char *s = "hi";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 7, '*'));
        ASSERT_TRUE(cgs_equal (buf, "**hi***"));
    }

    TEST("cgs_alignfmt: center 'abc' width=8 odd padding → 2 left 3 right");
    {
        char *s = "abc";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 8, ' '));
        ASSERT_TRUE(cgs_equal (buf, "  abc   "));
    }

    TEST("cgs_alignfmt: center 'x' width=4 odd padding → 1 left 2 right");
    {
        /* 4-1=3 padding, 3/2=1 left, 2 right */
        char *s = "x";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 4, '.'));
        ASSERT_TRUE(cgs_equal (buf, ".x.."));
    }

    TEST("cgs_alignfmt: center 'X' width=20 odd padding → 9 left 10 right");
    {
        /* 20-1=19 padding, 19/2=9 left, 10 right */
        char *s = "X";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 20, ' '));
        ASSERT_TRUE(strlen(buf) == 20);
        ASSERT_TRUE(buf[9]  == 'X');
        ASSERT_TRUE(buf[0]  == ' ');
        ASSERT_TRUE(buf[19] == ' ');
    }

    /* ── Width == content length (no padding added) ───────────── */

    TEST("cgs_alignfmt: left-align exact width → no padding");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 5, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: right-align exact width → no padding");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 5, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: center exact width → no padding");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 5, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: center 'x' width=1 exact → no padding");
    {
        char *s = "x";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 1, ' '));
        ASSERT_TRUE(cgs_equal (buf, "x"));
    }

    /* ── Width < content length (no truncation) ──────────────── */

    TEST("cgs_alignfmt: left-align width < length → full content, no truncation");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 3, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: right-align width < length → full content, no truncation");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 3, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: center width < length → full content, no truncation");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 3, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    /* ── Width == 0 ───────────────────────────────────────────── */

    TEST("cgs_alignfmt: left-align width=0 → content unchanged");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 0, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: right-align width=0 → content unchanged");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 0, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    TEST("cgs_alignfmt: center width=0 → content unchanged");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 0, ' '));
        ASSERT_TRUE(cgs_equal (buf, "hello"));
    }

    /* ── Empty string ─────────────────────────────────────────── */

    TEST("cgs_alignfmt: left-align empty string width=4 → all fill");
    {
        char *s = "";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 4, '_'));
        ASSERT_TRUE(cgs_equal (buf, "____"));
    }

    TEST("cgs_alignfmt: right-align empty string width=4 → all fill");
    {
        char *s = "";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 4, '_'));
        ASSERT_TRUE(cgs_equal (buf, "____"));
    }

    TEST("cgs_alignfmt: center empty string width=4 → all fill");
    {
        /* 4-0=4 padding, even → 2 left 2 right */
        char *s = "";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 4, '_'));
        ASSERT_TRUE(cgs_equal (buf, "____"));
    }

    TEST("cgs_alignfmt: center empty string width=5 → all fill odd → extra right");
    {
        /* 5 padding, odd → 2 left 3 right */
        char *s = "";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, CENTER, 5, '_'));
        ASSERT_TRUE(cgs_equal (buf, "_____"));
    }

    /* ── Non-string types ─────────────────────────────────────── */

    TEST("cgs_alignfmt: right-align int 42 width=6 zero fill → '000042'");
    {
        int n = 42;
        cgs_fmt(buf, "%?", cgs_alignfmt(n, RIGHT, 6, '0'));
        ASSERT_TRUE(cgs_equal (buf, "000042"));
    }

    TEST("cgs_alignfmt: left-align int 42 width=6 zero fill → '420000'");
    {
        int n = 42;
        cgs_fmt(buf, "%?", cgs_alignfmt(n, LEFT, 6, '0'));
        ASSERT_TRUE(cgs_equal (buf, "420000"));
    }

    TEST("cgs_alignfmt: center int 42 width=6 zero fill → '004200'");
    {
        /* "42" len=2, 6-2=4 padding, even → 2 left 2 right */
        int n = 42;
        cgs_fmt(buf, "%?", cgs_alignfmt(n, CENTER, 6, '0'));
        ASSERT_TRUE(cgs_equal (buf, "004200"));
    }

    TEST("cgs_alignfmt: right-align int 42 width=2 exact → '42'");
    {
        int n = 42;
        cgs_fmt(buf, "%?", cgs_alignfmt(n, RIGHT, 2, '0'));
        ASSERT_TRUE(cgs_equal (buf, "42"));
    }

    TEST("cgs_alignfmt: right-align int 42 width=1 smaller → '42' no truncation");
    {
        int n = 42;
        cgs_fmt(buf, "%?", cgs_alignfmt(n, RIGHT, 1, '0'));
        ASSERT_TRUE(cgs_equal (buf, "42"));
    }

    /* ── Large width ──────────────────────────────────────────── */

    TEST("cgs_alignfmt: left-align 'X' width=20 → 'X' followed by 19 spaces");
    {
        char *s = "X";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, LEFT, 20, ' '));
        ASSERT_TRUE(strlen(buf) == 20);
        ASSERT_TRUE(buf[0] == 'X');
        ASSERT_TRUE(buf[1] == ' ');
        ASSERT_TRUE(buf[19] == ' ');
    }

    TEST("cgs_alignfmt: right-align 'X' width=20 → 19 spaces then 'X'");
    {
        char *s = "X";
        cgs_fmt(buf, "%?", cgs_alignfmt(s, RIGHT, 20, ' '));
        ASSERT_TRUE(strlen(buf) == 20);
        ASSERT_TRUE(buf[0]  == ' ');
        ASSERT_TRUE(buf[19] == 'X');
    }

    /* ── Composition: alignfmt inside larger fmt string ──────── */

    TEST("cgs_alignfmt: embedded in fmt string with brackets");
    {
        char *s = "ok";
        cgs_fmt(buf, "[%?]", cgs_alignfmt(s, CENTER, 6, '-'));
        ASSERT_TRUE(cgs_equal (buf, "[--ok--]"));
    }

    TEST("cgs_alignfmt: two alignfmts as separate args in one fmt call");
    {
        char *s = "hello";
        cgs_fmt(buf, "%? | %?",
                cgs_alignfmt(s, LEFT, 8,  '.'),
                cgs_alignfmt(s, RIGHT, 8, '.'));
        ASSERT_TRUE(cgs_equal (buf, "hello... | ...hello"));
    }

    /* ── arrfmt inside alignfmt ───────────────────────────────── */

    TEST("cgs_alignfmt: left-align arrfmt result");
    {
        /* "{1, 2, 3}" is 9 chars, width=14 → 5 spaces right */
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, LEFT, 14, ' '));
        ASSERT_TRUE(cgs_equal (buf, "{1, 2, 3}     "));
    }

    TEST("cgs_alignfmt: right-align arrfmt result");
    {
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, RIGHT, 14, ' '));
        ASSERT_TRUE(cgs_equal (buf, "     {1, 2, 3}"));
    }

    TEST("cgs_alignfmt: center arrfmt result even padding");
    {
        /* "{1, 2, 3}" is 9 chars, width=13 → 4 padding even → 2 left 2 right */
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, CENTER, 13, ' '));
        ASSERT_TRUE(cgs_equal (buf, "  {1, 2, 3}  "));
    }

    TEST("cgs_alignfmt: center arrfmt result odd padding → extra right");
    {
        /* "{1, 2, 3}" is 9 chars, width=14 → 5 padding odd → 2 left 3 right */
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, CENTER, 14, '-'));
        ASSERT_TRUE(cgs_equal (buf, "--{1, 2, 3}---"));
    }

    TEST("cgs_alignfmt: right-align arrfmt with custom open/close/delim");
    {
        /* "[1|2|3]" is 7 chars, width=11 → 4 spaces left */
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3, "[", "]", "|", "");
        cgs_fmt(buf, "%?", cgs_alignfmt(af, RIGHT, 11, ' '));
        ASSERT_TRUE(cgs_equal (buf, "    [1|2|3]"));
    }

    TEST("cgs_alignfmt: center arrfmt exact width → no padding");
    {
        /* "{1, 2, 3}" is 9 chars, width=9 → no padding */
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, CENTER, 9, '*'));
        ASSERT_TRUE(cgs_equal (buf, "{1, 2, 3}"));
    }

    TEST("cgs_alignfmt: left-align arrfmt width < content → no truncation");
    {
        int arr[] = {1, 2, 3};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(af, LEFT, 3, ' '));
        ASSERT_TRUE(cgs_equal (buf, "{1, 2, 3}"));
    }

    /* ── alignfmt inside arrfmt ───────────────────────────────── */

    TEST("cgs_arrfmt: array of aligned ints via alignfmt");
    {
        /* each int aligned right in width 4 with '0':
         * 1 → "0001", 20 → "0020", 300 → "0300"
         * arrfmt default: "{0001, 0020, 0300}" */
        /* arrfmt takes T* and size_t, elements must have tostr.
         * CGS__AlignFmt has tostr, so make an array of them. */
        int n1 = 1, n2 = 20, n3 = 300;
        CGS__AlignFmt elems[3] = {
            cgs_alignfmt(n1, RIGHT, 4, '0'),
            cgs_alignfmt(n2, RIGHT, 4, '0'),
            cgs_alignfmt(n3, RIGHT, 4, '0'),
        };
        CGS_ArrayFmt af = cgs_arrfmt(elems, 3);
        cgs_fmt(buf, "%?", af);
        ASSERT_TRUE(cgs_equal (buf, "{0001, 0020, 0300}"));
    }

    TEST("cgs_arrfmt: array of center-aligned strings");
    {
        char *a = "a", *bb = "bb", *ccc = "ccc";
        CGS__AlignFmt elems[3] = {
            cgs_alignfmt(a,   CENTER, 5, '-'),
            cgs_alignfmt(bb,  CENTER, 5, '-'),
            cgs_alignfmt(ccc, CENTER, 5, '-'),
        };
        /* "a"   width=5 → 2 left 2 right → "--a--"
         * "bb"  width=5 → 1 left 2 right → "-bb--" (odd extra right)
         * "ccc" width=5 → 1 left 1 right → "-ccc-" */
        CGS_ArrayFmt af = cgs_arrfmt(elems, 3, "[", "]", ", ", "");
        cgs_fmt(buf, "%?", af);
        ASSERT_TRUE(cgs_equal (buf, "[--a--, -bb--, -ccc-]"));
    }

    /* ── both nested in cgs_fmt with other args ───────────────── */

    TEST("cgs_fmt: alignfmt and arrfmt as separate args in one call");
    {
        int arr[] = {7, 8, 9};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 3);
        char *label = "data";
        cgs_fmt(buf, "%?: %?",
                cgs_alignfmt(label, LEFT, 8, '.'),
                af);
        ASSERT_TRUE(cgs_equal (buf, "data....: {7, 8, 9}"));
    }

    TEST("cgs_fmt: aligned arrfmt next to aligned scalar");
    {
        int arr[] = {1, 2};
        CGS_ArrayFmt af  = cgs_arrfmt(arr, 2);
        int          n   = 42;
        cgs_fmt(buf, "[%?][%?]",
                cgs_alignfmt(af, RIGHT, 10, ' '),
                cgs_alignfmt(n,   LEFT, 6,  '0'));
        /* "{1, 2}" is 6 chars, right in 10 → "    {1, 2}"
         * "42"    is 2 chars, left  in  6 → "420000" */
        ASSERT_TRUE(cgs_equal (buf, "[    {1, 2}][420000]"));
    }

    TEST("cgs_fmt: arrfmt of alignfmts embedded inside another alignfmt");
    {
        /* inner: array of right-aligned ints → "{  1,  2,  3}"
         * outer: that whole string centered in width 17            */
        int n1 = 1, n2 = 2, n3 = 3;
        CGS__AlignFmt inner_elems[3] = {
            cgs_alignfmt(n1, RIGHT, 2, ' '),
            cgs_alignfmt(n2, RIGHT, 2, ' '),
            cgs_alignfmt(n3, RIGHT, 2, ' '),
        };
        CGS_ArrayFmt inner_af = cgs_arrfmt(inner_elems, 3);
        /* inner_af tostr → "{ 1,  2,  3}" (13 chars) */
        cgs_fmt(buf, "%?", cgs_alignfmt(inner_af, CENTER, 17, '='));
        /* 17-13=4 padding even → 2 left 2 right */
        ASSERT_TRUE(cgs_equal (buf, "=={ 1,  2,  3}==="));
    }

    TEST("cgs_fmt: three-level nesting alignfmt(arrfmt(alignfmt))");
    {
        /* level 1: right-align each int in width 3 with '0'
         *   1 → "001", 22 → "022", 333 → "333"
         * level 2: arrfmt with custom brackets → "<001|022|333>"  (13 chars)
         * level 3: left-align that in width 16 with '.' */
        int n1 = 1, n2 = 22, n3 = 333;
        CGS__AlignFmt l1[3] = {
            cgs_alignfmt(n1, RIGHT, 3, '0'),
            cgs_alignfmt(n2, RIGHT, 3, '0'),
            cgs_alignfmt(n3, RIGHT, 3, '0'),
        };
        CGS_ArrayFmt l2 = cgs_arrfmt(l1, 3, "<", ">", "|", "");
        cgs_fmt(buf, "%?", cgs_alignfmt(l2, LEFT, 16, '.'));
        ASSERT_TRUE(cgs_equal (buf, "<001|022|333>..."));
    }
}

void test_writer_counter(void)
{
    /* unsigned int* as writer_t — counts chars written, discards content */
    
    /* ── cgs_append ───────────────────────────────────────────── */
    
    TEST("writer unsigned int*: cgs_append counts chars of a plain string");
    {
        unsigned int n = 0;
        cgs_append(&n, "hello");
        ASSERT_TRUE(n == 5);
    }
    
    TEST("writer unsigned int*: cgs_append accumulates across multiple calls");
    {
        unsigned int n = 0;
        cgs_append(&n, "hello");
        cgs_append(&n, ", ");
        cgs_append(&n, "world");
        ASSERT_TRUE(n == 12);
    }
    
    TEST("writer unsigned int*: cgs_append of empty string adds 0");
    {
        unsigned int n = 0;
        cgs_append(&n, "hello");
        cgs_append(&n, "");
        ASSERT_TRUE(n == 5);
    }
    
    TEST("writer unsigned int*: cgs_append of StrView counts only the view length");
    {
        /* StrView of "hello" [1,4) → "ell" (3 chars) */
        unsigned int n = 0;
        cgs_append(&n, cgs_strv("hello", 1, 4));
        ASSERT_TRUE(n == 3);
    }
    
    TEST("writer unsigned int*: cgs_append starts from existing count");
    {
        unsigned int n = 10;
        cgs_append(&n, "hello");
        ASSERT_TRUE(n == 15);
    }
    
    /* ── cgs_putc ─────────────────────────────────────────────── */
    
    TEST("writer unsigned int*: cgs_putc counts 1");
    {
        unsigned int n = 0;
        cgs_putc(&n, 'x');
        ASSERT_TRUE(n == 1);
    }
    
    TEST("writer unsigned int*: cgs_putc accumulates");
    {
        unsigned int n = 0;
        cgs_putc(&n, 'a');
        cgs_putc(&n, 'b');
        cgs_putc(&n, 'c');
        ASSERT_TRUE(n == 3);
    }
    
    /* ── cgs_appendf ───────────────────────────────────────── */
    
    TEST("writer unsigned int*: cgs_appendf counts formatted output length");
    {
        unsigned int n = 0;
        cgs_appendf(&n, "%? + %? = %?", 1, 2, 3);
        /* "1 + 2 = 3" → 9 chars */
        ASSERT_TRUE(n == 9);
    }
    
    TEST("writer unsigned int*: cgs_appendf accumulates across calls");
    {
        unsigned int n = 0;
        cgs_appendf(&n, "%?", "hello");
        cgs_appendf(&n, "%?", "world");
        ASSERT_TRUE(n == 10);
    }
    
    TEST("writer unsigned int*: cgs_appendf with nfmt counts correctly");
    {
        /* cgs_nfmt(255, 'X') → "FF" (2 chars) */
        unsigned int n = 0;
        cgs_appendf(&n, "%?", cgs_nfmt(255, 'X'));
        ASSERT_TRUE(n == 2);
    }
    
    TEST("writer unsigned int*: cgs_appendf with arrfmt counts correctly");
    {
        /* "{1, 2, 3}" → 9 chars */
        int arr[] = {1, 2, 3};
        unsigned int n = 0;
        cgs_appendf(&n, "%?", cgs_arrfmt(arr, 3));
        ASSERT_TRUE(n == 9);
    }
    
    TEST("writer unsigned int*: cgs_appendf with alignfmt counts padded length");
    {
        /* "hello" left-aligned in width 10 → "hello     " → 10 chars */
        char *s = "hello";
        unsigned int n = 0;
        cgs_appendf(&n, "%?", cgs_alignfmt(s, LEFT, 10, ' '));
        ASSERT_TRUE(n == 10);
    }
    
    /* ── cgs_append_tostr / cgs_append_tostr_many ─────────────── */
    
    TEST("writer unsigned int*: cgs_append_tostr counts int tostr length");
    {
        unsigned int n = 0;
        cgs_append_tostr(&n, 12345);
        /* "12345" → 5 chars */
        ASSERT_TRUE(n == 5);
    }
    
    TEST("writer unsigned int*: cgs_append_tostr counts negative int tostr length");
    {
        unsigned int n = 0;
        cgs_append_tostr(&n, -99);
        /* "-99" → 3 chars */
        ASSERT_TRUE(n == 3);
    }
    
    TEST("writer unsigned int*: cgs_append_tostr_many sums all args");
    {
        /* "hello" + ", " + "42" → 5+2+2 = 9 chars */
        unsigned int n = 0;
        cgs_append_tostr_many(&n, "hello", ", ", 42);
        ASSERT_TRUE(n == 9);
    }
    
    TEST("writer unsigned int*: cgs_append_tostr_many with nfmt");
    {
        /* "FF" + " " + "255" → 2+1+3 = 6 chars */
        unsigned int n = 0;
        cgs_append_tostr_many(&n, cgs_nfmt(255, 'X'), " ", 255);
        ASSERT_TRUE(n == 6);
    }
    
    /* ── use as a dry-run before allocating ───────────────────── */
    
    TEST("writer unsigned int*: dry-run matches actual written length");
    {
        /* count first, then write to a real buffer, check lengths match */
        unsigned int n = 0;
        cgs_appendf(&n, "%? = %?", "answer", 42);
        
        char buf[64];
        cgs_fmt(buf, "%? = %?", "answer", 42);
        
        ASSERT_TRUE(n == cgs_len(buf));
    }
    
    TEST("writer unsigned int*: dry-run of complex nested fmt");
    {
        int arr[] = {10, 200, 3000};
        char *label = "vals";
        
        unsigned int n = 0;
        cgs_appendf(&n, "%?: %?", cgs_alignfmt(label, LEFT, 8, '.'), cgs_arrfmt(arr, 3));
        
        char buf[64];
        cgs_fmt(buf, "%?: %?", cgs_alignfmt(label, LEFT, 8 , '.'), cgs_arrfmt(arr, 3));
        
        ASSERT_TRUE(n == cgs_len(buf));
    }
    
    /* ── cgs_writer wrapping unsigned int* ────────────────────── */
    
    TEST("writer unsigned int*: cgs_writer wraps counter and counts via append");
    {
        unsigned int n = 0;
        CGS_Writer *w = cgs_writer_ptr(&n);
        cgs_append(w, "abc");
        cgs_append(w, "de");
        ASSERT_TRUE(n == 5);
    }
    
    TEST("writer unsigned int*: cgs_writer counts putc calls");
    {
        unsigned int n = 0;
        CGS_Writer *w = cgs_writer_ptr(&n);
        cgs_putc(w, 'x');
        cgs_putc(w, 'y');
        ASSERT_TRUE(n == 2);
    }
    
    TEST("writer unsigned int*: cgs_writer counts append_fmt output");
    {
        /* "1 + 2 = 3" → 9 chars */
        unsigned int n = 0;
        CGS_Writer *w = cgs_writer_ptr(&n);
        cgs_appendf(w, "%? + %? = %?", 1, 2, 3);
        ASSERT_TRUE(n == 9);
    }
}

typedef struct Point { int x, y; } Point;

CGS_Error point_tostr(CGS_Writer *dst, Point p, CGS_StrView opt)
{
    cgs_appendf(dst, "(%?, %?)", p.x, p.y);
    return (CGS_Error){CGS_OK};
}

#define ADD_TOSTR (Point, point_tostr)
#include "../cgs.h"

void test_repeatfmt(void)
{
    char buf[128];

    /* ── Basic repetition of a string ─────────────────────────── */

    TEST("cgs_repeatfmt: repeat a string literal n times");
    {
        char *s = "ab";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 3));
        ASSERT_STR_EQ(buf, "ababab");
    }

    TEST("cgs_repeatfmt: repeat single char string");
    {
        char *s = "x";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 5));
        ASSERT_STR_EQ(buf, "xxxxx");
    }

    TEST("cgs_repeatfmt: repeat multi-char string");
    {
        char *s = "-=";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 4));
        ASSERT_STR_EQ(buf, "-=-=-=-=");
    }

    /* ── Edge: n == 1 → identity ──────────────────────────────── */

    TEST("cgs_repeatfmt: n=1 → single copy");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 1));
        ASSERT_STR_EQ(buf, "hello");
    }

    /* ── Edge: n == 0 → empty output ──────────────────────────── */

    TEST("cgs_repeatfmt: n=0 → empty output");
    {
        char *s = "hello";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 0));
        ASSERT_STR_EQ(buf, "");
    }

    /* ── Edge: empty string repeated → empty output ───────────── */

    TEST("cgs_repeatfmt: empty string repeated n times → empty");
    {
        char *s = "";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 10));
        ASSERT_STR_EQ(buf, "");
    }

    TEST("cgs_repeatfmt: empty string with n=0 → empty");
    {
        char *s = "";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 0));
        ASSERT_STR_EQ(buf, "");
    }

    /* ── Non-string types (has tostr) ─────────────────────────── */

    TEST("cgs_repeatfmt: repeat an int's tostr");
    {
        /* 42 → "42", repeated 3x → "424242" */
        int n = 42;
        cgs_fmt(buf, "%?", cgs_repeatfmt(n, 3));
        ASSERT_STR_EQ(buf, "424242");
    }

    TEST("cgs_repeatfmt: repeat a negative int");
    {
        /* -7 → "-7", repeated 3x → "-7-7-7" */
        int n = -7;
        cgs_fmt(buf, "%?", cgs_repeatfmt(n, 3));
        ASSERT_STR_EQ(buf, "-7-7-7");
    }

    TEST("cgs_repeatfmt: repeat custom Point type tostr");
    {
        /* (1, 2) repeated twice → "(1, 2)(1, 2)" */
        Point p = {1, 2};
        cgs_fmt(buf, "%?", cgs_repeatfmt(p, 2));
        ASSERT_STR_EQ(buf, "(1, 2)(1, 2)");
    }

    /* ── Composition with other fmt objects ───────────────────── */

    TEST("cgs_repeatfmt: repeat an nfmt object");
    {
        cgs_nfmt_t(int, 'X') hex = cgs_nfmt(255, 'X'); /* "FF" */
        cgs_fmt(buf, "%?", cgs_repeatfmt(hex, 3));
        ASSERT_STR_EQ(buf, "FFFFFF");
    }

    TEST("cgs_repeatfmt: repeat an arrfmt object");
    {
        /* "{1, 2}" repeated twice → "{1, 2}{1, 2}" */
        int arr[] = {1, 2};
        CGS_ArrayFmt af = cgs_arrfmt(arr, 2);
        cgs_fmt(buf, "%?", cgs_repeatfmt(af, 2));
        ASSERT_STR_EQ(buf, "{1, 2}{1, 2}");
    }

    TEST("cgs_repeatfmt: repeat an alignfmt object");
    {
        /* "hi" centered in width 6 → "  hi  ", repeated twice */
        char *s = "hi";
        CGS__AlignFmt al = cgs_alignfmt(s, CENTER, 6, ' ');
        cgs_fmt(buf, "%?", cgs_repeatfmt(al, 2));
        ASSERT_STR_EQ(buf, "  hi    hi  ");
    }

    /* ── repeatfmt nested in alignfmt and vice versa ──────────── */

    TEST("cgs_repeatfmt: repeatfmt result aligned via alignfmt");
    {
        /* "ab" x3 = "ababab" (6 chars), right-aligned in width 10 */
        char *s = "ab";
        CGS__RepeatFmt rf = cgs_repeatfmt(s, 3);
        cgs_fmt(buf, "%?", cgs_alignfmt(rf, RIGHT, 10, '.'));
        ASSERT_STR_EQ(buf, "....ababab");
    }

    TEST("cgs_repeatfmt: alignfmt repeated via repeatfmt (already covered) sanity");
    {
        /* "x" left in width 3 = "x  ", repeated 3x = "x  x  x  " */
        char *s = "x";
        CGS__AlignFmt al = cgs_alignfmt(s, LEFT, 3, ' ');
        cgs_fmt(buf, "%?", cgs_repeatfmt(al, 3));
        ASSERT_STR_EQ(buf, "x  x  x  ");
    }

    /* ── repeatfmt inside a larger fmt string ─────────────────── */

    TEST("cgs_repeatfmt: embedded in a fmt string with surrounding text");
    {
        char *s = "=";
        cgs_fmt(buf, "[%?]", cgs_repeatfmt(s, 8));
        ASSERT_STR_EQ(buf, "[========]");
    }

    TEST("cgs_repeatfmt: multiple repeatfmts in one fmt call");
    {
        char *dash = "-", *star = "*";
        cgs_fmt(buf, "%?%?%?",
                cgs_repeatfmt(dash, 3),
                cgs_repeatfmt(star, 2),
                cgs_repeatfmt(dash, 3));
        ASSERT_STR_EQ(buf, "---**---");
    }

    TEST("cgs_repeatfmt: mixed with normal args in fmt");
    {
        char *s = ".";
        int count = 3;
        cgs_fmt(buf, "loading%? (%?)", cgs_repeatfmt(s, 3), count);
        ASSERT_STR_EQ(buf, "loading... (3)");
    }

    /* ── repeatfmt of a repeatfmt (nesting) ───────────────────── */

    TEST("cgs_repeatfmt: repeatfmt nested in repeatfmt");
    {
        /* inner "ab" x2 = "abab", outer repeats that 3x = "abababababab" */
        char *s = "ab";
        CGS__RepeatFmt inner = cgs_repeatfmt(s, 2);
        cgs_fmt(buf, "%?", cgs_repeatfmt(inner, 3));
        ASSERT_STR_EQ(buf, "abababababab");
    }

    /* ── Larger repetition count ──────────────────────────────── */

    TEST("cgs_repeatfmt: large n produces correct length");
    {
        char *s = "ab";
        cgs_fmt(buf, "%?", cgs_repeatfmt(s, 50));
        ASSERT_TRUE(cgs_len(buf) == 100);
        ASSERT_TRUE(buf[0] == 'a' && buf[1] == 'b');
        ASSERT_TRUE(buf[98] == 'a' && buf[99] == 'b');
    }

    /* ── Works with printf-family / counting writer ───────────── */

    TEST("cgs_repeatfmt: length via unsigned int* counting writer");
    {
        /* "xyz" x4 = 12 chars */
        char *s = "xyz";
        unsigned int len = 0;
        cgs_appendf(&len, "%?", cgs_repeatfmt(s, 4));
        ASSERT_TRUE(len == 12);
    }

    TEST("cgs_repeatfmt: via cgs_append_tostr_many");
    {
        char *s = "ha";
        CGS_StrBuf sb = cgs_strbuf_init_from_buf((char[64]){0});
        cgs_append_tostr_many(&sb, "laughs: ", cgs_repeatfmt(s, 3));
        ASSERT_STR_EQ(cgs_chars(&sb), "laughs: hahaha");
    }
}

/* ── spec-aware tostr types ───────────────────────────────────────
 * New tostr signature: CGS_Error (CGS_Writer *dst, T val, CGS_StrView spec)
 * The spec is the StrView between the parens in "%(...)" / "%[N(...)]".
 * ---------------------------------------------------------------- */

/* Tagged: echoes the spec it received, so tests can verify exactly
 * what StrView the parser captured (great for balanced brackets). */
typedef struct Tagged { int id; } Tagged;

CGS_Error tagged_tostr(CGS_Writer *dst, Tagged t, CGS_StrView spec)
{
    cgs_appendf(dst, "id=%?", t.id);
    if (spec.len > 0)
        cgs_appendf(dst, "[%?]", spec);
    return (CGS_Error){CGS_OK};
}

#define ADD_TOSTR (Tagged, tagged_tostr)
#include "../cgs.h"

/* Temp: the spec selects the unit, so output changes with the spec. */
typedef struct Temp { double celsius; } Temp;

CGS_Error temp_tostr(CGS_Writer *dst, Temp t, CGS_StrView spec)
{
    if (cgs_equal(spec, "F"))
        cgs_appendf(dst, "%?F", cgs_nfmt(t.celsius * 9.0 / 5.0 + 32.0, 'f', 1));
    else /* empty spec or "C" → celsius */
        cgs_appendf(dst, "%?C", cgs_nfmt(t.celsius, 'f', 1));
    return (CGS_Error){CGS_OK};
}

#define ADD_TOSTR (Temp, temp_tostr)
#include "../cgs.h"


void test_fmt_spec(void)
{
    char buf[128];
    
    /* ════════════════════════════════════════════════════════════
     * Basic "%(...)" spec passing
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt spec: simple spec is passed to tostr");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(hello)", t);
        ASSERT_STR_EQ(buf, "id=7[hello]");
    }
    
    TEST("fmt spec: empty spec '%()' → spec has length 0");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%()", t);
        ASSERT_STR_EQ(buf, "id=7");
    }
    
    TEST("fmt spec: '%?' passes an empty spec (same as '%()')");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%?", t);
        ASSERT_STR_EQ(buf, "id=7");
    }
    
    TEST("fmt spec: spec with spaces and punctuation preserved verbatim");
    {
        Tagged t = {3};
        cgs_fmt(buf, "%(a b, c!)", t);
        ASSERT_STR_EQ(buf, "id=3[a b, c!]");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Spec that changes behavior (Temp)
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt spec: Temp defaults to celsius with empty spec");
    {
        Temp t = {25.0};
        cgs_fmt(buf, "%?", t);
        ASSERT_STR_EQ(buf, "25.0C");
    }
    
    TEST("fmt spec: Temp '%(C)' → celsius");
    {
        Temp t = {25.0};
        cgs_fmt(buf, "%(C)", t);
        ASSERT_STR_EQ(buf, "25.0C");
    }
    
    TEST("fmt spec: Temp '%(F)' → fahrenheit conversion");
    {
        Temp t = {25.0};
        cgs_fmt(buf, "%(F)", t);
        ASSERT_STR_EQ(buf, "77.0F");
    }
    
    TEST("fmt spec: Temp 0C in fahrenheit → 32.0F");
    {
        Temp t = {0.0};
        cgs_fmt(buf, "%(F)", t);
        ASSERT_STR_EQ(buf, "32.0F");
    }
    
    TEST("fmt spec: Temp 100C in fahrenheit → 212.0F");
    {
        Temp t = {100.0};
        cgs_fmt(buf, "%(F)", t);
        ASSERT_STR_EQ(buf, "212.0F");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Balanced brackets inside the spec  ( () and [] )
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt spec balanced: nested parens '%(a(b)c)'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(a(b)c)", t);
        ASSERT_STR_EQ(buf, "id=7[a(b)c]");
    }
    
    TEST("fmt spec balanced: single bracket pair '%([x])'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%([x])", t);
        ASSERT_STR_EQ(buf, "id=7[[x]]");
    }
    
    TEST("fmt spec balanced: empty parens inside '%(())'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(())", t);
        ASSERT_STR_EQ(buf, "id=7[()]");
    }
    
    TEST("fmt spec balanced: empty brackets inside '%([])'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%([])", t);
        ASSERT_STR_EQ(buf, "id=7[[]]");
    }
    
    TEST("fmt spec balanced: brackets containing parens '%([a(b)c]d)'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%([a(b)c]d)", t);
        ASSERT_STR_EQ(buf, "id=7[[a(b)c]d]");
    }
    
    TEST("fmt spec balanced: deeply nested parens '%(((x)))'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(((x)))", t);
        ASSERT_STR_EQ(buf, "id=7[((x))]");
    }
    
    TEST("fmt spec balanced: deeply nested brackets '%([[y]])'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%([[y]])", t);
        ASSERT_STR_EQ(buf, "id=7[[[y]]]");
    }
    
    TEST("fmt spec balanced: mixed call-like content '%(f(1, 2)+g[3])'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(f(1, 2)+g[3])", t);
        ASSERT_STR_EQ(buf, "id=7[f(1, 2)+g[3]]");
    }
    
    TEST("fmt spec balanced: parens wrapping brackets wrapping parens '%(([()]))'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%(([()]))", t);
        ASSERT_STR_EQ(buf, "id=7[([()])]");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Positional argument syntax "%[N]"
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt positional: '%[0]' selects the first argument");
    {
        Tagged t = {5};
        cgs_fmt(buf, "%[0]", t);
        ASSERT_STR_EQ(buf, "id=5");
    }
    
    TEST("fmt positional: reorder with '%[1]' and '%[0]'");
    {
        Tagged a = {1}, b = {2};
        cgs_fmt(buf, "%[1] then %[0]", a, b);
        ASSERT_STR_EQ(buf, "id=2 then id=1");
    }
    
    TEST("fmt positional: repeat the same argument '%[0]%[0]%[0]'");
    {
        Tagged t = {9};
        cgs_fmt(buf, "%[0]%[0]%[0]", t);
        ASSERT_STR_EQ(buf, "id=9id=9id=9");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Positional + spec "%[N(...)]"
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt positional+spec: '%[0(hi)]' passes spec to indexed arg");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%[0(hi)]", t);
        ASSERT_STR_EQ(buf, "id=7[hi]");
    }
    
    TEST("fmt positional+spec: reorder temps with units");
    {
        Temp t1 = {25.0}, t2 = {0.0};
        /* arg0=t1, arg1=t2 → %[1(F)]→t2 in F, %[0(C)]→t1 in C */
        cgs_fmt(buf, "%[1(F)] %[0(C)]", t1, t2);
        ASSERT_STR_EQ(buf, "32.0F 25.0C");
    }
    
    TEST("fmt positional+spec: same arg with two different specs");
    {
        Temp t = {100.0};
        cgs_fmt(buf, "%[0(C)] = %[0(F)]", t);
        ASSERT_STR_EQ(buf, "100.0C = 212.0F");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Balanced brackets inside a positional spec
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt positional+spec balanced: '%[0(a(b)c)]'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%[0(a(b)c)]", t);
        ASSERT_STR_EQ(buf, "id=7[a(b)c]");
    }
    
    TEST("fmt positional+spec balanced: '%[0([nested])]'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%[0([nested])]", t);
        ASSERT_STR_EQ(buf, "id=7[[nested]]");
    }
    
    TEST("fmt positional+spec balanced: brackets and parens '%[0([p(q)]r)]'");
    {
        Tagged t = {7};
        cgs_fmt(buf, "%[0([p(q)]r)]", t);
        ASSERT_STR_EQ(buf, "id=7[[p(q)]r]");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Mixing sequential '%?' and sequential '%(...)'
     * (both auto-indexed; only positional '%[N]' may not be mixed in)
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt spec: mix '%?' and '%(...)' sequentially");
    {
        Tagged tag = {7};
        Temp   tmp = {25.0};
        cgs_fmt(buf, "%? %(F)", tag, tmp);
        ASSERT_STR_EQ(buf, "id=7 77.0F");
    }
    
    TEST("fmt spec: two spec args in one call");
    {
        Temp a = {0.0}, b = {100.0};
        cgs_fmt(buf, "%(C)|%(F)", a, b);
        ASSERT_STR_EQ(buf, "0.0C|212.0F");
    }
    
    TEST("fmt spec: spec args surrounded by literal text");
    {
        Tagged t = {42};
        cgs_fmt(buf, "<<%(tag)>>", t);
        ASSERT_STR_EQ(buf, "<<id=42[tag]>>");
    }
    
    /* ════════════════════════════════════════════════════════════
     * Works through printf-family / other sinks
     * ════════════════════════════════════════════════════════════ */
    
    TEST("fmt spec: append_fmt into DStr with spec");
    {
        Temp t = {37.0};
        CGS_DStr d = cgs_dstr_init();
        cgs_appendf(&d, "body temp: %(F)", t);
        ASSERT_STR_EQ(cgs_chars(&d), "body temp: 98.6F");
        cgs_dstr_deinit(&d);
    }
    
    TEST("fmt spec: length via unsigned int* counting writer");
    {
        /* "id=7[hello]" → 11 chars */
        Tagged t = {7};
        unsigned int n = 0;
        cgs_appendf(&n, "%(hello)", t);
        ASSERT_TRUE(n == 11);
    }
}

// ============================================================================
// Main
// ============================================================================

void test_str_len_edge_cases();
void test_str_cap_edge_cases();
void test_str_equal_edge_cases();
void test_str_find_edge_cases();
void test_str_count_edge_cases();
void test_str_clear_edge_cases();
void test_str_starts_ends_with_edge_cases();
void test_str_case_edge_cases();
void test_str_copy_edge_cases();
void test_str_putc_edge_cases();
void test_str_append_edge_cases();
void test_str_del_edge_cases();
void test_str_replace_edge_cases();
void test_str_replace_first_edge_cases();
void test_str_replace_range_edge_cases();
void test_str_split_edge_cases();
void test_str_insert_prepend_edge_cases();
void test_dstring_edge_cases();
void test_string_buffer_edge_cases();
void test_strv_edge_cases();
void test_file_io_edge_cases();
void test_appender_edge_cases();
void test_str_join_edge_cases();
void test_spn_cspn_tok();
void test_zstrv();
void test_chain_writer();
void test_custom_writer();


int test_count;
int passed_count;

int main() {
    printf("========================================\n");
    printf("String Library Edge Case Test Suite\n");
    printf("========================================\n\n");
    
    test_str_len_edge_cases();
    test_str_cap_edge_cases();
    test_str_equal_edge_cases();
    test_str_find_edge_cases();
    test_str_count_edge_cases();
    test_str_clear_edge_cases();
    test_str_starts_ends_with_edge_cases();
    test_str_case_edge_cases();
    test_str_copy_edge_cases();
    test_str_putc_edge_cases();
    test_str_append_edge_cases();
    test_str_del_edge_cases();
    test_str_replace_edge_cases();
    test_str_replace_first_edge_cases();
    test_str_replace_range_edge_cases();
    test_str_split_edge_cases();
    test_str_insert_prepend_edge_cases();
    test_dstring_edge_cases();
    test_string_buffer_edge_cases();
    test_strv_edge_cases();
    test_file_io_edge_cases();
    test_appender_edge_cases();
    test_str_join_edge_cases();
    test_tostr_edge_cases();
    test_stress_cases();
    test_special_characters();
    test_replace_all();
    test_alignfmt();
    test_writer_counter();
    test_spn_cspn_tok();
    test_repeatfmt();
    test_fmt_spec();
    test_zstrv();
    test_chain_writer();
    test_custom_writer();
    
    printf("\n========================================\n");
    printf("Test Results: %d/%d passed\n", passed_count, test_count);
    printf("========================================\n");
        
    return (passed_count == test_count) ? 0 : 1;
}
