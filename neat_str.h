#ifndef NEAT_STR_H
#define NEAT_STR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

// TODO add String_Builder
// TODO to_string functions can either return SString or struct { VString s; free_func; }

#ifndef NEAT_COMMON_UTILS
#define NEAT_COMMON_UTILS

#define neat_func_ptr(ret, ...) \
typeof(typeof(ret)(*)(__VA_ARGS__))

typedef neat_func_ptr(void*, void *ctx, size_t alignment, size_t n) neat_alloc_func;
typedef neat_func_ptr(void,  void *ctx, void *ptr, size_t n) neat_dealloc_func;
typedef neat_func_ptr(void*, void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size) neat_realloc_func;

typedef neat_func_ptr(void,  void **ctx) neat_allocator_init_func;
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

void *neat_default_allocator_alloc(void *ctx, size_t alignment, size_t n);
void neat_default_allocator_dealloc(void *ctx, void *ptr, size_t n);
void *neat_default_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size);
void neat_default_allocator_init(void **ctx);
void neat_default_allocator_deinit(void *ctx);

#define neat_get_default_allocator() ((void)0,     \
(Neat_Allocator){                                  \
    .alloc   = neat_default_allocator_alloc,   \
    .dealloc = neat_default_allocator_dealloc, \
    .realloc = neat_default_allocator_realloc, \
    .init    = neat_default_allocator_init,    \
    .deinit  = neat_default_allocator_deinit,  \
})

#define neat_alloc(allocator, T, n) \
((T*) allocator.alloc(allocator.ctx, _Alignof(T), n * sizeof(T)))

#define neat_dealloc(allocator, ptr, T, n) \
allocator.dealloc(allocator.ctx, ptr, sizeof((T){0}) * n)

#define neat_realloc(allocator, ptr, T, old_n, new_n) \
(T*) allocator.realloc(allocator.ctx, ptr, _Alignof(T), sizeof(T) * old_n, sizeof(T) * new_n)

#define neat_alloc_bytes(allocator, n) \
allocator.alloc(allocator.ctx, _Alignof(max_align_t), n)

#define neat_dealloc_bytes(allocator, ptr, n) \
allocator.dealloc(allocator.ctx, ptr, n)

#define neat_realloc_bytes(allocator, ptr, old_n, new_n) \
allocator.realloc(allocator.ctx, ptr, _Alignof(max_align_t), old_n, new_n)

#define neat_static_assertx(exp, msg) \
((void)sizeof(struct { _Static_assert(exp, msg); int dummy; }))

#define neat_has_type(exp, t) \
_Generic(exp, t: 1, default: 0)

#define neat_is_integer(exp) \
_Generic(exp, \
    bool: 1, \
    char: 1, \
    signed char: 1, \
    short int: 1, \
    int: 1, \
    long int: 1, \
    long long int: 1, \
    unsigned char: 1, \
    unsigned short int: 1, \
    unsigned int: 1, \
    unsigned long int: 1, \
    unsigned long long int: 1, \
    default: 0 \
)

typedef struct { int dummy; } neat_fail_type;

// NARG stuff
#define NEAT_DS_NARG(...) \
NEAT_DS_NARG_(__VA_OPT__(__VA_ARGS__,)NEAT_DS_5SEQ())

#define NEAT_DS_NARG_(...) \
NEAT_DS_ARG_N(__VA_ARGS__)

// increase SEQ ?

#define NEAT_DS_ARG_N(_1, _2, _3, _4, _5, N,...) N

#define NEAT_DS_5SEQ() 5,4,3,2,1,0

#define NEAT_DS_CAT(a, ...) NEAT_DS_PRIMITIVE_CAT(a, __VA_ARGS__)
#define NEAT_DS_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define NEAT_APPEND_NARG(macro, ...) NEAT_DS_CAT(macro, NEAT_DS_NARG(__VA_ARGS__))
// NARG stuff end

// gurantee stuff
#define neat_gurantee(exp, t) \
neat_gurantee_fallback(exp, t, (t){0})

#define neat_gurantee_integer(exp) \
neat_gurantee_integer_fallback(exp, 0)

#define neat_gurantee_ptr(exp, base_ty) \
_Generic(exp, \
    base_ty: &(typeof(exp)){exp}, \
    typeof(base_ty)*: exp \
)

#define neat_gurantee_scalar(exp, base_ty) \
_Generic(exp, \
    base_ty: exp, \
    typeof(base_ty)*: *neat_gurantee(exp, typeof(base_ty)*) \
)

#define neat_gurantee_fallback(exp, ty, fallback) \
_Generic(exp, \
    ty: exp, \
    default: fallback \
)

#define neat_gurantee_integer_fallback(exp, fallback) \
_Generic(exp, \
    bool: exp, \
    char: exp, \
    signed char: exp, \
    short int: exp, \
    int: exp, \
    long int: exp, \
    long long int: exp, \
    unsigned char: exp, \
    unsigned short int: exp, \
    unsigned int: exp, \
    unsigned long int: exp, \
    unsigned long long int: exp, \
    default: fallback \
)
// gurantee stuff end

#define NEAT_CARR_LEN(carr) (sizeof(carr) / sizeof(carr[0]))

// IF_DEF and ARG_n stuff
#define NEAT_COMMA()              ,
#define NEAT_ARG1_( _1, ... )     _1
#define NEAT_ARG1( ... )          NEAT_ARG1_( __VA_ARGS__ )
#define NEAT_ARG2_( _1, _2, ... ) _2
#define NEAT_ARG2( ... )          NEAT_ARG2_( __VA_ARGS__ )
#define NEAT_INCL( ... )          __VA_ARGS__
#define NEAT_OMIT( ... )
#define NEAT_IF_DEF( macro )      NEAT_ARG2( NEAT_COMMA macro () NEAT_INCL, NEAT_OMIT, )
#define NEAT_STRINGIIFY(X)        NEAT_STRINGIIFY_(X)
#define NEAT_STRINGIIFY_(X)       #X
// IF_DEF and ARG_n stuff end

#endif /* NEAT_COMMON_UTILS */

typedef struct VString
{
    uint64_t len;
    unsigned char chars[];
} VString;

typedef struct DString
{
    Neat_Allocator allocator;
    uint64_t len;
    uint64_t cap;
    unsigned char *chars;
} DString;

typedef struct String_View
{
    uint64_t len;
    unsigned char *chars;
} String_View;

// in C23 structs can be defined multiple times with the same tag and members, in C23 DECL_SSTRING is useless, older versions require it.
#if __STDC_VERSION__ >= 202311L
    #define SString(nb_chars) \
    struct SString##nb_chars \
    { \
        _Static_assert((1##nb_chars##1ul || nb_chars##8ul || 1) && (nb_chars > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure nb_chars is an integer literal */ \
        uint64_t len; \
        unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
    }
    
    #define DECL_SSTRING(nb_chars)
#else
    #define SString(nb_chars) \
    struct SString##nb_chars
    
    #define DECL_SSTRING(nb_chars) \
    struct SString##nb_chars \
    { \
        _Static_assert((1##nb_chars##1ul || nb_chars##8ul || 1) && (nb_chars > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure nb_chars is an integer literal */ \
        uint64_t len; \
        unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
    };
#endif

#define NEAT_SSTRING_NO_TAG(nb_chars) \
struct \
{ \
    uint64_t len; \
    unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
}


// TODO the same macro should call char16_t and char32_t just _Generic on .chars. NO. make differnt macros for each encoding (maybe not?)

#define NEAT_0_OR_1_ELMS_CHECK(T, ...) \
((sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T)) || (sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T) * 2))

#define NEAT_0_OR_1_CHECK(...) \
(1 __VA_OPT__(* NEAT_1_CHECK(__VA_ARGS__)))
//(() * __VA_OPT__(0))

#define NEAT_1_CHECK(a, ...) \
(1 __VA_OPT__(* 0))

#define NEAT_VA_OR(otherwise, ...) \
(0 __VA_OPT__(+ 1) ? (__VA_OPT__((void)) (typeof(otherwise)){0} __VA_OPT__(,) __VA_ARGS__) : otherwise)

#define NEAT_IS_SSTRING_PTR(to_test) \
(neat_has_type(&(typeof((to_test)->chars)){0}, unsigned char(*)[sizeof((to_test)->chars)]) && \
neat_has_type((to_test)->len, uint64_t) && \
(sizeof(*to_test) == sizeof(NEAT_SSTRING_NO_TAG(sizeof((to_test)->chars) - 1))) && \
_Alignof(typeof(*(to_test))) == _Alignof(typeof(NEAT_SSTRING_NO_TAG(sizeof((to_test)->chars) - 1))) \
) \

#define NEAT_SSTRING_PTR_GENERIC_CASE(exp, body) \
default: _Generic((typeof((exp)->chars)*){0}, \
    unsigned char(*)[sizeof((exp)->chars) + 1]: _Generic((exp)->len, \
        uint64_t: _Generic((char(*)[sizeof(*(exp))]){0}, \
            char(*)[sizeof(NEAT_SSTRING_NO_TAG(sizeof((exp)->chars)))]: _Generic((char(*)[_Alignof(typeof(*(exp)))]){0}, \
                char(*)[_Alignof(typeof(NEAT_SSTRING_NO_TAG(sizeof((exp)->chars))))]: body , \
                default: 0 \
            ), \
            default: 0 \
        ), \
        default: 0 \
    ), \
    default: 0)

#define sstring_from_cstring(sstring, cstring, ...) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*) || \
        neat_has_type(cstring, signed char*), \
        "arg 2 must be a C string" \
    ); \
    _Static_assert( \
        NEAT_IS_SSTRING_PTR(sstring), \
        "arg 1 must have type SString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "no such version of sstring_from_cstring" \
    ); \
    typeof(sstring) neat_sstring = sstring; \
    typeof((cstring)[0]) *neat_dst = cstring; \
    size_t neat_nb_chars_to_cpy = NEAT_VA_OR(strlen(neat_dst), __VA_ARGS__); \
    assert(neat_nb_chars_to_cpy + 1 <= sizeof(neat_sstring->chars)); \
    __VA_OPT__( assert(strlen(neat_dst) >= neat_nb_chars_to_cpy) ); \
    memmove(neat_sstring->chars, neat_dst, neat_nb_chars_to_cpy + 1); \
    neat_sstring->len = neat_nb_chars_to_cpy; \
    neat_sstring->chars[ neat_nb_chars_to_cpy ] = '\0'; \
    (void) 0; \
})

#define sstring_assign(sstring, src, ...) \
({ \
    _Static_assert( \
        NEAT_IS_SSTRING_PTR(sstring), \
        "arg 1 must have type SString*" \
    ); \
    _Static_assert( \
        neat_has_type(src, VString*) || \
        neat_has_type(src, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(src), \
        "arg 1 must have type VString* or String_View* or DString* or SString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "No such version of sstring_assign"\
    ); \
    typeof(src) neat_src = src; \
    typeof(sstring) neat_dst = sstring; \
    uint64_t neat_nb_chars_to_cpy = NEAT_VA_OR(neat_src->len, __VA_ARGS__); \
    assert(sizeof(neat_dst->chars) >= neat_nb_chars_to_cpy + 1); \
    memmove(neat_dst->chars, neat_src->chars, neat_nb_chars_to_cpy); \
    neat_dst->chars[ neat_nb_chars_to_cpy ] = '\0'; \
    neat_dst->len = neat_nb_chars_to_cpy; \
    (void) 0; \
})

#define vstring_from_cstring(vstring, cstring, ...) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*) || \
        neat_has_type(cstring, signed char*), \
        "arg 2 must be a C string" \
    ); \
    _Static_assert( \
        neat_has_type(vstring, VString*), \
        "arg 1 must have type VString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "no such version of sstring_from_cstring" \
    ); \
    VString *neat_vstring = vstring; \
    typeof(cstring[0]) *neat_dst = cstring; \
    size_t neat_nb_chars_to_cpy = NEAT_VA_OR(strlen(neat_dst), __VA_ARGS__); \
    __VA_OPT__( assert(strlen(neat_dst) >= neat_nb_chars_to_cpy) ); \
    memmove(neat_vstring->chars, neat_dst, neat_nb_chars_to_cpy + 1); \
    neat_vstring->len = neat_nb_chars_to_cpy; \
    neat_vstring->chars[ neat_nb_chars_to_cpy ] = '\0'; \
    (void) 0; \
})

#define vstring_assign(vstring, src, ...) \
({ \
    _Static_assert( \
        neat_has_type(vstring, VString*), \
        "arg 1 must have type VString*" \
    ); \
    _Static_assert( \
        neat_has_type(src, VString*) || \
        neat_has_type(src, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(src), \
        "arg 1 must have type VString* or String_View* or DString* or SString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "no such version of vstring_assign" \
    ); \
    typeof(src) neat_src = src; \
    size_t neat_nb_chars_to_cpy = NEAT_VA_OR(neat_src->len, __VA_ARGS__); \
    VString *neat_dst = vstring; \
    memmove(neat_dst->chars, neat_src->chars, neat_nb_chars_to_cpy); \
})

#define neat_string_view_into_cstring(cstring, ...) \
NEAT_APPEND_NARG(neat_string_view_into_cstring, cstring, ##__VA_ARGS__)(cstring, ##__VA_ARGS__)

#define neat_string_view_into_cstring1(cstring) \
({ \
    _Static_assert( \
        neat_has_type(cstring, char*) || \
        neat_has_type(cstring, unsigned char*) || \
        neat_has_type(cstring, signed char*), \
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
        neat_has_type(cstring, unsigned char*) || \
        neat_has_type(cstring, signed char*), \
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
        neat_has_type(cstring, unsigned char*) || \
        neat_has_type(cstring, signed char*), \
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

#define neat_string_view_into(string, ...) \
NEAT_APPEND_NARG(neat_string_view_into, string __VA_OPT__(,) __VA_ARGS__)(string, ##__VA_ARGS__)

#define neat_string_view_into1(string) \
({ \
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(string), \
        "arg 1 must have type VString* or String_View* or DString* or SString*" \
    ); \
    typeof(string) neat_string = string; \
    String_View neat_ret = { 0 }; \
    neat_ret.len = neat_string->len; \
    neat_ret.chars = neat_string->chars; \
    neat_ret; \
})

#define neat_string_view_into2(string, start) \
({ \
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(string), \
        "arg 1 must have type VString* or String_View* or DString* or SString*" \
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
    _Static_assert( \
        neat_has_type(string, VString*) || \
        neat_has_type(string, String_View*) || \
        neat_has_type(string, DString*) || \
        NEAT_IS_SSTRING_PTR(string), \
        "arg 1 must have type VString* or String_View* or DString* or SString*" \
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

#define new_vstring(nb_chars, ...) \
({ \
    _Static_assert( \
        neat_is_integer(nb_chars), \
        "arg 1 must be an integer" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "no such version of new_vstring" \
    ); \
    neat_make_new_vstring(nb_chars, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__)); \
})

#define new_dstring(cap, ...) \
({ \
    _Static_assert( \
        neat_is_integer(cap), \
        "arg 1 must be an integer" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_CHECK(__VA_ARGS__), \
        "no such version of new_dstring" \
    ); \
    _Static_assert( \
        neat_has_type(NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__), Neat_Allocator), \
        "arg 2 must have type Neat_Allocator" \
    ); \
    neat_make_new_dstring(cap, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__)); \
})

#define NEAT_DEFAULT_STRINGABLE_TYPES  \
bool:               neat_tostr_bool,   \
char*:              neat_tostr_str,    \
char:               neat_tostr_char,   \
signed char:        neat_tostr_schar,  \
unsigned char:      neat_tostr_uchar,  \
short:              neat_tostr_short,  \
unsigned short:     neat_tostr_ushort, \
int:                neat_tostr_int,    \
unsigned int:       neat_tostr_uint,   \
long:               neat_tostr_long,   \
unsigned long:      neat_tostr_ulong,  \
long long:          neat_tostr_llong,  \
unsigned long long: neat_tostr_ullong, \
float:              neat_tostr_float,  \
double:             neat_tostr_double

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
(neat_static_assertx(neat_is_stringable(ty), "type not stringable"), _Generic((ty){0}, \
    NEAT_ALL_STRINGABLE_TYPES \
))

#define neat_get_tostr_func_ft(ty) \
_Generic((ty){0}, \
    NEAT_ALL_STRINGABLE_TYPES, \
    default: (neat_tostr_fail){0} \
)

#define neat_to_string(x) \
neat_get_tostr_func(typeof(*(x)))(x)

#define neat_is_stringable(ty) \
(!neat_has_type(neat_get_tostr_func_ft(ty), neat_tostr_fail))

#define neat_tostr_ret(ty) \
typeof(neat_get_tostr_func(ty)(0))

// TODO WE CAN TOTALLY DO THE OLD neat_tostr println !!! WE CAN DETECT SString like we did above....shit we can't.. cuz the above assumes a Neat string
#define neat_fprint(f, s) \
({ \
    typeof(s) neat_s = s; \
    fwrite(neat_s->chars, sizeof(unsigned char), neat_s->len, f); \
})

#define neat_print(s) \
neat_fprint(stdout, s)

#define neat_fprintln(f, s) \
({ \
    neat_fprint(f, s); \
    fputc('\n', f); \
    (void) 0; \
})

#define neat_println(s) \
neat_fprintln(stdout, s)

#define NEAT_DECL_TOSTR_FUNC(n) \
typedef NEAT_ARG1(ADD_TOSTR) neat_tostr_type_##n; \
static inline typeof(NEAT_ARG2(ADD_TOSTR)(0)) neat_tostr_func_##n (neat_tostr_type_##n *obj) \
{ \
    typeof(NEAT_ARG2(ADD_TOSTR)(0)) totest; \
    _Static_assert(); \
    (void)neat_get_tostr_func_ft(typeof(*obj)); /* this is to cause an error if type already added */ \
    return NEAT_ARG2(ADD_TOSTR)(obj); \
}

DECL_SSTRING(5)
DECL_SSTRING(1)
DECL_SSTRING(6)
DECL_SSTRING(11)
DECL_SSTRING(10)
DECL_SSTRING(20)
DECL_SSTRING(30)

VString *neat_make_new_vstring(size_t nb_chars, Neat_Allocator allocator);
SString(5) neat_tostr_bool(bool *obj); 
DString neat_tostr_str(char **obj);  
SString(1) neat_tostr_char(char *obj); 
SString(1) neat_tostr_schar(signed char *obj);
SString(1) neat_tostr_uchar(unsigned char *obj);
SString(6) neat_tostr_short(short *obj);
SString(5) neat_tostr_ushort(unsigned short *obj);
SString(11) neat_tostr_int(int *obj);  
SString(10) neat_tostr_uint(unsigned int *obj); 
SString(20) neat_tostr_long(long *obj); 
SString(20) neat_tostr_ulong(unsigned long *obj);
SString(20) neat_tostr_llong(long long *obj);
SString(20) neat_tostr_ullong(unsigned long long *obj);
SString(20) neat_tostr_float(float *obj);
SString(30) neat_tostr_double(double *obj);
#endif /* NEAT_STR_H */

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
    return;
}

void neat_default_allocator_deinit(void *ctx)
{
    (void) ctx;
    return;
}

#endif /* NEAT_COMMON_UTILS_IMPLED */

VString *neat_make_new_vstring(size_t nb_chars, Neat_Allocator allocator)
{
    VString *ret = allocator.alloc(allocator.ctx, _Alignof(VString), sizeof(VString) + sizeof(unsigned char) * nb_chars);
    if(ret != NULL)
    {
        ret->len = 0;
        if(nb_chars > 0)
            ret->chars[0] = '\0';
    }
    return ret;
}

DString neat_make_new_dstring(size_t cap, Neat_Allocator allocator)
{
    DString ret = {.cap = cap, .allocator = allocator};
    ret.allocator.init(&ret.allocator.ctx);
    ret.chars = neat_alloc(ret.allocator, unsigned char, cap);
    if(ret.chars != NULL)
    {
        ret.chars[0] = '\0';
    }
    return ret;
}

SString(5) neat_tostr_bool(bool *obj)
{
    SString(5) ret = { 0 };
    sstring_from_cstring(&ret, *obj ? "true" : "false");
    return ret;
}

DString neat_tostr_str(char **obj)
{
    DString ret = neat_make_new_dstring(strlen(*obj), neat_get_default_allocator());
    memmove(ret.chars, *obj, ret.cap);
    return ret;
}

SString(1) neat_tostr_char(char *obj)
{
    return (SString(1)){.len = 1, .chars = {(char)*obj} };
}

SString(1) neat_tostr_schar(signed char *obj)
{
    return (SString(1)){.len = 1, .chars = {(signed char)*obj} };
}

SString(1) neat_tostr_uchar(unsigned char *obj)
{
    return (SString(1)){.len = 1, .chars = {*obj} };
}

SString(6) neat_tostr_short(short *obj)
{
    SString(6) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%hd", *obj);
    ret.len = len;
    return ret;
}

SString(5) neat_tostr_ushort(unsigned short *obj)
{
    SString(5) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%hu", *obj);
    ret.len = len;
    return ret;
}

SString(11) neat_tostr_int(int *obj)
{
    SString(11) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%d", *obj);
    ret.len = len;
    return ret;
}

SString(10) neat_tostr_uint(unsigned int *obj)
{
    SString(10) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%u", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_long(long *obj)
{
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%ld", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_ulong(unsigned long *obj)
{
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%lu", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_llong(long long *obj)
{
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%lld", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_ullong(unsigned long long *obj)
{
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%llu", *obj);
    ret.len = len;
    return ret;
}

SString(20) neat_tostr_float(float *obj)
{
    SString(20) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%g", *obj);
    ret.len = len;
    return ret;
}

SString(30) neat_tostr_double(double *obj)
{
    SString(30) ret = { 0 };
    uint64_t len = sprintf((char*)ret.chars, "%g", *obj);
    ret.len = len;
    return ret;
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

