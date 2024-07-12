/*
Contains helpful functionality used by all neat libs
*/

#ifndef NEAT_CORE_H
#define NEAT_CORE_H

#include <stdlib.h>

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

#if __STDC_VERSION__ >= 202311L

    #define NEAT_NODISCARD(...) [[nodiscard __VA_OPT__((__VA_ARGS__))]]

#elif defined(__GNUC__)

    #define NEAT_NODISCARD(...) __attribute__ ((warn_unused_result))

#elif defined(_MSC_VER)

    #define NEAT_NODISCARD(...) _Check_Return_

#else

    #define NEAT_NODISCARD(...)

#endif

#define neat_static_assertx(exp, msg) \
((void)sizeof(struct { _Static_assert(exp, msg); int dummy; }))

#define neat_has_type(exp, t) \
_Generic(exp, t: 1, default: 0)

#define neat_is_array_of(exp, ty) \
neat_has_type((typeof(exp)*){0}, typeof(ty)(*)[sizeof(exp)/sizeof(ty)])

#define neat_is_integer(exp) \
_Generic(exp, \
    bool: 1, \
    char: 1, \
    NEAT_SCHAR_CASE(signed char: 1,) \
    short int: 1, \
    int: 1, \
    long int: 1, \
    long long int: 1, \
    NEAT_UCHAR_CASE(unsigned char: 1,) \
    unsigned short int: 1, \
    unsigned int: 1, \
    unsigned long int: 1, \
    unsigned long long int: 1, \
    default: 0 \
)

typedef struct { int dummy; } neat_fail_type;

// NARG stuff
#define NEAT_NARG(...) \
NEAT_NARG_(__VA_OPT__(__VA_ARGS__,)NEAT_5SEQ())

#define NEAT_NARG_(...) \
NEAT_ARG_N(__VA_ARGS__)

// increase SEQ ? work towards getting rid of this shit

#define NEAT_ARG_N(_1, _2, _3, _4, _5, N,...) N

#define NEAT_5SEQ() 5,4,3,2,1,0

#define NEAT_CAT(a, ...) NEAT_PRIMITIVE_CAT(a, __VA_ARGS__)
#define NEAT_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

// append to this '(__VA_ARGS__)'
#define NEAT_APPEND_NARG(macro, ...) NEAT_CAT(macro, NEAT_NARG(__VA_ARGS__))

// NARG stuff end

// gurantee stuff
#define neat_gurantee(exp, t) \
neat_gurantee_fallback(exp, t, (t){0})

#define neat_gurantee_integer(exp) \
neat_gurantee_integer_fallback(exp, 0)

#define neat_gurantee_ptr(exp, base_ty) \
_Generic(exp, \
    base_ty: neat_as_pointer(exp), \
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
    NEAT_SCHAR_CASE(signed char: exp,) \
    short int: exp, \
    int: exp, \
    long int: exp, \
    long long int: exp, \
    NEAT_SCHAR_CASE(unsigned char: exp,) \
    unsigned short int: exp, \
    unsigned int: exp, \
    unsigned long int: exp, \
    unsigned long long int: exp, \
    default: fallback \
)

#define neat_gurantee_not(exp, not_ty, fallback_ty) \
_Generic(exp, \
    not_ty: (fallback_ty){0}, \
    default: exp \
)

#define neat_gurantee_not_integer(exp, fallback_ty)   \
_Generic(exp,                                         \
    bool:                          (fallback_ty){0},  \
    char:                          (fallback_ty){0},  \
    NEAT_SCHAR_CASE(signed char:   (fallback_ty){0},) \
    short int:                     (fallback_ty){0},  \
    int:                           (fallback_ty){0},  \
    long int:                      (fallback_ty){0},  \
    long long int:                 (fallback_ty){0},  \
    NEAT_UCHAR_CASE(unsigned char: (fallback_ty){0},) \
    unsigned short int:            (fallback_ty){0},  \
    unsigned int:                  (fallback_ty){0},  \
    unsigned long int:             (fallback_ty){0},  \
    unsigned long long int:        (fallback_ty){0},  \
    default: exp                                      \
)

#define neat_if_ty(exp, ty, macro, ...) \
_Generic(exp, \
    ty: macro(neat_gurantee(exp, ty) __VA_OPT__(,) __VA_ARGS__), \
    default: 0 \
)

#define neat_ifn_ty(exp, not_ty, fb_ty, macro, ...) \
_Generic(exp, \
    default: macro(neat_gurantee_not(exp, not_ty, fb_ty) __VA_OPT__(,) __VA_ARGS__), \
    not_ty: 0 \
)

#define neat_as_pointer(scalar) \
&(struct { typeof((void)0,scalar) t; }){scalar}.t

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
#define NEAT_STRINGIFY(X)         NEAT_STRINGIFY_(X)
#define NEAT_STRINGIFY_(X)        #X
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

#define NEAT_0_OR_1_ELMS_CHECK(T, ...) \
((sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T)) || (sizeof((typeof(T)[]){(T){0}, __VA_ARGS__}) == sizeof(T) * 2))

#define NEAT_0_OR_1_CHECK(...) \
(1 __VA_OPT__(* NEAT_1_CHECK(__VA_ARGS__)))

#define NEAT_1_CHECK(a, ...) \
(1 __VA_OPT__(* 0))

#define NEAT_VA_OR(otherwise, ...) \
__VA_ARGS__ NEAT_IF_EMPTY(otherwise, __VA_ARGS__)

#define NEAT_IF_EMPTY(then, ...) \
NEAT_CAT(NEAT_IF_EMPTY_, __VA_OPT__(0))(then)

#define NEAT_IF_EMPTY_(then) then
#define NEAT_IF_EMPTY_0(then)

#define NEAT_IF_NEMPTY(then, ...) __VA_OPT__(then)

#endif /* NEAT_CORE_H */
