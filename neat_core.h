/*
Contains helpful functionality used by all neat libs
*/

#ifndef NEAT_CORE_H
#define NEAT_CORE_H

#include <stdlib.h>

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

#define neat_get_default_allocator() ((void)0, \
(Neat_Allocator){                              \
    .alloc   = neat_default_allocator_alloc,   \
    .dealloc = neat_default_allocator_dealloc, \
    .realloc = neat_default_allocator_realloc, \
    .init    = neat_default_allocator_init,    \
    .deinit  = neat_default_allocator_deinit,  \
})

#define neat_get_noop_allocator() ((void)0, \
(Neat_Allocator){                           \
    .alloc   = neat_noop_allocator_alloc,   \
    .dealloc = neat_noop_allocator_dealloc, \
    .realloc = neat_noop_allocator_realloc, \
    .init    = neat_noop_allocator_init,    \
    .deinit  = neat_noop_allocator_deinit,  \
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
#define NEAT_NARG(...) \
NEAT_NARG_(__VA_OPT__(__VA_ARGS__,)NEAT_5SEQ())

#define NEAT_NARG_(...) \
NEAT_ARG_N(__VA_ARGS__)

// increase SEQ ?

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

#define neat_gurantee_not(exp, not_ty, fallback_ty) \
_Generic(exp, \
    not_ty: (fallback_ty){0}, \
    default: exp \
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
#define NEAT_IF_DEF( macro )      NEAT_ARG2( NEAT_COMMA macro () NEAT_INCL, NEAT_OMIT, )
#define NEAT_STRINGIIFY(X)        NEAT_STRINGIIFY_(X)
#define NEAT_STRINGIIFY_(X)       #X
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
(0 __VA_OPT__(+ 1) ? (__VA_OPT__((void)) (typeof(otherwise)){0} __VA_OPT__(,) __VA_ARGS__) : otherwise)

#endif /* NEAT_CORE_H */
