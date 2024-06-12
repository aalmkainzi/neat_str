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
typedef struct DString
{
    Neat_Allocator allocator;
    uint64_t cap; /* including the nul */
    uint64_t len;
    unsigned char *chars;
} DString;

// Used as a general purpose non-dynamic string buffer
typedef struct String_Buffer
{
    uint64_t cap; /* including the nul */
    uint64_t len;
    unsigned char *chars;
} String_Buffer;

// Used to view slices of other strings
typedef struct String_View
{
    uint64_t len;
    unsigned char *chars;
} String_View;

// An array of the above
typedef struct String_View_Array
{
    size_t nb;
    String_View strs[];
} String_View_Array;

// Used for passing SString around
typedef struct SString_Ref
{
    const uint64_t cap;
    struct {
        uint64_t len;
        unsigned char chars[];
    } *sstring;
} SString_Ref;

// Can be used as a reference to any string type
// (maybe make this a tagged union instead?)
typedef struct Any_String_Ref
{
    const uint64_t cap;
    uint64_t *len; /* may be NULL */
    unsigned char *chars;
} Any_String_Ref;

// in C23 structs can be defined multiple times with the same tag and members,
// in which NEAT_DECL_SSTRING is useless, but older standards require it.
#if __STDC_VERSION__ >= 202311L

    #define SString(cap) \
    struct SString_##cap \
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

// stupid stuff, don't look
#define NEAT_SSTRING_NTAG(cap) \
struct \
{ \
    uint64_t len; \
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
// stupid stuff end

#define neat_str_len(s) \
((void)0, \
_Generic(s, \
    char*:          strlen(neat_gurantee(s, char*)),            \
    unsigned char*: strlen(neat_gurantee(s, unsigned char*)),   \
    DString*:       neat_gurantee(s, DString*)->len,            \
    String_View*:   neat_gurantee(s, String_View*)->len,        \
    String_Buffer*: neat_gurantee(s, String_Buffer*)->len,      \
    SString_Ref:    neat_gurantee(s, SString_Ref).sstring->len; \
))

#define str_cap(any_str) \
((void)0, \
_Generic(any_str, \
    char*         : strlen(neat_gurantee(any_str, char*)),                      \
    unsigned char*: strlen((char*)neat_gurantee(any_str, unsigned char*)),      \
    DString*      : (*neat_gurantee(any_str, DString*))->cap,                   \
    String_View*  : neat_gurantee(any_str, String_View*)->len,                  \
    String_Buffer*: neat_gurantee(any_str, String_Buffer*)->cap,                \
    SString_Ref   : neat_gurantee(any_str, SString_Ref).cap,                    \
    Any_String_Ref: neat_anystr_ref_cap(neat_gurantee(any_str, Any_String_Ref)) \
))

#define str_equal(any_str1, any_str2) \
neat_strv_equal(strv(any_str1), strv(any_str2))

#define str_copy(any_str_dst, any_str_src) \
neat_anystr_ref_copy(neat_anystr_ref(any_str_dst), strv(any_str_src))

// TODO consider naming mut_str instead of any_str for macros that mutate

// this is to cause compile err if type is not mutable
#define neat_str_assert_mutable(str) \
(void)_Generic(str, \
    DString*      : 1 \
    String_Buffer*: 1 \
    SString_Ref   : 1 \
    Any_String_Ref: 1 \
)

// TODO idea: delete SString and keep only SString_Ref, u use it like:
// SString_Ref ref = sstr_ref(10); // makes an SString on the stack

// TODO make this fail with String_View
#define str_concat(any_str_dst, any_str_src) \
( \
neat_str_assert_mutable(any_str_dst), \
neat_anystr_ref_concat(neat_anystr_ref(any_str_dst), strv(any_str_src)) \
)

#define str_find(any_str_hay, any_str_needle) \
neat_strv_find_strv(strv(any_str_hay), strv(any_str_needle))

#define str_replace(any_str, any_str_target, any_str_replacement) \
( \
neat_str_assert_mutable(any_str), \
neat_anystr_ref_replace(neat_anystr_ref(any_str), strv(any_str_target), strv(any_str_replacement)) \
)

#define str_split(any_str, any_str_delim, ...) \
neat_strv_split(strv(any_str), strv(any_str_delim), NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

#define str_join(any_str_delim, ...) \
_Generic(NEAT_ARG1(__VA_ARGS__), \
    Neat_Allocator: neat_strvs_join(strv(any_str_delim), neat_gurantee(NEAT_ARG1(__VA_ARGS__), Neat_Allocator), NEAT_FOREACH(strv, NEAT_OMIT1(__VA_ARGS__))), \
    default       : neat_strvs_join(strv(any_str_delim), NEAT_FOREACH(strv, __VA_ARGS__)) \
)

#define str_join_into(any_str_dst, any_str_delim, ...) \
neat_strvs_join_into(neat_anystr_ref(any_str_dst), strv(any_str_delim), NEAT_FOREACH(strv, __VA_ARGS__))

#define str_read(any_str, stream) \
neat_anystr_ref_fread(neat_anystr_ref(any_str), stream)

#define str_read_concat(any_str, stream) \
neat_anystr_ref_fread_concat(neat_anystr_ref(any_str), stream)

#define str_write(any_str, stream) \
neat_strv_fwrite(strv(any_str), stream)

// strbuf(any_str)
// strbuf(cap)
// strbuf(cap, allocator)
#define neat_strbuf(str_or_cap, ...)                             \
_Generic((void(*)(typeof(str_or_cap))){0}),                      \
    void(*)(char*         ): neat_strbuf_of_cstr,                \
    void(*)(unsigned char*): neat_strbuf_of_cstr,                \
    void(*)(DString*      ): neat_strbuf_of_dstr,                \
    void(*)(String_View*  ): neat_strbuf_of_strv,                \
    void(*)(String_Buffer*): neat_strbuf_of_strbuf,              \
    void(*)(SString_Ref   ): neat_strbuf_of_sstr_ref,            \
    default                : neat_strbuf_new                     \
)(str_or_cap __VA_OPT__(,) __VA_ARGS__ NEAT_IF_EMPTY(neat_get_default_allocator(), __VA_ARGS__))

// _Generic(NEAT_U64_IF_INTEGER(str_or_cap),                                                     \
//     uint64_t: neat_strbuf_new(                                                                \
//                 neat_gurantee(str_or_cap, uint64_t),                                          \
//                 NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__)                         \
//               )                                                                               \
//     default:                                                                                  \
//         _Generic(str_or_cap,                                                                  \
//             char*         : neat_strbuf_of_cstr(neat_gurantee(str_or_cap, char*)),            \
//             unsigned char*: neat_strbuf_of_cstr(neat_gurantee(str_or_cap, unsigned char*)),   \
//             DString*      : neat_strbuf_of_dstr(neat_gurantee(str_or_cap, DString*)),         \
//             String_View*  : neat_strbuf_of_strv(neat_gurantee(str_or_cap, String_View*)),     \
//             String_Buffer*: neat_strbuf_of_strbuf(neat_gurantee(str_or_cap, String_Buffer*)), \
//             SString_Ref   : neat_strbuf_of_sstr_ref(neat_gurantee(str_or_cap, SString_Ref))   \
//         )                                                                                     \
// )


#define neat_anystr_ref(any_str)                 \
_Generic(any_str,                                \
    char*         : neat_anystr_ref_to_cstr,     \
    unsigned char*: neat_anystr_ref_to_cstr,     \
    DString*      : neat_anystr_ref_to_dstr,     \
    String_View*  : neat_anystr_ref_to_strv,     \
    String_Buffer*: neat_anystr_ref_to_strbuf,   \
    SString_Ref   : neat_anystr_ref_to_sstr_ref \
)(any_str)

#define sstr_ref(sstr_ptr) \
neat_sstr_ref_from_sstr_ptr(sstr_ptr, sizeof((sstr_ptr)->chars)) \

#define sstr_ref_new(nb) \
(SString_Ref){ \
    .cap = nb, \
    .sstring=(NEAT_SSTRING_NTAG(nb)){ 0 } \
};

#define strv(any_str, ...) \
NEAT_CAT(strv, NEAT_NARG(any_str __VA_OPT__(,) __VA_ARGS__))(any_str __VA_OPT__(,) __VA_ARGS__)

#define strv1(any_str) \
strv2(any_str, 0)

#define strv2(any_str, start)             \
_Generic(any_str,                         \
    char*         : neat_strv_cstr2,      \
    unsigned char*: neat_strv_cstr2,      \
    DString*      : neat_strv_dstr2,      \
    String_View*  : neat_strv_strv2,      \
    String_Buffer*: neat_strv_strbuf2,    \
    SString_Ref   : neat_strv_sstr_ref2,  \
    Any_String_Ref: neat_strv_anystr_ref2 \
)(any_str, start)

#define strv3(any_str, start, end)         \
_Generic(any_str,                          \
    char*         : neat_strv_cstr3,       \
    unsigned char*: neat_strv_cstr3,       \
    DString*      : neat_strv_dstr3,       \
    String_View*  : neat_strv_strv3,       \
    String_Buffer*: neat_strv_strbuf3,     \
    SString_Ref   : neat_strv_sstr_ref3,   \
    Any_String_Ref: neat_strv_anystr_ref3, \
)(any_str, start, end)

// TODO this should have 4 overloads dstr() dstr(cap) dstr(allocator) dstr(cap, allocator)
#define dstr(...) \
dstr1##__VA_OPT__(0)(__VA_ARGS__)

#define dstr0() \
neat_dstr_new(16, neat_get_default_allocator())

#define dstr01(cap_or_allocator, ...)                                       \
__VA_OPT__(dstr2(cap_or_allocator, __VA_ARGS__))                            \
NEAT_IF_EMPTY(                                                              \
    _Generic(cap_or_allocator,                                              \
        Neat_Allocator: neat_dstr_new(                                      \
                            16,                                             \
                            neat_gurantee(cap_or_allocator, Neat_Allocator) \
                        ),                                                  \
        default       : neat_dstr_new(                                      \
                            neat_gurantee_integer(cap_or_allocator),        \
                            neat_get_default_allocator()                    \
                        )                                                   \
    )                                                                       \
)

#define dstr2(cap, allocator) \
neat_dstr_new(cap, allocator)

#define dstr_deinit(dstr) \
neat_dstr_deinit_(dstr)

#define dstr_append(dstr, any_str) \
neat_dstr_append_strv(dstr, strv(any_str))

// for now...
#define dstr_append_tostr(dstr, stringable) \
dstr_append(dstr, neat_tostr(stringable))

#define dstr_append_tostr_p(dstr, stringable_ptr) \
dstr_append(dstr, neat_tostr_p(stringable))

#define dstr_prepend(dstr, any_str) \
neat_dstr_prepend_strv(dstr, strv(any_str))

#define dstr_prepend_tostr(dstr, stringable) \
dstr_prepend(dstr, neat_tostr(stringable))

#define dstr_prepend_tostr_p(dstr, stringable_ptr) \
dstr_prepend(dstr, neat_tostr_p(stringable_ptr))

#define dstr_insert(dstr, any_str, idx) \
neat_dstr_insert_strv(dstr, strv(any_str), idx)

#define dstr_insert_tostr(dstr, stringable, idx) \
dstr_insert(dstr, neat_tostr(stringable), idx)

#define dstr_insert_tostr_p(dstr, stringable_ptr, idx) \
dstr_insert(dstr, neat_tostr_p(stringable_ptr), idx)

// for now ignore tostrbuf

// IDEA for print:
// fprint(FILE*, int nb, ...String_View)
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

// TODO maybe the solution is to make this only return ptr types, stroing them in the current context
// NO!!! new plan. to_string functions can only return DString. but to_string_buf will be using a _Thread_local thing to take a ptr to any String type
#define neat_tostr(x) \
neat_tostr_p(neat_as_pointer(x))

#define neat_tostr_p(xp) \
neat_get_tostr_func(typeof(*xp))(xp)

// TODO REDO THIS WHOLE SHIT
#define neat_tostr_buf(x, buf, ...) \
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

DString neat_dstr_new(uint64_t cap, Neat_Allocator allocator);
SString_Ref neat_sstr_ref_from_sstr_ptr(void *sstr_ptr, size_t cap);
size_t neat_anystr_ref_copy(Any_String_Ref dst, String_View src);
size_t neat_anystr_ref_concat(Any_String_Ref dst, String_View src);

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

DString neat_dstr_new(uint64_t cap, Neat_Allocator allocator)
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

SString_Ref neat_sstr_ref_from_sstr_ptr(void *sstr_ptr, size_t cap)
{
    SString_Ref ret = {.cap = cap};
    ret.sstring = (typeof(ret.sstring)) sstr_ptr;
    return ret;
}

static inline uint64_t neat_min(uint64_t a, uint64_t b)
{
    return a < b ? a : b;
}

size_t neat_anystr_ref_copy(Any_String_Ref dst, String_View src)
{
    size_t chars_to_copy = neat_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    dst.chars[chars_to_copy] = '\0';
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    return chars_to_copy;
}

size_t neat_anystr_ref_concat(Any_String_Ref dst, String_View src)
{
    if(dst.len == NULL || dst.cap <= *dst.len) return 0;
    
    size_t chars_to_copy = neat_min(dst.cap - *dst.len - 1, src.len);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    *dst.len += chars_to_copy;
    dst.chars[*dst.len] = '\0';
    
    return chars_to_copy;
}

Any_String_Ref neat_anystr_ref_to_cstr(char *str)
{
    size_t len = strlen(str);
    
    return (Any_String_Ref){
        .cap   = len,
        .len   = NULL,
        .chars = (unsigned char*) str
    };;
}

Any_String_Ref neat_anystr_ref_to_dstr(DString *str)
{
    return (Any_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Any_String_Ref neat_anystr_ref_to_strv(String_View *str)
{
    return (Any_String_Ref){
        .cap   = str->len,
        .len   = &str->len,
        .chars = str->chars
    };
}

Any_String_Ref neat_anystr_ref_to_strbuf(String_Buffer *str)
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

String_Buffer neat_strbuf_new(uint64_t cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    return (String_Buffer){
        .cap   = cap,
        .len   = 0,
        .chars = neat_alloc(allocator, unsigned char, cap)
    };
}

String_Buffer neat_strbuf_of_cstr(char *str)
{
    size_t len = strlen(str);
    return (String_Buffer){
        .cap   = len,
        .len   = len,
        .chars = (unsigned char*) str
    };
}

String_Buffer neat_strbuf_of_dstr(DString *str)
{
    return (String_Buffer){
        .cap   = str->cap,
        .len   = str->len,
        .chars = str->chars
    };
}

String_Buffer neat_strbuf_of_strv(String_View *str)
{
    return (String_Buffer){
        .cap   = str->len,
        .len   = str->len,
        .chars = str->chars
    };
}

String_Buffer neat_strbuf_of_strbuf(String_Buffer *str)
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

String_View neat_strv_cstr2(char *str, uint64_t start)
{
    size_t len = strlen(str);
    
    if(start >= len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = len - start,
        .chars = (unsigned char*) str + start
    };
}

String_View neat_strv_dstr2(DString *str, uint64_t start)
{
    if(start >= str->len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strv2(String_View *str, uint64_t start)
{
    if(start >= str->len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_strbuf2(String_Buffer *str, uint64_t start)
{
    if(start >= str->len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

String_View neat_strv_sstr_ref2(SString_Ref str, uint64_t start)
{
    if(start >= str.sstring->len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = str.sstring->len   - start,
        .chars = str.sstring->chars + start
    };
}

String_View neat_strv_anystr_ref2(Any_String_Ref str, uint64_t start)
{
    uint64_t len = 0;
    if(str.len == NULL)
    {
        len = strlen((char*) str.chars);
    }
    else
    {
        len = *str.len;
    }
    
    if(start >= len)
    {
        return (String_View){
            .len   = 0,
            .chars = NULL
        };
    }
    
    return (String_View){
        .len   = len       - start,
        .chars = str.chars + start
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

