#ifndef NEAT_STR_H
#define NEAT_STR_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include "neat_core.h"

// Dynamic String
typedef struct Neat_DString
{
    Neat_Allocator allocator;
    unsigned int cap; /* including the nul */
    unsigned int len;
    unsigned char *chars;
} Neat_DString;

// Used as a general purpose non-dynamic string buffer
typedef struct Neat_String_Buffer
{
    unsigned int cap; /* including the nul */
    unsigned int len;
    unsigned char *chars;
} Neat_String_Buffer;

// Used to view slices of other strings
typedef struct Neat_String_View
{
    unsigned int len;
    unsigned char *chars;
} Neat_String_View;

// An array of the above
typedef struct Neat_String_View_Array
{
    unsigned int cap;
    unsigned int nb;
    Neat_String_View *strs;
} Neat_String_View_Array;

// Used for passing SString around
typedef struct Neat_SString_Ref
{
    const unsigned int cap;
    struct {
        unsigned int len;
        unsigned char chars[];
    } *sstring;
} Neat_SString_Ref;

// Can be used as a reference to any string type
typedef struct Neat_Mut_String_Ref
{
    const unsigned int cap;
    unsigned int *len; /* may be NULL (in case of cstr) */
    unsigned char *chars;
} Neat_Mut_String_Ref;

typedef struct Neat_Buffer {
    unsigned int size;
    void *ptr;
} Neat_Buffer;

// in C23 structs can be defined multiple times with the same tag and members,
// in which NEAT_DECL_SSTRING is useless, but older standards require it.
#if __STDC_VERSION__ >= 202311L

    #define Neat_SString(cap) \
    struct Neat_SString_##cap \
    { \
        _Static_assert((1##cap##1ul || cap##8ul || 1) && (cap > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure cap is an integer literal */ \
        unsigned int len; \
        unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
    }
    
    #define NEAT_DECL_SSTRING(cap)
#else

    #define Neat_SString(cap) \
    Neat_SString_##cap
    
    #define NEAT_DECL_SSTRING_(cap) \
    typedef struct Neat_SString_##cap \
    { \
        _Static_assert((1##cap##1ul || cap##8ul || 1) && (cap > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure cap is an integer literal */ \
        unsigned int len; \
        unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
    } Neat_SString_##cap

    #define NEAT_DECL_SSTRING(cap) \
    NEAT_DECL_SSTRING_(cap)
#endif

// SString stuff
#define NEAT_SSTRING_NTAG(cap) \
struct \
{ \
    unsigned int len; \
    unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
}

#define NEAT_TYPEOF_SSTR(sstr) \
NEAT_SSTRING_NTAG(sizeof((sstr)->chars)-1)

#define NEAT_IS_SSTRING_PTR(s) \
(sizeof(*(s)) == sizeof(NEAT_TYPEOF_SSTR(s))) && \
_Alignof(typeof(*(s))) == _Alignof(NEAT_TYPEOF_SSTR(s)) && \
neat_has_type(&(s)->chars, typeof(&(NEAT_TYPEOF_SSTR(s)){0}.chars))

#define NEAT_GURANTEE_STRPTR_CHARS_MEM(any_str)                \
_Generic(any_str,                                              \
    Neat_String_Buffer            : (Neat_String_Buffer*){0},  \
    char*                         : (Neat_String_Buffer*){0},  \
    NEAT_UCHAR_CASE(unsigned char*: (Neat_String_Buffer*){0},) \
    Neat_DString                  : (Neat_String_Buffer*){0},  \
    Neat_String_View              : (Neat_String_Buffer*){0},  \
    Neat_String_Buffer            : (Neat_String_Buffer*){0},  \
    Neat_SString_Ref              : (Neat_String_Buffer*){0},  \
    Neat_Mut_String_Ref           : (Neat_String_Buffer*){0},  \
    default                       : any_str                    \
)

// SString stuff end

// this is to cause compile err if type is not mutable string
#define neat_str_assert_mutable(str)    \
(void)_Generic(str,                     \
    char*                         : 1,  \
    NEAT_UCHAR_CASE(unsigned char*: 1,) \
    Neat_DString*                 : 1,  \
    Neat_String_Buffer*           : 1,  \
    Neat_SString_Ref              : 1,  \
    Neat_Mut_String_Ref           : 1   \
)

#define neat_str_assert_appendable(str) \
(void)_Generic(str,                     \
    Neat_DString*      : 1,             \
    Neat_String_Buffer*: 1,             \
    Neat_SString_Ref   : 1,             \
    Neat_Mut_String_Ref: 1              \
)

#define neat_str_at(any_str, idx)                            \
_Generic(any_str,                                            \
    char*                         : neat_cstr_char_at,       \
    NEAT_UCHAR_CASE(unsigned char*: neat_ucstr_char_at,)     \
    Neat_DString                  : neat_dstr_char_at,       \
    Neat_DString*                 : neat_dstr_ptr_char_at,   \
    Neat_String_View              : neat_strv_char_at,       \
    Neat_String_View*             : neat_strv_ptr_char_at,   \
    Neat_String_Buffer            : neat_strbuf_char_at,     \
    Neat_String_Buffer*           : neat_strbuf_ptr_char_at, \
    Neat_SString_Ref              : neat_sstr_ref_char_at,   \
    Neat_Mut_String_Ref           : neat_mutstr_ref_char_at  \
)(any_str, idx)

#define neat_str_len(any_str) \
strv(any_str).len

#define neat_str_cap(any_str) \
neat_strbuf(any_str).cap

#define neat_str_cstr(any_str) \
_Generic(any_str, \
    char*                         : neat_cstr_as_cstr,       \
    NEAT_UCHAR_CASE(unsigned char*: neat_ucstr_as_cstr,)     \
    Neat_DString                  : neat_dstr_as_cstr,       \
    Neat_DString*                 : neat_dstr_ptr_as_cstr,   \
    Neat_String_View              : neat_strv_as_cstr,       \
    Neat_String_View*             : neat_strv_ptr_as_cstr,   \
    Neat_String_Buffer            : neat_strbuf_as_cstr,     \
    Neat_String_Buffer*           : neat_strbuf_ptr_as_cstr, \
    Neat_SString_Ref              : neat_sstr_ref_as_cstr,   \
    Neat_Mut_String_Ref           : neat_mutstr_ref_as_cstr  \
)(any_str)

#define neat_str_equal(any_str1, any_str2) \
neat_strv_equal(neat_strv(any_str1), neat_strv(any_str2))

#define neat_str_copy(any_str_dst, any_str_src) \
( \
    neat_str_assert_mutable(any_str_dst), \
    neat_mutstr_ref_copy(neat_mutstr_ref(any_str_dst), neat_strv(any_str_src)) \
)

#define neat_str_concat(any_str_dst, any_str_src)                            \
(                                                                            \
neat_str_assert_appendable(any_str_dst),                                     \
neat_mutstr_ref_concat(neat_mutstr_ref(any_str_dst), neat_strv(any_str_src)) \
)

#define neat_str_insert(any_str_dst, any_str_src, idx) \
( \
    neat_str_assert_appendable(any_str_dst), \
    neat_mutstr_ref_insert_strv(neat_mutstr_ref(any_str_dst), neat_strv(any_str_src), idx) \
)

#define neat_str_prepend(neat_str_dst, neat_str_src) \
neat_str_insert(neat_str_dst, neat_str_src, 0)

#define neat_str_find(any_str_hay, any_str_needle) \
neat_strv_find(neat_strv(any_str_hay), neat_strv(any_str_needle))

#define neat_str_count(any_str_hay, any_str_needle) \
neat_strv_count(neat_strv(any_str_hay), neat_strv(any_str_needle))

#define neat_str_starts_with(any_str_hay, any_str_needle) \
neat_strv_starts_with(neat_strv(any_str_hay), neat_strv(any_str_needle))

#define neat_str_ends_with(any_str_hay, any_str_needle) \
neat_strv_ends_with(neat_strv(any_str_hay), neat_strv(any_str_needle))

#define neat_str_replace(any_str, any_str_target, any_str_replacement)                                       \
(                                                                                                            \
neat_str_assert_mutable(any_str),                                                                            \
neat_mutstr_ref_replace(neat_mutstr_ref(any_str), neat_strv(any_str_target), neat_strv(any_str_replacement)) \
)

#define neat_str_replace_first(any_str, any_str_target, any_str_replacement)                                       \
(                                                                                                                  \
neat_str_assert_mutable(any_str),                                                                                  \
neat_mutstr_ref_replace_first(neat_mutstr_ref(any_str), neat_strv(any_str_target), neat_strv(any_str_replacement)) \
)

#define neat_str_split(any_str_delim, any_str, ...) \
neat_strv_split(neat_strv(any_str_delim), neat_strv(any_str), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join_new(any_str_delim, strv_arr, ...) \
neat_strv_arr_join_new(neat_strv(any_str_delim), strv_arr, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join(any_str_dst, any_str_delim, strv_arr)                          \
(                                                                                    \
neat_str_assert_mutable(any_str_dst),                                                \
neat_strv_arr_join(neat_mutstr_ref(any_str_dst), neat_strv(any_str_delim), strv_arr) \
)

#define neat_str_del(any_str, begin, end) \
( \
    neat_str_assert_mutable(any_str), \
    neat_mutstr_ref_delete_range(neat_mutstr_ref(any_str), begin, end) \
)

#define neat_str_fread_line(any_str, stream) \
neat_mutstr_ref_fread_line(neat_mutstr_ref(any_str), stream)

#define neat_str_concat_fread_line(stream, any_str) \
neat_mutstr_ref_concat_fread_line(neat_mutstr_ref(any_str), stream)

#define neat_str_read_line(any_str) \
neat_mutstr_ref_fread_line(neat_mutstr_ref(any_str), stdin)

#define neat_str_concat_read_line(any_str) \
neat_mutstr_ref_concat_fread_line(neat_mutstr_ref(any_str), stdin)

// helper macro
#define neat_str_print_each(x) \
do \
{ \
    unsigned int neat_appended_len = 0; \
    Neat_Mut_String_Ref neat_anystr_ref_append_buf = \
    { \
        .cap = neat_as_mutstr_ref.cap - *neat_as_mutstr_ref.len, \
        .len = &neat_appended_len, \
        .chars = neat_as_mutstr_ref.chars + *neat_as_mutstr_ref.len, \
    }; \
    neat_tostr_into(neat_anystr_ref_append_buf, x); \
    *neat_as_mutstr_ref.len += neat_appended_len; \
} while(0);

#define neat_str_print(any_str_dst, ...) \
do \
{ \
    neat_str_assert_mutable(any_str_dst); \
    Neat_Mut_String_Ref neat_as_mutstr_ref = neat_mutstr_ref(any_str_dst); \
    unsigned int neat_mutstr_ref_len; \
    if(neat_as_mutstr_ref.len == NULL) \
    { \
        unsigned char *neat_str_end = memchr(neat_as_mutstr_ref.chars, '\0', neat_as_mutstr_ref.cap); \
        if(neat_str_end != NULL) \
        { \
            neat_mutstr_ref_len = neat_str_end - neat_as_mutstr_ref.chars; \
        } \
        else \
        { \
            neat_mutstr_ref_len = neat_as_mutstr_ref.cap - 1; \
        } \
        neat_as_mutstr_ref.len = &neat_mutstr_ref_len; \
    } \
    NEAT_FOREACH(neat_str_print_each, __VA_ARGS__); \
    \
} while(0)

#define neat_comma_tostr(s) \
, tostr(s)

#define neat_tostr_all_args(...) \
NEAT_FOREACH(neat_comma_tostr, __VA_ARGS__)

#define neat_count_arg(s) \
+1

#define neat_count_args(...) \
0 NEAT_FOREACH(neat_count_arg, __VA_ARGS__)

#define neat_str_print_new_wallocator(a, ...) \
neat_tostr_all_into_new_dstr(a, neat_count_args(__VA_ARGS__) neat_tostr_all_args(__VA_ARGS__))

#define neat_str_print_new_default(...) \
neat_tostr_all_into_new_dstr(neat_get_default_allocator(), neat_count_args(__VA_ARGS__) neat_tostr_all_args(__VA_ARGS__))

#define neat_str_print_new(...) \
neat_str_print_new_##__VA_OPT__(1)(__VA_ARGS__)

#define neat_str_print_new_1(stringable_or_allocator, ...) \
_Generic(stringable_or_allocator, \
    Neat_Allocator: neat_str_print_new_wallocator(neat_gurantee(stringable_or_allocator, Neat_Allocator), __VA_ARGS__), \
    default       : neat_str_print_new_default(neat_gurantee_not(stringable_or_allocator, Neat_Allocator, Neat_String_Buffer), __VA_ARGS__) \
)

#define neat_str_print_new_() \
neat_tostr_all_into_new_dstr(neat_get_default_allocator(), 0)

#define neat_strv_arr_carr(strv_carr, ...)                                     \
NEAT_IF_EMPTY(                                                                 \
    neat_strv_arr_from_carr(strv_carr, NEAT_CARR_LEN(strv_carr)), __VA_ARGS__  \
)                                                                              \
__VA_OPT__(neat_strv_arr_from_carr(strv_carr, __VA_ARGS__))

#define NEAT_STRV_COMMA(any_str, ...) \
neat_strv(any_str __VA_OPT__(,) __VA_ARGS__),

#define neat_strv_arr(...)                                                                     \
(                                                                                              \
neat_static_assertx(!neat_is_array_of(NEAT_ARG1(__VA_ARGS__), Neat_String_View), "strv_arr accepts variadic arguments of strings, not String_View[], call strv_arr_carr instead"), \
(Neat_String_View_Array) {                                                                     \
    .nb   = NEAT_CARR_LEN(((Neat_String_View[]){NEAT_FOREACH(NEAT_STRV_COMMA, __VA_ARGS__)})), \
    .strs = (Neat_String_View[]){NEAT_FOREACH(NEAT_STRV_COMMA, __VA_ARGS__)}                   \
}                                                                                              \
)

#define neat_strbuf(str_or_cap, ...) \
neat_strbuf_##__VA_OPT__(2)(str_or_cap __VA_OPT__(,) __VA_ARGS__)

#define neat_strbuf_(str_or_cap)                                                           \
_Generic((typeof(str_or_cap)*){0},                                                         \
    char(*)[sizeof(typeof(str_or_cap))]                          : neat_strbuf_from_buf,   \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(str_or_cap))] : neat_strbuf_from_buf,)  \
    default                                                      : neat_strbuf_new_default \
)(_Generic((typeof(str_or_cap)*){0},                                                       \
    char(*)[sizeof(typeof(str_or_cap))]: (Neat_Buffer){.ptr = neat_gurantee(str_or_cap, char*), .size = sizeof(typeof(str_or_cap))}, \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(str_or_cap))]: (Neat_Buffer){.ptr = neat_gurantee(str_or_cap, unsigned char*), .size = sizeof(typeof(str_or_cap))},) \
    default: str_or_cap \
))

#define neat_strbuf_2(cap_or_carr, allocator_or_cap) \
_Generic(allocator_or_cap, \
    Neat_Allocator: neat_strbuf_new, \
    default       : neat_strbuf_from_ptr \
)((neat_static_assertx(_Generic(allocator_or_cap, Neat_Allocator: 1, default: neat_has_type(cap_or_carr, char*) || neat_has_type(cap_or_carr, unsigned char*) || neat_has_type(cap_or_carr, void*)), "arg1 must be char* or unsigned char*"), cap_or_carr), allocator_or_cap)

#define neat_mutstr_ref(any_str, ...) \
neat_mutstr_ref_##__VA_OPT__(2)(any_str __VA_OPT__(,) __VA_ARGS__)

#define neat_mutstr_ref_(any_str)                                                             \
_Generic((typeof(any_str)*){0},                                                               \
    char**                          : neat_mutstr_ref_to_cstr,                                \
    NEAT_UCHAR_CASE(unsigned char** : neat_mutstr_ref_to_ucstr,)                              \
    Neat_DString**                  : neat_mutstr_ref_to_dstr_ptr,                            \
    /*Neat_String_View*             : neat_mutstr_ref_to_strv_ptr,*/                          \
    Neat_String_Buffer**            : neat_mutstr_ref_to_strbuf_ptr,                          \
    Neat_SString_Ref*               : neat_mutstr_ref_to_sstr_ref,                            \
    Neat_Mut_String_Ref*            : neat_mutstr_ref_to_mutstr_ref,                          \
    char(*)[sizeof(typeof(any_str))]: neat_mutstr_ref_to_mutstr_ref,                          \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(any_str))]: neat_mutstr_ref_to_mutstr_ref) \
)(_Generic((typeof(any_str)*){0},                                                   \
    char(*)[sizeof(typeof(any_str))]: (Neat_Mut_String_Ref){.len = NULL, .cap = sizeof(typeof(any_str)), .chars = (unsigned char*) neat_gurantee(any_str, char*)}, \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(any_str))]: (Neat_Mut_String_Ref){.len = NULL, .cap = sizeof(typeof(any_str)), .chars = neat_gurantee(any_str, unsigned char*)},) \
    default: any_str \
))

#define neat_mutstr_ref_2(carr_or_ptr, nb) \
( \
    neat_static_assertx(neat_has_type(carr_or_ptr, char*) || neat_has_type(carr_or_ptr, unsigned char*) || neat_has_type(carr_or_ptr, void*), "arg1 must be char* or unsigned char*"), \
    (Neat_Mut_String_Ref){.cap = nb, .chars = (unsigned char*) carr_or_ptr} \
)

#define neat_sstr_ref(sstr_ptr) \
( \
    neat_static_assertx(NEAT_IS_SSTRING_PTR(sstr_ptr), "Must pass SString(N)*"), \
    (Neat_SString_Ref){.cap = sizeof((sstr_ptr)->chars), .sstring = (void*) sstr_ptr} \
)

#define neat_sstr_ref_new(nb)                 \
(                                             \
    (void)0,                                  \
    (Neat_SString_Ref){                       \
        .cap = nb,                            \
        .sstring=(NEAT_SSTRING_NTAG(nb)){ 0 } \
    }                                         \
)

#define neat_strv(any_str, ...) \
NEAT_CAT(neat_strv, NEAT_NARG(any_str __VA_OPT__(,) __VA_ARGS__))(any_str __VA_OPT__(,) __VA_ARGS__)

#define neat_strv1(any_str) \
neat_strv2(any_str, 0)

#define neat_strv2(any_str, begin)                         \
_Generic(any_str,                                          \
    char*                         : neat_strv_cstr2,       \
    NEAT_UCHAR_CASE(unsigned char*: neat_strv_ucstr2,)     \
    Neat_DString                  : neat_strv_dstr2,       \
    Neat_DString*                 : neat_strv_dstr_ptr2,   \
    Neat_String_View              : neat_strv_strv2,       \
    Neat_String_View*             : neat_strv_strv_ptr2,   \
    Neat_String_Buffer            : neat_strv_strbuf2,     \
    Neat_String_Buffer*           : neat_strv_strbuf_ptr2, \
    Neat_SString_Ref              : neat_strv_sstr_ref2,   \
    Neat_Mut_String_Ref           : neat_strv_mutstr_ref2  \
)(any_str, begin)

#define neat_strv3(any_str, begin, end)                \
_Generic(any_str,                                      \
    char*              : neat_strv_cstr3,              \
    NEAT_UCHAR_CASE(unsigned char*: neat_strv_ucstr3,) \
    Neat_DString       : neat_strv_dstr3,              \
    Neat_DString*      : neat_strv_dstr_ptr3,          \
    Neat_String_View   : neat_strv_strv3,              \
    Neat_String_View*  : neat_strv_strv_ptr3,          \
    Neat_String_Buffer : neat_strv_strbuf3,            \
    Neat_String_Buffer*: neat_strv_strbuf_ptr3,        \
    Neat_SString_Ref   : neat_strv_sstr_ref3,          \
    Neat_Mut_String_Ref: neat_strv_mutstr_ref3         \
)(any_str, begin, end)

#define neat_dstr(...) \
neat_dstr0##__VA_OPT__(1)(__VA_ARGS__)

#define neat_dstr0() \
neat_dstr_new(16, neat_get_default_allocator())

#define neat_dstr01(cap_or_allocator, ...)                                        \
__VA_OPT__(neat_dstr2(cap_or_allocator, __VA_ARGS__))                             \
NEAT_IF_EMPTY(                                                                    \
_Generic(cap_or_allocator,                                                        \
    Neat_Allocator: neat_dstr_new(                                                \
                        16,                                                       \
                        neat_gurantee(cap_or_allocator, Neat_Allocator)           \
                    ),                                                            \
    default       : neat_dstr_new(                                                \
                        neat_gurantee_not(cap_or_allocator, Neat_Allocator, int), \
                        neat_get_default_allocator()                              \
                    )                                                             \
), __VA_ARGS__                                                                    \
)

#define neat_dstr2(cap, allocator) \
neat_dstr_new(cap, allocator)

#define neat_dstr_deinit(dstr) \
neat_dstr_deinit_(dstr)

#define neat_dstr_append(dstr, any_str) \
neat_dstr_append_strv(dstr, neat_strv(any_str))

#define neat_dstr_append_tostr(dstr, stringable) \
neat_dstr_append_tostr_(dstr, neat_tostr(stringable))

#define neat_dstr_append_tostr_p(dstr, stringable_ptr) \
neat_dstr_append_tostr_(dstr, neat_tostr_p(stringable))

#define neat_dstr_prepend(dstr, any_str) \
neat_dstr_prepend_strv(dstr, neat_strv(any_str))

#define neat_dstr_prepend_tostr(dstr, stringable) \
neat_dstr_prepend_tostr_(dstr, neat_tostr(stringable))

#define neat_dstr_prepend_tostr_p(dstr, stringable_ptr) \
neat_dstr_prepend_tostr_(dstr, neat_tostr_p(stringable_ptr))

#define neat_dstr_insert(dstr, any_str, idx) \
neat_dstr_insert_strv(dstr, neat_strv(any_str), idx)

#define neat_dstr_insert_tostr(dstr, stringable, idx) \
neat_dstr_insert_tostr_(dstr, neat_tostr(stringable), idx)

#define neat_dstr_insert_tostr_p(dstr, stringable_ptr, idx) \
neat_dstr_insert_tostr_(dstr, neat_tostr_p(stringable_ptr), idx)

#define neat_dstr_fread_line(dstr, stream) \
neat_dstr_fread_line_(dstr, stream)

#define neat_dstr_read_line(dstr) \
neat_dstr_fread_line_(dstr, stdin)

#define neat_dstr_append_fread_line(dstr, stream) \
neat_dstr_append_fread_line_(dstr, stream)

#define neat_dstr_append_read_line(dstr) \
neat_dstr_append_fread_line_(dstr, stdin)

#define neat_dstr_shrink_to_fit(dstr) \
neat_dstr_shrink_to_fit_(dstr)

#define neat_dstr_ensure_cap(dstr, new_cap) \
neat_dstr_ensure_cap_(dstr, new_cap)

#define neat_fprint(f, ...) \
do \
{ \
    FILE *neat_file_stream = f; \
    NEAT_FOREACH(neat_fprint_, __VA_ARGS__); \
} while(0)

#define neat_fprint_(x)                                                                                                                                              \
do                                                                                                                                                                   \
{                                                                                                                                                                    \
    Neat_DString neat_temp;                                                                                                                                          \
    _Generic(x,                                                                                                                                                      \
        char*                         : neat_fprint_strv(neat_file_stream, neat_strv_cstr2(neat_gurantee(x, char*), 0)),                                             \
        NEAT_UCHAR_CASE(unsigned char*: neat_fprint_strv(neat_file_stream, neat_strv_ucstr2(neat_gurantee(x, unsigned char*), 0)),)                                  \
        Neat_DString                  : neat_fprint_strv(neat_file_stream, neat_strv_dstr2(neat_gurantee(x, Neat_DString), 0)),                                      \
        Neat_DString*                 : neat_fprint_strv(neat_file_stream, neat_strv_dstr_ptr2(neat_gurantee(x, Neat_DString*), 0)),                                 \
        Neat_String_View              : neat_fprint_strv(neat_file_stream, neat_gurantee(x, Neat_String_View)),                                                      \
        Neat_String_View*             : neat_fprint_strv(neat_file_stream, *neat_gurantee(x, Neat_String_View*)),                                                    \
        Neat_String_Buffer            : neat_fprint_strv(neat_file_stream, neat_strv_strbuf2(neat_gurantee(x, Neat_String_Buffer), 0)),                              \
        Neat_String_Buffer*           : neat_fprint_strv(neat_file_stream, neat_strv_strbuf_ptr2(neat_gurantee(x, Neat_String_Buffer*), 0)),                         \
        Neat_SString_Ref              : neat_fprint_strv(neat_file_stream, neat_strv_sstr_ref2(neat_gurantee(x, Neat_SString_Ref), 0)),                              \
        Neat_Mut_String_Ref           : neat_fprint_strv(neat_file_stream, neat_strv_mutstr_ref2(neat_gurantee(x, Neat_Mut_String_Ref), 0)),                         \
        default                       : (neat_temp = neat_tostr(x), neat_fprint_strv(neat_file_stream, neat_strv_dstr2(neat_temp, 0)), neat_dstr_deinit(&neat_temp)) \
    );                                                                                                                                                               \
    (void) neat_temp;                                                                                                                                                \
} while(0);

#define neat_print(...) \
neat_fprint(stdout, __VA_ARGS__)

#define neat_fprintln(f, ...)    \
do                               \
{                                \
    neat_fprint(f, __VA_ARGS__); \
    fputc('\n', f);              \
} while(0)

#define neat_println(...) \
neat_fprintln(stdout, __VA_ARGS__)

#define NEAT_DEFAULT_TOSTR_TYPES                       \
bool                          : neat_tostr_bool,       \
char*                         : neat_tostr_cstr,        \
NEAT_UCHAR_CASE(unsigned char*: neat_tostr_ucstr,)      \
char                          : neat_tostr_char,       \
NEAT_SCHAR_CASE(signed char   : neat_tostr_schar,)     \
NEAT_UCHAR_CASE(unsigned char : neat_tostr_uchar,)     \
short                         : neat_tostr_short,      \
unsigned short                : neat_tostr_ushort,     \
int                           : neat_tostr_int,        \
unsigned int                  : neat_tostr_uint,       \
long                          : neat_tostr_long,       \
unsigned long                 : neat_tostr_ulong,      \
long long                     : neat_tostr_llong,      \
unsigned long long            : neat_tostr_ullong,     \
float                         : neat_tostr_float,      \
double                        : neat_tostr_double,     \
Neat_DString                  : neat_tostr_dstr,       \
Neat_DString*                 : neat_tostr_dstr_ptr,   \
Neat_String_View              : neat_tostr_strv,       \
Neat_String_View*             : neat_tostr_strv_ptr,   \
Neat_String_Buffer            : neat_tostr_strbuf,     \
Neat_String_Buffer*           : neat_tostr_strbuf_ptr, \
Neat_SString_Ref              : neat_tostr_sstr_ref,   \
Neat_Mut_String_Ref           : neat_tostr_mutstr_ref

#define NEAT_ALL_TOSTR_TYPES                                       \
NEAT_IF_DEF(NEAT_TOSTR1) (neat_tostr_type_1 : neat_tostr_func_1,)  \
NEAT_IF_DEF(NEAT_TOSTR2) (neat_tostr_type_2 : neat_tostr_func_2,)  \
NEAT_IF_DEF(NEAT_TOSTR3) (neat_tostr_type_3 : neat_tostr_func_3,)  \
NEAT_IF_DEF(NEAT_TOSTR4) (neat_tostr_type_4 : neat_tostr_func_4,)  \
NEAT_IF_DEF(NEAT_TOSTR5) (neat_tostr_type_5 : neat_tostr_func_5,)  \
NEAT_IF_DEF(NEAT_TOSTR6) (neat_tostr_type_6 : neat_tostr_func_6,)  \
NEAT_IF_DEF(NEAT_TOSTR7) (neat_tostr_type_7 : neat_tostr_func_7,)  \
NEAT_IF_DEF(NEAT_TOSTR8) (neat_tostr_type_8 : neat_tostr_func_8,)  \
NEAT_IF_DEF(NEAT_TOSTR9) (neat_tostr_type_9 : neat_tostr_func_9,)  \
NEAT_IF_DEF(NEAT_TOSTR10)(neat_tostr_type_10: neat_tostr_func_10,) \
NEAT_IF_DEF(NEAT_TOSTR11)(neat_tostr_type_11: neat_tostr_func_11,) \
NEAT_IF_DEF(NEAT_TOSTR12)(neat_tostr_type_12: neat_tostr_func_12,) \
NEAT_IF_DEF(NEAT_TOSTR13)(neat_tostr_type_13: neat_tostr_func_13,) \
NEAT_IF_DEF(NEAT_TOSTR14)(neat_tostr_type_14: neat_tostr_func_14,) \
NEAT_IF_DEF(NEAT_TOSTR15)(neat_tostr_type_15: neat_tostr_func_15,) \
NEAT_IF_DEF(NEAT_TOSTR16)(neat_tostr_type_16: neat_tostr_func_16,) \
NEAT_IF_DEF(NEAT_TOSTR17)(neat_tostr_type_17: neat_tostr_func_17,) \
NEAT_IF_DEF(NEAT_TOSTR18)(neat_tostr_type_18: neat_tostr_func_18,) \
NEAT_IF_DEF(NEAT_TOSTR19)(neat_tostr_type_19: neat_tostr_func_19,) \
NEAT_IF_DEF(NEAT_TOSTR20)(neat_tostr_type_20: neat_tostr_func_20,) \
NEAT_IF_DEF(NEAT_TOSTR21)(neat_tostr_type_21: neat_tostr_func_21,) \
NEAT_IF_DEF(NEAT_TOSTR22)(neat_tostr_type_22: neat_tostr_func_22,) \
NEAT_IF_DEF(NEAT_TOSTR23)(neat_tostr_type_23: neat_tostr_func_23,) \
NEAT_IF_DEF(NEAT_TOSTR24)(neat_tostr_type_24: neat_tostr_func_24,) \
NEAT_IF_DEF(NEAT_TOSTR25)(neat_tostr_type_25: neat_tostr_func_25,) \
NEAT_IF_DEF(NEAT_TOSTR26)(neat_tostr_type_26: neat_tostr_func_26,) \
NEAT_IF_DEF(NEAT_TOSTR27)(neat_tostr_type_27: neat_tostr_func_27,) \
NEAT_IF_DEF(NEAT_TOSTR28)(neat_tostr_type_28: neat_tostr_func_28,) \
NEAT_IF_DEF(NEAT_TOSTR29)(neat_tostr_type_29: neat_tostr_func_29,) \
NEAT_IF_DEF(NEAT_TOSTR30)(neat_tostr_type_30: neat_tostr_func_30,) \
NEAT_IF_DEF(NEAT_TOSTR31)(neat_tostr_type_31: neat_tostr_func_31,) \
NEAT_IF_DEF(NEAT_TOSTR32)(neat_tostr_type_32: neat_tostr_func_32,) \
NEAT_DEFAULT_TOSTR_TYPES

#define NEAT_DEFAULT_TOSTR_INTO_TYPES                       \
bool                          : neat_tostr_into_bool,       \
char*                         : neat_tostr_into_cstr,       \
NEAT_UCHAR_CASE(unsigned char*: neat_tostr_into_ucstr,)     \
char                          : neat_tostr_into_char,       \
NEAT_SCHAR_CASE(signed char   : neat_tostr_into_schar,)     \
NEAT_UCHAR_CASE(unsigned char : neat_tostr_into_uchar,)     \
short                         : neat_tostr_into_short,      \
unsigned short                : neat_tostr_into_ushort,     \
int                           : neat_tostr_into_int,        \
unsigned int                  : neat_tostr_into_uint,       \
long                          : neat_tostr_into_long,       \
unsigned long                 : neat_tostr_into_ulong,      \
long long                     : neat_tostr_into_llong,      \
unsigned long long            : neat_tostr_into_ullong,     \
float                         : neat_tostr_into_float,      \
double                        : neat_tostr_into_double,     \
Neat_DString                  : neat_tostr_into_dstr,       \
Neat_DString*                 : neat_tostr_into_dstr_ptr,   \
Neat_String_View              : neat_tostr_into_strv,       \
Neat_String_View*             : neat_tostr_into_strv_ptr,   \
Neat_String_Buffer            : neat_tostr_into_strbuf,     \
Neat_String_Buffer*           : neat_tostr_into_strbuf_ptr, \
Neat_SString_Ref              : neat_tostr_into_sstr_ref,   \
Neat_Mut_String_Ref           : neat_tostr_into_mutstr_ref  \

#define NEAT_ALL_TOSTR_INTO_TYPES                                                 \
NEAT_IF_DEF(NEAT_TOSTR_INTO1) (neat_tostr_into_type_1 : neat_tostr_into_func_1,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO2) (neat_tostr_into_type_2 : neat_tostr_into_func_2,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO3) (neat_tostr_into_type_3 : neat_tostr_into_func_3,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO4) (neat_tostr_into_type_4 : neat_tostr_into_func_4,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO5) (neat_tostr_into_type_5 : neat_tostr_into_func_5,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO6) (neat_tostr_into_type_6 : neat_tostr_into_func_6,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO7) (neat_tostr_into_type_7 : neat_tostr_into_func_7,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO8) (neat_tostr_into_type_8 : neat_tostr_into_func_8,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO9) (neat_tostr_into_type_9 : neat_tostr_into_func_9,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO10)(neat_tostr_into_type_10: neat_tostr_into_func_10,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO11)(neat_tostr_into_type_11: neat_tostr_into_func_11,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO12)(neat_tostr_into_type_12: neat_tostr_into_func_12,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO13)(neat_tostr_into_type_13: neat_tostr_into_func_13,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO14)(neat_tostr_into_type_14: neat_tostr_into_func_14,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO15)(neat_tostr_into_type_15: neat_tostr_into_func_15,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO16)(neat_tostr_into_type_16: neat_tostr_into_func_16,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO17)(neat_tostr_into_type_17: neat_tostr_into_func_17,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO18)(neat_tostr_into_type_18: neat_tostr_into_func_18,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO19)(neat_tostr_into_type_19: neat_tostr_into_func_19,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO20)(neat_tostr_into_type_20: neat_tostr_into_func_20,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO21)(neat_tostr_into_type_21: neat_tostr_into_func_21,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO22)(neat_tostr_into_type_22: neat_tostr_into_func_22,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO23)(neat_tostr_into_type_23: neat_tostr_into_func_23,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO24)(neat_tostr_into_type_24: neat_tostr_into_func_24,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO25)(neat_tostr_into_type_25: neat_tostr_into_func_25,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO26)(neat_tostr_into_type_26: neat_tostr_into_func_26,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO27)(neat_tostr_into_type_27: neat_tostr_into_func_27,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO28)(neat_tostr_into_type_28: neat_tostr_into_func_28,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO29)(neat_tostr_into_type_29: neat_tostr_into_func_29,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO30)(neat_tostr_into_type_30: neat_tostr_into_func_30,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO31)(neat_tostr_into_type_31: neat_tostr_into_func_31,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO32)(neat_tostr_into_type_32: neat_tostr_into_func_32,) \
NEAT_DEFAULT_TOSTR_INTO_TYPES

typedef neat_func_ptr(void, neat_fail_type*) neat_tostr_fail;

#define neat_get_tostr_func(ty) \
_Generic((ty){0}, \
    NEAT_ALL_TOSTR_TYPES \
)

#define neat_get_tostr_func_ft(ty) \
_Generic((ty){0}, \
    NEAT_ALL_TOSTR_TYPES, \
    default: (neat_tostr_fail){0} \
)

#define neat_tostr(x) \
neat_tostr_p(neat_as_pointer(x))

#define neat_tostr_p(xp) \
neat_get_tostr_func(typeof(*xp))(xp)

#define neat_has_tostr(ty) \
(!neat_has_type(neat_get_tostr_func_ft(ty), neat_tostr_fail))

#define neat_get_tostr_into_func(ty) \
_Generic((ty){0}, \
    NEAT_ALL_TOSTR_INTO_TYPES \
)

#define neat_get_tostr_into_func_ft(ty) \
_Generic((ty){0}, \
    NEAT_ALL_TOSTR_TYPES, \
    default: (neat_tostr_fail){0} \
)

#define neat_has_tostr_into(ty) \
(!neat_has_type(neat_get_tostr_into_func_ft(ty), neat_tostr_fail))

#define neat_tostr_into_dst_mutstr_ref(dst, x) \
neat_get_tostr_into_func(typeof(x))(dst, neat_as_pointer(x))

#define neat_tostr_into_p(dst, xp) \
( \
    neat_str_assert_mutable(dst), \
    neat_get_tostr_into_func(typeof(*xp))(neat_mutstr_ref(dst), xp) \
)

#define neat_tostr_into(dst, x) \
neat_tostr_into_p(dst, neat_as_pointer(x))

#define NEAT_DECL_TOSTR_FUNC(n) \
typedef typeof(NEAT_ARG1(ADD_TOSTR)) neat_tostr_type_##n; \
static inline Neat_DString neat_tostr_func_##n (neat_tostr_type_##n *obj) \
{ \
    _Static_assert(neat_has_type(NEAT_ARG2(ADD_TOSTR), Neat_DString(*)(neat_tostr_type_##n*)), "tostr function must have type DString (T*)"); \
    return NEAT_ARG2(ADD_TOSTR)(obj); \
}

#define NEAT_DECL_TOSTR_INTO_FUNC(n) \
typedef typeof(NEAT_ARG1(ADD_TOSTR_INTO)) neat_tostr_into_type_##n; \
static inline void neat_tostr_into_func_##n (Neat_Mut_String_Ref dst, neat_tostr_into_type_##n *obj) \
{ \
    _Static_assert(neat_has_type(NEAT_ARG2(ADD_TOSTR_INTO), typeof(void(*)(Neat_Mut_String_Ref, neat_tostr_into_type_##n*))), "tostr_into functions must have type void (T*)"); \
    if(dst.len != NULL) *dst.len = 0; \
    return NEAT_ARG2(ADD_TOSTR_INTO)(dst, obj); \
}

Neat_String_View neat_strv_cstr2(char *str, unsigned int begin);
Neat_String_View neat_strv_ucstr2(unsigned char *str, unsigned int begin);
Neat_String_View neat_strv_dstr2(Neat_DString str, unsigned int begin);
Neat_String_View neat_strv_dstr_ptr2(Neat_DString *str, unsigned int begin);
Neat_String_View neat_strv_strv2(Neat_String_View str, unsigned int begin);
Neat_String_View neat_strv_strv_ptr2(Neat_String_View *str, unsigned int begin);
Neat_String_View neat_strv_strbuf2(Neat_String_Buffer str, unsigned int begin);
Neat_String_View neat_strv_strbuf_ptr2(Neat_String_Buffer *str, unsigned int begin);
Neat_String_View neat_strv_sstr_ref2(Neat_SString_Ref str, unsigned int begin);
Neat_String_View neat_strv_mutstr_ref2(Neat_Mut_String_Ref str, unsigned int begin);

Neat_String_View neat_strv_cstr3(char *str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_ucstr3(unsigned char *str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_dstr3(Neat_DString str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_dstr_ptr3(Neat_DString *str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_strv3(Neat_String_View str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_strv_ptr3(Neat_String_View *str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_strbuf3(Neat_String_Buffer str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_strbuf_ptr3(Neat_String_Buffer *str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_sstr_ref3(Neat_SString_Ref str, unsigned int begin, unsigned int end);
Neat_String_View neat_strv_mutstr_ref3(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end);

Neat_String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator);
Neat_String_Buffer neat_strbuf_new_default(unsigned int cap);

Neat_String_Buffer neat_strbuf_from_ptr(void *ptr, unsigned int cap);
Neat_String_Buffer neat_strbuf_from_buf(Neat_Buffer buf);

Neat_Mut_String_Ref neat_mutstr_ref_to_cstr(char *str);
Neat_Mut_String_Ref neat_mutstr_ref_to_ucstr(unsigned char *str);
Neat_Mut_String_Ref neat_mutstr_ref_to_dstr_ptr(Neat_DString *str);
// Neat_Mut_String_Ref neat_mutstr_ref_to_strv_ptr(Neat_String_View *str);
Neat_Mut_String_Ref neat_mutstr_ref_to_strbuf_ptr(Neat_String_Buffer *str);
Neat_Mut_String_Ref neat_mutstr_ref_to_sstr_ref(Neat_SString_Ref str);
Neat_Mut_String_Ref neat_mutstr_ref_to_mutstr_ref(Neat_Mut_String_Ref str);

char *neat_cstr_as_cstr(char *str);
char *neat_ucstr_as_cstr(unsigned char *str);
char *neat_dstr_as_cstr(Neat_DString str);
char *neat_dstr_ptr_as_cstr(Neat_DString *str);
char *neat_strv_as_cstr(Neat_String_View str);
char *neat_strv_ptr_as_cstr(Neat_String_View *str);
char *neat_strbuf_as_cstr(Neat_String_Buffer str);
char *neat_strbuf_ptr_as_cstr(Neat_String_Buffer *str);
char *neat_sstr_ref_as_cstr(Neat_SString_Ref str);
char *neat_mutstr_ref_as_cstr(Neat_Mut_String_Ref str);

unsigned char neat_cstr_char_at(char *str, unsigned int idx);
unsigned char neat_ucstr_char_at(unsigned char *str, unsigned int idx);
unsigned char neat_dstr_char_at(Neat_DString str, unsigned int idx);
unsigned char neat_dstr_ptr_char_at(Neat_DString *str, unsigned int idx);
unsigned char neat_strv_char_at(Neat_String_View str, unsigned int idx);
unsigned char neat_strv_ptr_char_at(Neat_String_View *str, unsigned int idx);
unsigned char neat_strbuf_char_at(Neat_String_Buffer str, unsigned int idx);
unsigned char neat_strbuf_ptr_char_at(Neat_String_Buffer *str, unsigned int idx);
unsigned char neat_sstr_ref_char_at(Neat_SString_Ref str, unsigned int idx);
unsigned char neat_mutstr_ref_char_at(Neat_Mut_String_Ref str, unsigned int idx);

bool neat_is_strv_intersect(Neat_String_View base, Neat_String_View sub);

NEAT_NODISCARD("discarding a new DString will cause memory leak") Neat_DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator);
void neat_dstr_deinit_(Neat_DString *dstr);
void neat_dstr_append_strv(Neat_DString *dstr, Neat_String_View str);
void neat_dstr_prepend_strv(Neat_DString *dstr, Neat_String_View str);
NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_strv(Neat_DString *dstr, Neat_String_View str, unsigned int idx);
void neat_dstr_append_tostr_(Neat_DString *dstr, Neat_DString tostr);
void neat_dstr_prepend_tostr_(Neat_DString *dstr, Neat_DString tostr);
NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_tostr_(Neat_DString *dstr, Neat_DString tostr, unsigned int idx);
unsigned int neat_dstr_fread_line_(Neat_DString *dstr, FILE *stream);
unsigned int neat_dstr_append_fread_line_(Neat_DString *dstr, FILE *stream);
void neat_dstr_shrink_to_fit_(Neat_DString *dstr);
void neat_dstr_ensure_cap_(Neat_DString *dstr, unsigned int at_least);

unsigned int neat_mutstr_ref_copy(Neat_Mut_String_Ref dst, Neat_String_View src);
unsigned int neat_mutstr_ref_concat(Neat_Mut_String_Ref dst, Neat_String_View src);
NEAT_NODISCARD("str_del returns true on success, false on failure") bool neat_mutstr_ref_delete_range(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end);
unsigned int neat_mutstr_ref_insert_strv(Neat_Mut_String_Ref dst, Neat_String_View src, unsigned int idx);
unsigned int neat_mutstr_ref_replace(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement);
bool neat_mutstr_ref_replace_first(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement);
Neat_DString neat_tostr_all_into_new_dstr(Neat_Allocator allocator, unsigned int nb, ...);

NEAT_NODISCARD("str_split returns new String_View_Array") Neat_String_View_Array neat_strv_split(Neat_String_View delim, Neat_String_View str, Neat_Allocator allocator);
NEAT_NODISCARD("str_join_new returns new DString, discarding will cause memory leak") Neat_DString neat_strv_arr_join_new(Neat_String_View delim, Neat_String_View_Array strs, Neat_Allocator allocator);
unsigned int neat_strv_arr_join(Neat_Mut_String_Ref dst, Neat_String_View delim, Neat_String_View_Array strs);

Neat_String_View_Array neat_strv_arr_from_carr(Neat_String_View *carr, unsigned int nb);

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2);
Neat_String_View neat_strv_find(Neat_String_View hay, Neat_String_View needle);
unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle);
bool neat_strv_starts_with(Neat_String_View hay, Neat_String_View needle);
bool neat_strv_ends_with(Neat_String_View hay, Neat_String_View needle);

unsigned int neat_mutstr_ref_fread_line(Neat_Mut_String_Ref dst, FILE *stream);
unsigned int neat_mutstr_ref_concat_fread_line(Neat_Mut_String_Ref dst, FILE *stream);

unsigned int neat_fprint_strv(FILE *stream, Neat_String_View str);
unsigned int neat_fprintln_strv(FILE *stream, Neat_String_View str);

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_bool(bool *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_cstr(char **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ucstr(unsigned char **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_char(char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_schar(signed char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_uchar(unsigned char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_short(short *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ushort(unsigned short *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_int(int *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_uint(unsigned int *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_long(long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ulong(unsigned long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_llong(long long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ullong(unsigned long long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_float(float *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_double(double *obj);

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_dstr(Neat_DString *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_dstr_ptr(Neat_DString **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strv(Neat_String_View *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strv_ptr(Neat_String_View **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strbuf(Neat_String_Buffer *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strbuf_ptr(Neat_String_Buffer **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_sstr_ref(Neat_SString_Ref *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_mutstr_ref(Neat_Mut_String_Ref *obj);

void neat_tostr_into_bool(Neat_Mut_String_Ref dst, bool *obj);
void neat_tostr_into_cstr(Neat_Mut_String_Ref dst, char **obj);
void neat_tostr_into_ucstr(Neat_Mut_String_Ref dst, unsigned char **obj);
void neat_tostr_into_char(Neat_Mut_String_Ref dst, char *obj);
void neat_tostr_into_schar(Neat_Mut_String_Ref dst, signed char *obj);
void neat_tostr_into_uchar(Neat_Mut_String_Ref dst, unsigned char *obj);
void neat_tostr_into_short(Neat_Mut_String_Ref dst, short *obj);
void neat_tostr_into_ushort(Neat_Mut_String_Ref dst, unsigned short *obj);
void neat_tostr_into_int(Neat_Mut_String_Ref dst, int *obj);
void neat_tostr_into_uint(Neat_Mut_String_Ref dst, unsigned int *obj);
void neat_tostr_into_long(Neat_Mut_String_Ref dst, long *obj);
void neat_tostr_into_ulong(Neat_Mut_String_Ref dst, unsigned long *obj);
void neat_tostr_into_llong(Neat_Mut_String_Ref dst, long long *obj);
void neat_tostr_into_ullong(Neat_Mut_String_Ref dst, unsigned long long *obj);
void neat_tostr_into_float(Neat_Mut_String_Ref dst, float *obj);
void neat_tostr_into_double(Neat_Mut_String_Ref dst, double *obj);

void neat_tostr_into_dstr(Neat_Mut_String_Ref dst, Neat_DString *obj);
void neat_tostr_into_dstr_ptr(Neat_Mut_String_Ref dst, Neat_DString **obj);
void neat_tostr_into_strv(Neat_Mut_String_Ref dst, Neat_String_View *obj);
void neat_tostr_into_strv_ptr(Neat_Mut_String_Ref dst, Neat_String_View **obj);
void neat_tostr_into_strbuf(Neat_Mut_String_Ref dst, Neat_String_Buffer *obj);
void neat_tostr_into_strbuf_ptr(Neat_Mut_String_Ref dst, Neat_String_Buffer **obj);
void neat_tostr_into_sstr_ref(Neat_Mut_String_Ref dst, Neat_SString_Ref *obj);
void neat_tostr_into_mutstr_ref(Neat_Mut_String_Ref dst, Neat_Mut_String_Ref *obj);

#endif /* NEAT_STR_H */

#ifndef NEAT_STR_PREFIX

typedef Neat_Allocator Allocator;
typedef Neat_DString DString;
typedef Neat_String_Buffer String_Buffer;
typedef Neat_String_View String_View;
typedef Neat_String_View_Array String_View_Array;
typedef Neat_SString_Ref SString_Ref;
typedef Neat_Mut_String_Ref Mut_String_Ref;
#define SString(N) Neat_SString(N)

#define str_at(any_str, idx) neat_str_at(any_str, idx)
#define str_len(any_str) neat_str_len(any_str)
#define str_cap(any_str) neat_str_cap(any_str)
#define str_equal(any_str1, any_str2) neat_str_equal(any_str1, any_str2)
#define str_cstr(any_str) neat_str_cstr(any_str)
#define str_find(any_str_hay, any_str_needle) neat_str_find(any_str_hay, any_str_needle)
#define str_count(any_str_hay, any_str_needle) neat_str_count(any_str_hay, any_str_needle)
#define str_starts_with(any_str_hay, any_str_needle) neat_str_starts_with(any_str_hay, any_str_needle)
#define str_ends_with(any_str_hay, any_str_needle) neat_str_ends_with(any_str_hay, any_str_needle)
#define str_copy(any_str_dst, any_str_src) neat_str_copy(any_str_dst, any_str_src)
#define str_concat(cap_str_dst, any_str_src) neat_str_concat(cap_str_dst, any_str_src)
#define str_insert(any_str_dst, any_str_src, idx) neat_str_insert(any_str_dst, any_str_src, idx)
#define str_prepend(neat_str_dst, neat_str_src) neat_str_prepend(neat_str_dst, neat_str_src)
#define str_del(any_str, begin, end) neat_str_del(any_str, begin, end)
#define str_replace(mut_str, any_str_target, any_str_replacement) neat_str_replace(mut_str, any_str_target, any_str_replacement)
#define str_replace_first(any_str, any_str_target, any_str_replacement) neat_str_replace_first(any_str, any_str_target, any_str_replacement)
#define str_split(any_str_delim, any_str, ...) neat_str_split(any_str_delim, any_str __VA_OPT__(,) __VA_ARGS__)
#define str_join(mut_str_dst, any_str_delim, strv_arr) neat_str_join(mut_str_dst, any_str_delim, strv_arr)
#define str_join_new(any_str_delim, strv_arr, ...) neat_str_join_new(any_str_delim, strv_arr __VA_OPT__(,) __VA_ARGS__)
#define str_fread_line(any_str, stream) neat_str_fread_line(any_str, stream)
#define str_concat_fread_line(stream, any_str) neat_str_concat_fread_line(stream, any_str)
#define str_read_line(any_str) neat_str_read_line(any_str)
#define str_read_line_new(...) neat_str_read_line_new(__VA_ARGS__)
#define str_fread_line_new(stream, ...) neat_str_fread_line_new(stream __VA_OPT__(,) __VA_ARGS__)
#define str_concat_read_line(any_str) neat_str_concat_read_line(any_str)
#define str_print(any_str, ...) neat_str_print(any_str, __VA_ARGS__)
#define str_print_new(...) neat_str_print_new(__VA_ARGS__)

#define dstr(...) neat_dstr(__VA_ARGS__)
#define dstr_deinit(dstr) neat_dstr_deinit(dstr);

#define dstr_append(dstr, any_str) neat_dstr_append(dstr, any_str)
#define dstr_append_tostr(dstr, stringable) neat_dstr_append_tostr(dstr, stringable)
#define dstr_append_tostr_p(dstr, stringable_ptr) neat_dstr_append_tostr_p(dstr, stringable_ptr)
#define dstr_prepend(dstr, any_str) neat_dstr_prepend(dstr, any_str)
#define dstr_prepend_tostr(dstr, stringable) neat_dstr_prepend_tostr(dstr, stringable)
#define dstr_prepend_tostr_p(dstr, stringable_ptr) neat_dstr_prepend_tostr_p(dstr, stringable_ptr)
#define dstr_insert(dstr, any_str, idx) neat_dstr_insert(dstr, any_str, idx)
#define dstr_insert_tostr(dstr, stringable, idx) neat_dstr_insert_tostr(dstr, stringable, idx)
#define dstr_insert_tostr_p(dstr, stringable_ptr, idx) neat_dstr_insert_tostr_p(dstr, stringable_ptr, idx)
#define dstr_fread_line(dstr, stream) neat_dstr_fread_line(dstr, stream)
#define dstr_read_line(dstr) neat_dstr_read_line(dstr)
#define dstr_append_fread_line(dstr, stream) neat_dstr_append_fread_line(dstr, stream)
#define dstr_append_read_line(dstr) neat_dstr_append_read_line(dstr)
#define dstr_shrink_to_fit(dstr) neat_dstr_shrink_to_fit(dstr)
#define dstr_ensure_cap(dstr, new_cap) neat_dstr_ensure_cap(dstr, new_cap)

#define strbuf(str_or_cap, ...) neat_strbuf(str_or_cap __VA_OPT__(,) __VA_ARGS__)
#define sstr_ref(sstr_ptr) neat_sstr_ref(sstr_ptr)
#define strv(...) neat_strv(__VA_ARGS__)
#define mutstr_ref(any_str, ...) neat_mutstr_ref(any_str __VA_OPT__(,) __VA_ARGS__)
#define strv_arr(...) neat_strv_arr(__VA_ARGS__)
#define strv_arr_carr(strv_carr, ...) neat_strv_arr_carr(strv_carr __VA_OPT__(,) __VA_ARGS__)

#define tostr(x) neat_tostr(x)
#define tostr_p(x) neat_tostr_p(x)
#define tostr_into(any_str, x) neat_tostr_into(any_str, x)
#define tostr_into_p(any_str, x) neat_tostr_into_p(any_str, x)

#define print(...) neat_print(__VA_ARGS__)
#define println(...) neat_println(__VA_ARGS__)
#define fprint(stream, ...) neat_fprint(stream, __VA_ARGS__)
#define fprintln(stream, ...) neat_fprintln(stream, __VA_ARGS__)

#endif

#if defined(ADD_TOSTR)

#if !defined(NEAT_TOSTR1)
#define NEAT_TOSTR1
NEAT_DECL_TOSTR_FUNC(1)
#elif !defined(NEAT_TOSTR2)
#define NEAT_TOSTR2
NEAT_DECL_TOSTR_FUNC(2)
#elif !defined(NEAT_TOSTR3)
#define NEAT_TOSTR3
NEAT_DECL_TOSTR_FUNC(3)
#elif !defined(NEAT_TOSTR4)
#define NEAT_TOSTR4
NEAT_DECL_TOSTR_FUNC(4)
#elif !defined(NEAT_TOSTR5)
#define NEAT_TOSTR5
NEAT_DECL_TOSTR_FUNC(5)
#elif !defined(NEAT_TOSTR6)
#define NEAT_TOSTR6
NEAT_DECL_TOSTR_FUNC(6)
#elif !defined(NEAT_TOSTR7)
#define NEAT_TOSTR7
NEAT_DECL_TOSTR_FUNC(7)
#elif !defined(NEAT_TOSTR8)
#define NEAT_TOSTR8
NEAT_DECL_TOSTR_FUNC(8)
#elif !defined(NEAT_TOSTR9)
#define NEAT_TOSTR9
NEAT_DECL_TOSTR_FUNC(9)
#elif !defined(NEAT_TOSTR10)
#define NEAT_TOSTR10
NEAT_DECL_TOSTR_FUNC(10)
#elif !defined(NEAT_TOSTR11)
#define NEAT_TOSTR11
NEAT_DECL_TOSTR_FUNC(11)
#elif !defined(NEAT_TOSTR12)
#define NEAT_TOSTR12
NEAT_DECL_TOSTR_FUNC(12)
#elif !defined(NEAT_TOSTR13)
#define NEAT_TOSTR13
NEAT_DECL_TOSTR_FUNC(13)
#elif !defined(NEAT_TOSTR14)
#define NEAT_TOSTR14
NEAT_DECL_TOSTR_FUNC(14)
#elif !defined(NEAT_TOSTR15)
#define NEAT_TOSTR15
NEAT_DECL_TOSTR_FUNC(15)
#elif !defined(NEAT_TOSTR16)
#define NEAT_TOSTR16
NEAT_DECL_TOSTR_FUNC(16)
#elif !defined(NEAT_TOSTR17)
#define NEAT_TOSTR17
NEAT_DECL_TOSTR_FUNC(17)
#elif !defined(NEAT_TOSTR18)
#define NEAT_TOSTR18
NEAT_DECL_TOSTR_FUNC(18)
#elif !defined(NEAT_TOSTR19)
#define NEAT_TOSTR19
NEAT_DECL_TOSTR_FUNC(19)
#elif !defined(NEAT_TOSTR20)
#define NEAT_TOSTR20
NEAT_DECL_TOSTR_FUNC(20)
#elif !defined(NEAT_TOSTR21)
#define NEAT_TOSTR21
NEAT_DECL_TOSTR_FUNC(21)
#elif !defined(NEAT_TOSTR22)
#define NEAT_TOSTR22
NEAT_DECL_TOSTR_FUNC(22)
#elif !defined(NEAT_TOSTR23)
#define NEAT_TOSTR23
NEAT_DECL_TOSTR_FUNC(23)
#elif !defined(NEAT_TOSTR24)
#define NEAT_TOSTR24
NEAT_DECL_TOSTR_FUNC(24)
#elif !defined(NEAT_TOSTR25)
#define NEAT_TOSTR25
NEAT_DECL_TOSTR_FUNC(25)
#elif !defined(NEAT_TOSTR26)
#define NEAT_TOSTR26
NEAT_DECL_TOSTR_FUNC(26)
#elif !defined(NEAT_TOSTR27)
#define NEAT_TOSTR27
NEAT_DECL_TOSTR_FUNC(27)
#elif !defined(NEAT_TOSTR28)
#define NEAT_TOSTR28
NEAT_DECL_TOSTR_FUNC(28)
#elif !defined(NEAT_TOSTR29)
#define NEAT_TOSTR29
NEAT_DECL_TOSTR_FUNC(29)
#elif !defined(NEAT_TOSTR30)
#define NEAT_TOSTR30
NEAT_DECL_TOSTR_FUNC(30)
#elif !defined(NEAT_TOSTR31)
#define NEAT_TOSTR31
NEAT_DECL_TOSTR_FUNC(31)
#elif !defined(NEAT_TOSTR32)
#define NEAT_TOSTR32
NEAT_DECL_TOSTR_FUNC(32)
#else
#error "Maximum number of tostr functions is 32"
#endif

#undef ADD_TOSTR

#endif

#if defined(ADD_TOSTR_INTO)

#if !defined(NEAT_TOSTR_INTO1)
#define NEAT_TOSTR_INTO1
NEAT_DECL_TOSTR_INTO_FUNC(1)
#elif !defined(NEAT_TOSTR_INTO2)
#define NEAT_TOSTR_INTO2
NEAT_DECL_TOSTR_INTO_FUNC(2)
#elif !defined(NEAT_TOSTR_INTO3)
#define NEAT_TOSTR_INTO3
NEAT_DECL_TOSTR_INTO_FUNC(3)
#elif !defined(NEAT_TOSTR_INTO4)
#define NEAT_TOSTR_INTO4
NEAT_DECL_TOSTR_INTO_FUNC(4)
#elif !defined(NEAT_TOSTR_INTO5)
#define NEAT_TOSTR_INTO5
NEAT_DECL_TOSTR_INTO_FUNC(5)
#elif !defined(NEAT_TOSTR_INTO6)
#define NEAT_TOSTR_INTO6
NEAT_DECL_TOSTR_INTO_FUNC(6)
#elif !defined(NEAT_TOSTR_INTO7)
#define NEAT_TOSTR_INTO7
NEAT_DECL_TOSTR_INTO_FUNC(7)
#elif !defined(NEAT_TOSTR_INTO8)
#define NEAT_TOSTR_INTO8
NEAT_DECL_TOSTR_INTO_FUNC(8)
#elif !defined(NEAT_TOSTR_INTO9)
#define NEAT_TOSTR_INTO9
NEAT_DECL_TOSTR_INTO_FUNC(9)
#elif !defined(NEAT_TOSTR_INTO10)
#define NEAT_TOSTR_INTO10
NEAT_DECL_TOSTR_INTO_FUNC(10)
#elif !defined(NEAT_TOSTR_INTO11)
#define NEAT_TOSTR_INTO11
NEAT_DECL_TOSTR_INTO_FUNC(11)
#elif !defined(NEAT_TOSTR_INTO12)
#define NEAT_TOSTR_INTO12
NEAT_DECL_TOSTR_INTO_FUNC(12)
#elif !defined(NEAT_TOSTR_INTO13)
#define NEAT_TOSTR_INTO13
NEAT_DECL_TOSTR_INTO_FUNC(13)
#elif !defined(NEAT_TOSTR_INTO14)
#define NEAT_TOSTR_INTO14
NEAT_DECL_TOSTR_INTO_FUNC(14)
#elif !defined(NEAT_TOSTR_INTO15)
#define NEAT_TOSTR_INTO15
NEAT_DECL_TOSTR_INTO_FUNC(15)
#elif !defined(NEAT_TOSTR_INTO16)
#define NEAT_TOSTR_INTO16
NEAT_DECL_TOSTR_INTO_FUNC(16)
#elif !defined(NEAT_TOSTR_INTO17)
#define NEAT_TOSTR_INTO17
NEAT_DECL_TOSTR_INTO_FUNC(17)
#elif !defined(NEAT_TOSTR_INTO18)
#define NEAT_TOSTR_INTO18
NEAT_DECL_TOSTR_INTO_FUNC(18)
#elif !defined(NEAT_TOSTR_INTO19)
#define NEAT_TOSTR_INTO19
NEAT_DECL_TOSTR_INTO_FUNC(19)
#elif !defined(NEAT_TOSTR_INTO20)
#define NEAT_TOSTR_INTO20
NEAT_DECL_TOSTR_INTO_FUNC(20)
#elif !defined(NEAT_TOSTR_INTO21)
#define NEAT_TOSTR_INTO21
NEAT_DECL_TOSTR_INTO_FUNC(21)
#elif !defined(NEAT_TOSTR_INTO22)
#define NEAT_TOSTR_INTO22
NEAT_DECL_TOSTR_INTO_FUNC(22)
#elif !defined(NEAT_TOSTR_INTO23)
#define NEAT_TOSTR_INTO23
NEAT_DECL_TOSTR_INTO_FUNC(23)
#elif !defined(NEAT_TOSTR_INTO24)
#define NEAT_TOSTR_INTO24
NEAT_DECL_TOSTR_INTO_FUNC(24)
#elif !defined(NEAT_TOSTR_INTO25)
#define NEAT_TOSTR_INTO25
NEAT_DECL_TOSTR_INTO_FUNC(25)
#elif !defined(NEAT_TOSTR_INTO26)
#define NEAT_TOSTR_INTO26
NEAT_DECL_TOSTR_INTO_FUNC(26)
#elif !defined(NEAT_TOSTR_INTO27)
#define NEAT_TOSTR_INTO27
NEAT_DECL_TOSTR_INTO_FUNC(27)
#elif !defined(NEAT_TOSTR_INTO28)
#define NEAT_TOSTR_INTO28
NEAT_DECL_TOSTR_INTO_FUNC(28)
#elif !defined(NEAT_TOSTR_INTO29)
#define NEAT_TOSTR_INTO29
NEAT_DECL_TOSTR_INTO_FUNC(29)
#elif !defined(NEAT_TOSTR_INTO30)
#define NEAT_TOSTR_INTO30
NEAT_DECL_TOSTR_INTO_FUNC(30)
#elif !defined(NEAT_TOSTR_INTO31)
#define NEAT_TOSTR_INTO31
NEAT_DECL_TOSTR_INTO_FUNC(31)
#elif !defined(NEAT_TOSTR_INTO32)
#define NEAT_TOSTR_INTO32
NEAT_DECL_TOSTR_INTO_FUNC(32)
#else
#error "Maximum number of tostr_into functions is 32"
#endif

#undef ADD_TOSTR_INTO

#endif
