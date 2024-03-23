#ifndef NEAT_STR_H
#define NEAT_STR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct VString
{
    uint64_t len;
    unsigned char chars[];
} VString;

#define SString(nb_chars) \
struct SString##nb_chars \
{ \
    _Static_assert((1##nb_chars##1ul || nb_chars##8ul || 1) && (nb_chars > 0), "argument must be positive decimal integer literal"); /* the first term is to make sure nb_chars is an integer literal */ \
    uint64_t len; \
    unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
}

typedef struct String_View \
{
    uint64_t len;
    char *chars;
} String_View;

#define NEAT_SSTRING_NO_TAG(nb_chars) \
struct \
{ \
    uint64_t len; \
    unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
}

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

// TODO the same macro should call char16_t and char32_t just _Generic on .chars. NO. make differnt macros for each encoding (maybe not?)

#define NEAT_0_OR_1_ELMS_CHECK(T, ...) \
((sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T)) || (sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T) * 2))

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
        NEAT_0_OR_1_ELMS_CHECK(uint64_t, __VA_ARGS__), \
        "no such version of sstring_from_cstring" \
    ); \
    typeof(sstring) neat_sstring = sstring; \
    typeof(cstring[0]) *neat_dst = cstring; \
    size_t neat_nb_chars_to_cpy = NEAT_VA_OR(strlen(neat_dst), __VA_ARGS__); \
    assert(neat_nb_chars_to_cpy + 1 <= sizeof(neat_sstring->chars)); \
    __VA_OPT__( assert(strlen(neat_dst) >= neat_nb_chars_to_cpy) ); \
    memcpy(neat_sstring->chars, neat_dst, neat_nb_chars_to_cpy + 1); \
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
        NEAT_IS_SSTRING_PTR(src), \
        "arg 2 must have type VString* or String_View* or SString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_ELMS_CHECK(uint64_t, __VA_ARGS__), \
        "No such version of sstring_assign"\
    ); \
    typeof(src) neat_src = src; \
    typeof(sstring) neat_dst = sstring; \
    uint64_t neat_nb_chars_to_cpy = NEAT_VA_OR(neat_src->len, __VA_ARGS__); \
    assert(sizeof(neat_dst->chars) >= neat_nb_chars_to_cpy + 1); \
    memcpy(neat_dst->chars, neat_src->chars, neat_nb_chars_to_cpy); \
    neat_dst->chars[ neat_nb_chars_to_cpy ] = '\0'; \
    neat_dst->len = neat_nb_chars_to_cpy; \
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
        NEAT_IS_SSTRING_PTR(src), \
        "arg 2 must have type VString* or String_View* or SString*" \
    ); \
    _Static_assert( \
        NEAT_0_OR_1_ELMS_CHECK(uint64_t, __VA_ARGS__), \
        "no such version of vstring_assign" \
    ); \
    typeof(src) neat_src = src; \
    size_t neat_nb_chars_to_cpy = NEAT_VA_OR(neat_src->len, __VA_ARGS__); \
    VString *neat_dst = vstring; \
    memcpy(neat_dst->chars, neat_src->chars, ); \
})

#define neat_string_view_into(string, ...) \
NEAT_APPEND_NARG(neat_string_view_into, string __VA_OPT__(,) __VA_ARGS__)

#define neat_string_view_into1(string) \
({ \
    ; \
})

#define neat_string_view_into2(string, start)

#define neat_string_view_into3(string, start, nb)

#define vstring_alloc(nb_chars, ...) \
neat_vstring_alloc_(nb_chars, NEAT_VA_OR(neat_get_default_allocator(), __VA_ARGS__))

// TODO WE CAN TOTALLY DO THE OLD neat_tostr println !!! WE CAN DETECT SString like we did above
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

VString *neat_vstring_alloc_(size_t nb_chars, Neat_Allocator allocator);

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

VString *neat_vstring_alloc_(size_t nb_chars, Neat_Allocator allocator)
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

#endif /* NEAT_STR_IMPL */

