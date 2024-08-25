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
#include <ctype.h>

#define neat_func_ptr(ret, ...) \
typeof(typeof(ret)(*)(__VA_ARGS__))

typedef neat_func_ptr(void*, void *ctx, size_t alignment, size_t n, size_t *actual) neat_alloc_func;
typedef neat_func_ptr(void,  void *ctx, void *ptr, size_t n) neat_dealloc_func;
typedef neat_func_ptr(void*, void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual) neat_realloc_func;

typedef neat_func_ptr(void,  void **ctx, void *arg) neat_allocator_init_func;
typedef neat_func_ptr(void,  void *ctx)  neat_allocator_deinit_func;

typedef struct Neat_Allocator
{
    void *ctx;
    neat_alloc_func alloc;
    neat_dealloc_func dealloc;
    neat_realloc_func realloc;
    neat_allocator_init_func init;
    neat_allocator_deinit_func deinit;
} Neat_Allocator;

void *neat_default_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual);
void neat_default_allocator_dealloc(void *ctx, void *ptr, size_t n);
void *neat_default_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual);
void neat_default_allocator_init(void **ctx, void *arg);
void neat_default_allocator_deinit(void *ctx);

void *neat_noop_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual);
void neat_noop_allocator_dealloc(void *ctx, void *ptr, size_t n);
void *neat_noop_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual);
void neat_noop_allocator_init(void **ctx, void *arg);
void neat_noop_allocator_deinit(void *ctx);

void *neat_allocator_invoke_alloc(Neat_Allocator allocator, size_t alignment, size_t obj_size, size_t nb, size_t *actual);
void neat_allocator_invoke_dealloc(Neat_Allocator allocator, void *ptr, size_t obj_size, size_t nb);
void *neat_allocator_invoke_realloc(Neat_Allocator allocator, void *ptr, size_t alignment, size_t obj_size, size_t old_nb, size_t new_nb, size_t *actual);

Neat_Allocator neat_get_default_allocator();
Neat_Allocator neat_get_noop_allocator();

#define neat_alloc(allocator, T, n, actual) \
(typeof(T)*) neat_allocator_invoke_alloc(allocator, _Alignof(T), sizeof(T), n, actual)

#define neat_dealloc(allocator, ptr, T, n) \
neat_allocator_invoke_dealloc(allocator, ptr, sizeof(T), n)

#define neat_realloc(allocator, ptr, T, old_n, new_n, actual) \
(typeof(T)*) neat_allocator_invoke_realloc(allocator, ptr, _Alignof(T), sizeof(T), old_n, new_n, actual)

#define neat_alloc_bytes(allocator, n, actual) \
neat_allocator_invoke_alloc(allocator, _Alignof(max_align_t), 1, n, actual)

#define neat_dealloc_bytes(allocator, ptr, n) \
neat_allocator_invoke_dealloc(allocator, ptr, 1, n)

#define neat_realloc_bytes(allocator, ptr, old_n, new_n, actual) \
neat_allocator_invoke_realloc(allocator, ptr, _Alignof(max_align_t), 1, old_n, new_n, actual)

// below is needed for MSVC because (for some reason) 'signed char' and 'char' are type aliases in MSVC
#ifdef _MSC_VER

#if _CHAR_UNSIGNED

    #define NEAT_UCHAR_CASE(...)
    #define NEAT_SCHAR_CASE(...) __VA_ARGS__

#else

    #define NEAT_UCHAR_CASE(...) __VA_ARGS__
    #define NEAT_SCHAR_CASE(...)

#endif

#else

    #define NEAT_UCHAR_CASE(...) __VA_ARGS__
    #define NEAT_SCHAR_CASE(...) __VA_ARGS__

#endif

#if defined(_MSC_VER)

    #define NEAT_NODISCARD(...) _Check_return_

    #elif __STDC_VERSION__ >= 202311L

    #define NEAT_NODISCARD(...) [[nodiscard __VA_ARGS__]]

#elif defined(__GNUC__)

    #define NEAT_NODISCARD(...) __attribute__((warn_unused_result))

#else

    #define NEAT_NODISCARD(...)

#endif

#define neat_static_assertx(exp, msg) \
((void)sizeof(struct { _Static_assert(exp, msg); int dummy; }))

#define neat_has_type(exp, t) \
_Generic(exp, t: 1, default: 0)

#define neat_is_array_of(exp, ty) \
neat_has_type((typeof(exp)*){0}, typeof(ty)(*)[sizeof(exp)/sizeof(ty)])

#define NEAT_CAT(a, ...) NEAT_PRIMITIVE_CAT(a, __VA_ARGS__)
#define NEAT_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define neat_gurantee(exp, t) \
neat_gurantee_fallback(exp, t, (t){0})

#define neat_gurantee_fallback(exp, ty, fallback) \
_Generic(exp, \
    ty: exp, \
    default: fallback \
)

#define neat_gurantee_not(exp, not_ty, fallback_ty) \
_Generic(exp, \
    not_ty: (fallback_ty){0}, \
    default: exp \
)

#define neat_as_pointer(scalar) \
(&(typeof(((void)0, scalar))[]){scalar}[0])

#define NEAT_CARR_LEN(carr) (sizeof(carr) / sizeof(carr[0]))

// IF_DEF and ARG_n stuff
#define NEAT_COMMA()              ,
#define NEAT_ARG1_( _1, ... )     _1
#define NEAT_ARG1( ... )          NEAT_ARG1_( __VA_ARGS__ )
#define NEAT_ARG2_( _1, _2, ... ) _2
#define NEAT_ARG2( ... )          NEAT_ARG2_( __VA_ARGS__ )
#define NEAT_INCL( ... )          __VA_ARGS__
#define NEAT_OMIT( ... )
#define NEAT_OMIT1(a, ...)        __VA_ARGS__
#define NEAT_IF_DEF( macro )      NEAT_ARG2( NEAT_COMMA macro () NEAT_INCL, NEAT_OMIT, )
// IF_DEF and ARG_n stuff end

// FOREACH stuff
#define NEAT_PARENS ()

#define NEAT_EXPAND(...)  NEAT_EXPAND4(NEAT_EXPAND4(NEAT_EXPAND4(NEAT_EXPAND4(__VA_ARGS__))))
#define NEAT_EXPAND4(...) NEAT_EXPAND3(NEAT_EXPAND3(NEAT_EXPAND3(NEAT_EXPAND3(__VA_ARGS__))))
#define NEAT_EXPAND3(...) NEAT_EXPAND2(NEAT_EXPAND2(NEAT_EXPAND2(NEAT_EXPAND2(__VA_ARGS__))))
#define NEAT_EXPAND2(...) NEAT_EXPAND1(NEAT_EXPAND1(NEAT_EXPAND1(NEAT_EXPAND1(__VA_ARGS__))))
#define NEAT_EXPAND1(...) __VA_ARGS__

#define NEAT_FOREACH(macro, ...)                                    \
__VA_OPT__(NEAT_EXPAND(NEAT_FOREACH_HELPER(macro, __VA_ARGS__)))
#define NEAT_FOREACH_HELPER(macro, a1, ...)                         \
macro(a1)                                                     \
__VA_OPT__(NEAT_FOREACH_REPEAT NEAT_PARENS (macro, __VA_ARGS__))
#define NEAT_FOREACH_REPEAT() NEAT_FOREACH_HELPER
// FOREACH stuff end

#define NEAT_VA_OR(otherwise, ...) \
__VA_ARGS__ NEAT_IF_EMPTY(otherwise, __VA_ARGS__)

#define NEAT_IF_EMPTY(then, ...) \
NEAT_CAT(NEAT_IF_EMPTY_, __VA_OPT__(0))(then)

#define NEAT_IF_EMPTY_(then) then
#define NEAT_IF_EMPTY_0(then)

#define NEAT_IF_NEMPTY(then, ...) __VA_OPT__(then)

// Dynamic String
typedef struct Neat_DString
{
    Neat_Allocator allocator;
    unsigned int cap;
    unsigned int len;
    unsigned char *chars;
} Neat_DString;

// Used as a general purpose non-dynamic string buffer
typedef struct Neat_String_Buffer
{
    unsigned int cap;
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
    unsigned int len;
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

// Can be used as a reference to any mutable string type
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
        unsigned char chars[ cap ]; \
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
        unsigned char chars[ cap ]; \
    } Neat_SString_##cap

    #define NEAT_DECL_SSTRING(cap) \
    NEAT_DECL_SSTRING_(cap)
#endif

#define NEAT_SSTRING_NTAG(cap) \
struct \
{ \
    unsigned int len; \
    unsigned char chars[ cap ]; \
}

#define NEAT_TYPEOF_SSTR(sstr) \
NEAT_SSTRING_NTAG(sizeof((sstr)->chars))

#define NEAT_IS_SSTRING_PTR(s) \
(sizeof(*(s)) == sizeof(NEAT_TYPEOF_SSTR(s))) && \
_Alignof(typeof(*(s))) == _Alignof(NEAT_TYPEOF_SSTR(s)) && \
neat_has_type(&(s)->chars, typeof(&(NEAT_TYPEOF_SSTR(s)){0}.chars))

#define neat_str_assert_appendable(str)                        \
(void)_Generic((typeof(str)*){0},                              \
    char(*)[sizeof(typeof(str))]: 1,                           \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(str))]: 1,) \
    Neat_DString**      : 1,                                   \
    Neat_String_Buffer**: 1,                                   \
    Neat_SString_Ref*   : 1,                                   \
    Neat_Mut_String_Ref*: 1                                    \
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
((void) 0, neat_strv(any_str).len)

#define neat_str_cap(any_str)                                                                                                                                   \
_Generic((typeof(any_str)*){0},                                                                                                                                 \
    char(*)[sizeof(typeof(any_str))] : neat_buf_cap,                                                                                                            \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(any_str))]: neat_buf_cap,)                                                                                   \
    Neat_DString*                    : neat_dstr_cap,                                                                                                           \
    Neat_DString**                   : neat_dstr_ptr_cap,                                                                                                       \
    Neat_String_Buffer*              : neat_strbuf_cap,                                                                                                         \
    Neat_String_Buffer**             : neat_strbuf_ptr_cap,                                                                                                     \
    Neat_SString_Ref*                : neat_sstr_ref_cap,                                                                                                       \
    Neat_Mut_String_Ref*             : neat_mutstr_ref_cap                                                                                                      \
)(_Generic((typeof(any_str)*){0},                                                                                                                               \
    char(*)[sizeof(typeof(any_str))]: (Neat_Buffer){.ptr = neat_gurantee(any_str, char*), .size = sizeof(typeof(any_str))},                                     \
    NEAT_UCHAR_CASE(unsigned char(*)[sizeof(typeof(any_str))]: (Neat_Buffer){.ptr = neat_gurantee(any_str, unsigned char*), .size = sizeof(typeof(any_str))},)  \
    default: any_str                                                                                                                                            \
)                                                                                                                                                               \
)

#define neat_str_cstr(any_str)                               \
_Generic(any_str,                                            \
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
neat_mutstr_ref_copy(neat_mutstr_ref(any_str_dst), neat_strv(any_str_src))

#define neat_str_concat(any_str_dst, any_str_src)                            \
(                                                                            \
neat_str_assert_appendable(any_str_dst),                                     \
neat_mutstr_ref_concat(neat_mutstr_ref(any_str_dst), neat_strv(any_str_src)) \
)

#define neat_str_insert(any_str_dst, any_str_src, idx)                                 \
(                                                                                      \
neat_str_assert_appendable(any_str_dst),                                               \
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

#define neat_str_tolower(any_str) \
neat_mutstr_ref_tolower(neat_mutstr_ref(any_str))

#define neat_str_toupper(any_str) \
neat_mutstr_ref_toupper(neat_mutstr_ref(any_str))

#define neat_str_replace(any_str, any_str_target, any_str_replacement) \
neat_mutstr_ref_replace(neat_mutstr_ref(any_str), neat_strv(any_str_target), neat_strv(any_str_replacement))

#define neat_str_replace_first(any_str, any_str_target, any_str_replacement) \
neat_mutstr_ref_replace_first(neat_mutstr_ref(any_str), neat_strv(any_str_target), neat_strv(any_str_replacement))

#define neat_str_split(any_str, any_str_delim, ...) \
neat_strv_split(neat_strv(any_str), neat_strv(any_str_delim), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join_new(strv_arr, any_str_delim, ...) \
neat_strv_arr_join_new(strv_arr, neat_strv(any_str_delim), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join(any_str_dst, strv_arr, any_str_delim) \
neat_strv_arr_join(neat_mutstr_ref(any_str_dst), strv_arr, neat_strv(any_str_delim))

#define neat_str_del(any_str, begin, end) \
neat_mutstr_ref_delete_range(neat_mutstr_ref(any_str), begin, end)

#define neat_str_fread_line(any_str, stream) \
neat_mutstr_ref_fread_line(neat_mutstr_ref(any_str), stream)

#define neat_str_concat_fread_line(any_str, stream) \
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
    __VA_OPT__( \
    \
    unsigned int neat_appended_len = 0; \
    Neat_Mut_String_Ref neat_anystr_ref_append_buf = \
    { \
        .cap = neat_as_mutstr_ref.cap, \
        .len = &neat_appended_len, \
        .chars = neat_as_mutstr_ref.chars, \
    }; \
    neat_tostr_into(neat_anystr_ref_append_buf, NEAT_ARG1(__VA_ARGS__)); \
    *neat_as_mutstr_ref.len = neat_appended_len; \
    \
    NEAT_FOREACH(neat_str_print_each, NEAT_OMIT1(__VA_ARGS__)); \
    ) \
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
NEAT_CAT(neat_str_print_new_, __VA_OPT__(1))(__VA_ARGS__)

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

#define NEAT_STRV_COMMA(any_str) \
neat_strv(any_str),

#define neat_strv_arr(...)                                                                     \
(                                                                                              \
neat_static_assertx(!neat_is_array_of(NEAT_ARG1(__VA_ARGS__), Neat_String_View), "strv_arr accepts variadic arguments of strings, not String_View[], call strv_arr_carr instead"), \
(Neat_String_View_Array) {                                                                     \
    .len   = NEAT_CARR_LEN(((Neat_String_View[]){NEAT_FOREACH(NEAT_STRV_COMMA, __VA_ARGS__)})), \
    .cap  = NEAT_CARR_LEN(((Neat_String_View[]){NEAT_FOREACH(NEAT_STRV_COMMA, __VA_ARGS__)})), \
    .strs = (Neat_String_View[]){NEAT_FOREACH(NEAT_STRV_COMMA, __VA_ARGS__)}                   \
}                                                                                              \
)

#define neat_strbuf(str_or_cap, ...) \
NEAT_CAT(neat_strbuf_, __VA_OPT__(2))(str_or_cap __VA_OPT__(,) __VA_ARGS__)

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
)((neat_static_assertx(_Generic(allocator_or_cap, Neat_Allocator: 1, default: neat_has_type(cap_or_carr, char*) || neat_has_type(cap_or_carr, unsigned char*)), "arg1 must be char* or unsigned char*"), cap_or_carr), allocator_or_cap)

#define neat_mutstr_ref(any_str, ...) \
NEAT_CAT(neat_mutstr_ref_, __VA_OPT__(2))(any_str __VA_OPT__(,) __VA_ARGS__)

#define neat_mutstr_ref_(any_str)                                                             \
_Generic((typeof(any_str)*){0},                                                               \
    char**                          : neat_mutstr_ref_to_cstr,                                \
    NEAT_UCHAR_CASE(unsigned char** : neat_mutstr_ref_to_ucstr,)                              \
    Neat_DString**                  : neat_mutstr_ref_to_dstr_ptr,                            \
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
    neat_static_assertx(neat_has_type(carr_or_ptr, char*) || neat_has_type(carr_or_ptr, unsigned char*), "arg1 must be char* or unsigned char*"), \
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
__VA_OPT__(neat_strv2(any_str, __VA_ARGS__)) \
NEAT_IF_EMPTY(neat_strv1(any_str), __VA_ARGS__)

#define neat_strv1(any_str) \
neat_strv2(any_str, 0)

#define neat_strv2(any_str, begin, ...)                    \
__VA_OPT__(neat_strv3(any_str, begin, __VA_ARGS__))        \
NEAT_IF_EMPTY(                                             \
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
)(any_str, begin), __VA_ARGS__)

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
NEAT_CAT(neat_dstr0, __VA_OPT__(1))(__VA_ARGS__)

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

#define neat_fprint(f, ...)                  \
do                                           \
{                                            \
    FILE *neat_file_stream = f;              \
    (void) neat_file_stream;                 \
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

#define neat_fprintln(f, ...)              \
do                                         \
{                                          \
    FILE *neat_temp_f = f;                 \
    (void) neat_temp_f;                    \
    neat_fprint(neat_temp_f, __VA_ARGS__); \
    fputc('\n', neat_temp_f);              \
} while(0)

#define neat_println(...) \
neat_fprintln(stdout, __VA_ARGS__)

#define NEAT_DEFAULT_TOSTR_TYPES                       \
bool                          : neat_bool_tostr,       \
char*                         : neat_cstr_tostr,        \
NEAT_UCHAR_CASE(unsigned char*: neat_ucstr_tostr,)      \
char                          : neat_char_tostr,       \
NEAT_SCHAR_CASE(signed char   : neat_schar_tostr,)     \
NEAT_UCHAR_CASE(unsigned char : neat_uchar_tostr,)     \
short                         : neat_short_tostr,      \
unsigned short                : neat_ushort_tostr,     \
int                           : neat_int_tostr,        \
unsigned int                  : neat_uint_tostr,       \
long                          : neat_long_tostr,       \
unsigned long                 : neat_ulong_tostr,      \
long long                     : neat_llong_tostr,      \
unsigned long long            : neat_ullong_tostr,     \
float                         : neat_float_tostr,      \
double                        : neat_double_tostr,     \
Neat_DString                  : neat_dstr_tostr,       \
Neat_DString*                 : neat_dstr_ptr_tostr,   \
Neat_String_View              : neat_strv_tostr,       \
Neat_String_View*             : neat_strv_ptr_tostr,   \
Neat_String_Buffer            : neat_strbuf_tostr,     \
Neat_String_Buffer*           : neat_strbuf_ptr_tostr, \
Neat_SString_Ref              : neat_sstr_ref_tostr,   \
Neat_Mut_String_Ref           : neat_mutstr_ref_tostr

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
bool                          : neat_bool_tostr_into,       \
char*                         : neat_cstr_tostr_into,       \
NEAT_UCHAR_CASE(unsigned char*: neat_ucstr_tostr_into,)     \
char                          : neat_char_tostr_into,       \
NEAT_SCHAR_CASE(signed char   : neat_schar_tostr_into,)     \
NEAT_UCHAR_CASE(unsigned char : neat_uchar_tostr_into,)     \
short                         : neat_short_tostr_into,      \
unsigned short                : neat_ushort_tostr_into,     \
int                           : neat_int_tostr_into,        \
unsigned int                  : neat_uint_tostr_into,       \
long                          : neat_long_tostr_into,       \
unsigned long                 : neat_ulong_tostr_into,      \
long long                     : neat_llong_tostr_into,      \
unsigned long long            : neat_ullong_tostr_into,     \
float                         : neat_float_tostr_into,      \
double                        : neat_double_tostr_into,     \
Neat_DString                  : neat_dstr_tostr_into,       \
Neat_DString*                 : neat_dstr_ptr_tostr_into,   \
Neat_String_View              : neat_strv_tostr_into,       \
Neat_String_View*             : neat_strv_ptr_tostr_into,   \
Neat_String_Buffer            : neat_strbuf_tostr_into,     \
Neat_String_Buffer*           : neat_strbuf_ptr_tostr_into, \
Neat_SString_Ref              : neat_sstr_ref_tostr_into,   \
Neat_Mut_String_Ref           : neat_mutstr_ref_tostr_into  \

#define NEAT_ALL_TOSTR_INTO_TYPES                                                 \
NEAT_IF_DEF(NEAT_TOSTR_INTO1) (neat_type_1_tostr_into : neat_tostr_into_func_1,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO2) (neat_type_2_tostr_into : neat_tostr_into_func_2,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO3) (neat_type_3_tostr_into : neat_tostr_into_func_3,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO4) (neat_type_4_tostr_into : neat_tostr_into_func_4,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO5) (neat_type_5_tostr_into : neat_tostr_into_func_5,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO6) (neat_type_6_tostr_into : neat_tostr_into_func_6,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO7) (neat_type_7_tostr_into : neat_tostr_into_func_7,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO8) (neat_type_8_tostr_into : neat_tostr_into_func_8,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO9) (neat_type_9_tostr_into : neat_tostr_into_func_9,)  \
NEAT_IF_DEF(NEAT_TOSTR_INTO10)(neat_type_10_tostr_into: neat_tostr_into_func_10,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO11)(neat_type_11_tostr_into: neat_tostr_into_func_11,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO12)(neat_type_12_tostr_into: neat_tostr_into_func_12,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO13)(neat_type_13_tostr_into: neat_tostr_into_func_13,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO14)(neat_type_14_tostr_into: neat_tostr_into_func_14,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO15)(neat_type_15_tostr_into: neat_tostr_into_func_15,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO16)(neat_type_16_tostr_into: neat_tostr_into_func_16,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO17)(neat_type_17_tostr_into: neat_tostr_into_func_17,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO18)(neat_type_18_tostr_into: neat_tostr_into_func_18,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO19)(neat_type_19_tostr_into: neat_tostr_into_func_19,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO20)(neat_type_20_tostr_into: neat_tostr_into_func_20,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO21)(neat_type_21_tostr_into: neat_tostr_into_func_21,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO22)(neat_type_22_tostr_into: neat_tostr_into_func_22,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO23)(neat_type_23_tostr_into: neat_tostr_into_func_23,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO24)(neat_type_24_tostr_into: neat_tostr_into_func_24,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO25)(neat_type_25_tostr_into: neat_tostr_into_func_25,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO26)(neat_type_26_tostr_into: neat_tostr_into_func_26,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO27)(neat_type_27_tostr_into: neat_tostr_into_func_27,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO28)(neat_type_28_tostr_into: neat_tostr_into_func_28,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO29)(neat_type_29_tostr_into: neat_tostr_into_func_29,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO30)(neat_type_30_tostr_into: neat_tostr_into_func_30,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO31)(neat_type_31_tostr_into: neat_tostr_into_func_31,) \
NEAT_IF_DEF(NEAT_TOSTR_INTO32)(neat_type_32_tostr_into: neat_tostr_into_func_32,) \
NEAT_DEFAULT_TOSTR_INTO_TYPES

struct neat_fail_type;
typedef neat_func_ptr(void, struct neat_fail_type*) neat_tostr_fail;

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

#define neat_p_tostr_into(dst, xp) \
neat_get_tostr_into_func(typeof(*xp))(neat_mutstr_ref(dst), xp)

#define neat_tostr_into(dst, x) \
neat_p_tostr_into(dst, neat_as_pointer(x))

#define NEAT_DECL_TOSTR_FUNC(n) \
typedef typeof(NEAT_ARG1(ADD_TOSTR)) neat_tostr_type_##n; \
static inline Neat_DString neat_tostr_func_##n (neat_tostr_type_##n *obj) \
{ \
    _Static_assert(neat_has_type(NEAT_ARG2(ADD_TOSTR), Neat_DString(*)(neat_tostr_type_##n*)), "tostr function must have type DString (T*)"); \
    return NEAT_ARG2(ADD_TOSTR)(obj); \
}

#define NEAT_DECL_TOSTR_INTO_FUNC(n) \
typedef typeof(NEAT_ARG1(ADD_TOSTR_INTO)) neat_type_##n##_tostr_into; \
static inline void neat_tostr_into_func_##n (Neat_Mut_String_Ref dst, neat_type_##n##_tostr_into *obj) \
{ \
    _Static_assert(neat_has_type(NEAT_ARG2(ADD_TOSTR_INTO), typeof(void(*)(Neat_Mut_String_Ref, neat_type_##n##_tostr_into*))), "tostr_into functions must have type void (T*)"); \
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

unsigned int neat_dstr_cap(Neat_DString str);
unsigned int neat_dstr_ptr_cap(Neat_DString *str);
unsigned int neat_strbuf_cap(Neat_String_Buffer str);
unsigned int neat_strbuf_ptr_cap(Neat_String_Buffer *str);
unsigned int neat_sstr_ref_cap(Neat_SString_Ref str);
unsigned int neat_mutstr_ref_cap(Neat_Mut_String_Ref str);
unsigned int neat_buf_cap(Neat_Buffer buf);

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

NEAT_NODISCARD("str_split returns new String_View_Array") Neat_String_View_Array neat_strv_split(Neat_String_View str, Neat_String_View delim, Neat_Allocator allocator);
NEAT_NODISCARD("str_join_new returns new DString, discarding will cause memory leak") Neat_DString neat_strv_arr_join_new(Neat_String_View_Array strs, Neat_String_View delim, Neat_Allocator allocator);
unsigned int neat_strv_arr_join(Neat_Mut_String_Ref dst, Neat_String_View_Array strs, Neat_String_View delim);

Neat_String_View_Array neat_strv_arr_from_carr(Neat_String_View *carr, unsigned int nb);

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2);
Neat_String_View neat_strv_find(Neat_String_View hay, Neat_String_View needle);
unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle);
bool neat_strv_starts_with(Neat_String_View hay, Neat_String_View needle);
bool neat_strv_ends_with(Neat_String_View hay, Neat_String_View needle);

void neat_mutstr_ref_tolower(Neat_Mut_String_Ref str);
void neat_mutstr_ref_toupper(Neat_Mut_String_Ref str);

unsigned int neat_mutstr_ref_fread_line(Neat_Mut_String_Ref dst, FILE *stream);
unsigned int neat_mutstr_ref_concat_fread_line(Neat_Mut_String_Ref dst, FILE *stream);

unsigned int neat_fprint_strv(FILE *stream, Neat_String_View str);
unsigned int neat_fprintln_strv(FILE *stream, Neat_String_View str);

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_bool_tostr(bool *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_cstr_tostr(char **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ucstr_tostr(unsigned char **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_char_tostr(char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_schar_tostr(signed char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_uchar_tostr(unsigned char *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_short_tostr(short *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ushort_tostr(unsigned short *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_int_tostr(int *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_uint_tostr(unsigned int *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_long_tostr(long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ulong_tostr(unsigned long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_llong_tostr(long long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ullong_tostr(unsigned long long *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_float_tostr(float *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_double_tostr(double *obj);

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_dstr_tostr(Neat_DString *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_dstr_ptr_tostr(Neat_DString **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strv_tostr(Neat_String_View *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strv_ptr_tostr(Neat_String_View **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strbuf_tostr(Neat_String_Buffer *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strbuf_ptr_tostr(Neat_String_Buffer **obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_sstr_ref_tostr(Neat_SString_Ref *obj);
NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_mutstr_ref_tostr(Neat_Mut_String_Ref *obj);

void neat_bool_tostr_into(Neat_Mut_String_Ref dst, bool *obj);
void neat_cstr_tostr_into(Neat_Mut_String_Ref dst, char **obj);
void neat_ucstr_tostr_into(Neat_Mut_String_Ref dst, unsigned char **obj);
void neat_char_tostr_into(Neat_Mut_String_Ref dst, char *obj);
void neat_schar_tostr_into(Neat_Mut_String_Ref dst, signed char *obj);
void neat_uchar_tostr_into(Neat_Mut_String_Ref dst, unsigned char *obj);
void neat_short_tostr_into(Neat_Mut_String_Ref dst, short *obj);
void neat_ushort_tostr_into(Neat_Mut_String_Ref dst, unsigned short *obj);
void neat_int_tostr_into(Neat_Mut_String_Ref dst, int *obj);
void neat_uint_tostr_into(Neat_Mut_String_Ref dst, unsigned int *obj);
void neat_long_tostr_into(Neat_Mut_String_Ref dst, long *obj);
void neat_ulong_tostr_into(Neat_Mut_String_Ref dst, unsigned long *obj);
void neat_llong_tostr_into(Neat_Mut_String_Ref dst, long long *obj);
void neat_ullong_tostr_into(Neat_Mut_String_Ref dst, unsigned long long *obj);
void neat_float_tostr_into(Neat_Mut_String_Ref dst, float *obj);
void neat_double_tostr_into(Neat_Mut_String_Ref dst, double *obj);

void neat_dstr_tostr_into(Neat_Mut_String_Ref dst, Neat_DString *obj);
void neat_dstr_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_DString **obj);
void neat_strv_tostr_into(Neat_Mut_String_Ref dst, Neat_String_View *obj);
void neat_strv_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_String_View **obj);
void neat_strbuf_tostr_into(Neat_Mut_String_Ref dst, Neat_String_Buffer *obj);
void neat_strbuf_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_String_Buffer **obj);
void neat_sstr_ref_tostr_into(Neat_Mut_String_Ref dst, Neat_SString_Ref *obj);
void neat_mutstr_ref_tostr_into(Neat_Mut_String_Ref dst, Neat_Mut_String_Ref *obj);

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
#define str_tolower(any_str) neat_str_tolower(any_str)
#define str_toupper(any_str) neat_str_toupper(any_str)
#define str_copy(any_str_dst, any_str_src) neat_str_copy(any_str_dst, any_str_src)
#define str_concat(cap_str_dst, any_str_src) neat_str_concat(cap_str_dst, any_str_src)
#define str_insert(any_str_dst, any_str_src, idx) neat_str_insert(any_str_dst, any_str_src, idx)
#define str_prepend(neat_str_dst, neat_str_src) neat_str_prepend(neat_str_dst, neat_str_src)
#define str_del(any_str, begin, end) neat_str_del(any_str, begin, end)
#define str_replace(mut_str, any_str_target, any_str_replacement) neat_str_replace(mut_str, any_str_target, any_str_replacement)
#define str_replace_first(any_str, any_str_target, any_str_replacement) neat_str_replace_first(any_str, any_str_target, any_str_replacement)
#define str_split(any_str, any_str_delim, ...) neat_str_split(any_str, any_str_delim __VA_OPT__(,) __VA_ARGS__)
#define str_join(mut_str_dst, strv_arr, any_str_delim) neat_str_join(mut_str_dst, strv_arr, any_str_delim)
#define str_join_new(strv_arr, any_str_delim, ...) neat_str_join_new(strv_arr, any_str_delim __VA_OPT__(,) __VA_ARGS__)
#define str_fread_line(any_str, stream) neat_str_fread_line(any_str, stream)
#define str_concat_fread_line(any_str, stream) neat_str_concat_fread_line(any_str, stream)
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
#define tostr_into_p(any_str, x) neat_p_tostr_into(any_str, x)

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
#if defined(NEAT_STR_IMPL) && !defined(NEAT_STR_IMPLED)
#define NEAT_STR_IMPLED

void *neat_default_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual)
{
    (void) alignment;
    (void) ctx;
    if(actual) *actual = n;
    return malloc(n);
}

void neat_default_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) n;
    free(ptr);
}

void *neat_default_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual)
{
    (void) ctx;
    (void) alignment;
    (void) old_size;
    if(actual) *actual = new_size;
    return realloc(ptr, new_size);
}

void neat_default_allocator_init(void **ctx, void *arg)
{
    (void) ctx;
    (void) arg;
}

void neat_default_allocator_deinit(void *ctx)
{
    (void) ctx;
}

void *neat_noop_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual)
{
    (void) ctx;
    (void) alignment;
    (void) n;
    (void) actual;
    return NULL;
}

void neat_noop_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) ptr;
    (void) n;
}

void *neat_noop_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual)
{
    (void) ctx;
    (void) ptr;
    (void) alignment;
    (void) old_size;
    (void) new_size;
    (void) actual;
    return NULL;
}

void neat_noop_allocator_init(void **ctx, void *arg)
{
    (void) ctx;
    (void) arg;
}

void neat_noop_allocator_deinit(void *ctx)
{
    (void) ctx;
}

void *neat_allocator_invoke_alloc(Neat_Allocator allocator, size_t alignment, size_t obj_size, size_t nb, size_t *actual)
{
    return allocator.alloc(allocator.ctx, alignment, nb * obj_size, actual);
}

void neat_allocator_invoke_dealloc(Neat_Allocator allocator, void *ptr, size_t obj_size, size_t nb)
{
    allocator.dealloc(allocator.ctx, ptr, nb * obj_size);
}

void *neat_allocator_invoke_realloc(Neat_Allocator allocator, void *ptr, size_t alignment, size_t obj_size, size_t old_nb, size_t new_nb, size_t *actual)
{
    return allocator.realloc(allocator.ctx, ptr, alignment, old_nb * obj_size, new_nb * obj_size, actual);
}

Neat_Allocator neat_get_default_allocator()
{
    return
    (Neat_Allocator){
        .alloc   = neat_default_allocator_alloc,
        .dealloc = neat_default_allocator_dealloc,
        .realloc = neat_default_allocator_realloc,
        .init    = neat_default_allocator_init,
        .deinit  = neat_default_allocator_deinit,
    };
}

Neat_Allocator neat_get_noop_allocator()
{
    return
    (Neat_Allocator){
        .alloc   = neat_noop_allocator_alloc,
        .dealloc = neat_noop_allocator_dealloc,
        .realloc = neat_noop_allocator_realloc,
        .init    = neat_noop_allocator_init,
        .deinit  = neat_noop_allocator_deinit,
    };
}

static inline unsigned int neat_uint_min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

static inline unsigned int neat_uint_max(unsigned int a, unsigned int b)
{
    return a > b ? a : b;
}

static unsigned int neat_chars_strlen(unsigned char *chars, unsigned int cap)
{
    unsigned char *str_end = (unsigned char*) memchr(chars, '\0', cap);
    unsigned int len;
    
    if(str_end != NULL)
    {
        len = str_end - chars;
    }
    else
    {
        len = cap - 1;
    }
    
    return len;
}

static unsigned int neat_mutstr_ref_len(Neat_Mut_String_Ref str)
{
    if(str.len != NULL)
    {
        return *str.len;
    }
    else
    {
        return neat_chars_strlen(str.chars, str.cap);
    }
}

bool neat_is_strv_intersect(Neat_String_View base, Neat_String_View sub)
{
    uintptr_t begin = (uintptr_t) base.chars;
    uintptr_t end   = ((uintptr_t) base.chars) + base.len;
    uintptr_t sub_begin = (uintptr_t) sub.chars;
    return sub_begin >= begin && sub_begin <= end;
}

NEAT_NODISCARD("discarding a new DString will cause memory leak") Neat_DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    Neat_DString ret = { 0 };
    
    ret.allocator = allocator;
    size_t actual_allocated_cap;
    
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    if(ret.chars != NULL)
    {
        ret.chars[0] = '\0';
    }
    return ret;
}

void neat_dstr_deinit_(Neat_DString *dstr)
{
    neat_dealloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap);
    dstr->allocator.deinit(&dstr->allocator.ctx);
    dstr->cap = 0;
    dstr->len = 0;
    dstr->chars = NULL;
}

void neat_dstr_shrink_to_fit_(Neat_DString *dstr)
{
    size_t actual_new_cap;
    dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, dstr->len + 1, &actual_new_cap);
    dstr->cap = actual_new_cap;
}

void neat_dstr_maybe_grow(Neat_DString *dstr, unsigned int len_to_append)
{
    if(dstr->cap - dstr->len <= len_to_append)
    {
        // grow
        unsigned int new_cap = neat_uint_max(dstr->cap * 2, dstr->cap + len_to_append);
        
        size_t actual_allocated_cap;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
    }
}

void neat_dstr_append_strv(Neat_DString *dstr, Neat_String_View str)
{
    Neat_String_View to_append = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), to_append))
    {
        unsigned int begin_idx = to_append.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, to_append.len);
        to_append = (Neat_String_View){
            .len   = to_append.len,
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_append.len);
    }
    
    memmove(dstr->chars + dstr->len, to_append.chars, to_append.len * sizeof(unsigned char));
    
    dstr->len += to_append.len;
    dstr->chars[dstr->len] = '\0';
}

void neat_dstr_prepend_strv(Neat_DString *dstr, Neat_String_View src)
{
    Neat_String_View to_prepend = src;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = src.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, src.len);
        to_prepend = (Neat_String_View){
            .len = src.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_prepend.len);
    }
    
    memmove(dstr->chars + to_prepend.len, dstr->chars, dstr->len);
    memmove(dstr->chars, to_prepend.chars, to_prepend.len);
    
    dstr->len += to_prepend.len;
    dstr->chars[dstr->len] = '\0';
}

NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_strv(Neat_DString *dstr, Neat_String_View src, unsigned int idx)
{
    if(idx > dstr->len)
    {
        return false;
    }
    
    Neat_String_View to_insert = src;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = src.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, src.len);
        to_insert = (Neat_String_View){
            .len = src.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_insert.len);
    }
    
    memmove(dstr->chars + idx + to_insert.len, dstr->chars + idx, dstr->len - idx);
    memmove(dstr->chars + idx, to_insert.chars, to_insert.len);
    
    dstr->len += to_insert.len;
    dstr->chars[dstr->len] = '\0';
    
    return true;
}

void neat_dstr_append_tostr_(Neat_DString *dstr, Neat_DString tostr)
{
    neat_dstr_append_strv(dstr, neat_strv_dstr2(tostr, 0));
    neat_dstr_deinit_(&tostr);
}

void neat_dstr_prepend_tostr_(Neat_DString *dstr, Neat_DString tostr)
{
    neat_dstr_prepend_strv(dstr, neat_strv_dstr2(tostr, 0));
    neat_dstr_deinit_(&tostr);
}

NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_tostr_(Neat_DString *dstr, Neat_DString tostr, unsigned int idx)
{
    bool ret = neat_dstr_insert_strv(dstr, neat_strv_dstr2(tostr, 0), idx);
    neat_dstr_deinit_(&tostr);
    return ret;
}

unsigned int neat_dstr_fread_line_(Neat_DString *dstr, FILE *stream)
{
    dstr->len = 0;
    if(dstr->cap > 0)
    {
        dstr->chars[0] = '\0';
    }
    
    return neat_dstr_append_fread_line_(dstr, stream);
}

unsigned int neat_dstr_append_fread_line_(Neat_DString *dstr, FILE *stream)
{
    // TODO optimize this
    unsigned int prev_len = dstr->len;
    int c = 0;
    while(c != '\n' && (c=fgetc(stream)) != EOF)
    {
        unsigned char as_char = c;
        Neat_String_View as_strv = {.chars = &as_char, .len = 1};
        neat_dstr_append_strv(dstr, as_strv);
    }
    
    return dstr->len - prev_len;
}

void neat_dstr_ensure_cap_(Neat_DString *dstr, unsigned int at_least)
{
    if(dstr->cap <= at_least)
    {
        size_t actual_allocated_cap;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, at_least, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
    }
}

char *neat_cstr_as_cstr(char *str)
{
    return str;
}

char *neat_ucstr_as_cstr(unsigned char *str)
{
    return (char*) str;
}

char *neat_dstr_as_cstr(Neat_DString str)
{
    return (char*) str.chars;
}

char *neat_dstr_ptr_as_cstr(Neat_DString *str)
{
    return (char*) str->chars;
}

char *neat_strv_as_cstr(Neat_String_View str)
{
    return (char*) str.chars;
}

char *neat_strv_ptr_as_cstr(Neat_String_View *str)
{
    return (char*) str->chars;
}

char *neat_strbuf_as_cstr(Neat_String_Buffer str)
{
    return (char*) str.chars;
}

char *neat_strbuf_ptr_as_cstr(Neat_String_Buffer *str)
{
    return (char*) str->chars;
}

char *neat_sstr_ref_as_cstr(Neat_SString_Ref str)
{
    return (char*) str.sstring->chars;
}

char *neat_mutstr_ref_as_cstr(Neat_Mut_String_Ref str)
{
    return (char*) str.chars;
}

unsigned char neat_cstr_char_at(char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_ucstr_char_at(unsigned char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_dstr_char_at(Neat_DString str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_dstr_ptr_char_at(Neat_DString *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strv_char_at(Neat_String_View str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strv_ptr_char_at(Neat_String_View *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strbuf_char_at(Neat_String_Buffer str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strbuf_ptr_char_at(Neat_String_Buffer *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_sstr_ref_char_at(Neat_SString_Ref str, unsigned int idx)
{
    return str.sstring->chars[idx];
}

unsigned char neat_mutstr_ref_char_at(Neat_Mut_String_Ref str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned int neat_dstr_cap(Neat_DString str)
{
    return str.cap;
}

unsigned int neat_dstr_ptr_cap(Neat_DString *str)
{
    return str->cap;
}

unsigned int neat_strbuf_cap(Neat_String_Buffer str)
{
    return str.cap;
}

unsigned int neat_strbuf_ptr_cap(Neat_String_Buffer *str)
{
    return str->cap;
}
unsigned int neat_sstr_ref_cap(Neat_SString_Ref str)
{
    return str.cap;
}

unsigned int neat_mutstr_ref_cap(Neat_Mut_String_Ref str)
{
    return str.cap;
}

unsigned int neat_buf_cap(Neat_Buffer buf)
{
    return buf.size;
}

Neat_String_View neat_strv_strv2(Neat_String_View str, unsigned int begin)
{
    return neat_strv_strv_ptr2(&str, begin);
}

unsigned int neat_mutstr_ref_insert_strv(Neat_Mut_String_Ref dst, Neat_String_View src, unsigned int idx)
{
    unsigned int len = neat_mutstr_ref_len(dst);
    
    if(idx > len)
    {
        return 0;
    }
    
    unsigned int nb_chars_to_insert = neat_uint_min(dst.cap - len - 1, src.len);
    
    // shift right
    memmove(dst.chars + idx + nb_chars_to_insert, dst.chars + idx, len - idx);
    
    // insert the src
    memmove(dst.chars + idx, src.chars, nb_chars_to_insert);
    
    len += nb_chars_to_insert;
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    return nb_chars_to_insert;
}

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2)
{
    return
    (str1.len == str2.len) &&
    (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

Neat_String_View neat_strv_find(Neat_String_View hay, Neat_String_View needle)
{
    unsigned int end = hay.len - needle.len;
    for(unsigned int i = 0 ; i <= end ; i++)
    {
        Neat_String_View sv = neat_strv_strv_ptr3(&hay, i, i + needle.len);
        if(neat_strv_equal(needle, sv))
        {
            return sv;
        }
    }
    return (Neat_String_View){
        .len = 0,
        .chars = NULL
    };
}

unsigned int neat_mutstr_ref_copy(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    dst.chars[chars_to_copy] = '\0';
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    return chars_to_copy;
}

unsigned int neat_mutstr_ref_concat(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int dst_len = neat_mutstr_ref_len(dst);
    
    if(dst_len >= dst.cap - 1)
        return 0;
    
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - dst_len - 1);
    memmove(dst.chars + dst_len, src.chars, chars_to_copy);
    
    dst_len += chars_to_copy;
    
    if(dst.len != NULL)
        *dst.len = dst_len;
    
    dst.chars[dst_len] = '\0';
    
    return chars_to_copy;
}

NEAT_NODISCARD("str_del returns true on success, false on failure") bool neat_mutstr_ref_delete_range(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
    if(end > len || begin > end)
    {
        return false;
    }
    
    unsigned int substr_len = end - begin;
    
    memmove(str.chars + begin, str.chars + begin + substr_len, len - begin - substr_len);
    
    len -= substr_len;
    
    str.chars[len] = '\0';
    
    if(str.len != NULL)
    {
        *str.len = len;
    }
    
    return true;
}

Neat_String_View_Array neat_strv_arr_from_carr(Neat_String_View *carr, unsigned int nb)
{
    return (Neat_String_View_Array){
        .cap  = nb,
        .len   = nb,
        .strs = carr
    };
}

NEAT_NODISCARD("str_split returns new String_View_Array") Neat_String_View_Array neat_strv_split(Neat_String_View str, Neat_String_View delim, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    if(delim.len > str.len)
    {
        size_t alloced_size;
        Neat_String_View *entire_str = neat_alloc(allocator, Neat_String_View, 1, &alloced_size);
        if(alloced_size < sizeof(Neat_String_View) || entire_str == NULL)
        {
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            *entire_str = str;
            return (Neat_String_View_Array){
                .cap  = (unsigned int) alloced_size,
                .len   = 1,
                .strs = entire_str
            };
        }
    }
    else if(delim.len == 0)
    {
        size_t alloced_size;
        Neat_String_View *strs = neat_alloc(allocator, Neat_String_View, str.len, &alloced_size);
        
        if(alloced_size < (str.len * sizeof(Neat_String_View)) || strs == NULL)
        {
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap  = (unsigned int) alloced_size,
                .len   = str.len,
                .strs = strs
            };
            
            for(unsigned int i = 0 ; i < ret.len ; i++)
            {
                ret.strs[i] = neat_strv_strv3(str, i, i + 1);
            }
            
            return ret;
        }
    }
    else
    {
        unsigned int nb_delim = 0;
        unsigned int *delim_idx = (unsigned int*) calloc(str.len, sizeof(unsigned int));
        
        for(unsigned int i = 0 ; i <= str.len - delim.len ; )
        {
            Neat_String_View sub = neat_strv_strv3(str, i, i + delim.len);
            if(neat_strv_equal(sub, delim))
            {
                delim_idx[nb_delim] = i;
                nb_delim += 1;
                i += delim.len;
            }
            else
            {
                i += 1;
            }
        }
        
        delim_idx[nb_delim] = str.len;
        
        size_t alloced_size;
        Neat_String_View *strs = neat_alloc(allocator, Neat_String_View, nb_delim + 1, &alloced_size);
        
        if(alloced_size < ((nb_delim + 1) * sizeof(Neat_String_View))  || strs == NULL)
        {
            free(delim_idx);
            
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap = (unsigned int) alloced_size,
                .len  = nb_delim + 1,
                .strs = strs
            };
            
            ret.strs[0] = neat_strv_strv_ptr3(&str, 0, delim_idx[0]);
            for(unsigned int i = 1 ; i <= nb_delim ; i++)
            {
                ret.strs[i] = neat_strv_strv_ptr3(&str, delim_idx[i - 1] + delim.len, delim_idx[i]);
            }
            
            free(delim_idx);
            return ret;
        }
    }
}

NEAT_NODISCARD("str_join_new returns new DString, discarding will cause memory leak") Neat_DString neat_strv_arr_join_new(Neat_String_View_Array strs, Neat_String_View delim, Neat_Allocator allocator)
{
    Neat_DString ret = neat_dstr_new(16, allocator);
    
    if(strs.len > 0)
        neat_dstr_append_strv(&ret, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.len ; i++)
    {
        neat_dstr_append_strv(&ret, delim);
        neat_dstr_append_strv(&ret, strs.strs[i]);
    }
    
    return ret;
}

unsigned int neat_strv_arr_join(Neat_Mut_String_Ref dst, Neat_String_View_Array strs, Neat_String_View delim)
{
    unsigned int chars_copied = 0;
    
    Neat_String_Buffer dst_as_buf = { 0 };
    dst_as_buf.cap = dst.cap;
    dst_as_buf.len = 0;
    dst_as_buf.chars = dst.chars;
    
    Neat_Mut_String_Ref dst2 = neat_mutstr_ref_to_strbuf_ptr(&dst_as_buf);
    
    if(strs.len > 0)
    {
        chars_copied += neat_mutstr_ref_concat(dst2, strs.strs[0]);
    }
    for(unsigned int i = 1 ; i < strs.len ; i++)
    {
        chars_copied += neat_mutstr_ref_concat(dst2, delim);
        chars_copied += neat_mutstr_ref_concat(dst2, strs.strs[i]);
    }
    
    if(dst.len != NULL)
        *dst.len = chars_copied;
    
    dst.chars[chars_copied] = '\0';
    
    return chars_copied;
}

unsigned int neat_mutstr_ref_replace(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    unsigned int replacements = 0;
    
    unsigned int len;
    if(str.len == NULL)
    {
        len = neat_chars_strlen(str.chars, str.cap);
        str.len = &len;
    }
    
    if(target.len == 0)
    {
        unsigned int total_inserted = 0;
        unsigned int inserted = 1;
        for(unsigned int i = 0 ; i <= *str.len && inserted ; i += replacement.len + 1)
        {
            inserted = neat_mutstr_ref_insert_strv(str, replacement, i);
            total_inserted += inserted;
        }
        return total_inserted;
    }
    
    if(target.len < replacement.len)
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                if(str.cap > *str.len + (replacement.len - target.len))
                {
                    // shift right
                    memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
                    
                    // put the replacement
                    memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                    
                    *str.len += (replacement.len - target.len);
                    
                    i = idx + replacement.len;
                    replacements++;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else if(target.len > replacement.len)
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // shift left
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                *str.len -= (target.len - replacement.len);
                
                i = idx + replacement.len;
                replacements++;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                i = idx + replacement.len;
                replacements++;
            }
            else
            {
                break;
            }
        }
    }
    
    str.chars[*str.len] = '\0';
    return replacements;
}

bool neat_mutstr_ref_replace_first(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    bool replaced = false;
    
    unsigned int len = 0;
    if(str.len == NULL)
    {
        len = neat_chars_strlen(str.chars, str.cap);
        str.len = &len;
    }
    
    Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, 0), target);
    if(match.chars != NULL)
    {
        if(str.cap > 0 && str.cap - 1 > *str.len + (replacement.len - target.len))
        {
            unsigned int idx = match.chars - str.chars;
            
            // shift
            memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
            
            // put the replacement
            memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
            
            *str.len += (replacement.len - target.len);
            
            replaced = true;
        }
    }
    
    if(str.cap > 0)
        str.chars[*str.len] = '\0';
    
    return replaced;
}

unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle)
{
    if(needle.len == 0)
        return 0;
    
    unsigned int count = 0;
    for(unsigned int i = 0 ; i <= hay.len - needle.len ; )
    {
        //Neat_String_View view = neat_strv_strv3(hay, i, i + needle.len);
        if(memcmp(hay.chars + i, needle.chars, needle.len) == 0)
            //neat_strv_equal(view, needle))
        {
            count++;
            i += needle.len;
        }
        else
        {
            i++;
        }
    }
    return count;
}

bool neat_strv_starts_with(Neat_String_View hay, Neat_String_View needle)
{
    return (needle.len <= hay.len) && (memcmp(hay.chars, needle.chars, needle.len) == 0);
}

bool neat_strv_ends_with(Neat_String_View hay, Neat_String_View needle)
{
    return (needle.len <= hay.len) && (memcmp(hay.chars + hay.len - needle.len, needle.chars, needle.len) == 0);
}

void neat_mutstr_ref_tolower(Neat_Mut_String_Ref str)
{
    unsigned int len = neat_mutstr_ref_len(str);
    for(unsigned int i = 0 ; i < len ; i++)
    {
        str.chars[i] = tolower(str.chars[i]);
    }
}

void neat_mutstr_ref_toupper(Neat_Mut_String_Ref str)
{
    unsigned int len = neat_mutstr_ref_len(str);
    for(unsigned int i = 0 ; i < len ; i++)
    {
        str.chars[i] = toupper(str.chars[i]);
    }
}

Neat_Mut_String_Ref neat_mutstr_ref_to_cstr(char *str)
{
    unsigned int len = strlen(str);
    
    return (Neat_Mut_String_Ref){
        .cap   = len + 1,
        .len   = NULL,
        .chars = (unsigned char*) str
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    
    return (Neat_Mut_String_Ref){
        .cap   = len + 1,
        .len   = NULL,
        .chars = str
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_dstr_ptr(Neat_DString *str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_strbuf_ptr(Neat_String_Buffer *str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_sstr_ref(Neat_SString_Ref str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str.cap,
        .len   = &str.sstring->len,
        .chars = str.sstring->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_mutstr_ref(Neat_Mut_String_Ref str)
{
    return str;
}

Neat_String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    Neat_String_Buffer ret = { 0 };
    size_t actual_allocated_cap;
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    return ret;
}

Neat_String_Buffer neat_strbuf_new_default(unsigned int cap)
{
    return neat_strbuf_new(cap, neat_get_default_allocator());
}

Neat_String_Buffer neat_strbuf_from_ptr(void *ptr, unsigned int cap)
{
    unsigned int len = neat_chars_strlen(ptr, cap);
    
    return (Neat_String_Buffer){
        .cap = cap,
        .len = len,
        .chars = ptr
    };
}

Neat_String_Buffer neat_strbuf_from_buf(Neat_Buffer buf)
{
    return neat_strbuf_from_ptr(buf.ptr, buf.size);
}

Neat_String_View neat_strv_cstr2(char *str, unsigned int begin)
{
    unsigned int len = strlen(str);
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - begin,
        .chars = (unsigned char*) str + begin
    };
}

Neat_String_View neat_strv_ucstr2(unsigned char *str, unsigned int begin)
{
    unsigned int len = strlen((char*) str);
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - begin,
        .chars = str + begin
    };
}

Neat_String_View neat_strv_dstr2(Neat_DString str, unsigned int begin)
{
    return neat_strv_dstr_ptr2(&str, begin);
}

Neat_String_View neat_strv_dstr_ptr2(Neat_DString *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strv_ptr2(Neat_String_View *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strbuf_ptr2(Neat_String_Buffer *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strbuf2(Neat_String_Buffer str, unsigned int begin)
{
    return neat_strv_strbuf_ptr2(&str, begin);
}

Neat_String_View neat_strv_sstr_ref2(Neat_SString_Ref str, unsigned int begin)
{
    if(begin > str.sstring->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str.sstring->len   - begin,
        .chars = str.sstring->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref2(Neat_Mut_String_Ref str, unsigned int begin)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len       - begin,
        .chars = str.chars + begin
    };
}

Neat_String_View neat_strv_cstr3(char *str, unsigned int begin, unsigned int end)
{
    unsigned int len = strlen(str);
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = (unsigned char*) str + begin
    };
}

Neat_String_View neat_strv_ucstr3(unsigned char *str, unsigned int begin, unsigned int end)
{
    unsigned int len = strlen((char*) str);
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str + begin
    };
}

Neat_String_View neat_strv_dstr_ptr3(Neat_DString *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strv_ptr3(Neat_String_View *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strbuf_ptr3(Neat_String_Buffer *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_sstr_ref3(Neat_SString_Ref str, unsigned int begin, unsigned int end)
{
    if(begin > str.sstring->len || end > str.sstring->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str.sstring->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref3(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str.chars + begin
    };
}

Neat_String_View neat_strv_dstr3(Neat_DString str, unsigned int begin, unsigned int end)
{
    return neat_strv_dstr_ptr3(&str, begin, end);
}

Neat_String_View neat_strv_strv3(Neat_String_View str, unsigned int begin, unsigned int end)
{
    return neat_strv_strv_ptr3(&str, begin, end);
}

Neat_String_View neat_strv_strbuf3(Neat_String_Buffer str, unsigned int begin, unsigned int end)
{
    return neat_strv_strbuf_ptr3(&str, begin, end);
}

Neat_DString neat_tostr_all_into_new_dstr(Neat_Allocator allocator, unsigned int nb, ...)
{
    Neat_DString ret = neat_dstr_new(nb * 16, allocator);
    
    va_list args;
    va_start(args, nb);
    
    for(unsigned int i = 0 ; i < nb ; i++)
    {
        Neat_DString current = va_arg(args, Neat_DString);
        neat_dstr_append_strv(&ret, (Neat_String_View){.chars = current.chars, .len = current.len});
        neat_dstr_deinit_(&current);
    }
    
    va_end(args);
    
    return ret;
}

unsigned int neat_mutstr_ref_fread_line( Neat_Mut_String_Ref dst, FILE *stream)
{
    if(dst.cap == 0)
    {
        return 0;
    }
    
    unsigned int len = 0;
    int c = 0;
    while(len < dst.cap - 1 && c != '\n' && (c=fgetc(stream)) != EOF)
    {
        dst.chars[len] = c;
        len += 1;
    }
    
    dst.chars[len] = '\0';
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    return len;
}

unsigned int neat_mutstr_ref_concat_fread_line(Neat_Mut_String_Ref dst, FILE *stream)
{
    unsigned int dst_len = neat_mutstr_ref_len(dst);
    
    unsigned int concated_len = 0;
    
    Neat_Mut_String_Ref right = {
        .cap = dst.cap - dst_len,
        .len = &concated_len
    };
    right.chars = dst.chars + dst_len;
    
    unsigned int chars_read = neat_mutstr_ref_fread_line(right, stream);
    dst_len += chars_read;
    
    if(dst.len != NULL)
    {
        *dst.len = dst_len;
    }
    
    dst.chars[dst_len] = '\0';
    
    return chars_read;
}

unsigned int neat_fprint_strv(FILE *stream, Neat_String_View str)
{
    return fwrite(str.chars, sizeof(unsigned char), str.len, stream);
}

unsigned int neat_fprintln_strv(FILE *stream, Neat_String_View str)
{
    unsigned int written = fwrite(str.chars, sizeof(unsigned char), str.len, stream);
    written += fwrite(u8"\n", sizeof(unsigned char), 1, stream);
    return written;
}

// default tostr functions:

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_bool_tostr(bool *obj)
{
    Neat_DString ret = neat_dstr_new(6, neat_get_default_allocator());
    
    if(*obj)
        neat_str_copy(&ret, (char*) "true");
    else
        neat_str_copy(&ret, (char*) "false");
    
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_cstr_tostr(char **obj)
{
    Neat_DString ret = neat_dstr_new(15, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ucstr_tostr(unsigned char **obj)
{
    Neat_DString ret = neat_dstr_new(15, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_char_tostr(char *obj)
{
    Neat_DString ret = neat_dstr_new(2, neat_get_default_allocator());
    char cstr[2] = {*obj, 0};
    neat_dstr_append(&ret, cstr);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_schar_tostr(signed char *obj)
{
    Neat_DString ret = neat_dstr_new(5, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hhd", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_uchar_tostr(unsigned char *obj)
{
    Neat_DString ret = neat_dstr_new(2, neat_get_default_allocator());
    unsigned char cstr[2] = {*obj, 0};
    neat_dstr_append(&ret, cstr);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_short_tostr(short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hd", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ushort_tostr(unsigned short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_int_tostr(int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%d", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_uint_tostr(unsigned int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%u", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_long_tostr(long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%ld", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ulong_tostr(unsigned long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%lu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_llong_tostr(long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%lld", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_ullong_tostr(unsigned long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%llu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_float_tostr(float *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%g", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_double_tostr(double *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%g", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_dstr_tostr(Neat_DString *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_dstr_ptr_tostr(Neat_DString **obj)
{
    return neat_dstr_tostr(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strv_tostr(Neat_String_View *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append_strv(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strv_ptr_tostr(Neat_String_View **obj)
{
    return neat_strv_tostr(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strbuf_tostr(Neat_String_Buffer *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_strbuf_ptr_tostr(Neat_String_Buffer **obj)
{
    return neat_strbuf_tostr(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_sstr_ref_tostr(Neat_SString_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(obj->sstring->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_mutstr_ref_tostr(Neat_Mut_String_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

#define neat_sinteger_min(ty) \
_Generic((ty){0},             \
signed char: SCHAR_MIN,   \
short      : SHRT_MIN,    \
int        : INT_MIN,     \
long       : LONG_MIN,    \
long long  : LLONG_MIN    \
)

#define neat_min_tostr_into(ty) \
_Generic((ty){0},          \
signed char: neat_schar_min_tostr_into, \
short      : neat_short_min_tostr_into, \
int        : neat_int_min_tostr_into,   \
long       : neat_long_min_tostr_into,  \
long long  : neat_llong_min_tostr_into  \
)

void neat_schar_min_tostr_into(Neat_Mut_String_Ref dst)
{
    if(SCHAR_MIN == -128)
    {
        const char *numstr = "-128";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        neat_mutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len = snprintf(temp, sizeof(temp), "%hhd", SCHAR_MIN);
        neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

void neat_short_min_tostr_into(Neat_Mut_String_Ref dst)
{
    if(SHRT_MIN == -32768)
    {
        const char *numstr = "-32768";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        neat_mutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len = snprintf(temp, sizeof(temp), "%hd", SHRT_MIN);
        neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

void neat_int_min_tostr_into(Neat_Mut_String_Ref dst)
{
    if(INT_MIN == -2147483648)
    {
        const char *numstr = "-2147483648";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        neat_mutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%d", INT_MIN);
        neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

void neat_long_min_tostr_into(Neat_Mut_String_Ref dst)
{
    if(LONG_MIN == INT_MIN)
    {
        neat_int_min_tostr_into(dst);
        return;
    }
    else if(LONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        neat_mutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%ld", LONG_MIN);
        neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

void neat_llong_min_tostr_into(Neat_Mut_String_Ref dst)
{
    if(LLONG_MIN == LONG_MIN)
    {
        neat_long_min_tostr_into(dst);
        return;
    }
    else if(LLONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        neat_mutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%lld", LLONG_MIN);
        neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

static const long long neat_ten_pows[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
};

static const unsigned long long neat_ten_pows_ull[] = {
    1ull,
    10ull,
    100ull,
    1000ull,
    10000ull,
    100000ull,
    1000000ull,
    10000000ull,
    100000000ull,
    1000000000ull,
    10000000000ull,
    100000000000ull,
    1000000000000ull,
    10000000000000ull,
    100000000000000ull,
    1000000000000000ull,
    10000000000000000ull,
    100000000000000000ull,
    1000000000000000000ull,
    10000000000000000000ull,
};

unsigned int neat_numstr_len(long long num)
{
    unsigned int len = 1;
    for(unsigned int i = 1 ; i < NEAT_CARR_LEN(neat_ten_pows) && num >= neat_ten_pows[i++] ; len++);
    return len;
}

unsigned int neat_numstr_len_ull(unsigned long long num)
{
    unsigned int len = 1;
    for(unsigned int i = 1 ; i < NEAT_CARR_LEN(neat_ten_pows_ull) && num >= neat_ten_pows_ull[i++] ; len++);
    return len;
}

#define neat_sinteger_tostr_into() \
do { \
    if(dst.cap < 1) \
        return; \
    if(*obj == neat_sinteger_min(typeof(*obj))) \
    { \
        neat_min_tostr_into(typeof(*obj))(dst); \
        return; \
    } \
    unsigned int len_temp = 0; \
    unsigned int *len_p = &len_temp; \
    if(dst.len != NULL) \
    { \
        len_p = dst.len; \
        *len_p = 0; \
    } \
    \
    typeof(*obj) num = *obj; \
    \
    bool isneg = num < 0; \
    if(isneg) \
    { \
        num *= -1; \
        if(dst.cap > 1) \
        { \
            dst.chars[0] = '-'; \
        } \
    } \
    unsigned int len = neat_numstr_len(num); \
    unsigned int chars_to_copy = neat_uint_min(dst.cap - (1 + isneg), len); \
    num /= neat_ten_pows[len - chars_to_copy]; \
    for (unsigned int i = 0; i < chars_to_copy ; i++) \
    { \
        unsigned int rem = num % 10; \
        num = num / 10; \
        dst.chars[isneg + chars_to_copy - (i + 1)] = rem + '0'; \
    } \
    \
    *len_p += chars_to_copy; \
} while(0)

#define neat_uinteger_tostr_into() \
do { \
    if(dst.cap < 1) \
        return; \
    unsigned int len_temp = 0; \
    unsigned int *len_p = &len_temp; \
    if(dst.len != NULL) \
    { \
        len_p = dst.len; \
    } \
    \
    typeof(*obj) num = *obj; \
    unsigned int len = neat_numstr_len_ull(num); \
    unsigned int chars_to_copy = neat_uint_min(dst.cap - 1, len); \
    num /= neat_ten_pows[len - chars_to_copy]; \
    for (unsigned int i = 0; i < chars_to_copy ; i++) \
    { \
        unsigned int rem = num % 10; \
        num = num / 10; \
        dst.chars[chars_to_copy - (i + 1)] = rem + '0'; \
    } \
    \
    *len_p += chars_to_copy; \
} while(0)

void neat_bool_tostr_into(Neat_Mut_String_Ref dst, bool *obj)
{
    char *res = *obj ? "true" : "false";
    neat_cstr_tostr_into(dst, &res);
}

void neat_cstr_tostr_into(Neat_Mut_String_Ref dst, char **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) *obj, .len = strlen(*obj)});
}

void neat_ucstr_tostr_into(Neat_Mut_String_Ref dst, unsigned char **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = *obj, .len = strlen((char*) *obj)});
}

void neat_char_tostr_into(Neat_Mut_String_Ref dst, char *obj)
{
    if(dst.cap > 1)
    {
        dst.chars[0] = *obj;
        if(dst.len != NULL)
            *dst.len = 1;
    }
}

void neat_schar_tostr_into(Neat_Mut_String_Ref dst, signed char *obj)
{
    neat_sinteger_tostr_into();
}

void neat_uchar_tostr_into(Neat_Mut_String_Ref dst, unsigned char *obj)
{
    if(dst.cap > 1)
    {
        dst.chars[0] = *obj;
        if(dst.len != NULL)
            *dst.len = 1;
    }
}

void neat_short_tostr_into(Neat_Mut_String_Ref dst, short *obj)
{
    neat_sinteger_tostr_into();
}

void neat_ushort_tostr_into(Neat_Mut_String_Ref dst, unsigned short *obj)
{
    neat_uinteger_tostr_into();
}

void neat_int_tostr_into(Neat_Mut_String_Ref dst, int *obj)
{
    neat_sinteger_tostr_into();
}

void neat_uint_tostr_into(Neat_Mut_String_Ref dst, unsigned int *obj)
{
    neat_uinteger_tostr_into();
}

void neat_long_tostr_into(Neat_Mut_String_Ref dst, long *obj)
{
    neat_sinteger_tostr_into();
}

void neat_ulong_tostr_into(Neat_Mut_String_Ref dst, unsigned long *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%lu", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_llong_tostr_into(Neat_Mut_String_Ref dst, long long *obj)
{
    neat_sinteger_tostr_into();
}

void neat_ullong_tostr_into(Neat_Mut_String_Ref dst, unsigned long long *obj)
{
    neat_uinteger_tostr_into();
}

void neat_float_tostr_into(Neat_Mut_String_Ref dst, float *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%g", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_double_tostr_into(Neat_Mut_String_Ref dst, double *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%g", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_dstr_tostr_into(Neat_Mut_String_Ref dst, Neat_DString *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

void neat_dstr_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_DString **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj[0]->chars, .len = obj[0]->len});
}

void neat_strv_tostr_into(Neat_Mut_String_Ref dst, Neat_String_View *obj)
{
    neat_mutstr_ref_copy(dst, *obj);
}

void neat_strv_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_String_View **obj)
{
    neat_mutstr_ref_copy(dst, **obj);
}

void neat_strbuf_tostr_into(Neat_Mut_String_Ref dst, Neat_String_Buffer *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

void neat_strbuf_ptr_tostr_into(Neat_Mut_String_Ref dst, Neat_String_Buffer **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj[0]->chars, .len = obj[0]->len});
}

void neat_sstr_ref_tostr_into(Neat_Mut_String_Ref dst, Neat_SString_Ref *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->sstring->chars, .len = obj->sstring->len});
}

void neat_mutstr_ref_tostr_into(Neat_Mut_String_Ref dst, Neat_Mut_String_Ref *obj)
{
    unsigned int len = neat_mutstr_ref_len(*obj);
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = len});
}

#endif
