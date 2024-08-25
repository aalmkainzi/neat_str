#include <assert.h>
#define NEAT_STR_IMPL
#include "../neat_str.h"

struct FOO {
    int capacity;
    char drive;
};

DString foo_to_str( struct FOO *f )
{
    DString ret = str_print_new("FOO{ .drive=", f->drive, ":/", ", .capacity=", f->capacity, "}");
    return ret;
}

struct BAR {
    bool b;
    int i;
};

void bar_tostr_into(Mut_String_Ref str, struct BAR *b)
{
    str_print(str, "BAR{ b=", b->b, ", i=", b->i, " }");
}

#define ADD_TOSTR struct FOO, foo_to_str
#include "../neat_str.h"

#define ADD_TOSTR_INTO struct BAR, bar_tostr_into
#include "../neat_str.h"

void test_tostr()
{
    struct FOO a = {.capacity = 15, .drive = 'C'};
    DString a_str = tostr(a);
    assert(str_equal(a_str, "FOO{ .drive=C:/, .capacity=15}"));
    free(a_str.chars);
    
    char C[128] = {0};
    String_Buffer sb = strbuf(C);
    struct BAR bar = {.b = false, .i = 25};
    
    str_print(&sb, bar);
    assert(str_equal(sb, "BAR{ b=false, i=25 }"));
}

void test_str_del()
{
    char C[64] = {0};
    String_Buffer str = strbuf(C);
    str_copy(&str, "hello[DELETE THIS] world");
    
    String_View found = str_find(str, "[DELETE THIS]");
    bool ok = str_del(&str, found.chars - str.chars, found.chars - str.chars + found.len);
    
    assert(ok);
    assert(str_equal(str, "hello world"));
}

void test_str_count()
{
    String_View str = strv("aaabccdddd");
    assert(str_count(str, "a") == 3);
    assert(str_count(str, "b") == 1);
    assert(str_count(str, "c") == 2);
    assert(str_count(str, "d") == 4);
    assert(str_count("vvv", "vv") == 1);
    assert(str_count(str, "") == 0);
    
    char C[] = "hello world";
    String_Buffer sb = strbuf(C);
    unsigned int L = str_len(mutstr_ref("hello world"));
    assert(L == str_len("hello world") && L == strlen("hello world") && L == str_len(sb) && L == str_len(C));
}

void test_str_find()
{
    String_View str = strv("hello world");
    String_View empty = str_find(str, "");
    
    assert(empty.chars == str.chars && empty.len == 0);
    assert(str_equal(str_find(str, str), str));
    assert((str_find(str, "world").chars - str.chars) == 6);
    assert(str_find(str, "world").len == 5);
}

void test_str_replace()
{
    char C[64] = {0};
    String_Buffer sb = strbuf(C);
    str_copy(&sb, "h-e-l-l-o-,- -w-o-r-l-d");
    unsigned int replaced = str_replace(&sb, "-", "");
    assert(str_equal(sb, "hello, world"));
    assert(replaced == 11);
    
    str_copy(&sb, "hello world");
    replaced = str_replace(&sb, "-", ",");
    assert(str_equal(sb, "hello world"));
    assert(replaced == 0);
    
    str_copy(&sb, "h-e-l-l-o-,- -w-o-r-l-d");
    replaced = str_replace(&sb, "-", "_-_");
    assert(str_equal(sb, "h_-_e_-_l_-_l_-_o_-_,_-_ _-_w_-_o_-_r_-_l_-_d"));
    assert(replaced == 11);
    
    str_copy(&sb, "h_-_e_-_l_-_l_-_o_-_,_-_ _-_w_-_o_-_r_-_l_-_d");
    replaced = str_replace(&sb, "_-_", "X");
    assert(str_equal(sb, "hXeXlXlXoX,X XwXoXrXlXd"));
    assert(replaced == 11);
    
    str_copy(&sb, "hello world");
    str_replace(&sb, "", "-");
    assert(str_equal(sb, "-h-e-l-l-o- -w-o-r-l-d-"));
    
    str_copy(&sb, "hello world");
    str_replace(&sb, "", "XX");
    assert(str_equal(sb, "XXhXXeXXlXXlXXoXX XXwXXoXXrXXlXXdXX"));
}

void test_str_replace_first()
{
    char C[64] = {0};
    String_Buffer sb = strbuf(C);
    str_copy(&sb, "h-e-l-l-o-,- -w-o-r-l-d");
    bool replaced = str_replace_first(&sb, "-", "");
    assert(str_equal(sb, "he-l-l-o-,- -w-o-r-l-d"));
    assert(replaced);
    
    str_copy(&sb, "dont replace here");
    replaced = str_replace(&sb, "XX", "BIGGGGGGSTRINGGGGGG");
    assert(str_equal(sb, "dont replace here"));
    assert(!replaced);
    
    str_copy(&sb, "hello world");
    str_replace_first(&sb, "", "---");
    assert(str_equal(sb, "---hello world"));
}

void test_read()
{
    String_Buffer sb = strbuf(64);
    
    FILE *f = fopen("file", "r");
    str_fread_line(&sb, f);
    assert(str_equal(sb, "this is the first line\n"));
    unsigned int linelen = str_len(sb);
    str_concat_fread_line(&sb, f);
    assert(str_equal(strv(sb, linelen, sb.len), "no newline here"));
    
    assert(feof(f));
    
    fclose(f);
    free(sb.chars);
    
    f = fopen("file", "r");
    
    DString line = dstr();
    
    dstr_fread_line(&line, f);
    assert(str_equal(line, "this is the first line\n"));
    
    dstr_append_fread_line(&line, f);
    assert(str_equal(line, "this is the first line\nno newline here"));
    
    dstr_deinit(&line);
}

void test_misc()
{
    char *s = "this is my cstr";
    String_Buffer sb = strbuf((char[128]){0});
    for(unsigned i = 0, len = str_len(s) ; i < len ; i++)
    {
        str_print(&sb, sb, str_at(s, i));
    }
    assert(str_equal(sb, "this is my cstr"));
    assert(str_equal(sb, s));
    assert(str_starts_with(sb, sb));
    assert(str_ends_with(sb, sb));
    assert(str_starts_with(sb, "this"));
    assert(str_ends_with(sb, "cstr"));
    
    String_View_Array split = str_split("hello,world", "");
    DString d = str_join_new(split, "-");
    assert(str_equal(d, "h-e-l-l-o-,-w-o-r-l-d"));
    
    free(split.strs);
    free(d.chars);
    
    split = str_split("abc", "a");
    d = str_join_new(split, "X");
    assert(str_equal(d, "Xbc"));
    
    free(d.chars);
    free(split.strs);
}

void test_sprint()
{
    char C[128] = {0};
    String_Buffer sb = strbuf(C);
    
    str_print(&sb, sb);
    assert(str_equal(sb, ""));
    
    str_copy(&sb, "WOW");
    str_print(&sb, sb, 5);
    assert(str_equal(sb, "WOW5"));
    
    str_print(&sb, "w", sb, "w", ".");
    assert(str_equal(sb, "www."));
    
    str_print(&sb, "hello", " ", "world", 123);
    assert(str_equal(&sb, "hello world123"));
    
    str_print(&sb, sb, sb);
    assert(str_equal(sb, "hello world123hello world123"));
    
    str_copy(&sb, "hello world123");
    str_concat(&sb, sb);
    assert(str_equal(sb, "hello world123hello world123"));
    
    str_copy(&sb, "X-");
    str_print(&sb, sb, sb, sb);
    assert(str_equal(sb, "X-X-X-X-"));
    
    unsigned char CC2[256] = {0};
    String_Buffer sb2 = strbuf(CC2);
    unsigned short unsigned_short_max = -1;
    unsigned int unsigned_int_max = -1;
    unsigned long unsigned_long_max = -1;
    unsigned long long unsigned_long_long_max = -1;
    signed char signed_char_min = -128;
    signed char signed_char_max = 127;
    short short_min = -32768;
    short short_max = 32767;
    int int_min = -2147483648;
    int int_max = 2147483647;
    long long_min = LONG_MIN;
    long long_max = LONG_MAX;
    long long long_long_min = LLONG_MIN;
    long long long_long_max = LLONG_MAX;
    str_print(&sb2,signed_char_min, " ", short_min, " ", int_min, " ", long_min, " ", long_long_min, " ", signed_char_max, " ", short_max, " ", int_max, " ", long_max, " "
    , long_long_max, " ",unsigned_short_max, " ", unsigned_int_max, " ", unsigned_long_max, " ", unsigned_long_long_max);
    
    assert(str_equal(sb2, "-128 -32768 -2147483648 -9223372036854775808 -9223372036854775808 127 32767 2147483647 9223372036854775807 9223372036854775807 65535 4294967295 18446744073709551615 18446744073709551615"));
    
    str_print(&sb2, 0);
    assert(str_equal(sb2, "0"));
    
    unsigned char CC3[3] = {0};
    String_Buffer sb3 = strbuf(CC3);
    str_print(&sb3, 123456);
    assert(str_equal(sb3, "12"));
    str_print(&sb3, (unsigned long long) 5678);
    assert(str_equal(sb3, "56"));
}

void test_insert()
{
    char C[256] = {0};
    String_Buffer sb = strbuf(C);
    
    str_insert(&sb, "hello world", 0);
    assert(str_equal(sb, "hello world"));
    
    str_insert(&sb, ",", 5);
    assert(str_equal(sb, "hello, world"));
}

void test_sstr()
{
    NEAT_DECL_SSTRING(64);
    
    SString(64) sstr = {0};
    SString_Ref sstr_ref = sstr_ref(&sstr);
    
    str_copy(sstr_ref, "hello world");
    assert(str_equal(sstr_ref, "hello world") && str_equal(sstr.chars, "hello world"));
}

int main()
{
    test_tostr();
    test_str_del();
    test_str_count();
    test_str_find();
    test_str_replace();
    test_str_replace_first();
    test_read();
    test_misc();
    test_sprint();
    test_insert();
    test_sstr();
}
