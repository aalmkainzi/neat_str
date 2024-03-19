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
    _Static_assert(NEAT_DS_CAT(nb_chars, ul), "not an integer literal"); \
    uint64_t len; \
    unsigned char chars[ nb_chars + 1 ]; /* + 1 for the nul */ \
}

typedef struct String_View \
{
    uint64_t len;
    char *chars;
} String_View;

#ifndef NEAT_COMMON_UTILS
#define NEAT_COMMON_UTILS

#define neat_func_ptr(ret, ...) \
typeof(typeof(ret)(*)(__VA_ARGS__))

typedef neat_func_ptr(void*, void *context, size_t alignment, size_t n) neat_alloc_func;
typedef neat_func_ptr(void,  void *context, void *ptr, size_t n) neat_dealloc_func;
typedef neat_func_ptr(void*, void *context, void *ptr, size_t alignment, size_t old_size, size_t new_size) neat_realloc_func;

typedef neat_func_ptr(void,  void **context) neat_allocator_init_func;
typedef neat_func_ptr(void,  void *context)  neat_allocator_deinit_func;

typedef struct Neat_Allocator
{
    void *context;
    neat_alloc_func alloc;
    neat_dealloc_func dealloc;
    neat_realloc_func realloc;
    neat_allocator_init_func init;
    neat_allocator_deinit_func deinit;
} Neat_Allocator;

void *neat_default_allocator_alloc(void *context, size_t alignment, size_t n);
void neat_default_allocator_dealloc(void *context, void *ptr, size_t n);
void *neat_default_allocator_realloc(void *context, void *ptr, size_t alignment, size_t old_size, size_t new_size);
void neat_default_allocator_init(void **context);
void neat_default_allocator_deinit(void *context);

#define neat_get_default_allocator() ((void)0,     \
(Neat_Allocator){                                  \
        .alloc   = neat_default_allocator_alloc,   \
        .dealloc = neat_default_allocator_dealloc, \
        .realloc = neat_default_allocator_realloc, \
        .init    = neat_default_allocator_init,    \
        .deinit  = neat_default_allocator_deinit,  \
})

#define neat_alloc(allocator, T, n) \
((T*) allocator.alloc(allocator.context, _Alignof(T), n * sizeof(T)))

#define neat_dealloc(allocator, ptr, T, n) \
allocator.dealloc(allocator.context, ptr, sizeof((T){0}) * n)

#define neat_realloc(allocator, ptr, T, old_n, new_n) \
(T*) allocator.realloc(allocator.context, ptr, _Alignof(T), sizeof(T) * old_n, sizeof(T) * new_n)

#define neat_alloc_bytes(allocator, n) \
allocator.alloc(allocator.context, _Alignof(max_align_t), n)

#define neat_dealloc_bytes(allocator, ptr, n) \
allocator.dealloc(allocator.context, ptr, n)

#define neat_realloc_bytes(allocator, ptr, old_n, new_n) \
allocator.realloc(allocator.context, ptr, _Alignof(max_align_t), old_n, new_n)

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
NEAT_DS_NARG_(__VA_ARGS__,NEAT_DS_5SEQ())

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

// TODO the same macro should call char16_t and char32_t just _Generic on .chars
#define sstring_assign(sstring, src) \
({ \
    _Static_assert( \
        neat_has_type((typeof(src)*){0}, char(*)[sizeof(typeof(src))]) || \
        neat_has_type(src, char*) || \
        neat_has_type(src, unsigned char*) || \
        neat_has_type(src, signed char*) || \
        neat_has_type(src, VString*), \
        "arg 2 must be a string literal/array or a VString* or a char*" \
    ); \
    typeof(src) neat_src; \
    memcpy(&neat_src, &src, sizeof(typeof(src))); \
    typeof(sstring) *neat_dst = &(sstring); \
    _Generic(neat_src, \
        unsigned char*: ({ \
            unsigned char *neat_src_char = neat_gurantee(neat_src, unsigned char*); \
            neat_dst->len = strlen(neat_src_char); \
            assert((neat_dst->len + 1) * sizeof(unsigned char) <= sizeof(neat_dst->chars)); \
            memcpy(neat_dst->chars, neat_src_char, neat_dst->len + 1); \
        }), \
        signed char*: ({ \
            signed char *neat_src_char = neat_gurantee(neat_src, signed char*); \
            neat_dst->len = strlen(neat_src_char); \
            assert((neat_dst->len + 1) * sizeof(unsigned char) <= sizeof(neat_dst->chars)); \
            memcpy(neat_dst->chars, neat_src_char, neat_dst->len + 1); \
        }), \
        char*: ({ \
            char *neat_src_char = neat_gurantee(neat_src, char*); \
            neat_dst->len = strlen(neat_src_char); \
            assert((neat_dst->len + 1) * sizeof(unsigned char) <= sizeof(neat_dst->chars)); \
            memcpy(neat_dst->chars, neat_src_char, neat_dst->len + 1); \
        }), \
        VString*: ({ \
            VString *neat_src_vstring = neat_gurantee(neat_src, VString*); \
            assert(sizeof(neat_dst->chars) >= neat_src_vstring->len + 1); \
            *neat_dst = *(typeof(sstring)*) neat_src_vstring; \
            neat_dst->chars[ neat_dst->len ] = '\0'; \
        }) \
    ); \
})

#define vstring_assign(vstring, src) \
({ \
    _Static_assert(neat_has_type(vstring, VString*), "arg 1 must have type VString*"); \
    _Static_assert( \
        neat_has_type((typeof(src)*){0}, char(*)[sizeof(typeof(src))]) || \
        neat_has_type((typeof(src)*){0}, char**) || \
        neat_has_type(src, VString*), \
        "arg 2 must be a string literal/array or a VString* or a char*" \
    ); \
    typeof(src) neat_src; \
    memcpy(&neat_src, &src, sizeof(typeof(src))); \
    VString *neat_dst = vstring; \
    _Generic(neat_src, \
        char*: ({ \
            char *neat_src_char = neat_gurantee(neat_src, char*); \
            neat_dst->len = strlen(neat_src_char); \
            memcpy(neat_dst->chars, neat_src_char, neat_dst->len + 1); \
        }), \
        VString*: ({ \
            VString *neat_src_vstring = neat_gurantee(neat_src, VString*); \
            neat_dst->len = neat_src_vstring->len; \
            memcpy(neat_dst->chars, neat_src_vstring->chars, neat_src_vstring->len + 1); \
        }) \
    ); \
})

#define vstring_alloc(nb_chars, ...) \
NEAT_APPEND_NARG(vstring_alloc, nb_chars, ##__VA_ARGS__)(nb_chars, ##__VA_ARGS__)

#define vstring_alloc1(nb_chars) \
neat_vstring_alloc_(nb_chars, neat_get_default_allocator())

#define vstring_alloc2(nb_chars, allocator) \
neat_vstring_alloc_(nb_chars, allocator)

#define neat_string_copy(dst, src) \
(memcpy(dst->chars, src->chars, src->len), dst->len = src->len)

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
})

#define neat_println(s) \
neat_fprintln(stdout, s)

VString *neat_vstring_alloc_(size_t nbc_chars, Neat_Allocator allocator);

#endif /* NEAT_STR_H */

#if defined(NEAT_STR_IMPL) && !defined(NEAT_STR_IMPLED)
#define NEAT_STR_IMPLED

#ifndef NEAT_COMMON_UTILS_IMPLED
#define NEAT_COMMON_UTILS_IMPLED

void *neat_default_allocator_alloc(void *context, size_t alignment, size_t n)
{
    (void) alignment;
    (void) context;
    return malloc(n);
}

void neat_default_allocator_dealloc(void *context, void *ptr, size_t n)
{
    (void) context;
    (void) n;
    free(ptr);
}

void *neat_default_allocator_realloc(void *context, void *ptr, size_t alignment, size_t old_size, size_t new_size)
{
    (void) context;
    (void) alignment;
    (void) old_size;
    return realloc(ptr, new_size);
}

void neat_default_allocator_init(void **context)
{
    (void) context;
    return;
}

void neat_default_allocator_deinit(void *context)
{
    (void) context;
    return;
}

VString *neat_vstring_alloc_(size_t nb_chars, Neat_Allocator allocator)
{
    VString *ret = allocator.alloc(allocator.context, _Alignof(VString), sizeof(VString) + sizeof(unsigned char) * nb_chars);
    if(ret)
    {
        ret->len = 0;
        if(nb_chars > 0)
            ret->chars[0] = '\0';
    }
    return ret;
}

#endif

#endif /* NEAT_STR_IMPL */

