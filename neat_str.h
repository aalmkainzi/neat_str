#ifndef NEAT_STR_H
#define NEAT_STR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "neat_core.h"

// Dynamic String
typedef struct DString_Tag
{
    Neat_Allocator allocator;
    uint64_t cap; /* including the nul */
    uint64_t len;
    unsigned char chars[];
} *DString;

typedef struct String_Buffer
{
    uint64_t cap; /* including the nul */
    uint64_t len;
    unsigned char *chars;
} String_Buffer;

// Used for passing SString around
typedef struct SString_Ref
{
    const uint64_t cap;
    struct {
        uint64_t len;
        unsigned char chars[];
    } *sstring;
} SString_Ref;

typedef struct String_View
{
    uint64_t len;
    unsigned char *chars;
} String_View;

// in C23 structs can be defined multiple times with the same tag and members,
// in which NEAT_DECL_SSTRING is useless, but older standards require it.
#if __STDC_VERSION__ >= 202311L

    #define SString(cap) \
    struct SString##cap \
    { \
        _Static_assert((1##cap##1ul || cap##8ul || 1) && (cap > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure cap is an integer literal */ \
        uint64_t len; \
        unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
    }
    
    #define NEAT_DECL_SSTRING(cap)
#else

    #define SString(cap) \
    struct SString_##cap
    
    #define NEAT_DECL_SSTRING(cap) \
    struct SString_##cap \
    { \
        _Static_assert((1##cap##1ul || cap##8ul || 1) && (cap > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure cap is an integer literal */ \
        uint64_t len; \
        unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
    }
#endif

#define NEAT_SSTRING_NTAG(cap) \
struct \
{ \
    uint64_t len; \
    unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
}

#define NEAT_IS_SSTRING_PTR(to_test) \
( \
neat_has_type(&(typeof((to_test)->chars)){0}, unsigned char(*)[sizeof((to_test)->chars)]) && \
neat_has_type((to_test)->len, uint64_t) && \
(sizeof(*to_test) == sizeof(NEAT_SSTRING_NTAG(sizeof((to_test)->chars) - 1))) && \
_Alignof(typeof(*(to_test))) == _Alignof(typeof(NEAT_SSTRING_NTAG(sizeof((to_test)->chars) - 1))) \
) \

#define NEAT_SSTR_COPY_T(sstr) \
NEAT_SSTRING_NTAG(sizeof((sstr).chars)-1)

#define NEAT_SSTRING_PTR_GENERIC_CASE(s, body) \
default: \
_Generic((char(*)[ 1 + (sizeof(*s) == sizeof(NEAT_SSTRING_NTAG(sizeof(s->chars)-1))) ]){0}, \
    /*char(*)[1]: 0,*/ \
    char(*)[2]: _Generic((char(*)[ 1 + (_Alignof(typeof(*s)) == _Alignof(NEAT_SSTR_COPY_T(*s))) ]){0}, \
            /*char(*)[1]: 0,*/ \
            char(*)[2]: _Generic((char(*)[ 1 + neat_has_type(&s->chars, typeof(&(NEAT_SSTRING_NTAG(sizeof(s->chars)-1)){0}.chars)) ]){0}, \
                /*char(*)[1]: 0,*/ \
                char(*)[2]: body \
            ) \
        ) \
)

#define neat_str_len(s) \
((void)0, \
_Generic(s, \
    char*: strlen(neat_gurantee(s, char*)), \
    unsigned char*: strlen(neat_gurantee(s, unsigned char*)), \
    DString*: neat_gurantee(s, DString*)->len,                \
    String_View*: neat_gurantee(s, String_View*)->len,        \
    String_Buffer*: neat_gurantee(s, String_Buffer*)->len,    \
    SString_Ref: neat_gurantee(s, SString_Ref).sstring->len;  \
    NEAT_SSTRING_PTR_GENERIC_CASE(NEAT_GRNT_SSABLE(s), neat_gurantee_sstr_ptr(s)->len) \
))

NEAT_DECL_SSTRING(1);

#define neat_gurantee_sstr_ptr(any) \
_Generic((char(*)[ NEAT_IS_SSTRING_PTR(any) + 1 ]){0}, \
    char(*)[1]: &(SString(1)){0}, \
    char(*)[2]: any \
)

#define neat_strbuf(any_str)          \
_Generic(any_str,                                 \
    char*         : neat_string_buffer_from_cstr, \
    unsigned char*: neat_string_buffer_from_cstr, \
    DString       : neat_string_buffer_from_dstr, \
    DString*      : neat_string_buffer_from_dstr_ptr, \
    String_View   : neat_string_buffer_from_strv, \
    String_View*  : neat_string_buffer_from_strv_ptr, \
    String_Buffer : neat_string_buffer_from_strbuf, \
    String_Buffer*: neat_string_buffer_from_strbuf_ptr, \
    SString_Ref   : neat_string_buffer_from_sstring_ref, \
    NEAT_SSTRING_PTR_GENERIC_CASE( \
        (NEAT_GRNT_SSABLE(any_str)), \
        neat_string_buffer_from_sstr_ptr \
    ) \
)(any_str)

// gurantee any_str is of a type that has a 'chars' and 'len' members so it can be tested to be an SString
// terrible name
#define NEAT_GRNT_SSABLE(any_str) \
_Generic(any_str,                         \
    char*         : (String_Buffer*){0},  \
    unsigned char*: (String_Buffer*){0},  \
    SString_Ref   : (String_Buffer*){0},  \
    default       : any_str               \
)

#define str_cap(any_str) \
((void)0, \
_Generic(any_str, \
    char*: strlen(neat_gurantee(any_str, char*)),                   \
    unsigned char*: strlen((char*)neat_gurantee(any_str, unsigned char*)), \
    DString       : neat_gurantee(any_str, DString)->cap,                  \
    DString*      : (*neat_gurantee(any_str, DString*))->cap,             \
    String_View   : neat_gurantee(any_str, String_View).len,           \
    String_View*  : neat_gurantee(any_str, String_View*)->len,        \
    String_Buffer : neat_gurantee(any_str, String_Buffer).cap,       \
    SString_Ref   : neat_gurantee(any_str, SString_Ref).cap,           \
    NEAT_SSTRING_PTR_GENERIC_CASE(NEAT_GRNT_SSABLE(any_str), NEAT_CARR_LEN(NEAT_GRNT_SSABLE(any_str)->chars)) \
))

#define sstr_ref(sstr_ptr) \
((void)0, \
_Generic(sstr_ptr, \
    SString_Ref: sstr_ptr, \
    NEAT_SSTRING_PTR_GENERIC_CASE(NEAT_GRNT_SSABLE(sstr_ptr), \
    ((SString_Ref){.cap = str_cap(sstr_ptr), .sstring = (void*) neat_gurantee_not(sstr_ptr, SString_Ref, void*)}) \
    ) \
))

#define sstring_assign(sstr_ptr, rhs) \
( \
neat_static_assertx(NEAT_IS_SSTRING_PTR(sstr_ptr), "arg 1 must be SString(N)* or SString_Ref"), \
_Generic(sstr_ptr, \
    SString_Ref: sstring_ref_assign_, \
    default: sstring_assign_ \
)(sstr_ptr, rhs) \
)

#define neat_string_view(src, ...) \
NEAT_APPEND_NARG(neat_string_view, src, ##__VA_ARGS__)(src, ##__VA_ARGS__)

#define neat_string_view1(src) \
_Generic(src, \
    char*: neat_string_view_from_cstr,                   \
    unsigned char*: neat_string_view_from_cstr, \
    DString: neat_string_view_from_dstr,                  \
    DString*: neat_string_view_from_dstr_ptr,             \
    String_View: neat_string_view_from_strv,           \
    String_View*: neat_string_view_from_strv_ptr,        \
    String_Buffer: neat_gurantee(any_str, String_Buffer).cap,       \
    SString_Ref: neat_gurantee(any_str, SString_Ref).cap,           \
    NEAT_SSTRING_PTR_GENERIC_CASE(NEAT_GRNT_SSABLE(any_str)): (sizeof(NEAT_GRNT_SSABLE(any_str)->chars) / sizeof(NEAT_GRNT_SSABLE(any_str)->chars[0])) \
)

#define neat_string_view2(src, start_idx)

// delete all these below
#define neat_string_view_into_cstring(cstring, ...) \
NEAT_APPEND_NARG(neat_string_view_into_cstring, cstring, ##__VA_ARGS__)(cstring, ##__VA_ARGS__)

#define neat_string_view_into_cstring1(cstring) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*), \
        "arg 1 must be a C string" \
    ); \
    typeof(cstring[0]) *neat_cstring = cstring; \
    uint64_t neat_strlen = strlen((char*)neat_cstring); \
    String_View neat_ret = { 0 }; \
    neat_ret.chars = (unsigned char*)neat_cstring; \
    neat_ret.len = neat_strlen; \
    neat_ret; \
})

#define neat_string_view_into_cstring2(cstring, start) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*), \
        "arg 1 must be a C string" \
    ); \
    _Static_assert( \
        neat_is_integer(start), \
        "arg 2 must be an integer" \
    ); \
    typeof(cstring[0]) *neat_cstring = cstring; \
    uint64_t neat_start = start; \
    uint64_t neat_strlen = strlen((char*)neat_cstring); \
    assert( ((void)"string view out of bounds", neat_start < neat_strlen) ); \
    String_View neat_ret = { 0 }; \
    neat_ret.chars = (unsigned char*)neat_cstring + neat_start; \
    neat_ret.len = neat_strlen - neat_start; \
    neat_ret; \
})

#define neat_string_view_into_cstring3(cstring, start, nb) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*), \
        "arg 1 must be a C string" \
    ); \
    _Static_assert( \
        neat_is_integer(start), \
        "arg 2 must be an integer" \
    ); \
    _Static_assert( \
        neat_is_integer(nb), \
        "arg 3 must be an integer" \
    ); \
    typeof(cstring[0]) *neat_cstring = cstring; \
    uint64_t neat_start = start; \
    uint64_t neat_nb = nb; \
    uint64_t neat_strlen = strlen((char*)neat_cstring); \
    assert( ((void)"string view out of bounds", neat_start + neat_nb <= neat_strlen)); \
    String_View neat_ret = { 0 }; \
    neat_ret.chars = (unsigned char*)neat_cstring + neat_start; \
    neat_ret.len = nb; \
    neat_ret; \
})

// TODO make this handle all string types and pointers to them, then call it in other macros.
// TODO the secret to avoiding the variable name macro issue, is to never pass variables declared in the macro to another macro. OR MAKE SURE NAMES ARE DIFFERENT.

#define neat_string_view_into(string, ...) \
NEAT_APPEND_NARG(neat_string_view_into, string __VA_OPT__(,) __VA_ARGS__)(string, ##__VA_ARGS__)

#define neat_string_view_into1(string) \
({ \
    typeof(neat_gurantee_not(string, VString*, String_View*)) neat_src_not_vstring; \
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(neat_src_not_vstring), \
        "arg 1 must have type VString*, SString(N)*, String_View*, or DString*" \
    ); \
    typeof(string) neat_string = string; \
    String_View neat_ret = { 0 }; \
    neat_ret.len = neat_string->len; \
    neat_ret.chars = neat_string->chars; \
    neat_ret; \
})

#define neat_string_view_into2(string, start) \
({ \
    typeof(neat_gurantee_not(string, VString*, String_View*)) neat_src_not_vstring; \
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(neat_src_not_vstring), \
        "arg 1 must have type VString*, SString(N)*, String_View*, or DString*" \
    ); \
    _Static_assert( \
        neat_is_integer(start), \
        "arg 2 must be an integer" \
    ); \
    typeof(string) neat_string = string; \
    uint64_t neat_start = start; \
    assert(((void)"string view out of bounds", neat_start < neat_string->len)); \
    String_View neat_ret = { 0 }; \
    neat_ret.len = neat_string->len - neat_start; \
    neat_ret.chars = neat_string->chars + neat_start; \
    neat_ret; \
})

#define neat_string_view_into3(string, start, nb) \
({ \
    typeof(neat_gurantee_not(string, VString*, String_View*)) neat_src_not_vstring; \
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(neat_src_not_vstring), \
        "arg 1 must have type VString*, SString(N)*, String_View*, or DString*" \
    ); \
    _Static_assert( \
        neat_is_integer(start), \
        "arg 2 must be an integer" \
    ); \
    _Static_assert( \
        neat_is_integer(nb), \
        "arg 3 must be an integer" \
    ); \
    typeof(string) neat_string = string; \
    uint64_t neat_start = start; \
    uint64_t neat_nb = nb; \
    assert(((void)"string view out of bounds", neat_start < neat_string->len && neat_start + neat_nb <= neat_string->len)); \
    String_View neat_ret = { 0 }; \
    neat_ret.len = neat_nb; \
    neat_ret.chars = neat_string->chars + neat_start; \
    neat_ret; \
})

#define neat_dstring_new(...) \
NEAT_CAT(neat_dstring_new_, __VA_OPT__(wargs))(__VA_ARGS__)

#define neat_dstring_new_wargs(...) \
NEAT_APPEND_NARG(neat_dstring_new_wargs, ##__VA_ARGS__)(__VA_ARGS__)

#define neat_dstring_new_wargs1(cap_or_allocator) \
neat_dstring_new_( \
_Generic(cap_or_allocator, Neat_Allocator: 16, default: cap_or_allocator), \
_Generic(cap_or_allocator, Neat_Allocator: cap_or_allocator, default: neat_get_default_allocator()) \
)

// actually arg cap should be optional and default cap 16
#define neat_dstring_new1(cap) \
neat_dstring_new_(cap, neat_get_default_allocator())

#define neat_dstring_new2(cap, allocator) \
neat_dstring_new_

#define dstring_deinit(dstring) \
({ \
    DString *neat_tofree = dstring; \
    neat_dealloc(neat_tofree->allocator, neat_tofree->chars, unsigned char, neat_tofree->cap); \
})

#define dstring_maybe_grow(dstring, needed) \
({ \
    _Static_assert( \
        neat_has_type(dstring, DString*), \
        "arg 1 must have type DString*" \
    ); \
    DString *neat_to_grow = dstring; \
    if(neat_to_grow->cap - neat_to_grow->len <= needed) \
    { \
        uint64_t neat_newcap; \
        if(neat_to_grow->cap * 2 < neat_to_grow->len + needed + 1) \
        { \
            neat_newcap = neat_to_grow->len + needed + 1; \
        } \
        else \
        { \
            neat_newcap = neat_to_grow->cap * 2; \
        } \
        neat_to_grow->chars = \
            neat_realloc( \
                neat_to_grow->allocator, \
                neat_to_grow->chars, \
                typeof(neat_to_grow->chars[0]), \
                neat_to_grow->cap, \
                neat_newcap \
            ); \
        neat_to_grow->cap = neat_newcap; \
    } \
})

// make append_cstring
#define dstring_assign(dstring, src, ...)

#define dstring_append(dstring, src) \
({ \
    _Static_assert( \
        neat_has_type(dstring, DString*), \
        "arg 1 must have type DString*" \
    ); \
    typeof(src) neat_src_temp = src; \
    typeof(_Generic(neat_src_temp, VString*: (VString*){0}, default: &(typeof(neat_src_temp)){0} )) neat_src = \
    _Generic(neat_src_temp, VString*: neat_src_temp, default: neat_as_pointer(neat_src_temp)); \
    typeof(neat_gurantee_not(neat_src, VString*, String_View*)) neat_src_not_vstring; \
    _Static_assert( \
        neat_has_type(neat_src, DString*) || \
        neat_has_type(neat_src, String_View*) || \
        neat_has_type(neat_src, VString*) || \
        NEAT_IS_SSTRING_PTR(neat_src_not_vstring), \
        "arg 1 must have type VString*, SString(N)*, String_View*, or DString*" \
    ); \
    DString *neat_dst_to_append = dstring; \
    dstring_maybe_grow(neat_dst_to_append, neat_src->len); \
    memmove(neat_dst_to_append->chars + neat_dst_to_append->len, neat_src->chars, neat_src->len); \
    neat_dst_to_append->len = neat_dst_to_append->len + neat_src->len; \
    neat_dst_to_append->chars[ neat_dst_to_append->len ] = '\0'; \
    (void) _Generic(neat_src, \
        DString: dstring_deinit(neat_gurantee(neat_src, DString*)), \
        default: 0 \
    ); \
})

#define dstring_prepend

#define neat_fprint(f, ...) \
({ \
    FILE *neat_file = f; \
    (void) neat_file; \
    NEAT_FOREACH(neat_fprint_, __VA_ARGS__); \
    (void) 0; \
})

#define neat_fprint_(x) \
({ \
    typeof(neat_to_string(x)) neat_tostring_test = neat_to_string(x); \
    typeof(_Generic(neat_tostring_test, \
        VString*: (VString*){0}, \
        default: &(typeof(neat_tostring_test)){0} \
    )) neat_str; \
    neat_str = _Generic(neat_tostring_test, \
        VString*: neat_tostring_test, \
        default: neat_as_pointer(neat_tostring_test) \
    ); \
    fwrite(neat_str->chars, sizeof(neat_str->chars[0]), neat_str->len, neat_file); \
    (void)_Generic(neat_str, \
        DString*: ({ \
            DString *neat_as_dstring = neat_gurantee(neat_str, DString*); \
            neat_dealloc(neat_as_dstring->allocator, neat_as_dstring->chars, typeof(neat_as_dstring->chars[0]), neat_as_dstring->cap); \
        }), \
        default: 0 \
    ); \
});

#define neat_print(...) \
neat_fprint(stdout, __VA_ARGS__)

#define neat_fprintln(f, ...) \
({ \
    neat_fprint(f, __VA_ARGS__); \
    fputc('\n', f); \
    (void) 0; \
})

#define neat_println(...) \
neat_fprintln(stdout, __VA_ARGS__)

#define NEAT_DEFAULT_STRINGABLE_TYPES          \
bool:               neat_tostr_bool,           \
char*:              neat_tostr_str,            \
char:               neat_tostr_char,           \
signed char:        neat_tostr_schar,          \
unsigned char:      neat_tostr_uchar,          \
short:              neat_tostr_short,          \
unsigned short:     neat_tostr_ushort,         \
int:                neat_tostr_int,            \
unsigned int:       neat_tostr_uint,           \
long:               neat_tostr_long,           \
unsigned long:      neat_tostr_ulong,          \
long long:          neat_tostr_llong,          \
unsigned long long: neat_tostr_ullong,         \
float:              neat_tostr_float,          \
double:             neat_tostr_double,         \
DString:            neat_tostr_dstring,        \
DString*:           neat_tostr_dstring_ptr,    \
String_View:        neat_tostr_string_view,    \
String_View*:       neat_tostr_string_view_ptr

#define NEAT_ALL_STRINGABLE_TYPES \
NEAT_IF_DEF(NEAT_TOSTR1)(neat_tostr_type_1: neat_tostr_func_1,)    \
NEAT_IF_DEF(NEAT_TOSTR2)(neat_tostr_type_2: neat_tostr_func_2,)    \
NEAT_IF_DEF(NEAT_TOSTR3)(neat_tostr_type_3: neat_tostr_func_3,)    \
NEAT_IF_DEF(NEAT_TOSTR4)(neat_tostr_type_4: neat_tostr_func_4,)    \
NEAT_IF_DEF(NEAT_TOSTR5)(neat_tostr_type_5: neat_tostr_func_5,)    \
NEAT_IF_DEF(NEAT_TOSTR6)(neat_tostr_type_6: neat_tostr_func_6,)    \
NEAT_IF_DEF(NEAT_TOSTR7)(neat_tostr_type_7: neat_tostr_func_7,)    \
NEAT_IF_DEF(NEAT_TOSTR8)(neat_tostr_type_8: neat_tostr_func_8,)    \
NEAT_IF_DEF(NEAT_TOSTR9)(neat_tostr_type_9: neat_tostr_func_9,)    \
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
NEAT_DEFAULT_STRINGABLE_TYPES

typedef neat_func_ptr(SString(1), neat_fail_type*) neat_tostr_fail;

#define neat_get_tostr_func(ty) \
({ \
    _Static_assert(neat_is_stringable(ty), "type not stringable"); \
    _Generic((ty){0}, \
        NEAT_ALL_STRINGABLE_TYPES \
    ); \
})

#define neat_get_tostr_func_ft(ty) \
_Generic((ty){0}, \
    NEAT_ALL_STRINGABLE_TYPES, \
    default: (neat_tostr_fail){0} \
)

// TODO maybe the solution is to make this only return ptr types, stroing them in the current context
// NO!!! new plan. to_string functions can only return DString. but to_string_buf will be using a _Thread_local thing to take a ptr to any String type
#define neat_to_string(x) \
neat_get_tostr_func(typeof(x))(neat_as_pointer(x))

#define neat_to_string_buf(x, buf, ...) \
neat_get_tostr_func(typeof(x))(neat_as_pointer(x), (String_View){.chars = buf __VA_OPT__(, .len = __VA_ARGS__)})

#define neat_is_stringable(ty) \
(!neat_has_type(neat_get_tostr_func_ft(ty), neat_tostr_fail))

#define neat_tostr_ret(ty) \
typeof(neat_get_tostr_func(ty)(0))

// this seems to work really well. we can even add support for SString(N)* and VString* to all macros above

extern _Thread_local struct Neat_String_Buffer { uint64_t cap; uint64_t len; unsigned char *chars; bool should_write; } neat_buffer;

#define NEAT_TOSTR_BUF_TY(obj_ty) \
uint64_t(*)(typeof(obj_ty)*, String_View)

#define NEAT_TOSTR_DSTR_RET_TY(obj_ty) \
DString(*)(typeof(obj_ty)*)

#define NEAT_TOSTR_SV_RET_TY(obj_ty) \
String_View(*)(typeof(obj_ty)*)

#define NEAT_DECL_TOSTR_FUNC(n) \
typedef typeof(NEAT_ARG1(ADD_TOSTR)) neat_tostr_type_##n; \
static inline typeof( \
    _Generic(NEAT_ARG2(ADD_TOSTR), \
        NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)): (uint64_t){0}, \
        default: neat_gurantee_not(NEAT_ARG2(ADD_TOSTR), NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)), NEAT_TOSTR_SV_RET_TY(NEAT_ARG1(ADD_TOSTR)))(0) \
    ) \
) neat_tostr_func_##n (neat_tostr_type_##n *obj, String_View sv) \
{ \
    return _Generic(NEAT_ARG2(ADD_TOSTR), \
        NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)): ({ \
            neat_gurantee(NEAT_ARG2(ADD_TOSTR), NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)))(obj, sv); \
        }), \
        default: ({ \
            typeof(neat_gurantee_not(NEAT_ARG2(ADD_TOSTR), NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)), NEAT_TOSTR_SV_RET_TY(NEAT_ARG1(ADD_TOSTR)))(obj)) *ret_ty; \
            typeof(_Generic(ret_ty, VString**: (VString*){0}, default: ret_ty)) ret_ty2; \
            typeof(neat_gurantee_not(ret_ty2, VString*, String_View*)) ret_ty3; \
            _Static_assert( \
                neat_has_type(ret_ty2, DString*) || \
                neat_has_type(ret_ty2, String_View*) || \
                neat_has_type(ret_ty2, VString*) || \
                NEAT_IS_SSTRING_PTR(ret_ty3), \
                "provided to_string function has incorrect type, must either be: 'SString(N)/VString*/DString/String_View (T*)' or 'uint64_t (T*, String_View)'" \
            ); \
            neat_gurantee_not(NEAT_ARG2(ADD_TOSTR), NEAT_TOSTR_BUF_TY(NEAT_ARG1(ADD_TOSTR)), NEAT_TOSTR_SV_RET_TY(NEAT_ARG1(ADD_TOSTR)))(obj); \
        }) \
    ); \
}

DString neat_dstring_new_(uint64_t cap, Neat_Allocator allocator);
void neat_string_assign(String_Buffer *dst, String_Buffer *src);

/*
DString neat_tostr_dstring(DString *obj, String_View sv);
DString neat_tostr_dstring_ptr(DString **obj, String_View sv);
DString neat_tostr_string_view(String_View *obj, String_View sv);
DString neat_tostr_string_view_ptr(String_View **obj, String_View sv);
*/

DString neat_tostr_bool(bool *obj); 
DString neat_tostr_str(char **obj);  
DString neat_tostr_char(char *obj); 
DString neat_tostr_schar(signed char *obj);
DString neat_tostr_uchar(unsigned char *obj);
DString neat_tostr_short(short *obj);
DString neat_tostr_ushort(unsigned short *obj);
DString neat_tostr_int(int *obj);  
DString neat_tostr_uint(unsigned int *obj); 
DString neat_tostr_long(long *obj); 
DString neat_tostr_ulong(unsigned long *obj);
DString neat_tostr_llong(long long *obj);
DString neat_tostr_ullong(unsigned long long *obj);
DString neat_tostr_float(float *obj);
DString neat_tostr_double(double *obj);
#endif /* NEAT_STR_H */

#define NEAT_STR_IMPL // TODO DELETE THIS LINE

#if defined(NEAT_STR_IMPL) && !defined(NEAT_STR_IMPLED)
#define NEAT_STR_IMPLED

_Thread_local struct Neat_String_Buffer neat_buffer = { 0 };

#ifndef NEAT_COMMON_UTILS_IMPLED
#define NEAT_COMMON_UTILS_IMPLED

void *neat_default_allocator_alloc(void *ctx, size_t alignment, size_t n)
{
    (void) alignment;
    (void) ctx;
    return malloc(n);
}

void neat_default_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) n;
    free(ptr);
}

void *neat_default_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size)
{
    (void) ctx;
    (void) alignment;
    (void) old_size;
    return realloc(ptr, new_size);
}

void neat_default_allocator_init(void **ctx)
{
    (void) ctx;
}

void neat_default_allocator_deinit(void *ctx)
{
    (void) ctx;
}

void *neat_noop_allocator_alloc(void *ctx, size_t alignment, size_t n)
{
    (void)ctx;
    (void)alignment;
    (void)n;
    return NULL;
}

void neat_noop_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void)ctx;
    (void)ptr;
    (void)n;
}

void *neat_noop_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size)
{
    (void)ctx;
    (void)ptr;
    (void)alignment;
    (void)old_size;
    (void)new_size;
    return NULL;
}

void neat_noop_allocator_init(void **ctx)
{
    (void)ctx;
}

void neat_noop_allocator_deinit(void *ctx)
{
    (void)ctx;
}

#endif /* NEAT_COMMON_UTILS_IMPLED */

void neat_string_assign(String_Buffer *dst, String_Buffer *src)
{
    if(dst->cap <= src->len)
    {
        assert(0 && "destination buffer not big enough");
    }
    
    memmove(dst->chars, src->chars, src->len);
}

DString neat_dstring_new_(uint64_t cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    DString ret = (DString) neat_alloc_bytes(allocator, sizeof(*ret) + cap * sizeof(ret->chars[0]));
    ret->allocator = allocator;
    ret->len = 0;
    ret->cap = cap;
    if(ret != NULL)
    {
        ret->chars[0] = '\0';
    }
    return ret;
}

/*
DString neat_tostr_dstring(DString *obj)
{
    if(!neat_buffer.should_write)
    {
        DString ret = neat_dstring_new(obj[0]->len);
        memcpy(ret->chars, obj[0]->chars, obj[0]->len);
        ret->len = obj[0]->len;
        return ret;
    }
    else
    {
        assert(neat_buffer.cap >= obj[0]->len);
        memmove(neat_buffer.chars, obj[0]->chars, obj[0]->len);
        neat_buffer.len = obj[0]->len;
        return (DString){0};
    }
}

// TODO NEW IDEA: make the tostr functions optioanlly take a second arg, Neat_String_Buffer* that they can fill if should_write is true
// TODO figure out a way to handle utf16 and utf32 in the future
// TODO most of the macros can be functions instead (or at least call functions)
// TODO try to get rid of all statement expressions
// neat_gurantee ... this is A MIS-SPELL should be guarantee
// TODO rename NEAT_COMMON_UTLIS to NEAT_CORE and make it its own lib (or maybe make each neat_lib define its own everything (except Neat_Allocator))

DString neat_tostr_dstring_ptr(DString **obj)
{
    if(!neat_buffer.should_write)
    {
        DString ret = dstring_new((*obj)->len);
        memcpy(ret.chars, (*obj)->chars, (*obj)->len);
        ret.len = (*obj)->len;
        return ret;
    }
    else
    {
        assert(neat_buffer.cap >= (*obj)->len);
        memmove(neat_buffer.chars, (*obj)->chars, (*obj)->len);
        neat_buffer.len = (*obj)->len;
        return (DString){0};
    }
}

String_View neat_tostr_string_view(String_View *obj)
{
    if(neat_buffer == NULL)
    {
        DString ret = dstring_new(obj->len);
        memcpy(ret.chars, obj->chars, obj->len);
        ret.len = obj->len;
        return ret;
    }
    else
    {
        assert(neat_buffer_cap >= obj->len);
        memmove(neat_buffer, obj->chars, obj->len);
        return (DString){0};
    }
}

String_View neat_tostr_string_view_ptr(String_View **obj)
{
    if(neat_buffer == NULL)
    {
        DString ret = dstring_new((*obj)->len);
        memcpy(ret.chars, (*obj)->chars, (*obj)->len);
        ret.len = (*obj)->len;
        return ret;
    }
    else
    {
        assert(neat_buffer_cap >= (*obj)->len);
        memmove(neat_buffer, (*obj)->chars, (*obj)->len);
        return (DString){0};
    }
}

DString neat_tostr_bool(bool *obj)
{
    if(neat_buffer == NULL)
    {
        DString ret = dstring_new(5);
        if(*obj)
        {
            dstring_append(&ret, neat_string_view_into_cstring("true"));
        }
        else
        {
            dstring_append(&ret, neat_string_view_into_cstring("false"));
        }
        return ret;
    }
    else
    {
        if(*obj)
        {
            assert(neat_buffer_cap >= 4);
            memcpy(neat_buffer, "true", 4);
        }
        else
        {
            assert(neat_buffer_cap >= 5);
            memcpy(neat_buffer, "false", 5);
        }
        return (DString){0};
    }
}

// TODO who the f do we know the size written to neat_buffer??!?!?!?
DString neat_tostr_str(char **obj)
{
    if(neat_buffer == NULL)
    {
        String_View as_sv = neat_string_view_into_cstring(*obj);
        DString ret = dstring_new(as_sv.len);
        dstring_append(&ret, as_sv);
        return ret;
    }
    else
    {
        size_t len = strlen(*obj);
        assert(neat_buffer_cap >= len);
        memmove(neat_buffer, *obj, len);
        return (DString){0};
    }
}

SString(1) neat_tostr_char(char *obj, String_View sv)
{
    (void) sv;
    return (SString(1)){.len = 1, .chars = {(char)*obj} };
}

SString(1) neat_tostr_schar(signed char *obj, String_View sv)
{
    (void) sv;
    return (SString(1)){.len = 1, .chars = {(signed char)*obj} };
}

SString(1) neat_tostr_uchar(unsigned char *obj, String_View sv)
{
    (void) sv;
    return (SString(1)){.len = 1, .chars = {*obj} };
}

SString(6) neat_tostr_short(short *obj, String_View sv)
{
    (void) sv;
    SString(6) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%hd", *obj);
    ret.len = len;
    return ret;
}

SString(5) neat_tostr_ushort(unsigned short *obj, String_View sv)
{
    (void) sv;
    SString(5) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%hu", *obj);
    ret.len = len;
    return ret;
}

SString(11) neat_tostr_int(int *obj, String_View sv)
{
    (void) sv;
    SString(11) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%d", *obj);
    ret.len = len;
    return ret;
}

SString(10) neat_tostr_uint(unsigned int *obj, String_View sv)
{
    (void) sv;
    SString(10) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%u", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_long(long *obj, String_View sv)
{
    (void) sv;
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%ld", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_ulong(unsigned long *obj, String_View sv)
{
    (void) sv;
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%lu", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_llong(long long *obj, String_View sv)
{
    (void) sv;
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%lld", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_ullong(unsigned long long *obj, String_View sv)
{
    (void) sv;
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%llu", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_float(float *obj, String_View sv)
{
    (void) sv;
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%g", *obj);
    ret.len = len;
    return ret;
}

SString(30) neat_tostr_double(double *obj, String_View sv)
{
    (void) sv;
    SString(30) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%g", *obj);
    ret.len = len;
    return ret;
}
*/
String_Buffer neat_string_buffer_from_cstr(char *cstr)
{
    size_t len = strlen(cstr);
    return (String_Buffer){
        .cap   = len,
        .len   = len,
        .chars = (unsigned char*) cstr
    };
}

String_Buffer neat_string_buffer_from_dstr(DString dstr)
{
    return (String_Buffer){
        .cap   = dstr->cap,
        .len   = dstr->len,
        .chars = dstr->chars
    };
}

String_Buffer neat_string_buffer_from_strv(String_View *sv)
{
    return (String_Buffer){
        .chars = sv->chars,
        .cap   = sv->len,
        .len   = sv->len
    };
}

String_Buffer neat_string_buffer_from_bstr(String_Buffer *sb)
{
    return *sb;
}

String_Buffer neat_string_buffer_from_sstring_ref(SString_Ref sstr)
{
    return (String_Buffer){
        .cap   = sstr.cap,
        .len   = sstr.sstring->len,
        .chars = sstr.sstring->chars
    };
}

#endif /* NEAT_STR_IMPL */

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
#endif

#undef ADD_HASH
#endif

