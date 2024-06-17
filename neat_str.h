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
#include "neat_core.h"

// Dynamic String
typedef struct DString
{
    Neat_Allocator allocator;
    unsigned int cap; /* including the nul */
    unsigned int len;
    unsigned char *chars;
} DString;

// Used as a general purpose non-dynamic string buffer
typedef struct String_Buffer
{
    unsigned int cap; /* including the nul */
    unsigned int len;
    unsigned char *chars;
} String_Buffer;

// Used to view slices of other strings
typedef struct String_View
{
    unsigned int len;
    unsigned char *chars;
} String_View;

// An array of the above
typedef struct String_View_Array
{
    unsigned int nb;
    String_View *strs;
} String_View_Array;

// Used for passing SString around
typedef struct SString_Ref
{
    const unsigned int cap;
    struct {
        unsigned int len;
        unsigned char chars[];
    } *sstring;
} SString_Ref;

// Can be used as a reference to any string type
// (maybe make this a tagged union instead?)
typedef struct Any_String_Ref
{
    const unsigned int cap;
    unsigned int *len; /* may be NULL */
    unsigned char *chars;
} Any_String_Ref;

// in C23 structs can be defined multiple times with the same tag and members,
// in which NEAT_DECL_SSTRING is useless, but older standards require it.
#if __STDC_VERSION__ >= 202311L

    #define SString(cap) \
    struct SString_##cap \
    { \
        _Static_assert((1##cap##1ul || cap##8ul || 1) && (cap > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure cap is an integer literal */ \
        unsigned int len; \
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
        unsigned int len; \
        unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
    }
#endif

// SString stuff
#define NEAT_SSTRING_NTAG(cap) \
struct \
{ \
    unsigned int len; \
    unsigned char chars[ cap + 1 ]; /* + 1 for the nul */ \
}

#define NEAT_SSTR_COPY_T(sstr) \
NEAT_SSTRING_NTAG(sizeof((sstr).chars)-1)

#define NEAT_SSTRING_PTR_GENERIC_CASE(s, body) \
default: \
_Generic((char(*)[ 1 + (sizeof(*s) == sizeof(NEAT_SSTR_COPY_T(*s))) ]){0}, \
    /*char(*)[1]: 0,*/ \
    char(*)[2]: _Generic((char(*)[ 1 + (_Alignof(typeof(*s)) == _Alignof(NEAT_SSTR_COPY_T(*s))) ]){0}, \
            /*char(*)[1]: 0,*/ \
            char(*)[2]: _Generic((char(*)[ 1 + neat_has_type(&s->chars, typeof(&(NEAT_SSTR_COPY_T(*s)){0}.chars)) ]){0}, \
                /*char(*)[1]: 0,*/ \
                char(*)[2]: body \
            ) \
        ) \
)
// SString stuff end

// this is to cause compile err if type is not mutable string
#define neat_str_assert_mutable(str) \
(void)_Generic(str,                  \
    DString*      : 1,               \
    String_Buffer*: 1,               \
    SString_Ref   : 1,               \
    Any_String_Ref: 1                \
)

#define neat_str_len(any_str) \
strv(any_str).len

#define neat_str_cap(any_str) \
neat_anystr_ref(any_str).cap

#define neat_str_equal(any_str1, any_str2) \
neat_strv_equal(neat_strv(any_str1), neat_strv(any_str2))

#define neat_str_copy(any_str_dst, any_str_src) \
( \
    neat_str_assert_mutable(any_str_dst), \
    neat_anystr_ref_copy(neat_anystr_ref(any_str_dst), neat_strv(any_str_src)) \
)

#define neat_str_concat_new(any_str_1, any_str_2, ...) \
neat_strv_concat_new(neat_strv(any_str_1), neat_strv(any_str_2), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_concat(any_str_dst, any_str_src)                            \
(                                                                            \
neat_str_assert_mutable(any_str_dst),                                        \
neat_anystr_ref_concat(neat_anystr_ref(any_str_dst), neat_strv(any_str_src)) \
)

#define neat_str_concat_all(any_str_dst, strv_arr)                      \
(                                                                       \
neat_str_assert_mutable(any_str_dst),                                   \
neat_anystr_ref_concat_strv_arr(neat_anystr_ref(any_str_dst), strv_arr) \
)

#define neat_str_concat_all_new(strv_arr, ...) \
neat_anystr_ref_concat_strv_arr_new(strv_arr, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_find(any_str_hay, any_str_needle) \
neat_strv_find_strv(neat_strv(any_str_hay), neat_strv(any_str_needle))

#define neat_str_replace(any_str, any_str_target, any_str_replacement)                                       \
(                                                                                                            \
neat_str_assert_mutable(any_str),                                                                            \
neat_anystr_ref_replace(neat_anystr_ref(any_str), neat_strv(any_str_target), neat_strv(any_str_replacement)) \
)

#define neat_str_split(any_str, any_str_delim, ...) \
neat_strv_split(neat_strv(any_str), neat_strv(any_str_delim), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join_new(any_str_delim, strv_arr, ...) \
neat_strv_arr_join_new(neat_strv(any_str_delim), strv_arr, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define neat_str_join(any_str_dst, any_str_delim, strv_arr)                          \
(                                                                                    \
neat_str_assert_mutable(any_str_dst),                                                \
neat_strv_arr_join(neat_anystr_ref(any_str_dst), neat_strv(any_str_delim), strv_arr) \
)

#define neat_str_fread_line(any_str, stream) \
neat_anystr_ref_fread_line(neat_anystr_ref(any_str), stream)

#define neat_str_fread_concat(any_str, stream) \
neat_anystr_ref_concat_fread_line(neat_anystr_ref(any_str), stream)

#define neat_str_read_line(any_str) \
neat_anystr_ref_fread_line(neat_anystr_ref(any_str), stdin)

#define neat_str_read_concat(any_str) \
neat_anystr_ref_concat_fread_line(neat_anystr_ref(any_str), stdout)

#define neat_fprint_str(any_str, stream) \
neat_fprint_strv(neat_strv(any_str), stream)

#define neat_print_str(any_str) \
neat_fprint_str(any_str, stdout)

#define neat_strv_arr(strv_carr, ...)                                                      \
NEAT_IF_EMPTY(                                                                             \
    ((void) _Generic((typeof(strv_carr)*){0}, String_View(*)[NEAT_CARR_LEN(strv_carr)]: 0) \
    ,(String_View_Array){.nb = NEAT_CARR_LEN(strv_carr), .strs = strv_carr}), __VA_ARGS__  \
)                                                                                          \
__VA_OPT__((String_View_Array){.nb = (__VA_ARGS__), .strs = strv_carr})

// strbuf(any_str)
// strbuf(cap)
// strbuf(cap, allocator)
#define neat_strbuf(str_or_cap, ...) \
neat_strbuf_##__VA_OPT__(2)(str_or_cap __VA_OPT__(,) __VA_ARGS__)

#define neat_strbuf_(str_or_cap)               \
_Generic(str_or_cap,                           \
    char*         : neat_strbuf_of_cstr,       \
    unsigned char*: neat_strbuf_of_ucstr,      \
    DString       : neat_strbuf_of_dstr,       \
    DString*      : neat_strbuf_of_dstr_ptr,   \
    String_View   : neat_strbuf_of_strv,       \
    String_View*  : neat_strbuf_of_strv_ptr,   \
    String_Buffer : neat_strbuf_of_strvbuf,    \
    String_Buffer*: neat_strbuf_of_strbuf_ptr, \
    SString_Ref   : neat_strbuf_of_sstr_ref,   \
    Any_String_Ref: neat_strbuf_of_anystr_ref, \
    default       : neat_strbuf_new_default    \
)(str_or_cap)

#define neat_strbuf_2(cap, allocator) \
neat_strbuf_new(cap, allocator)

#define neat_anystr_ref(any_str)                   \
_Generic(any_str,                                  \
    char*         : neat_anystr_ref_to_cstr,       \
    unsigned char*: neat_anystr_ref_to_ucstr,      \
    DString*      : neat_anystr_ref_to_dstr_ptr,   \
    String_View*  : neat_anystr_ref_to_strv_ptr,   \
    String_Buffer*: neat_anystr_ref_to_strbuf_ptr, \
    SString_Ref   : neat_anystr_ref_to_sstr_ref,   \
    Any_String_Ref: neat_anystr_ref_to_anystr_ref  \
)(any_str)

#define neat_sstr_ref(sstr_ptr) \
neat_sstr_ref_from_sstr_ptr(sstr_ptr, sizeof((sstr_ptr)->chars)) \

#define neat_sstr_ref_new(nb)             \
(SString_Ref){                            \
    .cap = nb,                            \
    .sstring=(NEAT_SSTRING_NTAG(nb)){ 0 } \
};

#define neat_strv(any_str, ...) \
NEAT_CAT(neat_strv, NEAT_NARG(any_str __VA_OPT__(,) __VA_ARGS__))(any_str __VA_OPT__(,) __VA_ARGS__)

#define neat_strv1(any_str) \
neat_strv2(any_str, 0)

#define neat_strv2(any_str, start)         \
_Generic(any_str,                          \
    char*         : neat_strv_cstr2,       \
    unsigned char*: neat_strv_ucstr2,      \
    DString       : neat_strv_dstr2,       \
    DString*      : neat_strv_dstr_ptr2,   \
    String_View   : neat_strv_strv2,       \
    String_View*  : neat_strv_strv_ptr2,   \
    String_Buffer : neat_strv_strbuf2,     \
    String_Buffer*: neat_strv_strbuf_ptr2, \
    SString_Ref   : neat_strv_sstr_ref2,   \
    Any_String_Ref: neat_strv_anystr_ref2  \
)(any_str, start)

#define neat_strv3(any_str, start, end)         \
_Generic(any_str,                          \
    char*         : neat_strv_cstr3,       \
    unsigned char*: neat_strv_ucstr3,      \
    DString       : neat_strv_dstr3,       \
    DString*      : neat_strv_dstr_ptr3,   \
    String_View   : neat_strv_strv3,       \
    String_View*  : neat_strv_strv_ptr3,   \
    String_Buffer : neat_strv_strbuf3,     \
    String_Buffer*: neat_strv_strbuf_ptr3, \
    SString_Ref   : neat_strv_sstr_ref3,   \
    Any_String_Ref: neat_strv_anystr_ref3, \
)(any_str, start, end)

// TODO this should have 4 overloads dstr() dstr(cap) dstr(allocator) dstr(cap, allocator)
#define neat_dstr(...) \
neat_dstr0##__VA_OPT__(1)(__VA_ARGS__)

#define neat_dstr0() \
neat_dstr_new(16, neat_get_default_allocator())

#define neat_dstr01(cap_or_allocator, ...)                                             \
__VA_OPT__(neat_dstr2(cap_or_allocator, __VA_ARGS__))                                  \
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
)                                                                                 \
)

#define neat_dstr2(cap, allocator) \
neat_dstr_new(cap, allocator)

#define neat_dstr_deinit(dstr) \
neat_dstr_deinit_(dstr)

#define neat_dstr_append(dstr, any_str) \
neat_dstr_append_strv(dstr, neat_strv(any_str))

// TODO this leaks. change it later
#define neat_dstr_append_tostr(dstr, stringable) \
dstr_append(dstr, neat_tostr(stringable))

#define neat_dstr_append_tostr_p(dstr, stringable_ptr) \
dstr_append(dstr, neat_tostr_p(stringable))

#define neat_dstr_prepend(dstr, any_str) \
neat_dstr_prepend_strv(dstr, neat_strv(any_str))

#define neat_dstr_prepend_tostr(dstr, stringable) \
dstr_prepend(dstr, neat_tostr(stringable))

#define neat_dstr_prepend_tostr_p(dstr, stringable_ptr) \
dstr_prepend(dstr, neat_tostr_p(stringable_ptr))

#define neat_dstr_insert(dstr, any_str, idx) \
neat_dstr_insert_strv(dstr, neat_strv(any_str), idx)

#define neat_dstr_insert_tostr(dstr, stringable, idx) \
dstr_insert(dstr, neat_tostr(stringable), idx)

#define neat_dstr_insert_tostr_p(dstr, stringable_ptr, idx) \
dstr_insert(dstr, neat_tostr_p(stringable_ptr), idx)

// for now ignore tostrbuf

// IDEA for print:
// fprint(FILE*, int nb, ...String_View)
// no. make it do {} while(0) macro
#define neat_fprint(f, ...) \
({ \
    FILE *neat_file = f; \
    (void) neat_file; \
    NEAT_FOREACH(neat_fprint_, __VA_ARGS__); \
    (void) 0; \
})

#define neat_fprint_(x) \
({ \
    typeof(neat_tostr(x)) neat_tostring_test = neat_tostr(x); \
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

#define neat_tostr(x) \
neat_tostr_p(neat_as_pointer(x))

#define neat_tostr_p(xp) \
neat_get_tostr_func(typeof(*xp))(xp)

// TODO REDO THIS WHOLE SHIT
#define neat_tostr_buf(x, buf, ...) \
neat_get_tostr_func(typeof(x))(neat_as_pointer(x), (String_View){.chars = buf __VA_OPT__(, .len = __VA_ARGS__)})

#define neat_is_stringable(ty) \
(!neat_has_type(neat_get_tostr_func_ft(ty), neat_tostr_fail))

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

String_View neat_strv_cstr2(char *str, unsigned int start);
String_View neat_strv_ucstr2(unsigned char *str, unsigned int start);
String_View neat_strv_dstr2(DString str, unsigned int start);
String_View neat_strv_dstr_ptr2(DString *str, unsigned int start);
String_View neat_strv_strv2(String_View str, unsigned int start);
String_View neat_strv_strv_ptr2(String_View *str, unsigned int start);
String_View neat_strv_strbuf2(String_Buffer str, unsigned int start);
String_View neat_strv_strbuf_ptr2(String_Buffer *str, unsigned int start);
String_View neat_strv_sstr_ref2(SString_Ref str, unsigned int start);
String_View neat_strv_anystr_ref2(Any_String_Ref str, unsigned int start);

String_View neat_strv_cstr3(char *str, unsigned int start, unsigned int end);
String_View neat_strv_ucstr3(unsigned char *str, unsigned int start, unsigned int end);
String_View neat_strv_dstr3(DString str, unsigned int start, unsigned int end);
String_View neat_strv_dstr_ptr3(DString *str, unsigned int start, unsigned int end);
String_View neat_strv_strv3(String_View str, unsigned int start, unsigned int end);
String_View neat_strv_strv_ptr3(String_View *str, unsigned int start, unsigned int end);
String_View neat_strv_strbuf3(String_Buffer str, unsigned int start, unsigned int end);
String_View neat_strv_strbuf_ptr3(String_Buffer *str, unsigned int start, unsigned int end);
String_View neat_strv_sstr_ref3(SString_Ref str, unsigned int start, unsigned int end);
String_View neat_strv_anystr_ref3(Any_String_Ref str, unsigned int start, unsigned int end);

String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator);
String_Buffer neat_strbuf_new_default(unsigned int cap);

String_Buffer neat_strbuf_of_cstr(char *str);
String_Buffer neat_strbuf_of_ucstr(unsigned char *str);
String_Buffer neat_strbuf_of_dstr(DString str);
String_Buffer neat_strbuf_of_dstr_ptr(DString *str);
String_Buffer neat_strbuf_of_strv(String_View str);
String_Buffer neat_strbuf_of_strv_ptr(String_View *str);
String_Buffer neat_strbuf_of_strbuf(String_Buffer str);
String_Buffer neat_strbuf_of_strbuf_ptr(String_Buffer *str);
String_Buffer neat_strbuf_of_sstr_ref(SString_Ref str);
String_Buffer neat_strbuf_of_anystr_ref(Any_String_Ref str);

Any_String_Ref neat_anystr_ref_to_cstr(char *str);
Any_String_Ref neat_anystr_ref_to_ucstr(unsigned char *str);
Any_String_Ref neat_anystr_ref_to_dstr_ptr(DString *str);
Any_String_Ref neat_anystr_ref_to_strv_ptr(String_View *str);
Any_String_Ref neat_anystr_ref_to_strbuf_ptr(String_Buffer *str);
Any_String_Ref neat_anystr_ref_to_sstr_ref(SString_Ref str);
Any_String_Ref neat_anystr_ref_to_anystr_ref(Any_String_Ref str);

bool neat_is_strv_intersect(String_View base, String_View sub);

DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator);
void neat_dstr_deinit_(DString *dstr);
void neat_dstr_append_strv(DString *dstr, String_View str);
void neat_dstr_prepend_strv(DString *dstr, String_View str);
void neat_dstr_insert_strv(DString *dstr, String_View str, unsigned int idx);

SString_Ref neat_sstr_ref_from_sstr_ptr(void *sstr_ptr, unsigned int cap);

unsigned int neat_anystr_ref_copy(Any_String_Ref dst, String_View src);
unsigned int neat_anystr_ref_concat(Any_String_Ref dst, String_View src);
unsigned int neat_anystr_ref_concat_strv_arr(Any_String_Ref dst, String_View_Array src);
DString neat_strv_concat_new(String_View str1, String_View str2, Neat_Allocator allocator);
DString neat_anystr_ref_concat_strv_arr_new(String_View_Array src, Neat_Allocator allocator);

String_View_Array neat_strv_split(String_View str, String_View delim, Neat_Allocator allocator);
DString neat_strv_arr_join_new(String_View delim, String_View_Array strs, Neat_Allocator allocator);
unsigned int neat_strv_arr_join(Any_String_Ref dst, String_View delim, String_View_Array strs);

bool neat_strv_equal(String_View str1, String_View str2);
String_View neat_strv_find_strv(String_View hay, String_View needle);

unsigned int neat_anystr_ref_fread_line(Any_String_Ref dst, FILE *stream);
unsigned int neat_anystr_ref_concat_fread_line(Any_String_Ref dst, FILE *stream);
unsigned int neat_fprint_strv(String_View str, FILE *stream);

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
    (void) ctx;
    (void) alignment;
    (void) n;
    return NULL;
}

void neat_noop_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) ptr;
    (void) n;
}

void *neat_noop_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size)
{
    (void) ctx;
    (void) ptr;
    (void) alignment;
    (void) old_size;
    (void) new_size;
    return NULL;
}

void neat_noop_allocator_init(void **ctx)
{
    (void) ctx;
}

void neat_noop_allocator_deinit(void *ctx)
{
    (void) ctx;
}

#endif /* NEAT_COMMON_UTILS_IMPLED */

static inline unsigned int neat_uint_min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

static inline unsigned int neat_uint_max(unsigned int a, unsigned int b)
{
    return a > b ? a : b;
}

bool neat_is_strv_intersect(String_View base, String_View sub)
{
    uintptr_t start = (uintptr_t) base.chars;
    uintptr_t end   = (uintptr_t) base.chars + base.len;
    uintptr_t sub_start = (uintptr_t) sub.chars;
    return sub_start >= start && sub_start <= end;
}

DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    DString ret = {
        .allocator = allocator,
        .cap = cap,
        .len = 0,
    };
    ret.chars = neat_alloc(allocator, unsigned char, cap);
    if(ret.chars != NULL)
    {
        ret.chars[0] = '\0';
    }
    return ret;
}

void neat_dstr_deinit_(DString *dstr)
{
    neat_dealloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap);
    dstr->allocator.deinit(&dstr->allocator.ctx);
}

void neat_dstr_maybe_grow(DString *dstr, unsigned int len_to_append)
{
    if(dstr->cap - dstr->len <= len_to_append)
    {
        // grow
        unsigned int new_cap = neat_uint_max(dstr->cap * 2, dstr->cap + len_to_append);
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap);
        dstr->cap = new_cap;
    }
}

void neat_dstr_append_strv(DString *dstr, String_View str)
{
    String_View to_append = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_append = (String_View){
            .len = str.len, 
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

void neat_dstr_prepend_strv(DString *dstr, String_View str)
{
    String_View to_prepend = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_prepend = (String_View){
            .len = str.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_prepend.len);
    }
    
    memmove(dstr->chars + str.len, dstr->chars, dstr->len);
    memmove(dstr->chars, str.chars, str.len);
    
    dstr->len += str.len;
    dstr->chars[dstr->len] = '\0';
}

void neat_dstr_insert_strv(DString *dstr, String_View str, unsigned int idx)
{
    if(idx > dstr->len)
    {
        fprintf(stderr, "ERROR: dstr_insert index %u out of range %u\n", idx, str.len);
        exit(1);
    }
    
    String_View to_insert = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_insert = (String_View){
            .len = str.len, 
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
}

bool neat_strv_equal(String_View str1, String_View str2)
{
    return
    (str1.len == str2.len) &&
    (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

String_View neat_strv_find_strv(String_View hay, String_View needle)
{
    unsigned int end = hay.len - needle.len;
    for(unsigned int i = 0 ; i < end ; i++)
    {
        String_View sv = neat_strv_strv_ptr3(&hay, i, i + needle.len);
        if(neat_strv_equal(needle, sv))
        {
            return sv;
        }
    }
    return (String_View){
        .len=0,
        .chars = NULL
    };
}

SString_Ref neat_sstr_ref_from_sstr_ptr(void *sstr_ptr, unsigned int cap)
{
    SString_Ref ret = {.cap = cap};
    ret.sstring = (typeof(ret.sstring)) sstr_ptr;
    return ret;
}

unsigned int neat_anystr_ref_copy(Any_String_Ref dst, String_View src)
{
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    dst.chars[chars_to_copy] = '\0';
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    return chars_to_copy;
}

unsigned int neat_anystr_ref_concat(Any_String_Ref dst, String_View src)
{
    unsigned int len;
    if(dst.len != NULL)
    {
        len = *dst.len;
    }
    else
    {
        len = strlen((char*) dst.chars);
    }
    
    if(len >= dst.cap - 1)
        return 0;
    
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - len - 1);
    memmove(dst.chars + len, src.chars, chars_to_copy);
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    dst.chars[chars_to_copy] = '\0';
    
    return chars_to_copy;
}

DString neat_strv_concat_new(String_View str1, String_View str2, Neat_Allocator allocator)
{
    DString ret = neat_dstr_new(str1.len + str2.len + 1, allocator);
    neat_dstr_append_strv(&ret, str1);
    neat_dstr_append_strv(&ret, str2);
    return ret;
}

unsigned int neat_anystr_ref_concat_strv_arr(Any_String_Ref dst, String_View_Array src)
{
    unsigned int total_chars_copied = 0;
    unsigned int chars_copied = 1;
    
    for(unsigned int i = 0 ; i < src.nb && chars_copied != 0 ; i++)
    {
        chars_copied = neat_anystr_ref_concat(dst, src.strs[i]);
        total_chars_copied += chars_copied;
    }
    
    return total_chars_copied;
}

DString neat_anystr_ref_concat_strv_arr_new(String_View_Array src, Neat_Allocator allocator)
{
    // lazy solution, fix later.
    return neat_strv_arr_join_new(neat_strv_cstr2((char*) "", 0), src, allocator);
}

String_View_Array neat_strv_split(String_View str, String_View delim, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    
    if(delim.len > str.len)
    {
        String_View *copy = neat_alloc(allocator, String_View, 1);
        *copy = str;
        return (String_View_Array){
            .nb   = 1,
            .strs = copy
        };
    }
    
    unsigned int nb_delim = 0;
    unsigned int *delim_idx = (unsigned int*) calloc(str.len, sizeof(unsigned int));
    
    for(unsigned int i = 0 ; i < str.len - delim.len ; )
    {
        String_View sub = neat_strv_strv_ptr3(&str, i, i + delim.len);
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
    
    String_View_Array ret = { 0 };
    ret.strs = neat_alloc(allocator, String_View, nb_delim + 1);
    
    ret.nb = nb_delim + 1;
    
    ret.strs[0] = neat_strv_strv_ptr3(&str, 0, delim_idx[0]);
    for(unsigned int i = 1 ; i <= nb_delim ; i++)
    {
        ret.strs[i] = neat_strv_strv_ptr3(&str, delim_idx[i - 1] + delim.len, delim_idx[i]);
    }
    
    free(delim_idx);
    return ret;
}

DString neat_strv_arr_join_new(String_View delim, String_View_Array strs, Neat_Allocator allocator)
{
    DString ret = neat_dstr_new(16, allocator);
    
    if(strs.nb > 0)
        neat_dstr_append_strv(&ret, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.nb ; i++)
    {
        neat_dstr_append_strv(&ret, delim);
        neat_dstr_append_strv(&ret, strs.strs[i]);
    }
    
    return ret;
}

unsigned int neat_strv_arr_join(Any_String_Ref dst, String_View delim, String_View_Array strs)
{
    if(dst.len != NULL) *dst.len = 0;
    
    unsigned int chars_copied = 0;
    
    if(strs.nb > 0)
        chars_copied += neat_anystr_ref_concat(dst, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.nb ; i++)
    {
        chars_copied += neat_anystr_ref_concat(dst, delim);
        chars_copied += neat_anystr_ref_concat(dst, strs.strs[i]);
    }
    
    dst.chars[chars_copied] = '\0';
    
    return chars_copied;
}

Any_String_Ref neat_anystr_ref_to_cstr(char *str)
{
    unsigned int len = strlen(str);
    
    return (Any_String_Ref){
        .cap   = len,
        .len   = NULL,
        .chars = (unsigned char*) str
    };
}

Any_String_Ref neat_anystr_ref_to_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    
    return (Any_String_Ref){
        .cap   = len,
        .len   = NULL,
        .chars = str
    };
}

Any_String_Ref neat_anystr_ref_to_dstr_ptr(DString *str)
{
    return (Any_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Any_String_Ref neat_anystr_ref_to_strv_ptr(String_View *str)
{
    return (Any_String_Ref){
        .cap   = str->len,
        .len   = &str->len,
        .chars = str->chars
    };
}

Any_String_Ref neat_anystr_ref_to_strbuf_ptr(String_Buffer *str)
{
    return (Any_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Any_String_Ref neat_anystr_ref_to_sstr_ref(SString_Ref str)
{
    return (Any_String_Ref){
        .cap   = str.cap,
        .len   = NULL,
        .chars = str.sstring->chars
    };
}

Any_String_Ref neat_anystr_ref_to_anystr_ref(Any_String_Ref str)
{
    return str;
}

String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    return (String_Buffer){
        .cap   = cap,
        .len   = 0,
        .chars = neat_alloc(allocator, unsigned char, cap)
    };
}

String_Buffer neat_strbuf_new_default(unsigned int cap)
{
    return neat_strbuf_new(cap, neat_get_default_allocator());
}

String_Buffer neat_strbuf_of_cstr(char *str)
{
    unsigned int len = strlen(str);
    return (String_Buffer){
        .cap   = len + 1,
        .len   = len,
        .chars = (unsigned char*) str
    };
}

String_Buffer neat_strbuf_of_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    return (String_Buffer){
        .cap   = len + 1,
        .len   = len,
        .chars = (unsigned char*) str
    };
}

String_Buffer neat_strbuf_of_dstr_ptr(DString *str)
{
    return (String_Buffer){
        .cap   = str->cap,
        .len   = str->len,
        .chars = str->chars
    };
}

String_Buffer neat_strbuf_of_strv_ptr(String_View *str)
{
    return (String_Buffer){
        .cap   = str->len,
        .len   = str->len,
        .chars = str->chars
    };
}

String_Buffer neat_strbuf_of_strbuf_ptr(String_Buffer *str)
{
    return *str;
}

String_Buffer neat_strbuf_of_sstr_ref(SString_Ref str)
{
    return (String_Buffer){
        .cap   = str.cap,
        .len   = str.sstring->len,
        .chars = str.sstring->chars
    };
}

String_Buffer neat_strbuf_of_anystr_ref(Any_String_Ref str)
{
    return (String_Buffer){
        .cap   = str.cap,
        .len   = str.len != NULL ? *str.len : (unsigned int) strlen((char*) str.chars),
        .chars = str.chars
    };
}

String_Buffer neat_strbuf_of_dstr(DString str)
{
    return neat_strbuf_of_dstr_ptr(&str);
}

String_Buffer neat_strbuf_of_strv(String_View str)
{
    return neat_strbuf_of_strv_ptr(&str);
}

String_Buffer neat_strbuf_of_strbuf(String_Buffer str)
{
    return str;
}

String_View neat_strv_cstr2(char *str, unsigned int start)
{
    unsigned int len = strlen(str);
    
    if(start > len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, len);
        exit(1);
    }
    
    return (String_View){
        .len   = len - start,
        .chars = (unsigned char*) str + start
    };
}

String_View neat_strv_ucstr2(unsigned char *str, unsigned int start)
{
    unsigned int len = strlen((char*) str);
    
    if(start > len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, len);
        exit(1);
    }
    
    return (String_View){
        .len   = len - start,
        .chars = str + start
    };
}

String_View neat_strv_dstr2(DString str, unsigned int start)
{
    return neat_strv_dstr_ptr2(&str, start);
}

String_View neat_strv_dstr_ptr2(DString *str, unsigned int start)
{
    if(start > str->len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strv_ptr2(String_View *str, unsigned int start)
{
    if(start > str->len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strbuf_ptr2(String_Buffer *str, unsigned int start)
{
    if(start > str->len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_sstr_ref2(SString_Ref str, unsigned int start)
{
    if(start > str.sstring->len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, str.sstring->len);
        exit(1);
    }
    
    return (String_View){
        .len   = str.sstring->len   - start,
        .chars = str.sstring->chars + start
    };
}

String_View neat_strv_anystr_ref2(Any_String_Ref str, unsigned int start)
{
    unsigned int len = 0;
    if(str.len == NULL)
    {
        len = strlen((char*) str.chars);
    }
    else
    {
        len = *str.len;
    }
    
    if(start > len)
    {
        fprintf(stderr, "ERROR: strv start index %u exceeds string length %u\n", start, len);
        exit(1);
    }
    
    return (String_View){
        .len   = len       - start,
        .chars = str.chars + start
    };
}

String_View neat_strv_cstr3(char *str, unsigned int start, unsigned int end)
{
    unsigned int len = strlen(str);
    
    if(start > len || end > len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = (unsigned char*) str + start
    };
}

String_View neat_strv_ucstr3(unsigned char *str, unsigned int start, unsigned int end)
{
    unsigned int len = strlen((char*) str);
    
    if(start > len || end > len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str + start
    };
}

String_View neat_strv_dstr_ptr3(DString *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strv_ptr3(String_View *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strbuf_ptr3(String_Buffer *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, str->len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_sstr_ref3(SString_Ref str, unsigned int start, unsigned int end)
{
    if(start > str.sstring->len || end > str.sstring->len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, str.sstring->len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str.sstring->chars + start
    };
}

String_View neat_strv_anystr_ref3(Any_String_Ref str, unsigned int start, unsigned int end)
{
    unsigned int len = 0;
    if(str.len == NULL)
    {
        len = strlen((char*) str.chars);
    }
    else
    {
        len = *str.len;
    }
    
    if(start > len || end > len || end > start)
    {
        fprintf(stderr, "ERROR: strv start/end (%u, %u) exceeds string length %u\n", start, end, len);
        exit(1);
    }
    
    return (String_View){
        .len   = end - start,
        .chars = str.chars + start
    };
}

String_View neat_strv_dstr3(DString str, unsigned int start, unsigned int end)
{
    return neat_strv_dstr_ptr3(&str, start, end);
}

String_View neat_strv_strv3(String_View str, unsigned int start, unsigned int end)
{
    return neat_strv_strv_ptr3(&str, start, end);
}

String_View neat_strv_strbuf3(String_Buffer str, unsigned int start, unsigned int end)
{
    return neat_strv_strbuf_ptr3(&str, start, end);
}

unsigned int neat_anystr_ref_fread_line(Any_String_Ref dst, FILE *stream)
{
    unsigned int len;
    if(dst.len != NULL)
    {
        len = *dst.len;
    }
    else
    {
        len = strlen((char*) dst.chars);
    }
    
    unsigned char prev_len = len;
    int c = 0;
    while(c != '\n' && !feof(stream))
    {
        c = fgetc(stream);
        dst.chars[len] = c;
        len += 1;
    }
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    unsigned int chars_read = len - prev_len;
    return chars_read;
}

unsigned int neat_anystr_ref_concat_fread_line(Any_String_Ref dst, FILE *stream)
{
    unsigned int dst_len;
    if(dst.len != NULL)
    {
        dst_len = *dst.len;
    }
    else
    {
        dst_len = strlen((char*) dst.chars);
    }
    
    unsigned int concated_len = 0;
    
    Any_String_Ref right = {
        .cap = dst.cap - dst_len,
        .len = &concated_len
    };
    
    right.chars = dst.chars + dst_len;
    unsigned int chars_read = neat_anystr_ref_fread_line(right, stream);
    dst_len += chars_read;
    
    if(dst.len != NULL)
    {
        *dst.len = dst_len;
    }
    
    dst.chars[dst_len] = '\0';
    
    return chars_read;
}

unsigned int neat_fprint_strv(String_View str, FILE *stream)
{
    return fwrite(str.chars, sizeof(unsigned char), str.len, stream);
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
