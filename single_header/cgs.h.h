// This file is generated from amalgamator.c
#if !defined(CGS__H_INCLUDED) && !defined(ONLY_TOSTR)
#define CGS__H_INCLUDED

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#if !defined(CGS_API)
    #define CGS_API
#endif

#if __STDC_VERSION__ >= 202311L
    #define CGS__NODISCARD(...) [[nodiscard(__VA_ARGS__)]]
#elif defined(__GNUC__)
    #define CGS__NODISCARD(...) __attribute__((warn_unused_result))
#else
    #define CGS__NODISCARD(...)
#endif

#ifndef __clang__
    #define cgs__static_assertx(exp, msg) ((void)_Generic((char (*)[!!(exp) + 1])0, char (*)[2]: (msg)))
#else
    #define cgs__static_assertx(exp, msg) \
        ((void)sizeof(struct {            \
            _Static_assert(exp, msg);     \
            int dummy;                    \
        }))
#endif

// IF_DEF and ARG_n stuff
#define CGS__COMMA()              ,
#define CGS__ARG1_( _1, ... )     _1
#define CGS__ARG1( ... )          CGS__ARG1_( __VA_ARGS__ )
#define CGS__ARG2_( _1, _2, ... ) _2
#define CGS__ARG2( ... )          CGS__ARG2_( __VA_ARGS__ )
#define CGS__INCL( ... )          __VA_ARGS__
#define CGS__OMIT( ... )
#define CGS__OMIT1(a, ...)        __VA_ARGS__
#define CGS__IF_DEF( macro )      CGS__ARG2( CGS__COMMA macro () CGS__INCL, CGS__OMIT, )
// IF_DEF and ARG_n stuff end

// FOREACH stuff
#define CGS__PARENS ()

#define CGS__EXPAND(...)  CGS__EXPAND4(CGS__EXPAND4(CGS__EXPAND4(CGS__EXPAND4(__VA_ARGS__))))
#define CGS__EXPAND4(...) CGS__EXPAND3(CGS__EXPAND3(CGS__EXPAND3(CGS__EXPAND3(__VA_ARGS__))))
#define CGS__EXPAND3(...) CGS__EXPAND2(CGS__EXPAND2(CGS__EXPAND2(CGS__EXPAND2(__VA_ARGS__))))
#define CGS__EXPAND2(...) CGS__EXPAND1(CGS__EXPAND1(CGS__EXPAND1(CGS__EXPAND1(__VA_ARGS__))))
#define CGS__EXPAND1(...) __VA_ARGS__

#define CGS__FOREACH(macro, ...) __VA_OPT__(CGS__EXPAND(CGS__FOREACH_HELPER(macro, __VA_ARGS__)))
#define CGS__FOREACH_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(CGS__FOREACH_REPEAT CGS__PARENS(macro, __VA_ARGS__))
#define CGS__FOREACH_REPEAT() CGS__FOREACH_HELPER
// FOREACH stuff end

#define CGS__VA_OR(otherwise, ...) \
__VA_ARGS__ CGS__IF_EMPTY(otherwise, __VA_ARGS__)

#define CGS__IF_EMPTY(then, ...) \
CGS__JOIN(CGS__IF_EMPTY_, __VA_OPT__(0))(then)

#define CGS__IF_EMPTY_(then) then
#define CGS__IF_EMPTY_0(then)

// macro overloading, from https://github.com/stclib/STC/blob/main/include/stc/common.h
#define CGS__MACRO_OVERLOAD(name, ...) \
CGS__JOIN(name ## _,CGS__NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define CGS__JOIN0(a, b) a ## b
#define CGS__JOIN(a, b) CGS__JOIN0(a, b)
#define CGS__NUMARGS(...) CGS__APPLY_ARG_N((__VA_ARGS__, CGS__RSEQ_N))
#define CGS__APPLY_ARG_N(args) CGS__ARG_N args
#define CGS__RSEQ_N 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define CGS__ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) N

#define cgs__has_type(exp, t) \
_Generic((exp), t: true, default: false)

#define cgs__is_array_of(exp, ty) \
cgs__has_type((__typeof__(exp)*){0}, __typeof__(ty)(*)[sizeof(exp)/sizeof(ty)])

#define cgs__coerce_fallback(exp, ty, fallback) \
_Generic(exp, \
    ty: (exp), \
    default: (fallback) \
)

#define cgs__coerce(exp, t) \
cgs__coerce_fallback(exp, t, (t){0})

#define cgs__coerce2(exp, t1, t2) \
_Generic(exp, t1: exp, t2: exp, default: (t1){})

#define cgs__coerce4(exp, t1, t2, t3, t4) \
_Generic(exp, t1: exp, t2: exp, t3: exp, t4: exp, default: (t1){})

#define cgs__coerce_cstr(exp) \
cgs__coerce4(exp, char*, const char*, unsigned char*, const unsigned char*)

#define cgs__coerce_not(exp, not_ty, fallback_ty) \
_Generic(exp, \
    not_ty: (fallback_ty){0}, \
    default: (exp) \
)

#define CGS__CARR_LEN(carr) (sizeof(carr) / sizeof((carr)[0]))

#define CGS__T_APPENDABLE_STRINGS(macro, arg) \
macro(CGS_DStr*    , dstr_ptr  , arg)         \
macro(CGS_StrBuf*  , strbuf_ptr, arg)         \
macro(CGS_MutStrRef, mutstr_ref, arg)

#define CGS__T_NULL_TERMINATED_STRINGS(macro, arg) \
CGS__T_APPENDABLE_STRINGS(macro, arg)              \
macro(char*               , cstr      , arg)       \
macro(unsigned char*      , ucstr     , arg)       \
macro(const char*         , cstr      , arg)       \
macro(const unsigned char*, ucstr     , arg)       \
macro(CGS_ZStrView        , strv      , arg)       \
macro(CGS_DStr            , dstr      , arg)       \
macro(CGS_StrBuf          , strbuf    , arg)       \
macro(const CGS_DStr*     , dstr_ptr  , arg)       \
macro(const CGS_StrBuf*   , strbuf_ptr, arg)

#define CGS__T_MUTABLE_STRINGS(macro, arg) \
CGS__T_APPENDABLE_STRINGS(macro, arg)      \
macro(char*         , cstr , arg)          \
macro(unsigned char*, ucstr, arg)          \

#define CGS__T_ALL_STRINGS(macro, arg)       \
CGS__T_NULL_TERMINATED_STRINGS(macro, arg)  \
macro(CGS_StrView, strv, arg)             \

#define cgs__strv_type_erasure_generic_assoc(T, name, arg) \
T: *(CGS_StrView*)cgs__local_ref(cgs__coerce2(arg, CGS_StrView, CGS_ZStrView)),

#define cgs__strv_type_erasure(anystr) \
_Generic(anystr, \
    CGS_ZStrView: cgs__strv_zstrv1(cgs__coerce(anystr, CGS_ZStrView)), \
    default: anystr \
)

typedef struct CGS__INCOMPAT {char c;} CGS__INCOMPAT; // used in generic association to not match with

struct CGS_Allocator;

typedef struct CGS_Allocation
{
    void *ptr;
    size_t n;
} CGS_Allocation;

typedef CGS_Allocation(*cgs_alloc_func)  (struct CGS_Allocator*, size_t alignment, size_t n);
typedef void          (*cgs_dealloc_func)(struct CGS_Allocator*, void *ptr, size_t n);
typedef CGS_Allocation(*cgs_realloc_func)(struct CGS_Allocator*, void *ptr, size_t alignment, size_t old_size, size_t new_size);

typedef struct CGS_Allocator
{
    cgs_alloc_func alloc;
    cgs_dealloc_func dealloc;
    cgs_realloc_func realloc;
} CGS_Allocator;

typedef struct CGS_Buffer
{
    char *ptr;
    unsigned int cap;
} CGS_Buffer;

CGS_API CGS_Allocation cgs__allocator_invoke_alloc(CGS_Allocator *allocator, size_t alignment, size_t obj_size, size_t nb);
CGS_API void cgs__allocator_invoke_dealloc(CGS_Allocator *allocator, void *ptr, size_t obj_size, size_t nb);
CGS_API CGS_Allocation cgs__allocator_invoke_realloc(CGS_Allocator *allocator, void *ptr, size_t alignment, size_t obj_size, size_t old_nb, size_t new_nb);

CGS_API CGS_Allocator *cgs_get_default_allocator();

#define cgs_alloc(allocator, T, n) \
cgs__allocator_invoke_alloc(allocator, _Alignof(T), sizeof((T){0}), (n))

#define cgs_dealloc(allocator, ptr, T, n) \
cgs__allocator_invoke_dealloc((allocator), (ptr), sizeof((T){0}), (n))

#define cgs_realloc(allocator, ptr, T, old_n, new_n) \
cgs__allocator_invoke_realloc((allocator), (ptr), _Alignof(T), sizeof((T){0}), (old_n), (new_n))

#define cgs_alloc_bytes(allocator, n) \
cgs__allocator_invoke_alloc((allocator), _Alignof(max_align_t), 1, (n))

#define cgs_dealloc_bytes(allocator, ptr, n) \
cgs__allocator_invoke_dealloc((allocator), (ptr), 1, (n))

#define cgs_realloc_bytes(allocator, ptr, old_n, new_n, actual) \
cgs__allocator_invoke_realloc((allocator), (ptr), _Alignof(max_align_t), 1, (old_n), (new_n))

// Dynamic string
typedef struct CGS_DStr
{
    char *chars;
    CGS_Allocator *allocator;
    unsigned int cap;
    unsigned int len;
} CGS_DStr;

// Used as a general purpose non-dynamic string buffer
typedef struct CGS_StrBuf
{
    char *chars;
    unsigned int cap;
    unsigned int len;
} CGS_StrBuf;

// Used to view strings
typedef struct CGS_StrView
{
    const char *chars;
    unsigned int len;
} CGS_StrView;

typedef struct CGS_ZStrView
{
    const char *chars;
    unsigned int len;
} CGS_ZStrView;

// An array of the above, returned by cgs_split
typedef struct CGS_StrViewArray
{
    CGS_StrView *strs;
    unsigned int cap;
    unsigned int len;
} CGS_StrViewArray;

enum CGS__MutStrType
{
    CGS__DSTR_TY = 0,
    CGS__STRBUF_TY,
    CGS__BUF_TY
};

typedef struct CGS_MutStrRef
{
    unsigned char ty;
    union
    {
        CGS_Buffer buf;
        CGS_DStr *dstr;
        CGS_StrBuf *strbuf;
    } str;
} CGS_MutStrRef;

#define CGS__ERROR_NAMES(X)   \
    X(OK)                     \
    X(DST_TOO_SMALL)          \
    X(ALLOC_ERROR)            \
    X(INDEX_OUT_OF_BOUNDS)    \
    X(BAD_RANGE)              \
    X(NOT_FOUND)              \
    X(ALIASING_NOT_SUPPORTED) \
    X(ENCODING_ERROR)         \
    X(IO_ERROR)               \
    X(CALLBACK_EXIT)          \
    X(TOO_MANY_ARGS)          \
    X(NOT_ENOUGH_ARGS)        \
    X(BAD_FORMAT)

enum CGS__ErrorCode
{
#define CGS__PREPEND(e) CGS_##e,

    CGS__ERROR_NAMES(CGS__PREPEND)

#undef CGS__PREPEND
};

typedef struct CGS_Error
{
    unsigned char ec;
} CGS_Error;

typedef struct CGS_Writer
{
    CGS_Error (* const append)(struct CGS_Writer *dst, CGS_StrView str);
} CGS_Writer;

typedef struct CGS_FileWriter
{
    CGS_Writer base;
    FILE *file;
} CGS_FileWriter;

typedef struct CGS_LenWriter
{
    CGS_Writer base;
    unsigned int len;
} CGS_LenWriter;

typedef struct CGS_LenPtrWriter
{
    CGS_Writer base;
    unsigned int *len;
} CGS_LenPtrWriter;

typedef struct CGS_DStrWriter
{
    CGS_Writer base;
    CGS_DStr *dstr;
} CGS_DStrWriter;

typedef struct CGS_StrBufWriter
{
    CGS_Writer base;
    CGS_StrBuf *strbuf;
} CGS_StrBufWriter;

typedef struct CGS_CStrWriter
{
    CGS_Writer base;
    CGS_Buffer buf;
} CGS_CStrWriter;

typedef struct CGS_MutStrRefWriter
{
    CGS_Writer base;
    void *any;
    unsigned int cap_opt;
} CGS_MutStrRefWriter;

typedef struct CGS_ChainWriter
{
    CGS_Writer base;
    CGS_Writer *a;
    CGS_Writer *b;
} CGS_ChainWriter;

typedef struct CGS_CustomWriter
{
    CGS_Writer base;
    void *ctx;
} CGS_CustomWriter;

CGS_API CGS_Error cgs__idstr_append  (CGS_Writer *dst, CGS_StrView str);
CGS_API CGS_Error cgs__istrbuf_append(CGS_Writer *dst, CGS_StrView str);
CGS_API CGS_Error cgs__ibuf_append   (CGS_Writer *dst, CGS_StrView str);
CGS_API CGS_Error cgs__file_append   (CGS_Writer *dst, CGS_StrView str);

static CGS_Error (* const cgs__mutstr_ref_append_func[])(CGS_Writer *dst, CGS_StrView str) =
{
    [CGS__DSTR_TY]   = cgs__idstr_append,
    [CGS__STRBUF_TY] = cgs__istrbuf_append,
    [CGS__BUF_TY]    = cgs__ibuf_append,
};

typedef struct CGS__FixedMutStrRef
{
    char *chars;
    unsigned int *len;
    unsigned int cap;
} CGS__FixedMutStrRef;

typedef struct CGS_ArrayFmt
{
    void *array;
    size_t nb;
    size_t elm_size;
    
    CGS_Error(*elm_tostr)(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
    
    CGS_StrView open;
    CGS_StrView close;
    CGS_StrView separator;
    CGS_StrView trailing_separator;
} CGS_ArrayFmt;

typedef enum CGS__AlignModeEnum
{
    CGS__ALIGNMODE_CENTER,
    CGS__ALIGNMODE_LEFT,
    CGS__ALIGNMODE_RIGHT
} CGS__AlignModeEnum;

typedef struct CGS__AlignModeStruct
{
    unsigned char align_mode;
} CGS__AlignModeStruct;

#define CGS_ALIGN_CENTER (CGS__AlignModeStruct){CGS__ALIGNMODE_CENTER}
#define CGS_ALIGN_LEFT   (CGS__AlignModeStruct){CGS__ALIGNMODE_LEFT}
#define CGS_ALIGN_RIGHT  (CGS__AlignModeStruct){CGS__ALIGNMODE_RIGHT}

#define CGS__ALIGN_CENTER           CGS_ALIGN_CENTER
#define CGS__ALIGN_LEFT             CGS_ALIGN_LEFT
#define CGS__ALIGN_RIGHT            CGS_ALIGN_RIGHT

#define CGS__ALIGN_CGS_ALIGN_CENTER CGS_ALIGN_CENTER
#define CGS__ALIGN_CGS_ALIGN_LEFT   CGS_ALIGN_LEFT
#define CGS__ALIGN_CGS_ALIGN_RIGHT  CGS_ALIGN_RIGHT

typedef struct CGS__AlignFmt
{
    const void *obj;
    CGS_Error(*tostr_p)(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
    unsigned int width;
    CGS__AlignModeStruct align_mode;
    char fill_char;
} CGS__AlignFmt;

typedef struct CGS__RepeatFmt
{
    const void *obj;
    CGS_Error(*tostr_p)(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
    unsigned int n;
} CGS__RepeatFmt;

typedef struct CGS__DStrAppendAllocator
{
    CGS_Allocator base;
    struct CGS_DStr *owner;
} CGS__DStrAppendAllocator;

typedef struct CGS_AppenderState
{
    CGS_DStr appender_dstr;
    CGS__DStrAppendAllocator dstr_append_allocator;
    CGS_StrBuf appender_buf;
} CGS_AppenderState;

#define CGS_Result(T) \
CGS__Result_##T

#define CGS__DeclResult(T) \
typedef struct CGS_Result(T) { T val; CGS_Error err; } CGS_Result(T)

CGS__DeclResult(int);
CGS__DeclResult(CGS_StrView);

#define cgs__fmutstr_ref(s, ...) \
CGS__MACRO_OVERLOAD(cgs__fmutstr_ref, s __VA_OPT__(,) __VA_ARGS__)

#define cgs__fmutstr_ref_1_generic_assoc(T, name, arg) \
__typeof__(T)*: cgs__##name##_as_fmutstr_ref(cgs__coerce(arg, T)),

#define cgs__mutstr_ref_as_fmutstr_ref(s) \
cgs__mutstr_ref_as_fmutstr_ref2(s, &(unsigned int){0})

#define cgs__cstr_as_fmutstr_ref(s) \
cgs__buf_as_fmutstr_ref(cgs__buf_from_cstr(s), &(unsigned int){0})

#define cgs__ucstr_as_fmutstr_ref(s) \
cgs__buf_as_fmutstr_ref(cgs__buf_from_ucstr(s), &(unsigned int){0})

#define cgs__fmutstr_ref_1(s) \
_Generic((__typeof__(s)*)0, \
    CGS__T_MUTABLE_STRINGS(cgs__fmutstr_ref_1_generic_assoc, s) \
    CGS_Buffer*                             : cgs__buf_as_fmutstr_ref(cgs__coerce(s, CGS_Buffer), &(unsigned int){0}), \
    char(*)[sizeof(__typeof__(s))]          : cgs__buf_as_fmutstr_ref(cgs__buf_from_carr(cgs__coerce(s, char*), sizeof(__typeof__(s))), &(unsigned int){0}), \
    unsigned char(*)[sizeof(__typeof__(s))] : cgs__buf_as_fmutstr_ref(cgs__buf_from_ucarr(cgs__coerce(s, unsigned char*), sizeof(__typeof__(s))), &(unsigned int){0}) \
)

#define cgs__fmutstr_ref_2(s, lenp) \
_Generic((__typeof__(s)*)0, \
    char**                                  : cgs__buf_as_fmutstr_ref(cgs__buf_from_cstr(cgs__coerce(s, char*)), lenp), \
    unsigned char**                         : cgs__buf_as_fmutstr_ref(cgs__buf_from_ucstr(cgs__coerce(s, unsigned char*)), lenp), \
    char(*)[sizeof(__typeof__(s))]          : cgs__buf_as_fmutstr_ref(cgs__buf_from_carr(cgs__coerce(s, char*), sizeof(__typeof__(s))), lenp), \
    unsigned char(*)[sizeof(__typeof__(s))] : cgs__buf_as_fmutstr_ref(cgs__buf_from_ucarr(cgs__coerce(s, unsigned char*), sizeof(__typeof__(s))), lenp), \
    CGS_Buffer*                             : cgs__buf_as_fmutstr_ref(cgs__coerce(s, CGS_Buffer), lenp), \
    CGS_MutStrRef*                          : cgs__mutstr_ref_as_fmutstr_ref2(cgs__coerce(s, CGS_MutStrRef), lenp) \
)

#define cgs__fmutstr_ref_zero_len(s, ...) \
CGS__MACRO_OVERLOAD(cgs__fmutstr_ref_zero_len, s __VA_OPT__(,) __VA_ARGS__)

#define cgs__fmutstr_ref_zero_len_1(s) \
_Generic((__typeof__(s)*)0, \
    CGS_DStr**                              : cgs__dstr_ptr_as_fmutstr_ref(cgs__coerce(s, CGS_DStr*)), \
    CGS_Buffer*                             : cgs__buf_as_fmutstr_ref_zero_len(cgs__coerce(s, CGS_Buffer), &(unsigned int){0}), \
    CGS_StrBuf**                            : cgs__strbuf_ptr_as_fmutstr_ref(cgs__coerce(s, CGS_StrBuf*)), \
    char**                                  : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_cstr(cgs__coerce(s, char*)), &(unsigned int){0}), \
    unsigned char**                         : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_ucstr(cgs__coerce(s, unsigned char*)), &(unsigned int){0}), \
    CGS_MutStrRef*                          : cgs__mutstr_ref_as_fmutstr_ref_zero_len(cgs__coerce(s, CGS_MutStrRef), &(unsigned int){0}), \
    char(*)[sizeof(__typeof__(s))]          : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_carr(cgs__coerce(s, char*), sizeof(__typeof__(s))), &(unsigned int){0}), \
    unsigned char(*)[sizeof(__typeof__(s))] : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_ucarr(cgs__coerce(s, unsigned char*), sizeof(__typeof__(s))), &(unsigned int){0}) \
)

#define cgs__fmutstr_ref_zero_len_2(s, lenp) \
_Generic((__typeof__(s)*)0, \
    CGS_Buffer*                             : cgs__buf_as_fmutstr_ref_zero_len(cgs__coerce(s, CGS_Buffer), lenp), \
    char**                                  : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_cstr(cgs__coerce(s, char*)), lenp), \
    unsigned char**                         : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_ucstr(cgs__coerce(s, unsigned char*)), lenp), \
    CGS_MutStrRef*                          : cgs__mutstr_ref_as_fmutstr_ref_zero_len(cgs__coerce(s, CGS_MutStrRef), lenp), \
    char(*)[sizeof(__typeof__(s))]          : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_carr(cgs__coerce(s, char*), sizeof(__typeof__(s))), lenp), \
    unsigned char(*)[sizeof(__typeof__(s))] : cgs__buf_as_fmutstr_ref_zero_len(cgs__buf_from_ucarr(cgs__coerce(s, unsigned char*), sizeof(__typeof__(s))), lenp) \
)

#define cgs_len(anystr) \
_Generic(anystr, \
    char*                : strlen(cgs__coerce_fallback(anystr, char*, "")), \
    unsigned char*       : strlen((char*) cgs__coerce_fallback(anystr, unsigned char*, "")), \
    CGS_DStr             : ((void)0, cgs__coerce(anystr, CGS_DStr).len), \
    CGS_DStr*            : ((void)0, cgs__coerce(anystr, CGS_DStr*)->len), \
    CGS_StrView          : ((void)0, cgs__coerce(anystr, CGS_StrView).len), \
    CGS_ZStrView         : ((void)0, cgs__coerce(anystr, CGS_ZStrView).len), \
    CGS_StrBuf           : ((void)0, cgs__coerce(anystr, CGS_StrBuf).len), \
    CGS_StrBuf*          : ((void)0, cgs__coerce(anystr, CGS_StrBuf*)->len), \
    CGS_MutStrRef        : ((void)0, cgs__mutstr_ref_len(cgs__coerce(anystr, CGS_MutStrRef))), \
    const char*          : strlen(cgs__coerce_fallback(anystr, const char*, "")), \
    const unsigned char* : strlen((char*) cgs__coerce_fallback(anystr, const unsigned char*, "")), \
    const CGS_DStr*      : ((void)0, cgs__coerce(anystr, const CGS_DStr*)->len), \
    const CGS_StrBuf*    : ((void)0, cgs__coerce(anystr, const CGS_StrBuf*)->len) \
)

#define cgs__cap_generic_assoc(T, name, arg) \
__typeof__(T)*: cgs__##name##_cap,

#define cgs_cap(anystr)                                                       \
_Generic((__typeof__(anystr)*)0,                                              \
    CGS__T_ALL_STRINGS(cgs__cap_generic_assoc, anystr)                         \
    char(*)[sizeof(anystr)]: cgs__return_as_u32,                              \
    unsigned char(*)[sizeof(anystr)]: cgs__return_as_u32                      \
)(_Generic((__typeof__(anystr)*){0},                                          \
    char(*)[sizeof(__typeof__(anystr))]: sizeof(__typeof__(anystr)),          \
    unsigned char(*)[sizeof(__typeof__(anystr))]: sizeof(__typeof__(anystr)), \
    default: cgs__strv_type_erasure(anystr)                                   \
))

#define cgs__chars_generic_assoc(T, name, arg) \
T: cgs__##name##_as_cstr,

#define cgs_chars(anystr) \
_Generic(anystr, \
    CGS__T_NULL_TERMINATED_STRINGS(cgs__chars_generic_assoc, anystr) \
    CGS__INCOMPAT: 0 \
)(cgs__strv_type_erasure(anystr))

#define cgs_equal(anystr1, anystr2) \
cgs__strv_equal(cgs__strv_1(anystr1), cgs__strv_1(anystr2))

#define cgs_dup(anystr_src, ...) \
cgs__dstr_init_from(cgs__strv_1(anystr_src), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

#define cgs_copy(mutstr_dst, anystr_src) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_copy(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs__strv_1(anystr_src)), \
    CGS_DStr*     : cgs__dstr_copy(cgs__coerce(mutstr_dst, CGS_DStr*), cgs__strv_1(anystr_src)), \
    default       : cgs__fmutstr_ref_copy(cgs__fmutstr_ref_zero_len(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs__strv_1(anystr_src)) \
)

#define cgs_putc(writer_dst, c) \
cgs__invoke_writer(cgs_writer_ptr(writer_dst), (CGS_StrView){.chars = &(char){c}, .len = 1})

#define cgs_append(writer_dst, anystr_src) \
cgs__invoke_writer(cgs_writer_ptr(writer_dst), cgs__strv_1(anystr_src))

#define cgs_appendln(writer_dst, anystr_src) \
cgs__invoke_writer_ln(cgs_writer_ptr(writer_dst), cgs__strv_1(anystr_src))

#define cgs_fwrite(stream, anystr_src) \
cgs_append(_Generic(stream,FILE*:stream), anystr_src)

#define cgs_fwriteln(stream, anystr_src) \
cgs_appendln(_Generic(stream,FILE*:stream), anystr_src)

#define cgs_write(anystr_src) \
cgs_fwrite(stdout, anystr_src)

#define cgs_writeln(anystr_src) \
cgs_fwriteln(stdout, anystr_src)

#define cgs_insert(mutstr_dst, anystr_src, idx) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_insert(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs__strv_1(anystr_src), idx), \
    CGS_DStr*     : cgs__dstr_insert(cgs__coerce(mutstr_dst, CGS_DStr*), cgs__strv_1(anystr_src), idx), \
    default       : cgs__fmutstr_ref_insert(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs__strv_1(anystr_src), idx) \
)

#define cgs_prepend(mutstr_dst, anystr_src) \
cgs_insert(mutstr_dst, anystr_src, 0)

#define cgs_find(anystr_hay, anystr_needle) \
cgs__strv_find(cgs__strv_1(anystr_hay), cgs__strv_1(anystr_needle))

#define cgs_count(anystr_hay, anystr_needle) \
cgs__strv_count(cgs__strv_1(anystr_hay), cgs__strv_1(anystr_needle))

#define cgs_spn(anystr, anystr_charset) \
cgs__strv_spn(cgs__strv_1(anystr), cgs__strv_1(anystr_charset))

#define cgs_cspn(anystr, anystr_charset) \
cgs__strv_cspn(cgs__strv_1(anystr), cgs__strv_1(anystr_charset))

#define cgs_trim_view(anystr) \
cgs__trim_view(cgs__strv_1(anystr))

#define cgs_trim(mutstr) \
cgs__trim(cgs__fmutstr_ref(mutstr))

#define cgs_clear(mutstr) \
cgs__fmutstr_ref_clear(cgs__fmutstr_ref_zero_len(mutstr)) \

#define cgs_starts_with(anystr_hay, anystr_needle) \
cgs__strv_starts_with(cgs__strv_1(anystr_hay), cgs__strv_1(anystr_needle))

#define cgs_ends_with(anystr_hay, anystr_needle) \
cgs__strv_ends_with(cgs__strv_1(anystr_hay), cgs__strv_1(anystr_needle))

#define cgs_map_chars(mutstr, map_func, ...) \
cgs__map_chars(cgs__fmutstr_ref(mutstr), map_func, CGS__VA_OR(NULL, __VA_ARGS__))

#define cgs_tolower(mutstr) \
cgs__chars_tolower(cgs__fmutstr_ref(mutstr))

#define cgs_toupper(mutstr) \
cgs__chars_toupper(cgs__fmutstr_ref(mutstr))

#define cgs_replace(mutstr_dst, anystr_target, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)), \
    CGS_DStr* : cgs__dstr_replace(cgs__coerce(mutstr_dst, CGS_DStr*), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)), \
    default   : cgs__fmutstr_ref_replace(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)) \
)

#define cgs_replace_first(mutstr_dst, anystr_target, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace_first(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)), \
    CGS_DStr*     : cgs__dstr_replace_first(cgs__coerce(mutstr_dst, CGS_DStr*), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)), \
    default       : cgs__fmutstr_ref_replace_first(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs__strv_1(anystr_target), cgs__strv_1(anystr_replacement)) \
)

#define cgs_replace_range(mutstr_dst, begin, end, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace_range(cgs__coerce(mutstr_dst, CGS_MutStrRef), begin, end, cgs__strv_1(anystr_replacement)), \
    CGS_DStr*     : cgs__dstr_replace_range(cgs__coerce(mutstr_dst, CGS_DStr*), begin, end, cgs__strv_1(anystr_replacement)), \
    default       : cgs__fmutstr_ref_replace_range(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), begin, end, cgs__strv_1(anystr_replacement)) \
)

#define cgs_split(anystr, anystr_delim, ...) \
cgs__strv_split(cgs__strv_1(anystr), cgs__strv_1(anystr_delim), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

#define cgs_split_iter(anystr, anystr_delim, callback, ...) \
cgs__strv_split_iter(cgs__strv_1(anystr), cgs__strv_1(anystr_delim), callback, CGS__VA_OR(NULL, __VA_ARGS__))

#define cgs_join(mutstr_dst, strv_arr, anystr_delim) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__strv_arr_join(cgs__coerce(mutstr_dst, CGS_MutStrRef), strv_arr, cgs__strv_1(anystr_delim)), \
    CGS_DStr*     : cgs__strv_arr_join_into_dstr(cgs__coerce(mutstr_dst, CGS_DStr*), strv_arr, cgs__strv_1(anystr_delim)), \
    default       : cgs__strv_arr_join_into_fmutstr_ref(cgs__fmutstr_ref_zero_len(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), strv_arr, cgs__strv_1(anystr_delim)) \
)

#define cgs_next_tok(strv_ptr, delim) \
cgs__next_tok(strv_ptr, cgs__strv_1(delim))

#define cgs_next_tok_any(strv_ptr, delim_set) \
cgs__next_tok_any(strv_ptr, cgs__strv_1(delim_set))

#define cgs_skip(anystr_src, delim) \
cgs__skip(cgs__strv_1(anystr_src), cgs__strv_1(delim))

#define cgs_skip_any(anystr_src, delim_set) \
cgs__skip_any(cgs__strv_1(anystr_src), cgs__strv_1(delim_set))

#define cgs_del(mutstr_dst, begin, end) \
cgs__fmutstr_ref_delete_range(cgs__fmutstr_ref(mutstr_dst), begin, end)

#define cgs_fread_until(mutstr_dst, stream, delim) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_fread_until(cgs__coerce(mutstr_dst, CGS_MutStrRef), stream, delim), \
    CGS_DStr*     : cgs__dstr_fread_until(cgs__coerce(mutstr_dst, CGS_DStr*), stream, delim), \
    default       : cgs__fmutstr_ref_fread_until(cgs__fmutstr_ref_zero_len(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), stream, delim) \
)

#define cgs_append_fread_until(mutstr_dst, stream, delim) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_append_fread_until(cgs__coerce(mutstr_dst, CGS_MutStrRef), stream, delim), \
    CGS_DStr*     : cgs__dstr_append_fread_until(cgs__coerce(mutstr_dst, CGS_DStr*), stream, delim), \
    default       : cgs__fmutstr_ref_append_fread_until(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), stream, delim) \
)

#define cgs_fread_line(mutstr_dst, stream) \
cgs_fread_until(mutstr_dst, stream, '\n')

#define cgs_append_fread_line(mutstr_dst, stream) \
cgs_append_fread_until(mutstr_dst, stream, '\n')

#define cgs_read_line(mutstr_dst) \
cgs_fread_line(mutstr_dst, stdin)

#define cgs_append_read_line(mutstr_dst) \
cgs_append_fread_line(mutstr_dst, stdin)

#define cgs_read_until(mutstr_dst, delim) \
cgs_fread_until(mutstr_dst, stdin, delim)

#define cgs_append_read_until(mutstr_dst, delim) \
cgs_append_fread_until(mutstr_dst, stdin, delim)

static inline CGS_MutStrRefWriter cgs__mutstr_ref_to_writer(CGS_MutStrRef ref)
{
    CGS_MutStrRefWriter ret = {.base = {.append = cgs__mutstr_ref_append_func[ref.ty] }};
    ret.cap_opt = (unsigned int) -1;
    
    switch(ref.ty)
    {
        case CGS__DSTR_TY:
        case CGS__STRBUF_TY:
            ret.any = ref.str.dstr; // both share the same pointer, so its fine
            break;
        case CGS__BUF_TY:
            ret.any = ref.str.buf.ptr;
            ret.cap_opt = ref.str.buf.cap;
            break;
        default:
            ret.any = NULL;
    }
    
    return ret;
}

#define cgs__writer_impl(writer)  \
_Generic((__typeof__(writer)*)0,  \
    CGS_Writer*      : (writer),  \
    CGS_FileWriter*  : (writer),  \
    CGS_LenWriter*   : (writer),  \
    CGS_DStrWriter*  : (writer),  \
    CGS_CStrWriter*  : (writer),  \
    CGS_ChainWriter* : (writer),  \
    CGS_CustomWriter*: (writer),  \
    cgs__convert_to_writer_generic_associations(writer, CGS__EXPAND1) \
)

#define cgs_writer(writer, ...) \
__VA_OPT__( cgs__chain_writer(writer, __VA_ARGS__) ) \
CGS__IF_EMPTY(cgs__writer_impl(writer), __VA_ARGS__)

#define cgs__writer_ptr_impl(writer) \
(CGS_Writer*)                      \
_Generic((__typeof__(writer)*)0,   \
    CGS_Writer**      : (writer),  \
    CGS_FileWriter**  : (writer),  \
    CGS_LenWriter**   : (writer),  \
    CGS_DStrWriter**  : (writer),  \
    CGS_CStrWriter**  : (writer),  \
    CGS_ChainWriter** : (writer),  \
    CGS_CustomWriter**: (writer),  \
    cgs__convert_to_writer_generic_associations(writer, cgs__local_ref) \
)

#define cgs_writer_ptr(writer, ...) \
__VA_OPT__( \
    (CGS_Writer*) cgs__local_ref( \
        cgs__chain_writer(writer, __VA_ARGS__) \
    ) \
) \
CGS__IF_EMPTY( \
    cgs__writer_ptr_impl(writer), \
    __VA_ARGS__ \
)

#define cgs__convert_to_writer_generic_associations(writer, macro) \
    CGS_DStr**             : macro((CGS_DStrWriter){.base = {.append = cgs__idstr_append}, .dstr = cgs__coerce(writer, CGS_DStr*)}), \
    CGS_StrBuf**           : macro((CGS_StrBufWriter){.base = {.append = cgs__istrbuf_append}, .strbuf = cgs__coerce(writer, CGS_StrBuf*)}), \
    CGS_MutStrRef*         : macro(cgs__mutstr_ref_to_writer(cgs__coerce(writer, CGS_MutStrRef))), \
    char(*)[sizeof(__typeof__(writer))]: macro((CGS_CStrWriter){.base = {.append = cgs__ibuf_append}, .buf = {.cap = sizeof(__typeof__(writer)), .ptr = (char*) cgs__coerce(writer, char*)}}), \
    unsigned char(*)[sizeof(__typeof__(writer))]: macro((CGS_CStrWriter){.base = {.append = cgs__ibuf_append}, .buf = {.cap = sizeof(__typeof__(writer)), .ptr = (char*) cgs__coerce(writer, unsigned char*)}}), \
    FILE**                 : macro((CGS_FileWriter){.base = {.append = cgs__file_append}, .file = cgs__coerce(writer, FILE*)}), \
    unsigned int**         : macro((CGS_LenPtrWriter){.base = {.append = cgs__len_ptr_writer_append}, .len = cgs__coerce(writer, unsigned int*)}) \

#define cgs__local_ref(...) \
(&(__typeof__(__VA_ARGS__)[]){__VA_ARGS__}[0])

#define cgs_len_writer() \
(CGS_LenWriter){.base = {.append = cgs__len_writer_append}}

#define cgs__chain_writer(writer_1, writer_2) \
(CGS_ChainWriter){.base = {.append = cgs__chain_writer_append}, .a = cgs__writer_ptr_impl(writer_1), .b = cgs__writer_ptr_impl(writer_2)}

#define cgs_strv_arr_from(strv_carr, ...) \
cgs__strv_arr_from(strv_carr, CGS__VA_OR((cgs__static_assertx(cgs__is_array_of((strv_carr), CGS_StrView), "Must pass StrView[N] or StrView* with length argument"), CGS__CARR_LEN(strv_carr)), __VA_ARGS__))

#define CGS__STRV_COMMA(anystr) \
cgs__strv_1(anystr),

#define cgs_strv_arr(...)                                                                  \
(                                                                                          \
(CGS_StrViewArray) {                                                                       \
    .len  = CGS__CARR_LEN(((CGS_StrView[]){CGS__FOREACH(CGS__STRV_COMMA, __VA_ARGS__)})),  \
    .cap  = CGS__CARR_LEN(((CGS_StrView[]){CGS__FOREACH(CGS__STRV_COMMA, __VA_ARGS__)})),  \
    .strs = (CGS_StrView[]){CGS__FOREACH(CGS__STRV_COMMA, __VA_ARGS__)}                    \
}                                                                                          \
)

// Calls strlen on the cstr to determine its length
// if no capacity is passed, then:
// if char[] is passed, it uses the size of the array as capacity
// if char* is passed, the capacity is length+1
#define cgs_strbuf_init_from_cstr(cstr, ...) \
CGS__MACRO_OVERLOAD(cgs__strbuf_init_from_cstr, cstr __VA_OPT__(,) __VA_ARGS__)

#define cgs__strbuf_init_from_cstr_1(cstr) \
_Generic(&(__typeof__(cstr)){0},          \
    char(*)[sizeof(__typeof__(cstr))]         : cgs__strbuf_from_cstr_cap((const char*)(cstr), sizeof(__typeof__(cstr))), \
    unsigned char(*)[sizeof(__typeof__(cstr))]: cgs__strbuf_from_cstr_cap((const char*)(cstr), sizeof(__typeof__(cstr))), \
    char**                                    : cgs__strbuf_from_cstr((const char*)(cstr)), \
    unsigned char**                           : cgs__strbuf_from_cstr((const char*)(cstr))  \
)

#define cgs__strbuf_init_from_cstr_2(cstr, cap) \
cgs__strbuf_from_cstr_cap(_Generic((cstr), char*:(char*)(cstr), unsigned char*:(char*)(cstr) ), cap)

// Does not call strlen on the buf
// Sets the first byte to '\0'
#define cgs_strbuf_init_from_buf(buf, ...) \
CGS__MACRO_OVERLOAD(cgs__strbuf_init_from_buf, buf __VA_OPT__(,) __VA_ARGS__)

#define cgs__carr_to_buf(carr) \
_Generic(&(__typeof__(carr)){0}, \
    char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
    unsigned char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
    CGS_Buffer*: (carr) \
)

#define cgs__strbuf_init_from_buf_1(buf) \
cgs__strbuf_from_buf(cgs__carr_to_buf(buf))

#define cgs__strbuf_init_from_buf_2(buf, cap_) \
cgs__strbuf_from_buf((CGS_Buffer){.ptr = (char*) _Generic(buf,char*:(buf),unsigned char*:(buf),void*:(buf)), .cap = (cap_)})

#define cgs__cstr_to_buf(carr, ...) \
CGS__MACRO_OVERLOAD(cgs__cstr_to_buf, carr __VA_OPT__(,) __VA_ARGS__)

#define cgs__cstr_to_buf_1(carr) \
_Generic((__typeof__(carr)*){0}, \
    char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
    unsigned char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)} \
)

#define cgs__cstr_to_buf_2(carr_or_ptr, cap_) \
((void)_Generic(carr_or_ptr, \
    char(*)[sizeof(__typeof__(carr_or_ptr))]: 0, \
    unsigned char(*)[sizeof(__typeof__(carr_or_ptr))]: 0, \
    char*: 0, \
    unsigned char*: 0 \
), \
(CGS_Buffer){.ptr = (char*) (carr_or_ptr), .cap = (cap_)})

#define cgs_mutstr_ref(mutstr, ...) \
CGS__JOIN(cgs__mutstr_ref, __VA_OPT__(2))((mutstr) __VA_OPT__(,) __VA_ARGS__)

#define cgs__mutstr_ref(mutstr)                                               \
_Generic((__typeof__(mutstr)*){0},                                            \
char**                                       : cgs__cstr_as_mutstr_ref,       \
unsigned char**                              : cgs__ucstr_as_mutstr_ref,      \
CGS_DStr**                                   : cgs__dstr_ptr_as_mutstr_ref,   \
CGS_StrBuf**                                 : cgs__strbuf_ptr_as_mutstr_ref, \
CGS_MutStrRef*                               : cgs__mutstr_ref_as_mutstr_ref, \
char(*)[sizeof(__typeof__(mutstr))]          : cgs__buf_as_mutstr_ref,        \
unsigned char(*)[sizeof(__typeof__(mutstr))] : cgs__buf_as_mutstr_ref         \
)(_Generic((__typeof__(mutstr)*){0},                                          \
    char(*)[sizeof(__typeof__(mutstr))]          : (CGS_Buffer){.ptr =         cgs__coerce(mutstr, char*),          .cap = sizeof(__typeof__(mutstr))}, \
    unsigned char(*)[sizeof(__typeof__(mutstr))] : (CGS_Buffer){.ptr = (char*) cgs__coerce(mutstr, unsigned char*), .cap = sizeof(__typeof__(mutstr))}, \
    default: (mutstr) \
))

#define cgs__mutstr_ref2(carr_or_ptr, cap_) \
(CGS_MutStrRef){.ty = CGS__BUF_TY, .str.buf = cgs__cstr_to_buf(carr_or_ptr, cap_)} \

#define cgs_appender(mutstr_owner, appender_state) \
cgs__make_appender_mutstr_ref(cgs_mutstr_ref(mutstr_owner), (appender_state))

#define cgs_commit_appender(mutstr_owner, appender) \
cgs__mutstr_ref_commit_appender(cgs_mutstr_ref(mutstr_owner), appender)

#define cgs_strv(anystr, ...) \
CGS__MACRO_OVERLOAD(cgs__strv, anystr __VA_OPT__(,) __VA_ARGS__)

#define cgs__strv_generic_assoc(T, name, arg) \
T: cgs__strv_##name##arg,

#define cgs__strv_1(anystr)                        \
_Generic(anystr,                                   \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 1) \
    CGS__INCOMPAT: 0                               \
)(cgs__strv_type_erasure(anystr))

#define cgs__strv_2(anystr, begin, ...)            \
_Generic(anystr,                                   \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 2) \
    CGS__INCOMPAT: 0                               \
)(cgs__strv_type_erasure(anystr), begin)

#define cgs__strv_3(anystr, begin, end)            \
_Generic(anystr,                                   \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 3) \
    CGS__INCOMPAT: 0                               \
)(cgs__strv_type_erasure(anystr), begin, end)

#define cgs_zstrv(nstr, ...) \
CGS__MACRO_OVERLOAD(cgs__zstrv, nstr __VA_OPT__(,) __VA_ARGS__)

#define cgs__zstrv_generic_assoc(T, name, arg) \
T: cgs__zstrv_##name##arg,

#define cgs__zstrv_1(nstr) \
_Generic(nstr, \
    CGS__T_NULL_TERMINATED_STRINGS(cgs__zstrv_generic_assoc, 1) \
    CGS__INCOMPAT: 0 \
)(nstr)

#define cgs__zstrv_2(nstr, begin) \
_Generic(nstr, \
    CGS__T_NULL_TERMINATED_STRINGS(cgs__zstrv_generic_assoc, 2) \
    CGS__INCOMPAT: 0 \
)(nstr, begin)

#define cgs_dstr_init(...) \
__VA_OPT__(CGS__MACRO_OVERLOAD(cgs__dstr_init, __VA_ARGS__)) \
CGS__IF_EMPTY(cgs__dstr_init_0() __VA_OPT__(,) __VA_ARGS__)

#define cgs__dstr_init_0() \
cgs__dstr_init(0, cgs_get_default_allocator())

#define cgs__dstr_init_1(cap) \
cgs__dstr_init((cap), cgs_get_default_allocator())

#define cgs__dstr_init_2(cap, allocator) \
cgs__dstr_init((cap), (allocator))

#define cgs_dstr_init_from(anystr_src, ...) \
cgs__dstr_init_from(cgs__strv_1(anystr_src), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

#define cgs_dstr_deinit(dstr) \
cgs__dstr_deinit(dstr)

#define cgs_dstr_shrink_to_fit(dstr) \
cgs__dstr_shrink_to_fit(dstr)

#define cgs_dstr_ensure_cap(dstr, new_cap) \
cgs__dstr_ensure_cap(dstr, new_cap)

// helper macro
#define cgs__tostr_each(x) \
cgs__get_tostr_func(__typeof__(x))(cgs__as_writer, x, (CGS_StrView){}); \

#define cgs__tostr_foreach_arg(...) \
__VA_OPT__( \
CGS__FOREACH(cgs__tostr_each, __VA_ARGS__); \
)

#define cgs_append_tostr_many(writer_dst, ...) \
do \
{ \
    CGS_Writer *cgs__as_writer = cgs_writer_ptr(writer_dst); \
    (void)cgs__as_writer; \
    cgs__tostr_foreach_arg(__VA_ARGS__); \
} while(0)

#define cgs_tostr_many(mutstr_dst, ...) \
do \
{ \
    CGS_MutStrRef cgs__dst_as_mutstr_ref = cgs_mutstr_ref(mutstr_dst); \
    cgs_clear(cgs__dst_as_mutstr_ref); \
    CGS_Writer *cgs__as_writer = cgs_writer_ptr(cgs__dst_as_mutstr_ref); \
    cgs__tostr_foreach_arg(__VA_ARGS__); \
} while(0)

#define cgs__arg_count_each(a) \
+1

#define cgs__as_ptr(a) \
(void*)&(__typeof__(((void)0,a))[]){(a),}[0]

#define cgs__as_ptr_elm(a) \
cgs__as_ptr(a),

#define cgs__tostr_p_func_elm(a) \
(CGS_Error(*)(CGS_Writer*, const void*, CGS_StrView))cgs__get_tostr_p_func(__typeof__(a)),

#define cgs__fmt_helper(fmt_func, writer, fmt, ...) \
    __VA_OPT__(fmt_func(writer, cgs_zstrv(fmt), 0 CGS__FOREACH(cgs__arg_count_each, __VA_ARGS__), (void*[]){CGS__FOREACH(cgs__as_ptr_elm, __VA_ARGS__)}, (CGS_Error(*[])(CGS_Writer*,const void*, CGS_StrView)){CGS__FOREACH(cgs__tostr_p_func_elm, __VA_ARGS__)})) \
    CGS__IF_EMPTY((fmt_func(cgs_writer_ptr(writer), cgs_zstrv(fmt), 0, NULL, NULL)), __VA_ARGS__)

#define cgs_appendf(writer_dst, fmt, ...) \
cgs__fmt_helper(cgs__append_fmt, cgs_writer_ptr(writer_dst), fmt, __VA_ARGS__)

#define cgs_appendfln(writer_dst, fmt, ...) \
cgs__fmt_helper(cgs__appendln_fmt_, cgs_writer_ptr(writer_dst), fmt, __VA_ARGS__)

#define cgs_fmt(mutstr_dst, fmt, ...) \
cgs__fmt_helper(cgs__append_fmt, cgs_writer_ptr(cgs__clear_and_return(cgs_mutstr_ref(mutstr_dst))), fmt, __VA_ARGS__)

#define cgs_fprintf(f, fmt, ...) \
cgs_appendf(_Generic(f, FILE*:f), fmt, __VA_ARGS__)

#define cgs_fprintfln(f, fmt, ...) \
cgs_appendfln(_Generic(f, FILE*:f), fmt, __VA_ARGS__)

#define cgs_printf(fmt, ...) \
cgs_fprintf(stdout, fmt, __VA_ARGS__)

#define cgs_printfln(fmt, ...) \
cgs_fprintfln(stdout, fmt, __VA_ARGS__)

#define cgs_sprintf(mutstr_dst, fmt, ...) \
cgs_fmt(mutstr_dst, fmt, __VA_ARGS__)

#define cgs_sprintfln(mutstr_dst, fmt, ...) \
cgs__fmt_helper(cgs__appendln_fmt_, cgs_writer_ptr(cgs__clear_and_return(cgs_mutstr_ref(mutstr_dst))), fmt, __VA_ARGS__)

#define cgs_asprintf(allocator_or_fmt, ...) \
cgs__fmt_helper( _Generic(allocator_or_fmt, CGS_Allocator*: cgs__asprintf_with_allocator, default: cgs__asprintf), cgs_writer_ptr(cgs__local_ref( \
cgs_dstr_init(0, _Generic(allocator_or_fmt, CGS_Allocator*: cgs__coerce(allocator_or_fmt, CGS_Allocator*), default: cgs_get_default_allocator())) \
)), _Generic(allocator_or_fmt, CGS_Allocator*: (CGS__asprintf_FMT_ARG(allocator_or_fmt, __VA_ARGS__)), default: cgs__coerce_cstr(allocator_or_fmt)), __VA_ARGS__)

// if allocator:
//  if __VA_ARGS__ empty: error
//  if __VA_ARGS__ not empty: use ARG1 as fmt
#define CGS__asprintf_FMT_ARG(allocator, ...) \
__VA_OPT__(CGS__ARG1(__VA_ARGS__)) \
CGS__IF_EMPTY( \
    (cgs__static_assertx(_Generic(allocator, CGS_Allocator*: 0, default: 1), "Missing `const char *fmt` argument"), \
    cgs__coerce_cstr(allocator)), \
    __VA_ARGS__ \
)

typedef char               cgs__c;
typedef signed char        cgs__sc;
typedef unsigned char      cgs__uc;
typedef short              cgs__s;
typedef unsigned short     cgs__us;
typedef int                cgs__i;
typedef unsigned int       cgs__ui;
typedef long               cgs__l;
typedef unsigned long      cgs__ul;
typedef long long          cgs__ll;
typedef unsigned long long cgs__ull;

#define CGS__MCALL(macro, arglist) macro arglist

#define CGS__INTEGER_TYPES(CGS__X, extra, ...) \
CGS__X(cgs__c , extra) \
CGS__X(cgs__sc, extra) \
CGS__X(cgs__uc, extra) \
CGS__X(cgs__s , extra) \
CGS__X(cgs__us, extra) \
CGS__X(cgs__i , extra) \
CGS__X(cgs__ui, extra) \
CGS__X(cgs__l , extra) \
CGS__X(cgs__ul, extra) \
CGS__X(cgs__ll, extra) \
CGS__MCALL(CGS__VA_OR(CGS__X, __VA_ARGS__), (cgs__ull, extra))

#define CGS__FLOATING_TYPES(CGS__X, extra, last_call) \
CGS__X(float, extra) \
last_call(double, extra)

#define CGS__X(ty, extra) \
typedef struct CGS__Integer_d_Fmt_##ty \
{ \
    ty obj; \
} CGS__Integer_d_Fmt_##ty; \
typedef struct CGS__Integer_x_Fmt_##ty \
{ \
    ty obj; \
} CGS__Integer_x_Fmt_##ty; \
typedef struct CGS__Integer_o_Fmt_##ty \
{ \
    ty obj; \
} CGS__Integer_o_Fmt_##ty; \
typedef struct CGS__Integer_b_Fmt_##ty \
{ \
    ty obj; \
} CGS__Integer_b_Fmt_##ty; \
typedef struct CGS__Integer_X_Fmt_##ty \
{ \
    ty obj; \
} CGS__Integer_X_Fmt_##ty;

CGS__INTEGER_TYPES(CGS__X, ignore)

#undef CGS__X

#define CGS__X(ty, extra) \
typedef struct CGS__Floating_f_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_f_Fmt_##ty; \
typedef struct CGS__Floating_g_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_g_Fmt_##ty; \
typedef struct CGS__Floating_e_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_e_Fmt_##ty; \
typedef struct CGS__Floating_a_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_a_Fmt_##ty; \
typedef struct CGS__Floating_F_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_F_Fmt_##ty; \
typedef struct CGS__Floating_G_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_G_Fmt_##ty; \
typedef struct CGS__Floating_E_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_E_Fmt_##ty; \
typedef struct CGS__Floating_A_Fmt_##ty \
{ \
    ty obj; \
    int precision; \
} CGS__Floating_A_Fmt_##ty;

CGS__FLOATING_TYPES(CGS__X, ignore, CGS__X)

#undef CGS__X

#define CGS__X_IS_TY(ty, extra) \
ty: 1,

#define CGS__IS_FLOATING(obj) \
_Generic(obj, \
CGS__FLOATING_TYPES(CGS__X_IS_TY, ignore, CGS__X_IS_TY) \
default: 0)

#define CGS__IS_INTEGER(obj) \
_Generic(obj, \
CGS__INTEGER_TYPES(CGS__X_IS_TY, ignore) \
default: 0)

#define CGS__INTEGER_FMT_GENERIC_ASSOCIATIONS(ty, extra) \
ty: \
_Generic((char(*)[CGS__ARG2 extra]){0}, \
char(*)['d']: (CGS__Integer_d_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty)},  \
char(*)['x']: (CGS__Integer_x_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty)},  \
char(*)['o']: (CGS__Integer_o_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty)},  \
char(*)['b']: (CGS__Integer_b_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty)},  \
char(*)['X']: (CGS__Integer_X_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty)},  \
default: 0),

#define CGS__3_VA_OR(otherwise, a,b, ...) \
CGS__VA_OR(otherwise, __VA_ARGS__)

#define CGS__FLOATING_FMT_LAST_GENERIC_ASSOC(ty, extra) \
ty: \
_Generic((char(*)[CGS__ARG2 extra]){0}, \
char(*)['f']: (CGS__Floating_f_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['g']: (CGS__Floating_g_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['e']: (CGS__Floating_e_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['a']: (CGS__Floating_a_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, (-1, CGS__EXPAND1 extra))}, \
char(*)['F']: (CGS__Floating_F_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['G']: (CGS__Floating_G_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['E']: (CGS__Floating_E_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, ( 6, CGS__EXPAND1 extra))}, \
char(*)['A']: (CGS__Floating_A_Fmt_##ty){cgs__coerce(CGS__ARG1 extra, ty), CGS__MCALL(CGS__3_VA_OR, (-1, CGS__EXPAND1 extra))}, \
default: 0)

#define CGS__FLOATING_FMT_GENERIC_ASSOC(ty, extra) \
CGS__FLOATING_FMT_LAST_GENERIC_ASSOC(ty, extra),

#define cgs_nfmt(x, fmt_chr, ...) \
( \
    cgs__static_assertx( (CGS__IS_FLOATING(x) && (fmt_chr == 'f' || fmt_chr == 'g' || fmt_chr == 'e' || fmt_chr == 'a' || fmt_chr == 'F' || fmt_chr == 'G' || fmt_chr == 'E' || fmt_chr == 'A')  ) || (CGS__IS_INTEGER(x) && (fmt_chr == 'd' || fmt_chr == 'x' || fmt_chr == 'o' || fmt_chr == 'b' || fmt_chr == 'X')), "Incorrect formatting char for the type"), \
    cgs__static_assertx((CGS__IS_FLOATING(x) || (1 __VA_OPT__(-1))), "nfmt Integers dont take a third parameter"), \
    _Generic((x), \
        CGS__INTEGER_TYPES(CGS__INTEGER_FMT_GENERIC_ASSOCIATIONS, (x, fmt_chr)) \
        CGS__FLOATING_TYPES(CGS__FLOATING_FMT_GENERIC_ASSOC, (x, fmt_chr __VA_OPT__(,) __VA_ARGS__), CGS__FLOATING_FMT_LAST_GENERIC_ASSOC) \
    ) \
)

#define cgs_nfmt_t(ty, fmt_chr) \
__typeof__(cgs_nfmt((ty)0, fmt_chr))

#define cgs_arrfmt(array, nb, ...) \
CGS__IF_EMPTY(cgs__arrfmt_, __VA_ARGS__) \
__VA_OPT__(cgs__arrfmt_2) \
(array, nb __VA_OPT__(,) __VA_ARGS__)

#define cgs__arrfmt_(array_, nb_) \
((CGS_ArrayFmt){ \
    .array = (array_), \
    .nb = (nb_), \
    .elm_size = sizeof((array_)[0]), \
    .elm_tostr = cgs__get_tostr_p_func(__typeof__((array_)[0])), \
    .open = cgs_strv("{"), \
    .close = cgs_strv("}"), \
    .separator = cgs_strv(", "), \
    .trailing_separator = cgs_strv("") \
})

#define cgs__arrfmt_2(array_, nb_, open_, close_, seperator_, ...) \
((CGS_ArrayFmt){ \
    .array = (array_), \
    .nb = (nb_), \
    .elm_size = sizeof((array_)[0]), \
    .elm_tostr = cgs__get_tostr_p_func(__typeof__((array_)[0])), \
    .open = cgs__strv_1(open_), \
    .close = cgs__strv_1(close_), \
    .separator = cgs__strv_1(seperator_), \
    .trailing_separator = cgs_strv(CGS__VA_OR("", __VA_ARGS__)) \
})

#define cgs_alignfmt(obj_, align_mode_, width_, ...) \
((CGS__AlignFmt){ \
    .obj = cgs__as_ptr(obj_), \
    .tostr_p = cgs__get_tostr_p_func(__typeof__(obj_)), \
    .align_mode = CGS__ALIGN_##align_mode_, \
    .width = width_, \
    .fill_char = CGS__VA_OR(' ', __VA_ARGS__) \
})

#define cgs_repeatfmt(obj_, n_) \
((CGS__RepeatFmt){ \
    .obj = cgs__as_ptr(obj_), \
    .tostr_p = cgs__get_tostr_p_func(__typeof__(obj_)), \
    .n = n_, \
})

#define CGS__INTEGER_TOSTR_GENERIC_ASSOC(ty, extra)        \
CGS__Integer_d_Fmt_##ty : cgs__Integer_d_Fmt_##ty##_tostr, \
CGS__Integer_x_Fmt_##ty : cgs__Integer_x_Fmt_##ty##_tostr, \
CGS__Integer_o_Fmt_##ty : cgs__Integer_o_Fmt_##ty##_tostr, \
CGS__Integer_b_Fmt_##ty : cgs__Integer_b_Fmt_##ty##_tostr, \
CGS__Integer_X_Fmt_##ty : cgs__Integer_X_Fmt_##ty##_tostr, \

#define CGS__FLOATING_TOSTR_LAST_GENERIC_ASSOC(ty, extra)    \
CGS__Floating_f_Fmt_##ty : cgs__Floating_f_Fmt_##ty##_tostr, \
CGS__Floating_g_Fmt_##ty : cgs__Floating_g_Fmt_##ty##_tostr, \
CGS__Floating_e_Fmt_##ty : cgs__Floating_e_Fmt_##ty##_tostr, \
CGS__Floating_a_Fmt_##ty : cgs__Floating_a_Fmt_##ty##_tostr, \
CGS__Floating_F_Fmt_##ty : cgs__Floating_F_Fmt_##ty##_tostr, \
CGS__Floating_G_Fmt_##ty : cgs__Floating_G_Fmt_##ty##_tostr, \
CGS__Floating_E_Fmt_##ty : cgs__Floating_E_Fmt_##ty##_tostr, \
CGS__Floating_A_Fmt_##ty : cgs__Floating_A_Fmt_##ty##_tostr

#define CGS__INTEGER_TOSTR_P_GENERIC_ASSOC(ty, extra)        \
CGS__Integer_d_Fmt_##ty : cgs__Integer_d_Fmt_##ty##_tostr_p, \
CGS__Integer_x_Fmt_##ty : cgs__Integer_x_Fmt_##ty##_tostr_p, \
CGS__Integer_o_Fmt_##ty : cgs__Integer_o_Fmt_##ty##_tostr_p, \
CGS__Integer_b_Fmt_##ty : cgs__Integer_b_Fmt_##ty##_tostr_p, \
CGS__Integer_X_Fmt_##ty : cgs__Integer_X_Fmt_##ty##_tostr_p,

#define CGS__FLOATING_TOSTR_P_LAST_GENERIC_ASSOC(ty, extra)    \
CGS__Floating_f_Fmt_##ty : cgs__Floating_f_Fmt_##ty##_tostr_p, \
CGS__Floating_g_Fmt_##ty : cgs__Floating_g_Fmt_##ty##_tostr_p, \
CGS__Floating_e_Fmt_##ty : cgs__Floating_e_Fmt_##ty##_tostr_p, \
CGS__Floating_a_Fmt_##ty : cgs__Floating_a_Fmt_##ty##_tostr_p, \
CGS__Floating_F_Fmt_##ty : cgs__Floating_F_Fmt_##ty##_tostr_p, \
CGS__Floating_G_Fmt_##ty : cgs__Floating_G_Fmt_##ty##_tostr_p, \
CGS__Floating_E_Fmt_##ty : cgs__Floating_E_Fmt_##ty##_tostr_p, \
CGS__Floating_A_Fmt_##ty : cgs__Floating_A_Fmt_##ty##_tostr_p

#define CGS__FLOATING_TOSTR_GENERIC_ASSOC(ty, extra) \
CGS__FLOATING_TOSTR_LAST_GENERIC_ASSOC(ty, extra),

#define CGS__FLOATING_TOSTR_P_GENERIC_ASSOC(ty, extra) \
CGS__FLOATING_TOSTR_P_LAST_GENERIC_ASSOC(ty, extra),

#define CGS__DEFAULT_TOSTR_GENERIC_ASSOCIATIONS             \
bool                      : cgs__bool_tostr,                \
char*                     : cgs__cstr_tostr,                \
unsigned char*            : cgs__ucstr_tostr,               \
char                      : cgs__char_tostr,                \
signed char               : cgs__schar_tostr,               \
unsigned char             : cgs__uchar_tostr,               \
short                     : cgs__short_tostr,               \
unsigned short            : cgs__ushort_tostr,              \
int                       : cgs__int_tostr,                 \
unsigned int              : cgs__uint_tostr,                \
long                      : cgs__long_tostr,                \
unsigned long             : cgs__ulong_tostr,               \
long long                 : cgs__llong_tostr,               \
unsigned long long        : cgs__ullong_tostr,              \
float                     : cgs__float_tostr,               \
double                    : cgs__double_tostr,              \
CGS_DStr                  : cgs__dstr_tostr,                \
CGS_DStr*                 : cgs__dstr_ptr_tostr,            \
CGS_StrView               : cgs__strv_tostr,                \
CGS_ZStrView              : cgs__zstrv_tostr,               \
CGS_StrBuf                : cgs__strbuf_tostr,              \
CGS_StrBuf*               : cgs__strbuf_ptr_tostr,          \
CGS_MutStrRef             : cgs__mutstr_ref_tostr,          \
const char*               : cgs__cstr_tostr,                \
const unsigned char*      : cgs__ucstr_tostr,               \
const CGS_DStr*           : cgs__dstr_ptr_tostr,            \
const CGS_StrBuf*         : cgs__strbuf_ptr_tostr,          \
CGS_Error                 : cgs__error_tostr,               \
CGS_ArrayFmt              : cgs__array_fmt_tostr,           \
CGS__AlignFmt             : cgs__align_fmt_tostr,           \
CGS__RepeatFmt            : cgs__repeat_fmt_tostr,          \
CGS__INTEGER_TYPES(CGS__INTEGER_TOSTR_GENERIC_ASSOC, ignore) \
CGS__FLOATING_TYPES(CGS__FLOATING_TOSTR_GENERIC_ASSOC, ignore, CGS__FLOATING_TOSTR_LAST_GENERIC_ASSOC)

#define CGS__DEFAULT_TOSTR_P_GENERIC_ASSOCIATIONS                 \
bool                       : cgs__bool_tostr_p,                \
char*                      : cgs__cstr_tostr_p,                \
unsigned char*             : cgs__ucstr_tostr_p,               \
char                       : cgs__char_tostr_p,                \
signed char                : cgs__schar_tostr_p,               \
unsigned char              : cgs__uchar_tostr_p,               \
short                      : cgs__short_tostr_p,               \
unsigned short             : cgs__ushort_tostr_p,              \
int                        : cgs__int_tostr_p,                 \
unsigned int               : cgs__uint_tostr_p,                \
long                       : cgs__long_tostr_p,                \
unsigned long              : cgs__ulong_tostr_p,               \
long long                  : cgs__llong_tostr_p,               \
unsigned long long         : cgs__ullong_tostr_p,              \
float                      : cgs__float_tostr_p,               \
double                     : cgs__double_tostr_p,              \
CGS_DStr                   : cgs__dstr_tostr_p,                \
CGS_DStr*                  : cgs__dstr_ptr_tostr_p,            \
CGS_StrView                : cgs__strv_tostr_p,                \
CGS_ZStrView               : cgs__zstrv_tostr_p,               \
CGS_StrBuf                 : cgs__strbuf_tostr_p,              \
CGS_StrBuf*                : cgs__strbuf_ptr_tostr_p,          \
CGS_MutStrRef              : cgs__mutstr_ref_tostr_p,          \
const char*                : cgs__cstr_tostr_p,                \
const unsigned char*       : cgs__ucstr_tostr_p,               \
const CGS_DStr*            : cgs__dstr_ptr_tostr_p,            \
const CGS_StrBuf*          : cgs__strbuf_ptr_tostr_p,          \
CGS_Error                  : cgs__error_tostr_p,               \
CGS_ArrayFmt               : cgs__array_fmt_tostr_p,           \
CGS__AlignFmt              : cgs__align_fmt_tostr_p,           \
CGS__RepeatFmt             : cgs__repeat_fmt_tostr_p,          \
CGS__INTEGER_TYPES(CGS__INTEGER_TOSTR_P_GENERIC_ASSOC, ignore)  \
CGS__FLOATING_TYPES(CGS__FLOATING_TOSTR_P_GENERIC_ASSOC, ignore, CGS__FLOATING_TOSTR_P_LAST_GENERIC_ASSOC)

#define CGS__TOSTR_FUNCS_GENERIC_ASSOCIATIONS                      \
CGS__IF_DEF(CGS__TOSTR1) (cgs__tostr_type_1 : cgs__tostr_func_1,)  \
CGS__IF_DEF(CGS__TOSTR2) (cgs__tostr_type_2 : cgs__tostr_func_2,)  \
CGS__IF_DEF(CGS__TOSTR3) (cgs__tostr_type_3 : cgs__tostr_func_3,)  \
CGS__IF_DEF(CGS__TOSTR4) (cgs__tostr_type_4 : cgs__tostr_func_4,)  \
CGS__IF_DEF(CGS__TOSTR5) (cgs__tostr_type_5 : cgs__tostr_func_5,)  \
CGS__IF_DEF(CGS__TOSTR6) (cgs__tostr_type_6 : cgs__tostr_func_6,)  \
CGS__IF_DEF(CGS__TOSTR7) (cgs__tostr_type_7 : cgs__tostr_func_7,)  \
CGS__IF_DEF(CGS__TOSTR8) (cgs__tostr_type_8 : cgs__tostr_func_8,)  \
CGS__IF_DEF(CGS__TOSTR9) (cgs__tostr_type_9 : cgs__tostr_func_9,)  \
CGS__IF_DEF(CGS__TOSTR10)(cgs__tostr_type_10: cgs__tostr_func_10,) \
CGS__IF_DEF(CGS__TOSTR11)(cgs__tostr_type_11: cgs__tostr_func_11,) \
CGS__IF_DEF(CGS__TOSTR12)(cgs__tostr_type_12: cgs__tostr_func_12,) \
CGS__IF_DEF(CGS__TOSTR13)(cgs__tostr_type_13: cgs__tostr_func_13,) \
CGS__IF_DEF(CGS__TOSTR14)(cgs__tostr_type_14: cgs__tostr_func_14,) \
CGS__IF_DEF(CGS__TOSTR15)(cgs__tostr_type_15: cgs__tostr_func_15,) \
CGS__IF_DEF(CGS__TOSTR16)(cgs__tostr_type_16: cgs__tostr_func_16,) \
CGS__IF_DEF(CGS__TOSTR17)(cgs__tostr_type_17: cgs__tostr_func_17,) \
CGS__IF_DEF(CGS__TOSTR18)(cgs__tostr_type_18: cgs__tostr_func_18,) \
CGS__IF_DEF(CGS__TOSTR19)(cgs__tostr_type_19: cgs__tostr_func_19,) \
CGS__IF_DEF(CGS__TOSTR20)(cgs__tostr_type_20: cgs__tostr_func_20,) \
CGS__IF_DEF(CGS__TOSTR21)(cgs__tostr_type_21: cgs__tostr_func_21,) \
CGS__IF_DEF(CGS__TOSTR22)(cgs__tostr_type_22: cgs__tostr_func_22,) \
CGS__IF_DEF(CGS__TOSTR23)(cgs__tostr_type_23: cgs__tostr_func_23,) \
CGS__IF_DEF(CGS__TOSTR24)(cgs__tostr_type_24: cgs__tostr_func_24,) \
CGS__IF_DEF(CGS__TOSTR25)(cgs__tostr_type_25: cgs__tostr_func_25,) \
CGS__IF_DEF(CGS__TOSTR26)(cgs__tostr_type_26: cgs__tostr_func_26,) \
CGS__IF_DEF(CGS__TOSTR27)(cgs__tostr_type_27: cgs__tostr_func_27,) \
CGS__IF_DEF(CGS__TOSTR28)(cgs__tostr_type_28: cgs__tostr_func_28,) \
CGS__IF_DEF(CGS__TOSTR29)(cgs__tostr_type_29: cgs__tostr_func_29,) \
CGS__IF_DEF(CGS__TOSTR30)(cgs__tostr_type_30: cgs__tostr_func_30,) \
CGS__IF_DEF(CGS__TOSTR31)(cgs__tostr_type_31: cgs__tostr_func_31,) \
CGS__IF_DEF(CGS__TOSTR32)(cgs__tostr_type_32: cgs__tostr_func_32,) \
CGS__DEFAULT_TOSTR_GENERIC_ASSOCIATIONS

#define CGS__TOSTR_P_FUNCS_GENERIC_ASSOCIATIONS                      \
CGS__IF_DEF(CGS__TOSTR1) (cgs__tostr_type_1 : cgs__tostr_p_func_1,)  \
CGS__IF_DEF(CGS__TOSTR2) (cgs__tostr_type_2 : cgs__tostr_p_func_2,)  \
CGS__IF_DEF(CGS__TOSTR3) (cgs__tostr_type_3 : cgs__tostr_p_func_3,)  \
CGS__IF_DEF(CGS__TOSTR4) (cgs__tostr_type_4 : cgs__tostr_p_func_4,)  \
CGS__IF_DEF(CGS__TOSTR5) (cgs__tostr_type_5 : cgs__tostr_p_func_5,)  \
CGS__IF_DEF(CGS__TOSTR6) (cgs__tostr_type_6 : cgs__tostr_p_func_6,)  \
CGS__IF_DEF(CGS__TOSTR7) (cgs__tostr_type_7 : cgs__tostr_p_func_7,)  \
CGS__IF_DEF(CGS__TOSTR8) (cgs__tostr_type_8 : cgs__tostr_p_func_8,)  \
CGS__IF_DEF(CGS__TOSTR9) (cgs__tostr_type_9 : cgs__tostr_p_func_9,)  \
CGS__IF_DEF(CGS__TOSTR10)(cgs__tostr_type_10: cgs__tostr_p_func_10,) \
CGS__IF_DEF(CGS__TOSTR11)(cgs__tostr_type_11: cgs__tostr_p_func_11,) \
CGS__IF_DEF(CGS__TOSTR12)(cgs__tostr_type_12: cgs__tostr_p_func_12,) \
CGS__IF_DEF(CGS__TOSTR13)(cgs__tostr_type_13: cgs__tostr_p_func_13,) \
CGS__IF_DEF(CGS__TOSTR14)(cgs__tostr_type_14: cgs__tostr_p_func_14,) \
CGS__IF_DEF(CGS__TOSTR15)(cgs__tostr_type_15: cgs__tostr_p_func_15,) \
CGS__IF_DEF(CGS__TOSTR16)(cgs__tostr_type_16: cgs__tostr_p_func_16,) \
CGS__IF_DEF(CGS__TOSTR17)(cgs__tostr_type_17: cgs__tostr_p_func_17,) \
CGS__IF_DEF(CGS__TOSTR18)(cgs__tostr_type_18: cgs__tostr_p_func_18,) \
CGS__IF_DEF(CGS__TOSTR19)(cgs__tostr_type_19: cgs__tostr_p_func_19,) \
CGS__IF_DEF(CGS__TOSTR20)(cgs__tostr_type_20: cgs__tostr_p_func_20,) \
CGS__IF_DEF(CGS__TOSTR21)(cgs__tostr_type_21: cgs__tostr_p_func_21,) \
CGS__IF_DEF(CGS__TOSTR22)(cgs__tostr_type_22: cgs__tostr_p_func_22,) \
CGS__IF_DEF(CGS__TOSTR23)(cgs__tostr_type_23: cgs__tostr_p_func_23,) \
CGS__IF_DEF(CGS__TOSTR24)(cgs__tostr_type_24: cgs__tostr_p_func_24,) \
CGS__IF_DEF(CGS__TOSTR25)(cgs__tostr_type_25: cgs__tostr_p_func_25,) \
CGS__IF_DEF(CGS__TOSTR26)(cgs__tostr_type_26: cgs__tostr_p_func_26,) \
CGS__IF_DEF(CGS__TOSTR27)(cgs__tostr_type_27: cgs__tostr_p_func_27,) \
CGS__IF_DEF(CGS__TOSTR28)(cgs__tostr_type_28: cgs__tostr_p_func_28,) \
CGS__IF_DEF(CGS__TOSTR29)(cgs__tostr_type_29: cgs__tostr_p_func_29,) \
CGS__IF_DEF(CGS__TOSTR30)(cgs__tostr_type_30: cgs__tostr_p_func_30,) \
CGS__IF_DEF(CGS__TOSTR31)(cgs__tostr_type_31: cgs__tostr_p_func_31,) \
CGS__IF_DEF(CGS__TOSTR32)(cgs__tostr_type_32: cgs__tostr_p_func_32,) \
CGS__DEFAULT_TOSTR_P_GENERIC_ASSOCIATIONS

struct cgs__fail_type { int dummy; };
typedef void(*cgs__tostr_fail)(struct cgs__fail_type*);

#define cgs__get_tostr_func(ty) \
_Generic((ty){0}, \
    CGS__TOSTR_FUNCS_GENERIC_ASSOCIATIONS \
)

#define cgs__get_tostr_func_with_default(ty) \
_Generic((ty){0}, \
    CGS__TOSTR_FUNCS_GENERIC_ASSOCIATIONS, \
    default: (cgs__tostr_fail){0} \
)

#define cgs__get_tostr_p_func(ty) \
_Generic((ty){0}, \
    CGS__TOSTR_P_FUNCS_GENERIC_ASSOCIATIONS \
)

// TODO optimization idea, check if whether dst is a string type, and src is a default tostr type, if so, then call an optimized tostr function that writes directly to buffer
#define cgs_tostr(dst, src) \
cgs__get_tostr_func(__typeof__(src))(cgs_writer_ptr(cgs__clear_and_return(cgs_mutstr_ref(dst))), (src), (CGS_StrView){})

#define cgs_append_tostr(dst, src) \
cgs__get_tostr_func(__typeof__(src))(cgs_writer_ptr(dst), (src), (CGS_StrView){})

#define cgs_has_tostr(ty) \
(!cgs__has_type(cgs__get_tostr_func_with_default(__typeof__(ty)), cgs__tostr_fail))

#define cgs_tostr_p(dst, srcp) \
cgs__get_tostr_p_func(__typeof__(*(srcp)))(cgs_writer_ptr(dst), (srcp), (CGS_StrView){})

#define cgs_tostr_len(src) \
cgs__invoke_tostr_len((CGS_Error(*)(CGS_Writer*,const void*, CGS_StrView)) cgs__get_tostr_p_func(__typeof__(src)), &(__typeof__(src)[]){src}[0])

#define cgs_tostr_p_len(srcp) \
cgs__invoke_tostr_len((CGS_Error(*)(CGS_Writer*,const void*, CGS_StrView)) cgs__get_tostr_p_func(__typeof__(*(srcp))), (srcp))

#define CGS__DECL_TOSTR_FUNC(n) \
typedef __typeof__(CGS__MCALL(CGS__ARG1, ADD_TOSTR)) cgs__tostr_type_##n; \
static inline CGS_Error cgs__tostr_func_##n (CGS_Writer *dst, cgs__tostr_type_##n obj, CGS_StrView fmt_arg) \
{ \
    _Static_assert(cgs__has_type(CGS__MCALL(CGS__ARG2, ADD_TOSTR), __typeof__(CGS_Error(*)(CGS_Writer*, cgs__tostr_type_##n, CGS_StrView))), "tostr functions must have signature `CGS_Error(CGS_Writer *dst, T src, CGS_StrView fmt_arg)`"); \
    return CGS__MCALL(CGS__ARG2, ADD_TOSTR) (dst, obj, fmt_arg); \
} \
static inline CGS_Error cgs__tostr_p_func_##n (CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg) \
{ \
    return cgs__tostr_func_##n(dst, * (cgs__tostr_type_##n*) obj, fmt_arg); \
}

CGS_API CGS_StrView cgs__strv_cstr1(const char *str);
CGS_API CGS_StrView cgs__strv_ucstr1(const unsigned char *str);
CGS_API CGS_StrView cgs__strv_dstr1(CGS_DStr str);
CGS_API CGS_StrView cgs__strv_dstr_ptr1(const CGS_DStr *str);
CGS_API CGS_StrView cgs__strv_strv1(CGS_StrView str);
CGS_API CGS_StrView cgs__strv_zstrv1(CGS_ZStrView str);
CGS_API CGS_StrView cgs__strv_strbuf1(CGS_StrBuf str);
CGS_API CGS_StrView cgs__strv_strbuf_ptr1(const CGS_StrBuf *str);
CGS_API CGS_StrView cgs__strv_mutstr_ref1(CGS_MutStrRef str);

CGS_API CGS_StrView cgs__strv_cstr2(const char *str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_ucstr2(const unsigned char *str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_dstr2(CGS_DStr str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_dstr_ptr2(const CGS_DStr *str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_strv2(CGS_StrView str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_zstrv2(CGS_ZStrView str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_strbuf2(CGS_StrBuf str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_strbuf_ptr2(const CGS_StrBuf *str, unsigned int begin);
CGS_API CGS_StrView cgs__strv_mutstr_ref2(CGS_MutStrRef str, unsigned int begin);

CGS_API CGS_StrView cgs__strv_cstr3(const char *str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_ucstr3(const unsigned char *str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_dstr3(CGS_DStr str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_dstr_ptr3(const CGS_DStr *str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_strv3(CGS_StrView str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_zstrv3(CGS_ZStrView str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_strbuf3(CGS_StrBuf str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_strbuf_ptr3(const CGS_StrBuf *str, unsigned int begin, unsigned int end);
CGS_API CGS_StrView cgs__strv_mutstr_ref3(CGS_MutStrRef str, unsigned int begin, unsigned int end);

CGS_API CGS_ZStrView cgs__zstrv_cstr1(const char *str);
CGS_API CGS_ZStrView cgs__zstrv_ucstr1(const unsigned char *str);
CGS_API CGS_ZStrView cgs__zstrv_dstr1(CGS_DStr str);
CGS_API CGS_ZStrView cgs__zstrv_dstr_ptr1(const CGS_DStr *str);
CGS_API CGS_ZStrView cgs__zstrv_strv1(CGS_ZStrView str);
CGS_API CGS_ZStrView cgs__zstrv_strbuf1(CGS_StrBuf str);
CGS_API CGS_ZStrView cgs__zstrv_strbuf_ptr1(const CGS_StrBuf *str);
CGS_API CGS_ZStrView cgs__zstrv_mutstr_ref1(CGS_MutStrRef str);

CGS_API CGS_ZStrView cgs__zstrv_cstr2(const char *str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_ucstr2(const unsigned char *str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_dstr2(CGS_DStr str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_dstr_ptr2(const CGS_DStr *str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_strv2(CGS_ZStrView str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_strbuf2(CGS_StrBuf str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_strbuf_ptr2(const CGS_StrBuf *str, unsigned int begin);
CGS_API CGS_ZStrView cgs__zstrv_mutstr_ref2(CGS_MutStrRef str, unsigned int begin);

CGS_API CGS_StrBuf cgs__strbuf_from_cstr_cap(const char *ptr, unsigned int cap);
CGS_API CGS_StrBuf cgs__strbuf_from_cstr(const char *ptr);
CGS_API CGS_StrBuf cgs__strbuf_from_buf(CGS_Buffer buf);

CGS_API CGS_Buffer cgs__buf_from_cstr(const char *str);
CGS_API CGS_Buffer cgs__buf_from_ucstr(const unsigned char *str);
CGS_API CGS_Buffer cgs__buf_from_carr(const char *str, size_t cap);
CGS_API CGS_Buffer cgs__buf_from_ucarr(const unsigned char *str, size_t cap);

CGS_API CGS_MutStrRef cgs__cstr_as_mutstr_ref(const char *str);
CGS_API CGS_MutStrRef cgs__ucstr_as_mutstr_ref(const unsigned char *str);
CGS_API CGS_MutStrRef cgs__buf_as_mutstr_ref(CGS_Buffer str);
CGS_API CGS_MutStrRef cgs__dstr_ptr_as_mutstr_ref(const CGS_DStr *str);
CGS_API CGS_MutStrRef cgs__strbuf_ptr_as_mutstr_ref(const CGS_StrBuf *str);
CGS_API CGS_MutStrRef cgs__mutstr_ref_as_mutstr_ref(CGS_MutStrRef str);

CGS_API CGS__FixedMutStrRef cgs__buf_as_fmutstr_ref(CGS_Buffer buf, unsigned int *len_ptr);
CGS_API CGS__FixedMutStrRef cgs__buf_as_fmutstr_ref_zero_len(CGS_Buffer buf, unsigned int *len_ptr);
CGS_API CGS__FixedMutStrRef cgs__strbuf_ptr_as_fmutstr_ref(CGS_StrBuf *strbuf);
CGS_API CGS__FixedMutStrRef cgs__mutstr_ref_as_fmutstr_ref2(CGS_MutStrRef mutstr_ref, unsigned int *len_ptr);
CGS_API CGS__FixedMutStrRef cgs__mutstr_ref_as_fmutstr_ref_zero_len(CGS_MutStrRef mutstr_ref, unsigned int *len_ptr);
CGS_API CGS__FixedMutStrRef cgs__dstr_ptr_as_fmutstr_ref(CGS_DStr *dstr);

CGS_API CGS_MutStrRef cgs__make_appender_mutstr_ref(CGS_MutStrRef owner, CGS_AppenderState *state);
CGS_API CGS_Error cgs__mutstr_ref_commit_appender(CGS_MutStrRef owner, CGS_MutStrRef appender);

CGS_API char *cgs__cstr_as_cstr(const char *str);
CGS_API char *cgs__ucstr_as_cstr(const unsigned char *str);
CGS_API char *cgs__dstr_as_cstr(CGS_DStr str);
CGS_API char *cgs__dstr_ptr_as_cstr(const CGS_DStr *str);
CGS_API char *cgs__strv_as_cstr(CGS_StrView str);
CGS_API char *cgs__strbuf_as_cstr(CGS_StrBuf str);
CGS_API char *cgs__strbuf_ptr_as_cstr(const CGS_StrBuf *str);
CGS_API char *cgs__mutstr_ref_as_cstr(CGS_MutStrRef str);

CGS_API unsigned int cgs__strv_cap(CGS_StrView sv);
CGS_API unsigned int cgs__dstr_cap(CGS_DStr str);
CGS_API unsigned int cgs__dstr_ptr_cap(const CGS_DStr *str);
CGS_API unsigned int cgs__strbuf_cap(CGS_StrBuf str);
CGS_API unsigned int cgs__strbuf_ptr_cap(const CGS_StrBuf *str);
CGS_API unsigned int cgs__mutstr_ref_cap(CGS_MutStrRef str);
CGS_API unsigned int cgs__mutstr_ref_len(CGS_MutStrRef str);

CGS__NODISCARD("discarding a new DStr may cause a memory leak")
CGS_API CGS_DStr cgs__dstr_init(unsigned int cap, CGS_Allocator *allocator);
CGS__NODISCARD("discarding a new DStr may cause a memory leak")
CGS_API CGS_DStr cgs__dstr_init_from(CGS_StrView from, CGS_Allocator *allocator);
CGS_API void cgs__dstr_deinit(CGS_DStr *dstr);
CGS_API CGS_Error cgs__dstr_append(CGS_DStr *dstr, CGS_StrView src);
CGS_API CGS_Error cgs__dstr_prepend_strv(CGS_DStr *dstr, CGS_StrView src);
CGS_API CGS_Error cgs__dstr_insert(CGS_DStr *dstr, CGS_StrView src, unsigned int idx);
CGS_API CGS_Error cgs__dstr_fread_until(CGS_DStr *dstr, FILE *stream, int delim);
CGS_API CGS_Error cgs__dstr_append_fread_until(CGS_DStr *dstr, FILE *stream, int delim);
CGS_API CGS_Error cgs__dstr_shrink_to_fit(CGS_DStr *dstr);
CGS_API CGS_Error cgs__dstr_ensure_cap(CGS_DStr *dstr, unsigned int at_least);

CGS_API CGS_Error cgs__mutstr_ref_putc(CGS_MutStrRef dst, char c);
CGS_API CGS_Error cgs__mutstr_ref_copy(CGS_MutStrRef dst, CGS_StrView src);
CGS_API CGS_Error cgs__mutstr_ref_append(CGS_MutStrRef dst, CGS_StrView src);
CGS_API CGS_Error cgs__mutstr_ref_delete_range(CGS_MutStrRef str, unsigned int begin, unsigned int end);
CGS_API CGS_Error cgs__mutstr_ref_insert(CGS_MutStrRef dst, CGS_StrView src, unsigned int idx);
CGS_API CGS_Result(int) cgs__mutstr_ref_replace(CGS_MutStrRef str, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__mutstr_ref_replace_first(CGS_MutStrRef str, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__mutstr_ref_replace_range(CGS_MutStrRef str, unsigned int begin, unsigned int end, CGS_StrView replacement);
CGS_API CGS_Error cgs__mutstr_ref_clear(CGS_MutStrRef str);
CGS_API CGS_Error cgs__strv_arr_join(CGS_MutStrRef dst, CGS_StrViewArray strs, CGS_StrView delim);
CGS_API CGS_Result(CGS_StrView) cgs__next_tok(CGS_StrView *base, CGS_StrView delim);
CGS_API CGS_Result(CGS_StrView) cgs__next_tok_any(CGS_StrView *base, CGS_StrView delim_set);
CGS_API CGS_StrView cgs__skip(CGS_StrView src, CGS_StrView delim);
CGS_API CGS_StrView cgs__skip_any(CGS_StrView src, CGS_StrView delim_set);

CGS_API CGS_Error cgs__fmutstr_ref_putc(CGS__FixedMutStrRef dst, char c);
CGS_API CGS_Error cgs__fmutstr_ref_copy(CGS__FixedMutStrRef dst, CGS_StrView src);
CGS_API CGS_Error cgs__fmutstr_ref_append(CGS__FixedMutStrRef dst, CGS_StrView src);
CGS_API CGS_Error cgs__fmutstr_ref_delete_range(CGS__FixedMutStrRef str, unsigned int begin, unsigned int end);
CGS_API CGS_Error cgs__fmutstr_ref_insert(CGS__FixedMutStrRef dst, CGS_StrView src, unsigned int idx);
CGS_API CGS_Result(int) cgs__fmutstr_ref_replace(CGS__FixedMutStrRef str, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__fmutstr_ref_replace_first(CGS__FixedMutStrRef str, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__fmutstr_ref_replace_range(CGS__FixedMutStrRef str, unsigned int begin, unsigned int end, CGS_StrView replacement);
CGS_API CGS_Error cgs__fmutstr_ref_clear(CGS__FixedMutStrRef str);
CGS_API CGS_Error cgs__strv_arr_join_into_fmutstr_ref(CGS__FixedMutStrRef dst, CGS_StrViewArray strs, CGS_StrView delim);

CGS_API CGS_Error cgs__dstr_putc(CGS_DStr *dst, char c);
CGS_API CGS_Error cgs__dstr_copy(CGS_DStr *dstr, CGS_StrView src);
CGS_API CGS_Result(int) cgs__dstr_replace(CGS_DStr *dstr, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__dstr_replace_first(CGS_DStr *dstr, CGS_StrView target, CGS_StrView replacement);
CGS_API CGS_Error cgs__dstr_replace_range(CGS_DStr *dstr, unsigned int begin, unsigned int end, CGS_StrView replacement);
CGS_API CGS_Error cgs__strv_arr_join_into_dstr(CGS_DStr *dstr, CGS_StrViewArray strs, CGS_StrView delim);

CGS__NODISCARD("str_split returns a heap allocated array")
CGS_API CGS_StrViewArray cgs__strv_split(CGS_StrView str, CGS_StrView delim, CGS_Allocator* allocator);
CGS_API CGS_Error cgs__strv_split_iter(CGS_StrView str, CGS_StrView delim, bool(*cb)(CGS_StrView found, void *ctx), void *ctx);

CGS_API CGS_StrViewArray cgs__strv_arr_from(const CGS_StrView *carr, unsigned int nb);

CGS_API bool         cgs__strv_equal(CGS_StrView str1, CGS_StrView str2);
CGS_API CGS_StrView  cgs__strv_find(CGS_StrView hay, CGS_StrView needle);
CGS_API unsigned int cgs__strv_count(CGS_StrView hay, CGS_StrView needle);
CGS_API CGS_StrView  cgs__trim_view(CGS_StrView str);
CGS_API CGS_Error    cgs__trim(CGS__FixedMutStrRef str);
CGS_API CGS_StrView  cgs__strv_cspn(CGS_StrView src, CGS_StrView charset);
CGS_API CGS_StrView  cgs__strv_spn(CGS_StrView src, CGS_StrView charset);
CGS_API bool cgs__strv_starts_with(CGS_StrView hay, CGS_StrView needle);
CGS_API bool cgs__strv_ends_with(CGS_StrView hay, CGS_StrView needle);

CGS_API CGS_Error cgs__map_chars(CGS__FixedMutStrRef str, bool(*map)(char *c,void *arg), void *arg);
CGS_API void cgs__chars_tolower(CGS__FixedMutStrRef str);
CGS_API void cgs__chars_toupper(CGS__FixedMutStrRef str);

CGS_API CGS_Error cgs__mutstr_ref_fread_until(CGS_MutStrRef dst, FILE *stream, int delim);
CGS_API CGS_Error cgs__mutstr_ref_append_fread_until(CGS_MutStrRef dst, FILE *stream, int delim);

CGS_API CGS_Error cgs__fmutstr_ref_fread_until(CGS__FixedMutStrRef dst, FILE *stream, int delim);
CGS_API CGS_Error cgs__fmutstr_ref_append_fread_until(CGS__FixedMutStrRef dst, FILE *stream, int delim);

CGS_API unsigned int cgs__fprint_strv(FILE *stream, CGS_StrView str);
CGS_API unsigned int cgs__fprintln_strv(FILE *stream, CGS_StrView str);

CGS_API CGS_Error cgs__append_fmt(CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error(*tostr_p_funcs[])(CGS_Writer*, const void*, CGS_StrView));
CGS_API CGS_Error cgs__appendln_fmt_(CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error(*tostr_p_funcs[])(CGS_Writer*, const void*, CGS_StrView));
CGS_API CGS_DStr cgs__asprintf(CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error(*tostr_p_funcs[])(CGS_Writer*, const void*, CGS_StrView));
CGS_API CGS_DStr cgs__asprintf_with_allocator(CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error(*tostr_p_funcs[])(CGS_Writer*, const void*, CGS_StrView));

CGS_API CGS_Error cgs__bool_tostr(CGS_Writer *dst, bool obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__cstr_tostr(CGS_Writer *dst, const char *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ucstr_tostr(CGS_Writer *dst, const unsigned char *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__char_tostr(CGS_Writer *dst, char obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__schar_tostr(CGS_Writer *dst, signed char obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__uchar_tostr(CGS_Writer *dst, unsigned char obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__short_tostr(CGS_Writer *dst, short obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ushort_tostr(CGS_Writer *dst, unsigned short obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__int_tostr(CGS_Writer *dst, int obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__uint_tostr(CGS_Writer *dst, unsigned int obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__long_tostr(CGS_Writer *dst, long obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ulong_tostr(CGS_Writer *dst, unsigned long obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__llong_tostr(CGS_Writer *dst, long long obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ullong_tostr(CGS_Writer *dst, unsigned long long obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__float_tostr(CGS_Writer *dst, float obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__double_tostr(CGS_Writer *dst, double obj, CGS_StrView fmt_arg);

CGS_API CGS_Error cgs__dstr_tostr(CGS_Writer *dst, CGS_DStr obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__dstr_ptr_tostr(CGS_Writer *dst, const CGS_DStr *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strv_tostr(CGS_Writer *dst, CGS_StrView obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__zstrv_tostr(CGS_Writer *dst, CGS_ZStrView obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strbuf_tostr(CGS_Writer *dst, CGS_StrBuf obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strbuf_ptr_tostr(CGS_Writer *dst, const CGS_StrBuf *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__mutstr_ref_tostr(CGS_Writer *dst, CGS_MutStrRef obj, CGS_StrView fmt_arg);

CGS_API CGS_Error cgs__error_tostr(CGS_Writer *dst, CGS_Error obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__array_fmt_tostr(CGS_Writer *dst, CGS_ArrayFmt obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__align_fmt_tostr(CGS_Writer *dst, CGS__AlignFmt obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__repeat_fmt_tostr(CGS_Writer *dst, CGS__RepeatFmt obj, CGS_StrView fmt_arg);

CGS_API CGS_Error cgs__bool_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__cstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ucstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__char_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__schar_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__uchar_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__short_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ushort_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__int_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__uint_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__long_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ulong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__llong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__ullong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__float_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__double_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);

CGS_API CGS_Error cgs__dstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__dstr_ptr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strv_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__zstrv_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strbuf_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__strbuf_ptr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__mutstr_ref_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);

CGS_API CGS_Error cgs__error_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__array_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__align_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);
CGS_API CGS_Error cgs__repeat_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);

#define CGS__X(ty, extra)                                                                                                \
    CGS_API CGS_Error cgs__Integer_d_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_d_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Integer_x_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_x_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Integer_o_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_o_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Integer_b_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_b_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Integer_X_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_X_Fmt_##ty obj, CGS_StrView fmt_arg); \
                                                                                                                         \
    CGS_API CGS_Error cgs__Integer_d_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);           \
    CGS_API CGS_Error cgs__Integer_x_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);           \
    CGS_API CGS_Error cgs__Integer_o_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);           \
    CGS_API CGS_Error cgs__Integer_b_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);           \
    CGS_API CGS_Error cgs__Integer_X_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);

CGS__INTEGER_TYPES(CGS__X, ignore)

#undef CGS__X

#define CGS__X(ty, extra)                                                                                                  \
    CGS_API CGS_Error cgs__Floating_f_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_f_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_g_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_g_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_e_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_e_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_a_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_a_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_F_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_F_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_G_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_G_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_E_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_E_Fmt_##ty obj, CGS_StrView fmt_arg); \
    CGS_API CGS_Error cgs__Floating_A_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_A_Fmt_##ty obj, CGS_StrView fmt_arg); \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_f_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_g_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_e_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_a_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_F_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_G_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_E_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);            \
    CGS_API CGS_Error cgs__Floating_A_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg);

CGS__FLOATING_TYPES(CGS__X, ignore, CGS__X)

#undef CGS__X

static inline CGS_MutStrRef cgs__clear_and_return(CGS_MutStrRef dst)
{
    cgs_clear(dst);
    return dst;
}

static inline unsigned int cgs__return_as_u32(size_t a)
{
    return (unsigned int) a;
}

static inline unsigned int cgs__cstr_cap(const char *s)
{
    return (unsigned int) strlen(s);
}

static inline unsigned int cgs__ucstr_cap(const unsigned char *s)
{
    return (unsigned int) strlen((const char*) s);
}

static inline unsigned int cgs__strv_len(CGS_StrView sv)
{
    return sv.len;
}

static inline CGS_Error cgs__invoke_writer(CGS_Writer *dst, CGS_StrView str)
{
    return dst->append(dst, str);
}

static inline CGS_Error cgs__invoke_writer_ln(CGS_Writer *dst, CGS_StrView str)
{
    CGS_Error err = dst->append(dst, str);
    if(err.ec == CGS_OK)
        err = dst->append(dst, (CGS_StrView){.chars = (char*) "\n", .len = 1});
    return err;
}

static inline CGS_Error cgs__writer_putc(CGS_Writer *dst, char c)
{
    return dst->append(dst, (CGS_StrView){.chars = &c, .len = 1});
}

static inline CGS_Error cgs__len_writer_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_LenWriter *len_writer = (CGS_LenWriter*)dst;
    len_writer->len += str.len;
    return (CGS_Error){CGS_OK};
}

static inline CGS_Error cgs__len_ptr_writer_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_LenPtrWriter *len_writer = (CGS_LenPtrWriter*)dst;
    *len_writer->len += str.len;
    return (CGS_Error){CGS_OK};
}

static inline CGS_Error cgs__chain_writer_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_ChainWriter *chain_writer = (CGS_ChainWriter*)dst;
    
    CGS_Error err1 = cgs__invoke_writer(chain_writer->a, str);
    CGS_Error err2 = cgs__invoke_writer(chain_writer->b, str);
    
    return err1.ec == CGS_OK ? err2 : err1;
}

static inline unsigned int cgs__invoke_tostr_len(CGS_Error(*tostr_p)(CGS_Writer*, const void*, CGS_StrView fmt_arg), const void *obj)
{
    CGS_LenWriter len_writer = cgs_len_writer();
    tostr_p((CGS_Writer*) &len_writer, obj, (CGS_StrView){});
    return len_writer.len;
}

static inline CGS_Error cgs__cstr_append(CGS_Writer *dst, CGS_StrView str)
{
    (void)dst;
    if(str.len != 0)
        return (CGS_Error){CGS_DST_TOO_SMALL};
    else
        return (CGS_Error){CGS_OK};
}

#endif // CGS__H_INCLUDED

#ifdef ADD_TOSTR

_Static_assert( cgs__has_type(cgs__get_tostr_func_with_default(CGS__ARG1 ADD_TOSTR), cgs__tostr_fail), "Type already has a tostr" );

#if !defined(CGS__TOSTR1)
#define CGS__TOSTR1
CGS__DECL_TOSTR_FUNC(1)
#elif !defined(CGS__TOSTR2)
#define CGS__TOSTR2
CGS__DECL_TOSTR_FUNC(2)
#elif !defined(CGS__TOSTR3)
#define CGS__TOSTR3
CGS__DECL_TOSTR_FUNC(3)
#elif !defined(CGS__TOSTR4)
#define CGS__TOSTR4
CGS__DECL_TOSTR_FUNC(4)
#elif !defined(CGS__TOSTR5)
#define CGS__TOSTR5
CGS__DECL_TOSTR_FUNC(5)
#elif !defined(CGS__TOSTR6)
#define CGS__TOSTR6
CGS__DECL_TOSTR_FUNC(6)
#elif !defined(CGS__TOSTR7)
#define CGS__TOSTR7
CGS__DECL_TOSTR_FUNC(7)
#elif !defined(CGS__TOSTR8)
#define CGS__TOSTR8
CGS__DECL_TOSTR_FUNC(8)
#elif !defined(CGS__TOSTR9)
#define CGS__TOSTR9
CGS__DECL_TOSTR_FUNC(9)
#elif !defined(CGS__TOSTR10)
#define CGS__TOSTR10
CGS__DECL_TOSTR_FUNC(10)
#elif !defined(CGS__TOSTR11)
#define CGS__TOSTR11
CGS__DECL_TOSTR_FUNC(11)
#elif !defined(CGS__TOSTR12)
#define CGS__TOSTR12
CGS__DECL_TOSTR_FUNC(12)
#elif !defined(CGS__TOSTR13)
#define CGS__TOSTR13
CGS__DECL_TOSTR_FUNC(13)
#elif !defined(CGS__TOSTR14)
#define CGS__TOSTR14
CGS__DECL_TOSTR_FUNC(14)
#elif !defined(CGS__TOSTR15)
#define CGS__TOSTR15
CGS__DECL_TOSTR_FUNC(15)
#elif !defined(CGS__TOSTR16)
#define CGS__TOSTR16
CGS__DECL_TOSTR_FUNC(16)
#elif !defined(CGS__TOSTR17)
#define CGS__TOSTR17
CGS__DECL_TOSTR_FUNC(17)
#elif !defined(CGS__TOSTR18)
#define CGS__TOSTR18
CGS__DECL_TOSTR_FUNC(18)
#elif !defined(CGS__TOSTR19)
#define CGS__TOSTR19
CGS__DECL_TOSTR_FUNC(19)
#elif !defined(CGS__TOSTR20)
#define CGS__TOSTR20
CGS__DECL_TOSTR_FUNC(20)
#elif !defined(CGS__TOSTR21)
#define CGS__TOSTR21
CGS__DECL_TOSTR_FUNC(21)
#elif !defined(CGS__TOSTR22)
#define CGS__TOSTR22
CGS__DECL_TOSTR_FUNC(22)
#elif !defined(CGS__TOSTR23)
#define CGS__TOSTR23
CGS__DECL_TOSTR_FUNC(23)
#elif !defined(CGS__TOSTR24)
#define CGS__TOSTR24
CGS__DECL_TOSTR_FUNC(24)
#elif !defined(CGS__TOSTR25)
#define CGS__TOSTR25
CGS__DECL_TOSTR_FUNC(25)
#elif !defined(CGS__TOSTR26)
#define CGS__TOSTR26
CGS__DECL_TOSTR_FUNC(26)
#elif !defined(CGS__TOSTR27)
#define CGS__TOSTR27
CGS__DECL_TOSTR_FUNC(27)
#elif !defined(CGS__TOSTR28)
#define CGS__TOSTR28
CGS__DECL_TOSTR_FUNC(28)
#elif !defined(CGS__TOSTR29)
#define CGS__TOSTR29
CGS__DECL_TOSTR_FUNC(29)
#elif !defined(CGS__TOSTR30)
#define CGS__TOSTR30
CGS__DECL_TOSTR_FUNC(30)
#elif !defined(CGS__TOSTR31)
#define CGS__TOSTR31
CGS__DECL_TOSTR_FUNC(31)
#elif !defined(CGS__TOSTR32)
#define CGS__TOSTR32
CGS__DECL_TOSTR_FUNC(32)
#else
#error "Maximum number of tostr functions is 32"
#endif

#undef ADD_TOSTR

#endif

#if defined(CGS_IMPL)
#ifndef CGS__STR_C_INCLUDED
#define CGS__STR_C_INCLUDED

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CGS_API
    // for functions exposed in the header
    #define CGS_API
#endif
#ifndef CGS_PRIVATE
    // for functions only in cgs.c
    #define CGS_PRIVATE static inline
#endif



#ifndef CGS_debug_break
    #ifndef CGS_NDEBUG
        #if defined(_MSC_VER)
            #define CGS_debug_break() __debugbreak()
        #elif defined(__clang__)
            #define CGS_debug_break() __builtin_debugtrap()
        #elif defined(__GNUC__)
            #define CGS_debug_break() __builtin_trap()
        #else
            #include <signal.h>
            #define CGS_debug_break() raise(SIGTRAP)
        #endif
    #else
        #define CGS_debug_break() CGS_unreachable()
    #endif
#endif

#ifndef CGS_unreachable
    #if defined(_MSC_VER)
        #define CGS_unreachable() __assume(0)
    #elif defined(__GNUC__)
        #define CGS_unreachable() __builtin_unreachable()
    #elif defined(unreachable)
        #define CGS_unreachable() unreachable()
    #else
        #define CGS_unreachable() (*(volatile int *)0 = 0)
    #endif
#endif

#ifndef CGS_assume
    #if defined(_MSC_VER)
        #define CGS_assume(...) __assume(__VA_ARGS__)
    #elif defined(__clang__)
        #define CGS_assume(...) [[clang::assume(__VA_ARGS__)]]
    #elif defined(__GNUC__)
        #define CGS_assume(...) [[gnu::assume(__VA_ARGS__)]]
    #else
        #define CGS_assume(...)       \
            do                        \
            {                         \
                if (!(__VA_ARGS__))   \
                    CGS_unreachable() \
            } while (0)
    #endif
#endif

CGS_PRIVATE CGS_Allocation cgs__default_allocator_alloc(CGS_Allocator *allocator, size_t align, size_t n);
CGS_PRIVATE void cgs__default_allocator_dealloc(CGS_Allocator *allocator, void *ptr, size_t n);
CGS_PRIVATE CGS_Allocation cgs__default_allocator_realloc(CGS_Allocator *allocator, void *ptr, size_t align, size_t old_size, size_t new_size);

static CGS_Allocator cgs__default_allocator = {
    .alloc   = cgs__default_allocator_alloc,
    .dealloc = cgs__default_allocator_dealloc,
    .realloc = cgs__default_allocator_realloc,
};

static const CGS_StrView cgs__error_to_string[] = {
#define CGS__ERROR_TO_STRV(e) [CGS_##e] = {.len = sizeof(#e) - 1, .chars = #e},

    CGS__ERROR_NAMES(CGS__ERROR_TO_STRV)

#undef CGS__ERROR_TO_STRV
};

static const unsigned long long cgs__ten_pows_ull[] = {
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

static const CGS_StrView cgs__uc_to_string[256] = {
    {.chars = "0", .len = 1},   {.chars = "1", .len = 1},   {.chars = "2", .len = 1},   {.chars = "3", .len = 1},   {.chars = "4", .len = 1},
    {.chars = "5", .len = 1},   {.chars = "6", .len = 1},   {.chars = "7", .len = 1},   {.chars = "8", .len = 1},   {.chars = "9", .len = 1},
    {.chars = "10", .len = 2},  {.chars = "11", .len = 2},  {.chars = "12", .len = 2},  {.chars = "13", .len = 2},  {.chars = "14", .len = 2},
    {.chars = "15", .len = 2},  {.chars = "16", .len = 2},  {.chars = "17", .len = 2},  {.chars = "18", .len = 2},  {.chars = "19", .len = 2},
    {.chars = "20", .len = 2},  {.chars = "21", .len = 2},  {.chars = "22", .len = 2},  {.chars = "23", .len = 2},  {.chars = "24", .len = 2},
    {.chars = "25", .len = 2},  {.chars = "26", .len = 2},  {.chars = "27", .len = 2},  {.chars = "28", .len = 2},  {.chars = "29", .len = 2},
    {.chars = "30", .len = 2},  {.chars = "31", .len = 2},  {.chars = "32", .len = 2},  {.chars = "33", .len = 2},  {.chars = "34", .len = 2},
    {.chars = "35", .len = 2},  {.chars = "36", .len = 2},  {.chars = "37", .len = 2},  {.chars = "38", .len = 2},  {.chars = "39", .len = 2},
    {.chars = "40", .len = 2},  {.chars = "41", .len = 2},  {.chars = "42", .len = 2},  {.chars = "43", .len = 2},  {.chars = "44", .len = 2},
    {.chars = "45", .len = 2},  {.chars = "46", .len = 2},  {.chars = "47", .len = 2},  {.chars = "48", .len = 2},  {.chars = "49", .len = 2},
    {.chars = "50", .len = 2},  {.chars = "51", .len = 2},  {.chars = "52", .len = 2},  {.chars = "53", .len = 2},  {.chars = "54", .len = 2},
    {.chars = "55", .len = 2},  {.chars = "56", .len = 2},  {.chars = "57", .len = 2},  {.chars = "58", .len = 2},  {.chars = "59", .len = 2},
    {.chars = "60", .len = 2},  {.chars = "61", .len = 2},  {.chars = "62", .len = 2},  {.chars = "63", .len = 2},  {.chars = "64", .len = 2},
    {.chars = "65", .len = 2},  {.chars = "66", .len = 2},  {.chars = "67", .len = 2},  {.chars = "68", .len = 2},  {.chars = "69", .len = 2},
    {.chars = "70", .len = 2},  {.chars = "71", .len = 2},  {.chars = "72", .len = 2},  {.chars = "73", .len = 2},  {.chars = "74", .len = 2},
    {.chars = "75", .len = 2},  {.chars = "76", .len = 2},  {.chars = "77", .len = 2},  {.chars = "78", .len = 2},  {.chars = "79", .len = 2},
    {.chars = "80", .len = 2},  {.chars = "81", .len = 2},  {.chars = "82", .len = 2},  {.chars = "83", .len = 2},  {.chars = "84", .len = 2},
    {.chars = "85", .len = 2},  {.chars = "86", .len = 2},  {.chars = "87", .len = 2},  {.chars = "88", .len = 2},  {.chars = "89", .len = 2},
    {.chars = "90", .len = 2},  {.chars = "91", .len = 2},  {.chars = "92", .len = 2},  {.chars = "93", .len = 2},  {.chars = "94", .len = 2},
    {.chars = "95", .len = 2},  {.chars = "96", .len = 2},  {.chars = "97", .len = 2},  {.chars = "98", .len = 2},  {.chars = "99", .len = 2},
    {.chars = "100", .len = 3}, {.chars = "101", .len = 3}, {.chars = "102", .len = 3}, {.chars = "103", .len = 3}, {.chars = "104", .len = 3},
    {.chars = "105", .len = 3}, {.chars = "106", .len = 3}, {.chars = "107", .len = 3}, {.chars = "108", .len = 3}, {.chars = "109", .len = 3},
    {.chars = "110", .len = 3}, {.chars = "111", .len = 3}, {.chars = "112", .len = 3}, {.chars = "113", .len = 3}, {.chars = "114", .len = 3},
    {.chars = "115", .len = 3}, {.chars = "116", .len = 3}, {.chars = "117", .len = 3}, {.chars = "118", .len = 3}, {.chars = "119", .len = 3},
    {.chars = "120", .len = 3}, {.chars = "121", .len = 3}, {.chars = "122", .len = 3}, {.chars = "123", .len = 3}, {.chars = "124", .len = 3},
    {.chars = "125", .len = 3}, {.chars = "126", .len = 3}, {.chars = "127", .len = 3}, {.chars = "128", .len = 3}, {.chars = "129", .len = 3},
    {.chars = "130", .len = 3}, {.chars = "131", .len = 3}, {.chars = "132", .len = 3}, {.chars = "133", .len = 3}, {.chars = "134", .len = 3},
    {.chars = "135", .len = 3}, {.chars = "136", .len = 3}, {.chars = "137", .len = 3}, {.chars = "138", .len = 3}, {.chars = "139", .len = 3},
    {.chars = "140", .len = 3}, {.chars = "141", .len = 3}, {.chars = "142", .len = 3}, {.chars = "143", .len = 3}, {.chars = "144", .len = 3},
    {.chars = "145", .len = 3}, {.chars = "146", .len = 3}, {.chars = "147", .len = 3}, {.chars = "148", .len = 3}, {.chars = "149", .len = 3},
    {.chars = "150", .len = 3}, {.chars = "151", .len = 3}, {.chars = "152", .len = 3}, {.chars = "153", .len = 3}, {.chars = "154", .len = 3},
    {.chars = "155", .len = 3}, {.chars = "156", .len = 3}, {.chars = "157", .len = 3}, {.chars = "158", .len = 3}, {.chars = "159", .len = 3},
    {.chars = "160", .len = 3}, {.chars = "161", .len = 3}, {.chars = "162", .len = 3}, {.chars = "163", .len = 3}, {.chars = "164", .len = 3},
    {.chars = "165", .len = 3}, {.chars = "166", .len = 3}, {.chars = "167", .len = 3}, {.chars = "168", .len = 3}, {.chars = "169", .len = 3},
    {.chars = "170", .len = 3}, {.chars = "171", .len = 3}, {.chars = "172", .len = 3}, {.chars = "173", .len = 3}, {.chars = "174", .len = 3},
    {.chars = "175", .len = 3}, {.chars = "176", .len = 3}, {.chars = "177", .len = 3}, {.chars = "178", .len = 3}, {.chars = "179", .len = 3},
    {.chars = "180", .len = 3}, {.chars = "181", .len = 3}, {.chars = "182", .len = 3}, {.chars = "183", .len = 3}, {.chars = "184", .len = 3},
    {.chars = "185", .len = 3}, {.chars = "186", .len = 3}, {.chars = "187", .len = 3}, {.chars = "188", .len = 3}, {.chars = "189", .len = 3},
    {.chars = "190", .len = 3}, {.chars = "191", .len = 3}, {.chars = "192", .len = 3}, {.chars = "193", .len = 3}, {.chars = "194", .len = 3},
    {.chars = "195", .len = 3}, {.chars = "196", .len = 3}, {.chars = "197", .len = 3}, {.chars = "198", .len = 3}, {.chars = "199", .len = 3},
    {.chars = "200", .len = 3}, {.chars = "201", .len = 3}, {.chars = "202", .len = 3}, {.chars = "203", .len = 3}, {.chars = "204", .len = 3},
    {.chars = "205", .len = 3}, {.chars = "206", .len = 3}, {.chars = "207", .len = 3}, {.chars = "208", .len = 3}, {.chars = "209", .len = 3},
    {.chars = "210", .len = 3}, {.chars = "211", .len = 3}, {.chars = "212", .len = 3}, {.chars = "213", .len = 3}, {.chars = "214", .len = 3},
    {.chars = "215", .len = 3}, {.chars = "216", .len = 3}, {.chars = "217", .len = 3}, {.chars = "218", .len = 3}, {.chars = "219", .len = 3},
    {.chars = "220", .len = 3}, {.chars = "221", .len = 3}, {.chars = "222", .len = 3}, {.chars = "223", .len = 3}, {.chars = "224", .len = 3},
    {.chars = "225", .len = 3}, {.chars = "226", .len = 3}, {.chars = "227", .len = 3}, {.chars = "228", .len = 3}, {.chars = "229", .len = 3},
    {.chars = "230", .len = 3}, {.chars = "231", .len = 3}, {.chars = "232", .len = 3}, {.chars = "233", .len = 3}, {.chars = "234", .len = 3},
    {.chars = "235", .len = 3}, {.chars = "236", .len = 3}, {.chars = "237", .len = 3}, {.chars = "238", .len = 3}, {.chars = "239", .len = 3},
    {.chars = "240", .len = 3}, {.chars = "241", .len = 3}, {.chars = "242", .len = 3}, {.chars = "243", .len = 3}, {.chars = "244", .len = 3},
    {.chars = "245", .len = 3}, {.chars = "246", .len = 3}, {.chars = "247", .len = 3}, {.chars = "248", .len = 3}, {.chars = "249", .len = 3},
    {.chars = "250", .len = 3}, {.chars = "251", .len = 3}, {.chars = "252", .len = 3}, {.chars = "253", .len = 3}, {.chars = "254", .len = 3},
    {.chars = "255", .len = 3},
};

static const CGS_StrView cgs__sc_to_string[] = {
    {.chars = "0", .len = 1},    {.chars = "1", .len = 1},    {.chars = "2", .len = 1},    {.chars = "3", .len = 1},
    {.chars = "4", .len = 1},    {.chars = "5", .len = 1},    {.chars = "6", .len = 1},    {.chars = "7", .len = 1},
    {.chars = "8", .len = 1},    {.chars = "9", .len = 1},    {.chars = "10", .len = 2},   {.chars = "11", .len = 2},
    {.chars = "12", .len = 2},   {.chars = "13", .len = 2},   {.chars = "14", .len = 2},   {.chars = "15", .len = 2},
    {.chars = "16", .len = 2},   {.chars = "17", .len = 2},   {.chars = "18", .len = 2},   {.chars = "19", .len = 2},
    {.chars = "20", .len = 2},   {.chars = "21", .len = 2},   {.chars = "22", .len = 2},   {.chars = "23", .len = 2},
    {.chars = "24", .len = 2},   {.chars = "25", .len = 2},   {.chars = "26", .len = 2},   {.chars = "27", .len = 2},
    {.chars = "28", .len = 2},   {.chars = "29", .len = 2},   {.chars = "30", .len = 2},   {.chars = "31", .len = 2},
    {.chars = "32", .len = 2},   {.chars = "33", .len = 2},   {.chars = "34", .len = 2},   {.chars = "35", .len = 2},
    {.chars = "36", .len = 2},   {.chars = "37", .len = 2},   {.chars = "38", .len = 2},   {.chars = "39", .len = 2},
    {.chars = "40", .len = 2},   {.chars = "41", .len = 2},   {.chars = "42", .len = 2},   {.chars = "43", .len = 2},
    {.chars = "44", .len = 2},   {.chars = "45", .len = 2},   {.chars = "46", .len = 2},   {.chars = "47", .len = 2},
    {.chars = "48", .len = 2},   {.chars = "49", .len = 2},   {.chars = "50", .len = 2},   {.chars = "51", .len = 2},
    {.chars = "52", .len = 2},   {.chars = "53", .len = 2},   {.chars = "54", .len = 2},   {.chars = "55", .len = 2},
    {.chars = "56", .len = 2},   {.chars = "57", .len = 2},   {.chars = "58", .len = 2},   {.chars = "59", .len = 2},
    {.chars = "60", .len = 2},   {.chars = "61", .len = 2},   {.chars = "62", .len = 2},   {.chars = "63", .len = 2},
    {.chars = "64", .len = 2},   {.chars = "65", .len = 2},   {.chars = "66", .len = 2},   {.chars = "67", .len = 2},
    {.chars = "68", .len = 2},   {.chars = "69", .len = 2},   {.chars = "70", .len = 2},   {.chars = "71", .len = 2},
    {.chars = "72", .len = 2},   {.chars = "73", .len = 2},   {.chars = "74", .len = 2},   {.chars = "75", .len = 2},
    {.chars = "76", .len = 2},   {.chars = "77", .len = 2},   {.chars = "78", .len = 2},   {.chars = "79", .len = 2},
    {.chars = "80", .len = 2},   {.chars = "81", .len = 2},   {.chars = "82", .len = 2},   {.chars = "83", .len = 2},
    {.chars = "84", .len = 2},   {.chars = "85", .len = 2},   {.chars = "86", .len = 2},   {.chars = "87", .len = 2},
    {.chars = "88", .len = 2},   {.chars = "89", .len = 2},   {.chars = "90", .len = 2},   {.chars = "91", .len = 2},
    {.chars = "92", .len = 2},   {.chars = "93", .len = 2},   {.chars = "94", .len = 2},   {.chars = "95", .len = 2},
    {.chars = "96", .len = 2},   {.chars = "97", .len = 2},   {.chars = "98", .len = 2},   {.chars = "99", .len = 2},
    {.chars = "100", .len = 3},  {.chars = "101", .len = 3},  {.chars = "102", .len = 3},  {.chars = "103", .len = 3},
    {.chars = "104", .len = 3},  {.chars = "105", .len = 3},  {.chars = "106", .len = 3},  {.chars = "107", .len = 3},
    {.chars = "108", .len = 3},  {.chars = "109", .len = 3},  {.chars = "110", .len = 3},  {.chars = "111", .len = 3},
    {.chars = "112", .len = 3},  {.chars = "113", .len = 3},  {.chars = "114", .len = 3},  {.chars = "115", .len = 3},
    {.chars = "116", .len = 3},  {.chars = "117", .len = 3},  {.chars = "118", .len = 3},  {.chars = "119", .len = 3},
    {.chars = "120", .len = 3},  {.chars = "121", .len = 3},  {.chars = "122", .len = 3},  {.chars = "123", .len = 3},
    {.chars = "124", .len = 3},  {.chars = "125", .len = 3},  {.chars = "126", .len = 3},  {.chars = "127", .len = 3},
    {.chars = "-128", .len = 4}, {.chars = "-127", .len = 4}, {.chars = "-126", .len = 4}, {.chars = "-125", .len = 4},
    {.chars = "-124", .len = 4}, {.chars = "-123", .len = 4}, {.chars = "-122", .len = 4}, {.chars = "-121", .len = 4},
    {.chars = "-120", .len = 4}, {.chars = "-119", .len = 4}, {.chars = "-118", .len = 4}, {.chars = "-117", .len = 4},
    {.chars = "-116", .len = 4}, {.chars = "-115", .len = 4}, {.chars = "-114", .len = 4}, {.chars = "-113", .len = 4},
    {.chars = "-112", .len = 4}, {.chars = "-111", .len = 4}, {.chars = "-110", .len = 4}, {.chars = "-109", .len = 4},
    {.chars = "-108", .len = 4}, {.chars = "-107", .len = 4}, {.chars = "-106", .len = 4}, {.chars = "-105", .len = 4},
    {.chars = "-104", .len = 4}, {.chars = "-103", .len = 4}, {.chars = "-102", .len = 4}, {.chars = "-101", .len = 4},
    {.chars = "-100", .len = 4}, {.chars = "-99", .len = 3},  {.chars = "-98", .len = 3},  {.chars = "-97", .len = 3},
    {.chars = "-96", .len = 3},  {.chars = "-95", .len = 3},  {.chars = "-94", .len = 3},  {.chars = "-93", .len = 3},
    {.chars = "-92", .len = 3},  {.chars = "-91", .len = 3},  {.chars = "-90", .len = 3},  {.chars = "-89", .len = 3},
    {.chars = "-88", .len = 3},  {.chars = "-87", .len = 3},  {.chars = "-86", .len = 3},  {.chars = "-85", .len = 3},
    {.chars = "-84", .len = 3},  {.chars = "-83", .len = 3},  {.chars = "-82", .len = 3},  {.chars = "-81", .len = 3},
    {.chars = "-80", .len = 3},  {.chars = "-79", .len = 3},  {.chars = "-78", .len = 3},  {.chars = "-77", .len = 3},
    {.chars = "-76", .len = 3},  {.chars = "-75", .len = 3},  {.chars = "-74", .len = 3},  {.chars = "-73", .len = 3},
    {.chars = "-72", .len = 3},  {.chars = "-71", .len = 3},  {.chars = "-70", .len = 3},  {.chars = "-69", .len = 3},
    {.chars = "-68", .len = 3},  {.chars = "-67", .len = 3},  {.chars = "-66", .len = 3},  {.chars = "-65", .len = 3},
    {.chars = "-64", .len = 3},  {.chars = "-63", .len = 3},  {.chars = "-62", .len = 3},  {.chars = "-61", .len = 3},
    {.chars = "-60", .len = 3},  {.chars = "-59", .len = 3},  {.chars = "-58", .len = 3},  {.chars = "-57", .len = 3},
    {.chars = "-56", .len = 3},  {.chars = "-55", .len = 3},  {.chars = "-54", .len = 3},  {.chars = "-53", .len = 3},
    {.chars = "-52", .len = 3},  {.chars = "-51", .len = 3},  {.chars = "-50", .len = 3},  {.chars = "-49", .len = 3},
    {.chars = "-48", .len = 3},  {.chars = "-47", .len = 3},  {.chars = "-46", .len = 3},  {.chars = "-45", .len = 3},
    {.chars = "-44", .len = 3},  {.chars = "-43", .len = 3},  {.chars = "-42", .len = 3},  {.chars = "-41", .len = 3},
    {.chars = "-40", .len = 3},  {.chars = "-39", .len = 3},  {.chars = "-38", .len = 3},  {.chars = "-37", .len = 3},
    {.chars = "-36", .len = 3},  {.chars = "-35", .len = 3},  {.chars = "-34", .len = 3},  {.chars = "-33", .len = 3},
    {.chars = "-32", .len = 3},  {.chars = "-31", .len = 3},  {.chars = "-30", .len = 3},  {.chars = "-29", .len = 3},
    {.chars = "-28", .len = 3},  {.chars = "-27", .len = 3},  {.chars = "-26", .len = 3},  {.chars = "-25", .len = 3},
    {.chars = "-24", .len = 3},  {.chars = "-23", .len = 3},  {.chars = "-22", .len = 3},  {.chars = "-21", .len = 3},
    {.chars = "-20", .len = 3},  {.chars = "-19", .len = 3},  {.chars = "-18", .len = 3},  {.chars = "-17", .len = 3},
    {.chars = "-16", .len = 3},  {.chars = "-15", .len = 3},  {.chars = "-14", .len = 3},  {.chars = "-13", .len = 3},
    {.chars = "-12", .len = 3},  {.chars = "-11", .len = 3},  {.chars = "-10", .len = 3},  {.chars = "-9", .len = 2},
    {.chars = "-8", .len = 2},   {.chars = "-7", .len = 2},   {.chars = "-6", .len = 2},   {.chars = "-5", .len = 2},
    {.chars = "-4", .len = 2},   {.chars = "-3", .len = 2},   {.chars = "-2", .len = 2},   {.chars = "-1", .len = 2}
};

static const char cgs__byte_to_hex[][2] = {
    {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'}, {'0', '4'}, {'0', '5'}, {'0', '6'}, {'0', '7'}, {'0', '8'}, {'0', '9'}, {'0', 'a'},
    {'0', 'b'}, {'0', 'c'}, {'0', 'd'}, {'0', 'e'}, {'0', 'f'}, {'1', '0'}, {'1', '1'}, {'1', '2'}, {'1', '3'}, {'1', '4'}, {'1', '5'},
    {'1', '6'}, {'1', '7'}, {'1', '8'}, {'1', '9'}, {'1', 'a'}, {'1', 'b'}, {'1', 'c'}, {'1', 'd'}, {'1', 'e'}, {'1', 'f'}, {'2', '0'},
    {'2', '1'}, {'2', '2'}, {'2', '3'}, {'2', '4'}, {'2', '5'}, {'2', '6'}, {'2', '7'}, {'2', '8'}, {'2', '9'}, {'2', 'a'}, {'2', 'b'},
    {'2', 'c'}, {'2', 'd'}, {'2', 'e'}, {'2', 'f'}, {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'}, {'3', '4'}, {'3', '5'}, {'3', '6'},
    {'3', '7'}, {'3', '8'}, {'3', '9'}, {'3', 'a'}, {'3', 'b'}, {'3', 'c'}, {'3', 'd'}, {'3', 'e'}, {'3', 'f'}, {'4', '0'}, {'4', '1'},
    {'4', '2'}, {'4', '3'}, {'4', '4'}, {'4', '5'}, {'4', '6'}, {'4', '7'}, {'4', '8'}, {'4', '9'}, {'4', 'a'}, {'4', 'b'}, {'4', 'c'},
    {'4', 'd'}, {'4', 'e'}, {'4', 'f'}, {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'}, {'5', '4'}, {'5', '5'}, {'5', '6'}, {'5', '7'},
    {'5', '8'}, {'5', '9'}, {'5', 'a'}, {'5', 'b'}, {'5', 'c'}, {'5', 'd'}, {'5', 'e'}, {'5', 'f'}, {'6', '0'}, {'6', '1'}, {'6', '2'},
    {'6', '3'}, {'6', '4'}, {'6', '5'}, {'6', '6'}, {'6', '7'}, {'6', '8'}, {'6', '9'}, {'6', 'a'}, {'6', 'b'}, {'6', 'c'}, {'6', 'd'},
    {'6', 'e'}, {'6', 'f'}, {'7', '0'}, {'7', '1'}, {'7', '2'}, {'7', '3'}, {'7', '4'}, {'7', '5'}, {'7', '6'}, {'7', '7'}, {'7', '8'},
    {'7', '9'}, {'7', 'a'}, {'7', 'b'}, {'7', 'c'}, {'7', 'd'}, {'7', 'e'}, {'7', 'f'}, {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'},
    {'8', '4'}, {'8', '5'}, {'8', '6'}, {'8', '7'}, {'8', '8'}, {'8', '9'}, {'8', 'a'}, {'8', 'b'}, {'8', 'c'}, {'8', 'd'}, {'8', 'e'},
    {'8', 'f'}, {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'}, {'9', '4'}, {'9', '5'}, {'9', '6'}, {'9', '7'}, {'9', '8'}, {'9', '9'},
    {'9', 'a'}, {'9', 'b'}, {'9', 'c'}, {'9', 'd'}, {'9', 'e'}, {'9', 'f'}, {'a', '0'}, {'a', '1'}, {'a', '2'}, {'a', '3'}, {'a', '4'},
    {'a', '5'}, {'a', '6'}, {'a', '7'}, {'a', '8'}, {'a', '9'}, {'a', 'a'}, {'a', 'b'}, {'a', 'c'}, {'a', 'd'}, {'a', 'e'}, {'a', 'f'},
    {'b', '0'}, {'b', '1'}, {'b', '2'}, {'b', '3'}, {'b', '4'}, {'b', '5'}, {'b', '6'}, {'b', '7'}, {'b', '8'}, {'b', '9'}, {'b', 'a'},
    {'b', 'b'}, {'b', 'c'}, {'b', 'd'}, {'b', 'e'}, {'b', 'f'}, {'c', '0'}, {'c', '1'}, {'c', '2'}, {'c', '3'}, {'c', '4'}, {'c', '5'},
    {'c', '6'}, {'c', '7'}, {'c', '8'}, {'c', '9'}, {'c', 'a'}, {'c', 'b'}, {'c', 'c'}, {'c', 'd'}, {'c', 'e'}, {'c', 'f'}, {'d', '0'},
    {'d', '1'}, {'d', '2'}, {'d', '3'}, {'d', '4'}, {'d', '5'}, {'d', '6'}, {'d', '7'}, {'d', '8'}, {'d', '9'}, {'d', 'a'}, {'d', 'b'},
    {'d', 'c'}, {'d', 'd'}, {'d', 'e'}, {'d', 'f'}, {'e', '0'}, {'e', '1'}, {'e', '2'}, {'e', '3'}, {'e', '4'}, {'e', '5'}, {'e', '6'},
    {'e', '7'}, {'e', '8'}, {'e', '9'}, {'e', 'a'}, {'e', 'b'}, {'e', 'c'}, {'e', 'd'}, {'e', 'e'}, {'e', 'f'}, {'f', '0'}, {'f', '1'},
    {'f', '2'}, {'f', '3'}, {'f', '4'}, {'f', '5'}, {'f', '6'}, {'f', '7'}, {'f', '8'}, {'f', '9'}, {'f', 'a'}, {'f', 'b'}, {'f', 'c'},
    {'f', 'd'}, {'f', 'e'}, {'f', 'f'}
};

static const char cgs__byte_to_heX[][2] = {
    {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'}, {'0', '4'}, {'0', '5'}, {'0', '6'}, {'0', '7'}, {'0', '8'}, {'0', '9'}, {'0', 'A'},
    {'0', 'B'}, {'0', 'C'}, {'0', 'D'}, {'0', 'E'}, {'0', 'F'}, {'1', '0'}, {'1', '1'}, {'1', '2'}, {'1', '3'}, {'1', '4'}, {'1', '5'},
    {'1', '6'}, {'1', '7'}, {'1', '8'}, {'1', '9'}, {'1', 'A'}, {'1', 'B'}, {'1', 'C'}, {'1', 'D'}, {'1', 'E'}, {'1', 'F'}, {'2', '0'},
    {'2', '1'}, {'2', '2'}, {'2', '3'}, {'2', '4'}, {'2', '5'}, {'2', '6'}, {'2', '7'}, {'2', '8'}, {'2', '9'}, {'2', 'A'}, {'2', 'B'},
    {'2', 'C'}, {'2', 'D'}, {'2', 'E'}, {'2', 'F'}, {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'}, {'3', '4'}, {'3', '5'}, {'3', '6'},
    {'3', '7'}, {'3', '8'}, {'3', '9'}, {'3', 'A'}, {'3', 'B'}, {'3', 'C'}, {'3', 'D'}, {'3', 'E'}, {'3', 'F'}, {'4', '0'}, {'4', '1'},
    {'4', '2'}, {'4', '3'}, {'4', '4'}, {'4', '5'}, {'4', '6'}, {'4', '7'}, {'4', '8'}, {'4', '9'}, {'4', 'A'}, {'4', 'B'}, {'4', 'C'},
    {'4', 'D'}, {'4', 'E'}, {'4', 'F'}, {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'}, {'5', '4'}, {'5', '5'}, {'5', '6'}, {'5', '7'},
    {'5', '8'}, {'5', '9'}, {'5', 'A'}, {'5', 'B'}, {'5', 'C'}, {'5', 'D'}, {'5', 'E'}, {'5', 'F'}, {'6', '0'}, {'6', '1'}, {'6', '2'},
    {'6', '3'}, {'6', '4'}, {'6', '5'}, {'6', '6'}, {'6', '7'}, {'6', '8'}, {'6', '9'}, {'6', 'A'}, {'6', 'B'}, {'6', 'C'}, {'6', 'D'},
    {'6', 'E'}, {'6', 'F'}, {'7', '0'}, {'7', '1'}, {'7', '2'}, {'7', '3'}, {'7', '4'}, {'7', '5'}, {'7', '6'}, {'7', '7'}, {'7', '8'},
    {'7', '9'}, {'7', 'A'}, {'7', 'B'}, {'7', 'C'}, {'7', 'D'}, {'7', 'E'}, {'7', 'F'}, {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'},
    {'8', '4'}, {'8', '5'}, {'8', '6'}, {'8', '7'}, {'8', '8'}, {'8', '9'}, {'8', 'A'}, {'8', 'B'}, {'8', 'C'}, {'8', 'D'}, {'8', 'E'},
    {'8', 'F'}, {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'}, {'9', '4'}, {'9', '5'}, {'9', '6'}, {'9', '7'}, {'9', '8'}, {'9', '9'},
    {'9', 'A'}, {'9', 'B'}, {'9', 'C'}, {'9', 'D'}, {'9', 'E'}, {'9', 'F'}, {'A', '0'}, {'A', '1'}, {'A', '2'}, {'A', '3'}, {'A', '4'},
    {'A', '5'}, {'A', '6'}, {'A', '7'}, {'A', '8'}, {'A', '9'}, {'A', 'A'}, {'A', 'B'}, {'A', 'C'}, {'A', 'D'}, {'A', 'E'}, {'A', 'F'},
    {'B', '0'}, {'B', '1'}, {'B', '2'}, {'B', '3'}, {'B', '4'}, {'B', '5'}, {'B', '6'}, {'B', '7'}, {'B', '8'}, {'B', '9'}, {'B', 'A'},
    {'B', 'B'}, {'B', 'C'}, {'B', 'D'}, {'B', 'E'}, {'B', 'F'}, {'C', '0'}, {'C', '1'}, {'C', '2'}, {'C', '3'}, {'C', '4'}, {'C', '5'},
    {'C', '6'}, {'C', '7'}, {'C', '8'}, {'C', '9'}, {'C', 'A'}, {'C', 'B'}, {'C', 'C'}, {'C', 'D'}, {'C', 'E'}, {'C', 'F'}, {'D', '0'},
    {'D', '1'}, {'D', '2'}, {'D', '3'}, {'D', '4'}, {'D', '5'}, {'D', '6'}, {'D', '7'}, {'D', '8'}, {'D', '9'}, {'D', 'A'}, {'D', 'B'},
    {'D', 'C'}, {'D', 'D'}, {'D', 'E'}, {'D', 'F'}, {'E', '0'}, {'E', '1'}, {'E', '2'}, {'E', '3'}, {'E', '4'}, {'E', '5'}, {'E', '6'},
    {'E', '7'}, {'E', '8'}, {'E', '9'}, {'E', 'A'}, {'E', 'B'}, {'E', 'C'}, {'E', 'D'}, {'E', 'E'}, {'E', 'F'}, {'F', '0'}, {'F', '1'},
    {'F', '2'}, {'F', '3'}, {'F', '4'}, {'F', '5'}, {'F', '6'}, {'F', '7'}, {'F', '8'}, {'F', '9'}, {'F', 'A'}, {'F', 'B'}, {'F', 'C'},
    {'F', 'D'}, {'F', 'E'}, {'F', 'F'}
};

CGS_API CGS__FixedMutStrRef cgs__buf_as_fmutstr_ref(CGS_Buffer buf, unsigned int *len_ptr)
{
    *len_ptr                = (unsigned int)((char *)memchr(buf.ptr, '\0', buf.cap) - buf.ptr);
    CGS__FixedMutStrRef ret = {.chars = buf.ptr, .cap = buf.cap, .len = len_ptr};
    return ret;
}

CGS_API CGS__FixedMutStrRef cgs__buf_as_fmutstr_ref_zero_len(CGS_Buffer buf, unsigned int *len_ptr)
{
    *len_ptr                = 0;
    CGS__FixedMutStrRef ret = {.chars = buf.ptr, .cap = buf.cap, .len = len_ptr};
    return ret;
}

CGS_API CGS__FixedMutStrRef cgs__strbuf_ptr_as_fmutstr_ref(CGS_StrBuf *strbuf)
{
    CGS__FixedMutStrRef ret = {.chars = strbuf->chars, .cap = strbuf->cap, .len = &strbuf->len};
    return ret;
}

CGS_API CGS__FixedMutStrRef cgs__mutstr_ref_as_fmutstr_ref2(CGS_MutStrRef mutstr_ref, unsigned int *len_ptr)
{
    switch (mutstr_ref.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_ptr_as_fmutstr_ref(mutstr_ref.str.dstr);
        case CGS__STRBUF_TY:
            return cgs__strbuf_ptr_as_fmutstr_ref(mutstr_ref.str.strbuf);
        case CGS__BUF_TY:
            return cgs__buf_as_fmutstr_ref(mutstr_ref.str.buf, len_ptr);
        default:
            CGS_unreachable();
    }
}

CGS_API CGS__FixedMutStrRef cgs__mutstr_ref_as_fmutstr_ref_zero_len(CGS_MutStrRef mutstr_ref, unsigned int *len_ptr)
{
    switch (mutstr_ref.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_ptr_as_fmutstr_ref(mutstr_ref.str.dstr);
        case CGS__STRBUF_TY:
            return cgs__strbuf_ptr_as_fmutstr_ref(mutstr_ref.str.strbuf);
        case CGS__BUF_TY:
            return cgs__buf_as_fmutstr_ref_zero_len(mutstr_ref.str.buf, len_ptr);
        default:
            CGS_unreachable();
    }
}

CGS_API CGS__FixedMutStrRef cgs__dstr_ptr_as_fmutstr_ref(CGS_DStr *dstr)
{
    CGS__FixedMutStrRef ret = {.chars = dstr->chars, .cap = dstr->cap, .len = &dstr->len};
    return ret;
}

CGS_API CGS_Allocation cgs__default_allocator_alloc(CGS_Allocator *allocator, size_t align, size_t n)
{
    (void)align;
    (void)allocator;
    void *mem = malloc(n);
    return (CGS_Allocation) {
        .ptr = mem,
        .n   = n,
    };
}

CGS_API void cgs__default_allocator_dealloc(CGS_Allocator *allocator, void *ptr, size_t n)
{
    (void)allocator;
    (void)n;
    free(ptr);
}

CGS_API CGS_Allocation cgs__default_allocator_realloc(CGS_Allocator *allocator, void *ptr, size_t align, size_t old_size, size_t new_size)
{
    (void)allocator;
    (void)align;
    (void)old_size;
    void *mem = realloc(ptr, new_size);
    return (CGS_Allocation) {
        .ptr = mem,
        .n   = new_size,
    };
}

CGS_PRIVATE CGS_Allocation cgs__dstr_append_allocator_alloc(CGS_Allocator *allocator, size_t align, size_t n)
{
    CGS_unreachable();
    (void)allocator;
    (void)align;
    (void)n;
    return (CGS_Allocation) {0};
}

CGS_PRIVATE void cgs__dstr_append_allocator_dealloc(CGS_Allocator *allocator, void *ptr, size_t n)
{
    CGS_unreachable();
    (void)allocator;
    (void)ptr;
    (void)n;
}

CGS_PRIVATE CGS_Allocation
cgs__dstr_append_allocator_realloc(CGS_Allocator *allocator, void *ptr, size_t align, size_t old_size, size_t new_size)
{
    (void)align;

    CGS__DStrAppendAllocator *dstr_append_allocator = (__typeof__(dstr_append_allocator))allocator;

    CGS_DStr *owner = dstr_append_allocator->owner;

    assert(old_size == owner->cap - owner->len);
    assert(((char *)ptr - owner->chars) == owner->len);

    cgs_dstr_ensure_cap(owner, owner->cap + (unsigned int)(new_size - old_size));
    return (CGS_Allocation) {
        .ptr = owner->chars + owner->len,
        .n   = owner->cap - owner->len,
    };
}

CGS_API CGS_Allocation cgs__allocator_invoke_alloc(CGS_Allocator *allocator, size_t align, size_t obj_size, size_t nb)
{
    return allocator->alloc(allocator, align, nb * obj_size);
}

CGS_API void cgs__allocator_invoke_dealloc(CGS_Allocator *allocator, void *ptr, size_t obj_size, size_t nb)
{
    allocator->dealloc(allocator, ptr, nb * obj_size);
}

CGS_API CGS_Allocation
cgs__allocator_invoke_realloc(CGS_Allocator *allocator, void *ptr, size_t align, size_t obj_size, size_t old_nb, size_t new_nb)
{
    return allocator->realloc(allocator, ptr, align, old_nb * obj_size, new_nb * obj_size);
}

CGS_API CGS_Allocator *cgs_get_default_allocator()
{
    return &cgs__default_allocator;
}

CGS_PRIVATE void cgs__make_dstr_append_allocator(CGS_DStr *dstr, CGS__DStrAppendAllocator *out)
{
    *out = (CGS__DStrAppendAllocator) {
        .base =
            {
                .alloc   = cgs__dstr_append_allocator_alloc,
                .dealloc = cgs__dstr_append_allocator_dealloc,
                .realloc = cgs__dstr_append_allocator_realloc,
            },
        .owner = dstr
    };
}

CGS_PRIVATE CGS_DStr cgs__make_appender_dstr(CGS_DStr *owner, CGS__DStrAppendAllocator *allocator)
{
    cgs__make_dstr_append_allocator(owner, allocator);
    return (CGS_DStr) {
        .allocator = (void *)allocator,
        .cap       = owner->cap - owner->len,
        .len       = 0,
        .chars     = owner->chars + owner->len,
    };
}

CGS_PRIVATE CGS_StrBuf cgs__make_appender_strbuf(CGS_MutStrRef owner)
{
    return (CGS_StrBuf) {
        .cap   = cgs__mutstr_ref_cap(owner) - cgs__mutstr_ref_len(owner),
        .len   = 0,
        .chars = cgs__mutstr_ref_as_cstr(owner) + cgs__mutstr_ref_len(owner)
    };
}

CGS_API CGS_MutStrRef cgs__make_appender_mutstr_ref(CGS_MutStrRef owner, CGS_AppenderState *state)
{
    switch (owner.ty)
    {
        case CGS__DSTR_TY:
            state->appender_dstr = cgs__make_appender_dstr(owner.str.dstr, &state->dstr_append_allocator);
            return cgs__dstr_ptr_as_mutstr_ref(&state->appender_dstr);
        case CGS__STRBUF_TY:
        case CGS__BUF_TY:;
            CGS_MutStrRef ret = {.ty = CGS__STRBUF_TY};
            ret.str.strbuf    = &state->appender_buf;
            *ret.str.strbuf   = cgs__make_appender_strbuf(owner);
            return ret;
        default:
            CGS_unreachable();
    }
}

CGS_PRIVATE unsigned int cgs__uint_min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

CGS_PRIVATE unsigned int cgs__uint_max(unsigned int a, unsigned int b)
{
    return a > b ? a : b;
}

CGS_PRIVATE unsigned int cgs__chars_strlen(const char *chars, unsigned int cap)
{
    const char *str_end = memchr(chars, '\0', cap);
    unsigned int len;

    if (str_end != NULL)
    {
        len = (unsigned int)(str_end - chars);
    }
    else
    {
        len = cap - 1;
    }

    return len;
}

CGS_PRIVATE bool cgs__is_strv_within(CGS_StrView base, CGS_StrView sub)
{
    uintptr_t begin     = (uintptr_t)base.chars;
    uintptr_t end       = (uintptr_t)(base.chars + base.len);
    uintptr_t sub_begin = (uintptr_t)sub.chars;
    return sub_begin >= begin && sub_begin < end;
}

CGS__NODISCARD("discarding a new DStr may cause memory leak")
CGS_API CGS_DStr cgs__dstr_init(unsigned int cap, CGS_Allocator *allocator)
{
    CGS_DStr ret = {.allocator = allocator};
    if (cap != 0)
    {
        CGS_Allocation allocation = cgs_alloc(allocator, unsigned char, cap);
        ret.chars                 = allocation.ptr;
        ret.cap                   = (unsigned int)allocation.n;

        if (ret.chars != NULL && ret.cap != 0)
        {
            ret.chars[0] = '\0';
        }
    }
    return ret;
}

CGS_API CGS_DStr cgs__dstr_init_from(CGS_StrView from, CGS_Allocator *allocator)
{
    CGS_DStr ret = cgs__dstr_init(from.len + 1, allocator);

    cgs__dstr_copy(&ret, from);

    return ret;
}

CGS_API void cgs__dstr_deinit(CGS_DStr *dstr)
{
    cgs_dealloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap);
    dstr->cap   = 0;
    dstr->len   = 0;
    dstr->chars = NULL;
}

CGS_API CGS_Error cgs__dstr_shrink_to_fit(CGS_DStr *dstr)
{
    CGS_Allocation allocation = cgs_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, dstr->len + 1);
    if (allocation.ptr == NULL)
    {
        return (CGS_Error) {CGS_ALLOC_ERROR};
    }
    else
    {
        dstr->chars = allocation.ptr;
        dstr->cap   = (unsigned int)allocation.n;
        return (CGS_Error) {CGS_OK};
    }
}

CGS_PRIVATE CGS_Error cgs__dstr_maybe_grow(CGS_DStr *dstr, unsigned int len_to_append)
{
    return cgs__dstr_ensure_cap(dstr, dstr->len + len_to_append + 1);
}

CGS_API CGS_Error cgs__dstr_append(CGS_DStr *dstr, CGS_StrView src)
{
    CGS_StrView to_append = src;
    CGS_Error err         = (CGS_Error) {CGS_OK};
    if (cgs__is_strv_within(cgs__strv_dstr_ptr2(dstr, 0), to_append))
    {
        unsigned int begin_idx = (unsigned int)(to_append.chars - dstr->chars);
        err                    = cgs__dstr_maybe_grow(dstr, to_append.len);
        to_append              = (CGS_StrView) {
            .len   = to_append.len,
            .chars = dstr->chars + begin_idx,
        };
    }
    else
    {
        err = cgs__dstr_maybe_grow(dstr, to_append.len);
    }

    if (err.ec == CGS_OK)
    {
        memmove(dstr->chars + dstr->len, to_append.chars, to_append.len * sizeof(unsigned char));

        dstr->len += to_append.len;
        dstr->chars[dstr->len] = '\0';
    }

    return err;
}

CGS_API CGS_Error cgs__dstr_prepend_strv(CGS_DStr *dstr, CGS_StrView src)
{
    CGS_StrView to_prepend = src;
    CGS_Error err          = (CGS_Error) {CGS_OK};

    if (cgs__is_strv_within(cgs__strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = (unsigned int)(src.chars - dstr->chars);
        err                    = cgs__dstr_maybe_grow(dstr, src.len);
        to_prepend             = (CGS_StrView) {
            .len   = src.len,
            .chars = dstr->chars + begin_idx,
        };
    }
    else
    {
        err = cgs__dstr_maybe_grow(dstr, to_prepend.len);
    }

    if (err.ec == CGS_OK)
    {
        memmove(dstr->chars + to_prepend.len, dstr->chars, dstr->len);
        memmove(dstr->chars, to_prepend.chars, to_prepend.len);

        dstr->len += to_prepend.len;
        dstr->chars[dstr->len] = '\0';
    }

    return err;
}

CGS_API CGS_Error cgs__dstr_insert(CGS_DStr *dstr, CGS_StrView src, unsigned int idx)
{
    if (idx > dstr->len)
    {
        return (CGS_Error) {CGS_INDEX_OUT_OF_BOUNDS};
    }

    CGS_StrView to_insert = src;

    if (cgs__is_strv_within(cgs__strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = (unsigned int)(src.chars - dstr->chars);
        CGS_Error err          = cgs__dstr_maybe_grow(dstr, src.len);
        if (err.ec != CGS_OK)
            return err;

        to_insert = (CGS_StrView) {
            .len   = src.len,
            .chars = dstr->chars + begin_idx,
        };
    }
    else
    {
        CGS_Error err = cgs__dstr_maybe_grow(dstr, to_insert.len);
        if (err.ec != CGS_OK)
            return err;
    }

    memmove(dstr->chars + idx + to_insert.len, dstr->chars + idx, dstr->len - idx);
    memmove(dstr->chars + idx, to_insert.chars, to_insert.len);

    dstr->len += to_insert.len;
    dstr->chars[dstr->len] = '\0';

    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__dstr_ensure_cap(CGS_DStr *dstr, unsigned int at_least)
{
    if (dstr->cap < at_least)
    {
        char *save                = dstr->chars;
        size_t new_cap            = cgs__uint_max(at_least, dstr->cap * 2);
        CGS_Allocation allocation = cgs_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap);
        dstr->chars               = allocation.ptr;
        dstr->cap                 = (unsigned int)allocation.n;

        if (dstr->chars == NULL)
        {
            dstr->chars = save;
            return (CGS_Error) {CGS_ALLOC_ERROR};
        }
        if (dstr->cap < at_least)
        {
            return (CGS_Error) {CGS_ALLOC_ERROR};
        }
    }

    return (CGS_Error) {CGS_OK};
}

CGS_API char *cgs__cstr_as_cstr(const char *str)
{
    return (char *)str;
}

CGS_API char *cgs__ucstr_as_cstr(const unsigned char *str)
{
    return (char *)str;
}

CGS_API char *cgs__dstr_as_cstr(const CGS_DStr str)
{
    return str.chars;
}

CGS_API char *cgs__dstr_ptr_as_cstr(const CGS_DStr *str)
{
    return str->chars;
}

CGS_API char *cgs__strv_as_cstr(CGS_StrView str)
{
    return (char *)str.chars;
}

CGS_API char *cgs__strbuf_as_cstr(const CGS_StrBuf str)
{
    return str.chars;
}

CGS_API char *cgs__strbuf_ptr_as_cstr(const CGS_StrBuf *str)
{
    return str->chars;
}

CGS_API char *cgs__mutstr_ref_as_cstr(const CGS_MutStrRef str)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return str.str.dstr->chars;
        case CGS__STRBUF_TY:
            return str.str.strbuf->chars;
        case CGS__BUF_TY:
            return str.str.buf.ptr;
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__mutstr_ref_commit_appender(CGS_MutStrRef owner, CGS_MutStrRef appender)
{
    unsigned int appender_len = cgs__mutstr_ref_len(appender);
    switch (owner.ty)
    {
        case CGS__DSTR_TY:
            owner.str.dstr->len += appender_len;
            break;
        case CGS__STRBUF_TY:
            owner.str.strbuf->len += appender_len;
            break;
        case CGS__BUF_TY:
            break;
        default:
            CGS_unreachable();
    };

    return (CGS_Error) {CGS_OK};
}

CGS_API unsigned int cgs__strv_cap(CGS_StrView sv)
{
    return sv.len;
}

CGS_API unsigned int cgs__dstr_cap(const CGS_DStr str)
{
    return str.cap;
}

CGS_API unsigned int cgs__dstr_ptr_cap(const CGS_DStr *str)
{
    return str->cap;
}

CGS_API unsigned int cgs__strbuf_cap(const CGS_StrBuf str)
{
    return str.cap;
}

CGS_API unsigned int cgs__strbuf_ptr_cap(const CGS_StrBuf *str)
{
    return str->cap;
}

CGS_API unsigned int cgs__mutstr_ref_cap(const CGS_MutStrRef str)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return str.str.dstr->cap;
        case CGS__STRBUF_TY:
            return str.str.strbuf->cap;
        case CGS__BUF_TY:
            return str.str.buf.cap;
        default:
            CGS_unreachable();
    };
}

CGS_API unsigned int cgs__mutstr_ref_len(const CGS_MutStrRef str)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return str.str.dstr->len;
        case CGS__STRBUF_TY:
            return str.str.strbuf->len;
        case CGS__BUF_TY:
            return (unsigned int)strlen(str.str.buf.ptr);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__fmutstr_ref_insert(CGS__FixedMutStrRef dst, CGS_StrView src, unsigned int idx)
{
    unsigned int len = *dst.len;
    if (idx > len)
    {
        return (CGS_Error) {CGS_INDEX_OUT_OF_BOUNDS};
    }

    unsigned int nb_chars_to_insert = cgs__uint_min(dst.cap - len - 1, src.len);

    // shift right
    memmove(dst.chars + idx + nb_chars_to_insert, dst.chars + idx, len - idx);

    // insert the src
    memmove(dst.chars + idx, src.chars, nb_chars_to_insert);

    len += nb_chars_to_insert;

    *dst.len = len;

    return nb_chars_to_insert == src.len ? (CGS_Error) {CGS_OK} : (CGS_Error) {CGS_DST_TOO_SMALL};
}

CGS_API CGS_Error cgs__mutstr_ref_insert(CGS_MutStrRef dst, CGS_StrView src, unsigned int idx)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_insert(dst.str.dstr, src, idx);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_insert(cgs__fmutstr_ref(dst.str.strbuf), src, idx);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_insert(cgs__fmutstr_ref(dst.str.buf, &(unsigned int) {0}), src, idx);
        default:
            CGS_unreachable();
    };
}

CGS_API bool cgs__strv_equal(CGS_StrView str1, CGS_StrView str2)
{
    if (str1.len == 0)
        return str2.len == 0; // passing NULL to memcmp is UB, even if 0 bytes. But CGS allows strings to be NULL if len is 0
    return (str1.len == str2.len) && (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

CGS_API CGS_StrView cgs__strv_find(CGS_StrView hay, CGS_StrView needle)
{
    if (hay.chars == NULL || needle.chars == NULL || needle.len > hay.len)
        return (CGS_StrView) {
            .chars = NULL,
            .len   = 0,
        };
    if (needle.len == 0)
        return (CGS_StrView) {
            .chars = hay.chars,
            .len   = 0,
        };

    unsigned int scan_end        = hay.len - needle.len;
    const char *max_possible_ptr = &hay.chars[scan_end];
    const char *first_char       = hay.chars;
    unsigned int remaining_len;

    while (first_char && first_char <= max_possible_ptr)
    {
        if (memcmp(first_char, needle.chars, needle.len) == 0)
            return (CGS_StrView) {
                .chars = (char *)first_char,
                .len   = needle.len,
            };
        remaining_len = scan_end - (unsigned int)(first_char - hay.chars);
        first_char    = memchr(first_char + 1, needle.chars[0], remaining_len);
    }

    return (CGS_StrView) {
        .chars = NULL,
        .len   = 0,
    };
}

CGS_API CGS_Error cgs__fmutstr_ref_copy(CGS__FixedMutStrRef dst, CGS_StrView src)
{
    if (dst.cap == 0)
    {
        return src.len == 0 ? (CGS_Error) {CGS_OK} : (CGS_Error) {CGS_DST_TOO_SMALL};
    }
    unsigned int chars_to_copy = cgs__uint_min(src.len, dst.cap - 1);

    memmove(dst.chars, src.chars, chars_to_copy * sizeof(unsigned char));
    dst.chars[chars_to_copy] = '\0';

    *dst.len = chars_to_copy;

    return chars_to_copy == src.len ? (CGS_Error) {CGS_OK} : (CGS_Error) {CGS_DST_TOO_SMALL};
}

CGS_API CGS_Error cgs__dstr_copy(CGS_DStr *dstr, CGS_StrView src)
{
    CGS_Error err = cgs__dstr_ensure_cap(dstr, src.len + 1);

    if (err.ec == CGS_OK)
    {
        memmove(dstr->chars, src.chars, src.len * sizeof(unsigned char));

        dstr->len              = src.len;
        dstr->chars[dstr->len] = '\0';
    }

    return err;
}

CGS_API CGS_Error cgs__mutstr_ref_copy(CGS_MutStrRef dst, CGS_StrView src)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_copy(dst.str.dstr, src);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_copy(cgs__fmutstr_ref(dst.str.strbuf), src);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_copy(cgs__fmutstr_ref(dst.str.buf, &(unsigned int) {0}), src);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__dstr_putc(CGS_DStr *dst, char c)
{
    CGS_Error err = cgs__dstr_ensure_cap(dst, dst->len + 2);
    if (err.ec != CGS_OK)
        return err;

    dst->chars[dst->len]     = c;
    dst->chars[dst->len + 1] = '\0';
    dst->len += 1;

    return err;
}

CGS_API CGS_Error cgs__fmutstr_ref_putc(CGS__FixedMutStrRef dst, char c)
{
    if (dst.cap - *dst.len <= 1)
    {
        return (CGS_Error) {CGS_DST_TOO_SMALL};
    }

    dst.chars[*dst.len]     = c;
    dst.chars[*dst.len + 1] = '\0';
    *dst.len += 1;

    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__mutstr_ref_putc(CGS_MutStrRef dst, char c)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_putc(dst.str.dstr, c);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_putc(cgs__fmutstr_ref(dst.str.strbuf), c);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_putc(cgs__fmutstr_ref(dst.str.buf, &(unsigned int) {0}), c);
        default:
            CGS_unreachable();
    }
}

CGS_API CGS_Error cgs__fmutstr_ref_append(CGS__FixedMutStrRef dst, CGS_StrView src)
{
    if (dst.cap == 0)
    {
        return (CGS_Error) {CGS_DST_TOO_SMALL};
    }

    unsigned int dst_len = *dst.len;

    unsigned int chars_to_copy = cgs__uint_min(src.len, dst.cap - dst_len - 1);
    memmove(dst.chars + dst_len, src.chars, chars_to_copy);

    dst_len += chars_to_copy;

    if (dst.len != NULL)
        *dst.len = dst_len;

    dst.chars[dst_len] = '\0';

    return chars_to_copy == src.len ? (CGS_Error) {CGS_OK} : (CGS_Error) {CGS_DST_TOO_SMALL};
}

CGS_API CGS_Error cgs__mutstr_ref_append(CGS_MutStrRef dst, CGS_StrView src)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_append(dst.str.dstr, src);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_append(cgs__fmutstr_ref(dst.str.strbuf), src);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_append(cgs__fmutstr_ref(dst.str.buf, &(unsigned int) {0}), src);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__fmutstr_ref_delete_range(CGS__FixedMutStrRef str, unsigned int begin, unsigned int end)
{
    unsigned int len = *str.len;

    if (end > len || begin > len)
    {
        return (CGS_Error) {CGS_INDEX_OUT_OF_BOUNDS};
    }
    if (begin > end)
    {
        return (CGS_Error) {CGS_BAD_RANGE};
    }

    unsigned int substr_len = end - begin;

    memmove(str.chars + begin, str.chars + begin + substr_len, len - begin - substr_len);

    len -= substr_len;

    str.chars[len] = '\0';

    if (str.len != NULL)
    {
        *str.len = len;
    }

    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__mutstr_ref_delete_range(CGS_MutStrRef str, unsigned int begin, unsigned int end)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__fmutstr_ref_delete_range(cgs__fmutstr_ref(str.str.dstr), begin, end);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_delete_range(cgs__fmutstr_ref(str.str.strbuf), begin, end);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_delete_range(cgs__fmutstr_ref(str.str.buf, &(unsigned int) {0}), begin, end);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_StrViewArray cgs__strv_arr_from(const CGS_StrView *carr, unsigned int nb)
{
    return (CGS_StrViewArray) {
        .cap  = nb,
        .len  = nb,
        .strs = (CGS_StrView *)carr,
    };
}

CGS_API CGS_Error cgs__strv_split_iter(CGS_StrView str, CGS_StrView delim, bool (*cb)(CGS_StrView found, void *ctx), void *ctx)
{
    if (delim.len > str.len || str.len == 0)
    {
        return !cb(str, ctx) ? (CGS_Error) {CGS_CALLBACK_EXIT} : (CGS_Error) {CGS_OK};
    }
    else if (delim.len == 0)
    {
        for (unsigned int i = 0; i < str.len; i++)
        {
            if (!cb(cgs__strv_strv3(str, i, i + 1), ctx))
            {
                return (CGS_Error) {CGS_CALLBACK_EXIT};
            }
        }

        return (CGS_Error) {CGS_OK};
    }
    else
    {
        unsigned int prev = 0;
        for (unsigned int i = 0; i <= str.len - delim.len;)
        {
            CGS_StrView rem = cgs__strv_strv2(str, i);
            if (cgs__strv_starts_with(rem, delim))
            {
                CGS_StrView sub = cgs__strv_strv3(str, prev, i);
                if (!cb(sub, ctx))
                {
                    return (CGS_Error) {CGS_CALLBACK_EXIT};
                }
                i += delim.len;
                prev = i;
            }
            else
            {
                i += 1;
            }
        }

        if (!cb(cgs__strv_strv3(str, prev, str.len), ctx))
        {
            return (CGS_Error) {CGS_CALLBACK_EXIT};
        }
        else
        {
            return (CGS_Error) {CGS_OK};
        }
    }
}

CGS_PRIVATE bool cgs__combine_views_into_array(CGS_StrView str, void *ctx)
{
    struct
    {
        CGS_Allocator *allocator;
        CGS_StrViewArray array;
    } *tctx = ctx;

    CGS_StrViewArray *array  = &tctx->array;
    CGS_Allocator *allocator = tctx->allocator;

    if (array->cap <= array->len)
    {
        CGS_Allocation allocation = cgs_realloc(allocator, array->strs, CGS_StrView, array->cap, (size_t)2 * (array->len + 1));
        array->strs               = allocation.ptr;
        array->cap                = (unsigned int)(allocation.n / sizeof(CGS_StrView));
    }

    array->strs[array->len++] = str;

    return true;
}

CGS__NODISCARD("str_split returns new String_View_Array")
CGS_API CGS_StrViewArray cgs__strv_split(CGS_StrView str, CGS_StrView delim, CGS_Allocator *allocator)
{
    struct
    {
        CGS_Allocator *allocator;
        CGS_StrViewArray array;
    } ctx = {.allocator = allocator};

    cgs__strv_split_iter(str, delim, cgs__combine_views_into_array, &ctx);

    return ctx.array;
}

CGS_API CGS_Error cgs__strv_arr_join_into_dstr(CGS_DStr *dstr, CGS_StrViewArray strs, CGS_StrView delim)
{
    CGS_Error err = (CGS_Error) {CGS_OK};

    if (strs.len > 0)
        err = cgs__dstr_copy(dstr, strs.strs[0]);

    for (unsigned int i = 1; i < strs.len && err.ec == CGS_OK; i++)
    {
        cgs__dstr_append(dstr, delim);
        err = cgs__dstr_append(dstr, strs.strs[i]);
    }

    return err;
}

CGS_API CGS_Error cgs__strv_arr_join_into_fmutstr_ref(CGS__FixedMutStrRef dst, CGS_StrViewArray strs, CGS_StrView delim)
{
    CGS_Error err = (CGS_Error) {CGS_OK};

    if (strs.len > 0)
        err = cgs__fmutstr_ref_copy(dst, strs.strs[0]);

    for (unsigned int i = 1; i < strs.len && err.ec == CGS_OK; i++)
    {
        cgs__fmutstr_ref_append(dst, delim);
        err = cgs__fmutstr_ref_append(dst, strs.strs[i]);
    }

    return err;
}

CGS_API CGS_Error cgs__strv_arr_join(CGS_MutStrRef dst, CGS_StrViewArray strs, CGS_StrView delim)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__strv_arr_join_into_dstr(dst.str.dstr, strs, delim);
        case CGS__STRBUF_TY:
            return cgs__strv_arr_join_into_fmutstr_ref(cgs__fmutstr_ref(dst.str.strbuf), strs, delim);
        case CGS__BUF_TY:
            return cgs__strv_arr_join_into_fmutstr_ref(cgs__fmutstr_ref(dst.str.buf, &(unsigned int) {0}), strs, delim);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Result(CGS_StrView) cgs__next_tok(CGS_StrView *base, CGS_StrView delim)
{
    CGS_StrView rest = cgs__skip(*base, delim);
    *base            = rest;

    if (rest.len == 0)
    {
        // string consists entirely of delims
        return (CGS_Result(CGS_StrView)) {.val = rest, .err = {CGS_NOT_FOUND}};
    }

    CGS_StrView found = cgs__strv_find(*base, delim);
    if (found.chars)
    {
        unsigned int index = (unsigned int)(found.chars - base->chars);
        CGS_StrView ret    = {.chars = base->chars, .len = index};

        base->chars += index;
        base->len -= index;

        return (CGS_Result(CGS_StrView)) {.val = ret, .err = {CGS_OK}};
    }
    else
    {
        CGS_StrView ret = *base;
        base->chars += base->len;
        base->len = 0;

        return (CGS_Result(CGS_StrView)) {.val = ret, .err = {CGS_OK}};
        ;
    }
}

CGS_API CGS_Result(CGS_StrView) cgs__next_tok_any(CGS_StrView *base, CGS_StrView delim_set)
{
    CGS_StrView rest = cgs__skip_any(*base, delim_set);
    *base            = rest;

    if (rest.len == 0)
    {
        return (CGS_Result(CGS_StrView)) {.val = rest, .err = {CGS_NOT_FOUND}};
    }

    CGS_StrView tok = cgs__strv_cspn(*base, delim_set);

    base->chars += tok.len;
    base->len -= tok.len;

    return (CGS_Result(CGS_StrView)) {.err = {CGS_OK}, .val = tok};
}

CGS_API CGS_StrView cgs__skip(CGS_StrView src, CGS_StrView delim)
{
    while (cgs_starts_with(src, delim))
    {
        src.len -= delim.len;
        src.chars += delim.len;
    }

    return src;
}

CGS_API CGS_StrView cgs__skip_any(CGS_StrView src, CGS_StrView delim_set)
{
    while (src.len > 0 && memchr(delim_set.chars, src.chars[0], delim_set.len))
    {
        src.len -= 1;
        src.chars += 1;
    }

    return src;
}

CGS_API CGS_Error cgs__dstr_replace_range(CGS_DStr *dstr, unsigned int begin, unsigned int end, CGS_StrView replacement)
{
    if (begin > dstr->len || end > dstr->len)
        return (CGS_Error) {CGS_INDEX_OUT_OF_BOUNDS};
    if (begin > end)
        return (CGS_Error) {CGS_BAD_RANGE};
    if (cgs__is_strv_within(cgs__strv_dstr_ptr2(dstr, 0), replacement))
        return (CGS_Error) {CGS_ALIASING_NOT_SUPPORTED};

    unsigned int len_to_delete = end - begin;
    if (len_to_delete > replacement.len)
    {
        // shift left
        memmove(dstr->chars + begin + replacement.len, dstr->chars + end, dstr->len - end + 1);
        // insert the replacement
        memmove(dstr->chars + begin, replacement.chars, replacement.len);
    }
    else if (len_to_delete < replacement.len)
    {
        cgs__dstr_ensure_cap(dstr, dstr->len + replacement.len - len_to_delete + 1);

        // shift right
        memmove(dstr->chars + end + (replacement.len - len_to_delete), dstr->chars + end, dstr->len - end + 1);
        // insert the replacement
        memmove(dstr->chars + begin, replacement.chars, replacement.len);
    }
    else
    {
        memmove(dstr->chars + begin, replacement.chars, replacement.len);
    }

    dstr->len = dstr->len - len_to_delete + replacement.len;
    return (CGS_Error) {CGS_OK};
}

CGS_PRIVATE void cgs__fmutstr_ref_replace_range_unsafe(CGS__FixedMutStrRef str, unsigned int begin, unsigned int end, CGS_StrView replacement)
{
    unsigned int len_to_delete = end - begin;
    if (len_to_delete > replacement.len)
    {
        // shift left
        memmove(str.chars + begin + replacement.len, str.chars + end, *str.len - end + 1);
        // insert the replacement
        memmove(str.chars + begin, replacement.chars, replacement.len);

        *str.len -= len_to_delete - replacement.len;
    }
    else if (len_to_delete < replacement.len)
    {
        unsigned int new_space = cgs__uint_min(replacement.len - len_to_delete, str.cap - *str.len - 1);

        // shift right
        memmove(str.chars + begin + new_space, str.chars + begin, *str.len - begin);
        // insert the replacement
        memmove(str.chars + begin, replacement.chars, cgs__uint_min(replacement.len, len_to_delete + new_space));

        *str.len += new_space;

        str.chars[*str.len] = '\0';
    }
    else
    {
        memmove(str.chars + begin, replacement.chars, replacement.len);
    }
}

CGS_PRIVATE CGS_StrView cgs__strv_fmutstr_ref2(CGS__FixedMutStrRef str, unsigned int begin);

CGS_Error cgs__fmutstr_ref_replace_range(CGS__FixedMutStrRef str, unsigned int begin, unsigned int end, CGS_StrView replacement)
{
    if (begin >= *str.len)
        return (CGS_Error) {CGS_INDEX_OUT_OF_BOUNDS};
    if (begin >= end || end > *str.len)
        return (CGS_Error) {CGS_BAD_RANGE};
    if (cgs__is_strv_within(cgs__strv_fmutstr_ref2(str, 0), replacement))
        return (CGS_Error) {CGS_ALIASING_NOT_SUPPORTED};

    CGS_Error err = (*str.len - (end - begin) + replacement.len) >= str.cap ? (CGS_Error) {CGS_DST_TOO_SMALL} : (CGS_Error) {CGS_OK};
    cgs__fmutstr_ref_replace_range_unsafe(str, begin, end, replacement);

    return err;
}

CGS_API CGS_Error cgs__mutstr_ref_replace_range(CGS_MutStrRef str, unsigned int begin, unsigned int end, CGS_StrView replacement)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_replace_range(str.str.dstr, begin, end, replacement);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_replace_range(cgs__fmutstr_ref(str.str.strbuf), begin, end, replacement);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_replace_range(cgs__fmutstr_ref(str.str.buf, &(unsigned int) {0}), begin, end, replacement);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Result(int) cgs__fmutstr_ref_replace(CGS__FixedMutStrRef str, CGS_StrView target, CGS_StrView replacement)
{
    CGS_StrView as_strv = cgs__strv_fmutstr_ref2(str, 0);
    if (cgs__is_strv_within(as_strv, target) || cgs__is_strv_within(as_strv, replacement))
    {
        return (CGS_Result(int)) {.val = 0, .err = {CGS_ALIASING_NOT_SUPPORTED}};
    }

    CGS_Error err     = {CGS_OK};
    int replace_count = 0;

    if (target.len == 0)
    {
        for (unsigned int i = 0; i <= *str.len && (err.ec == CGS_OK); i += replacement.len + 1)
        {
            err = cgs__fmutstr_ref_insert(str, replacement, i);
            replace_count += 1;
        }
        goto out;
    }

    if (target.len < replacement.len)
    {
        for (unsigned int i = 0; i <= *str.len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_fmutstr_ref2(str, i), target);
            if (match.chars != NULL)
            {
                unsigned int idx = (unsigned int)(match.chars - str.chars);

                if (str.cap > *str.len + (replacement.len - target.len))
                {
                    // shift right
                    memmove(
                        str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char)
                    );

                    // put the replacement
                    memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                    *str.len += (replacement.len - target.len);

                    i = idx + replacement.len;

                    replace_count += 1;
                }
                else
                {
                    err = (CGS_Error) {CGS_DST_TOO_SMALL};
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else if (target.len > replacement.len)
    {
        for (unsigned int i = 0; i <= *str.len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_fmutstr_ref2(str, i), target);
            if (match.chars != NULL)
            {
                unsigned int idx = (unsigned int)(match.chars - str.chars);

                // shift left
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));

                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                *str.len -= (target.len - replacement.len);

                i = idx + replacement.len;

                replace_count += 1;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i <= *str.len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_fmutstr_ref2(str, i), target);
            if (match.chars != NULL)
            {
                err.ec           = CGS_OK;
                unsigned int idx = (unsigned int)(match.chars - str.chars);

                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                i = idx + replacement.len;

                replace_count += 1;
            }
            else
            {
                break;
            }
        }
    }

    if (str.cap > 0)
        str.chars[*str.len] = '\0';

out:
    if (replace_count == 0 && err.ec == CGS_OK)
        err.ec = CGS_NOT_FOUND;
    return (CGS_Result(int)) {.val = replace_count, .err = err};
}

CGS_API CGS_Result(int) cgs__dstr_replace(CGS_DStr *dstr, CGS_StrView target, CGS_StrView replacement)
{
    CGS_StrView as_strv = cgs__strv_dstr_ptr2(dstr, 0);
    if (cgs__is_strv_within(as_strv, target) || cgs__is_strv_within(as_strv, replacement))
    {
        return (CGS_Result(int)) {.val = 0, .err = {CGS_ALIASING_NOT_SUPPORTED}};
    }

    CGS_Error err     = {CGS_OK};
    int replace_count = 0;

    if (target.len == 0)
    {
        err.ec = CGS_OK;
        for (unsigned int i = 0; i <= dstr->len && (err.ec == CGS_OK); i += replacement.len + 1)
        {
            err = cgs__dstr_insert(dstr, replacement, i);
            replace_count += 1;
        }
        goto out;
    }

    if (target.len < replacement.len)
    {
        for (unsigned int i = 0; i <= dstr->len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_dstr_ptr2(dstr, i), target);
            if (match.chars != NULL)
            {
                unsigned int idx = (unsigned int)(match.chars - dstr->chars);

                err = cgs__dstr_ensure_cap(dstr, dstr->len + (replacement.len - target.len) + 1);

                // shift right
                memmove(
                    dstr->chars + idx + replacement.len, dstr->chars + idx + target.len, (dstr->len - idx - target.len) * sizeof(unsigned char)
                );

                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                dstr->len += (replacement.len - target.len);

                i = idx + replacement.len;

                replace_count += 1;
            }
            else
            {
                break;
            }
        }
    }
    else if (target.len > replacement.len)
    {
        for (unsigned int i = 0; i <= dstr->len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_dstr_ptr2(dstr, i), target);
            if (match.chars != NULL)
            {
                unsigned int idx = (unsigned int)(match.chars - dstr->chars);

                // shift left
                memmove(
                    dstr->chars + idx + replacement.len, dstr->chars + idx + target.len, (dstr->len - idx - target.len) * sizeof(unsigned char)
                );

                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                dstr->len -= (target.len - replacement.len);

                i = idx + replacement.len;

                replace_count += 1;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i <= dstr->len - target.len;)
        {
            CGS_StrView match = cgs__strv_find(cgs__strv_dstr_ptr2(dstr, i), target);
            if (match.chars != NULL)
            {
                unsigned int idx = (unsigned int)(match.chars - dstr->chars);

                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

                i = idx + replacement.len;

                replace_count += 1;
            }
            else
            {
                break;
            }
        }
    }

    dstr->chars[dstr->len] = '\0';

out:
    if (replace_count == 0 && err.ec == CGS_OK)
        err.ec = CGS_NOT_FOUND;
    return (CGS_Result(int)) {.val = replace_count, .err = err};
}

CGS_API CGS_Result(int) cgs__mutstr_ref_replace(CGS_MutStrRef str, CGS_StrView target, CGS_StrView replacement)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_replace(str.str.dstr, target, replacement);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_replace(cgs__fmutstr_ref(str.str.strbuf), target, replacement);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_replace(cgs__fmutstr_ref(str.str.buf, &(unsigned int) {0}), target, replacement);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__dstr_replace_first(CGS_DStr *dstr, CGS_StrView target, CGS_StrView replacement)
{
    CGS_Error err = {CGS_NOT_FOUND};

    CGS_StrView match = cgs__strv_find(cgs__strv_dstr_ptr2(dstr, 0), target);
    if (match.chars != NULL)
    {
        unsigned int begin = (unsigned int)(match.chars - dstr->chars);
        unsigned int end   = begin + match.len;
        err                = cgs__dstr_replace_range(dstr, begin, end, replacement);
    }

    return err;
}

CGS_API CGS_Error cgs__fmutstr_ref_replace_first(CGS__FixedMutStrRef str, CGS_StrView target, CGS_StrView replacement)
{
    CGS_Error err = {CGS_NOT_FOUND};

    CGS_StrView match = cgs__strv_find(cgs__strv_fmutstr_ref2(str, 0), target);
    if (match.chars != NULL)
    {
        // TODO make this fill as much as possible. just call replace_range
        if (str.cap > 0 && str.cap - 1 > *str.len + (replacement.len - target.len))
        {
            unsigned int idx = (unsigned int)(match.chars - str.chars);

            // shift
            memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));

            // put the replacement
            memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));

            *str.len += (replacement.len - target.len);

            err.ec = CGS_OK;
        }
        else
        {
            err.ec = CGS_DST_TOO_SMALL;
        }
    }

    if (str.cap > 0)
        str.chars[*str.len] = '\0';

    return err;
}

CGS_API CGS_Error cgs__mutstr_ref_replace_first(CGS_MutStrRef str, CGS_StrView target, CGS_StrView replacement)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_replace_first(str.str.dstr, target, replacement);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_replace_first(cgs__fmutstr_ref(str.str.strbuf), target, replacement);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_replace_first(cgs__fmutstr_ref(str.str.buf, &(unsigned int) {0}), target, replacement);
        default:
            CGS_unreachable();
    };
}

CGS_API unsigned int cgs__strv_count(CGS_StrView hay, CGS_StrView needle)
{
    if (needle.len == 0)
        return hay.len + 1;

    unsigned int count = 0;
    CGS_StrView found  = cgs__strv_find(hay, needle);

    while (found.chars != NULL)
    {
        count += 1;
        found = cgs__strv_find(cgs__strv_strv2(hay, (unsigned int)(found.chars - hay.chars) + found.len), needle);
    }

    return count;
}

CGS_API CGS_StrView cgs__strv_cspn(CGS_StrView src, CGS_StrView charset)
{
    if (charset.len == 1)
    {
        char *found = (char *)memchr(src.chars, charset.chars[0], src.len);
        if (found)
        {
            return (CGS_StrView) {
                .chars = src.chars,
                .len   = (unsigned int)(found - src.chars),
            };
        }
        else
        {
            return src;
        }
    }

    for (unsigned int i = 0; i < src.len; i++)
    {
        if (memchr(charset.chars, src.chars[i], charset.len))
        {
            return (CGS_StrView) {
                .chars = src.chars,
                .len   = i,
            };
        }
    }

    return src;
}

CGS_API CGS_StrView cgs__strv_spn(CGS_StrView src, CGS_StrView charset)
{
    for (unsigned int i = 0; i < src.len; i++)
    {
        if (memchr(charset.chars, src.chars[i], charset.len) == NULL)
        {
            return (CGS_StrView) {
                .chars = src.chars,
                .len   = i,
            };
        }
    }

    return src;
}

CGS_API CGS_StrView cgs__trim_view(CGS_StrView str)
{
    if (str.len == 0)
    {
        return str;
    }
    unsigned int begin = 0;
    for (; begin < str.len; begin++)
    {
        if (!isspace(str.chars[begin]))
        {
            break;
        }
    }
    unsigned int end = str.len;
    for (; end > begin; end--)
    {
        if (!isspace(str.chars[end - 1]))
        {
            break;
        }
    }

    return cgs__strv_strv3(str, begin, end);
}

CGS_API CGS_Error cgs__trim(CGS__FixedMutStrRef str)
{
    unsigned int begin = 0;
    for (; begin < *str.len; begin++)
    {
        if (!isspace(str.chars[begin]))
        {
            break;
        }
    }

    unsigned int end = *str.len;
    for (; end > begin; end--)
    {
        if (!isspace(str.chars[end - 1]))
        {
            break;
        }
    }

    unsigned int len = end - begin;
    memmove(str.chars, str.chars + begin, len);

    *str.len       = len;
    str.chars[len] = '\0';

    return (CGS_Error) {CGS_OK};
}

CGS_API bool cgs__strv_starts_with(CGS_StrView hay, CGS_StrView needle)
{
    if (needle.len == 0)
        return true;
    if (hay.len == 0)
        return false;
    return (needle.len <= hay.len) && (memcmp(hay.chars, needle.chars, needle.len) == 0);
}

CGS_API bool cgs__strv_ends_with(CGS_StrView hay, CGS_StrView needle)
{
    if (needle.len == 0)
        return true;
    if (hay.len == 0)
        return false;
    return (needle.len <= hay.len) && (memcmp(hay.chars + hay.len - needle.len, needle.chars, needle.len) == 0);
}

CGS_API CGS_Error cgs__map_chars(CGS__FixedMutStrRef str, bool (*map)(char *c, void *arg), void *arg)
{
    for (unsigned int i = 0; i < *str.len; i++)
    {
        if (!map(str.chars + i, arg))
            return (CGS_Error) {CGS_CALLBACK_EXIT};
    }
    return (CGS_Error) {CGS_OK};
}

CGS_PRIVATE bool cgs__map_tolower(char *c, void *arg)
{
    (void)arg;
    *c = (char)tolower(*c);
    return true;
}

CGS_PRIVATE bool cgs__map_toupper(char *c, void *arg)
{
    (void)arg;
    *c = (char)toupper(*c);
    return true;
}

CGS_API void cgs__chars_tolower(CGS__FixedMutStrRef str)
{
    cgs__map_chars(str, cgs__map_tolower, NULL);
}

CGS_API void cgs__chars_toupper(CGS__FixedMutStrRef str)
{
    cgs__map_chars(str, cgs__map_toupper, NULL);
}

CGS_API CGS_Error cgs__fmutstr_ref_clear(CGS__FixedMutStrRef str)
{
    *str.len = 0;
    if (str.cap > 0)
    {
        str.chars[0] = '\0';
    }
    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__mutstr_ref_clear(CGS_MutStrRef str)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__fmutstr_ref_clear(cgs__dstr_ptr_as_fmutstr_ref(str.str.dstr));
            break;
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_clear(cgs__strbuf_ptr_as_fmutstr_ref(str.str.strbuf));
            break;
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_clear(cgs__buf_as_fmutstr_ref(str.str.buf, &(unsigned int) {0}));
            break;
        default:
            CGS_unreachable();
    }
}

CGS_API CGS_MutStrRef cgs__cstr_as_mutstr_ref(const char *str)
{
    unsigned int len = (unsigned int)strlen(str);

    CGS_Buffer asbuf = {.ptr = (char *)str, .cap = len + 1};

    return cgs__buf_as_mutstr_ref(asbuf);
}

CGS_API CGS_MutStrRef cgs__ucstr_as_mutstr_ref(const unsigned char *str)
{
    unsigned int len = (unsigned int)strlen((char *)str);

    CGS_Buffer asbuf = {.ptr = (char *)str, .cap = len + 1};

    return cgs__buf_as_mutstr_ref(asbuf);
}

CGS_API CGS_MutStrRef cgs__buf_as_mutstr_ref(const CGS_Buffer str)
{
    return (CGS_MutStrRef) {
        .ty      = CGS__BUF_TY,
        .str.buf = *(CGS_Buffer *)&str,
    };
}

CGS_API CGS_MutStrRef cgs__dstr_ptr_as_mutstr_ref(const CGS_DStr *str)
{
    return (CGS_MutStrRef) {
        .ty       = CGS__DSTR_TY,
        .str.dstr = (CGS_DStr *)str,
    };
}

CGS_API CGS_MutStrRef cgs__strbuf_ptr_as_mutstr_ref(const CGS_StrBuf *str)
{
    return (CGS_MutStrRef) {
        .ty         = CGS__STRBUF_TY,
        .str.strbuf = (CGS_StrBuf *)str,
    };
}

CGS_API CGS_MutStrRef cgs__mutstr_ref_as_mutstr_ref(const CGS_MutStrRef str)
{
    return str;
}

CGS_API CGS_StrBuf cgs__strbuf_from_cstr(const char *ptr)
{
    unsigned int len = (unsigned int)strlen(ptr);
    unsigned int cap = len + 1;

    return (CGS_StrBuf) {
        .cap   = cap,
        .len   = len,
        .chars = (char *)ptr,
    };
}

CGS_API CGS_StrBuf cgs__strbuf_from_cstr_cap(const char *ptr, unsigned int cap)
{
    unsigned int len = cgs__chars_strlen(ptr, cap);

    return (CGS_StrBuf) {
        .cap   = cap,
        .len   = len,
        .chars = (char *)ptr,
    };
}

CGS_API CGS_StrBuf cgs__strbuf_from_buf(const CGS_Buffer buf)
{
    CGS_StrBuf ret = {.cap = buf.cap, .len = 0, .chars = buf.ptr};

    if (ret.cap > 0)
        ret.chars[0] = '\0';

    return ret;
}

CGS_API CGS_Buffer cgs__buf_from_cstr(const char *str)
{
    return (CGS_Buffer) {
        .ptr = (char *)str,
        .cap = (unsigned int)strlen(str) + 1,
    };
}

CGS_API CGS_Buffer cgs__buf_from_ucstr(const unsigned char *str)
{
    return (CGS_Buffer) {
        .ptr = (char *)str,
        .cap = (unsigned int)strlen((char *)str) + 1,
    };
}

CGS_API CGS_Buffer cgs__buf_from_carr(const char *str, size_t cap)
{
    return (CGS_Buffer) {
        .ptr = (char *)str,
        .cap = (unsigned int)cap,
    };
}

CGS_API CGS_Buffer cgs__buf_from_ucarr(const unsigned char *str, size_t cap)
{
    return (CGS_Buffer) {
        .ptr = (char *)str,
        .cap = (unsigned int)cap,
    };
}

CGS_API CGS_StrView cgs__strv_cstr1(const char *str)
{
    return (CGS_StrView) {
        .chars = (char *)str,
        .len   = (unsigned int)strlen(str),
    };
}

CGS_API CGS_StrView cgs__strv_ucstr1(const unsigned char *str)
{
    return (CGS_StrView) {
        .chars = (char *)str,
        .len   = (unsigned int)strlen((char *)str),
    };
}

CGS_API CGS_StrView cgs__strv_dstr1(const CGS_DStr str)
{
    return (CGS_StrView) {
        .chars = str.chars,
        .len   = str.len,
    };
}

CGS_API CGS_StrView cgs__strv_dstr_ptr1(const CGS_DStr *str)
{
    return (CGS_StrView) {
        .chars = str->chars,
        .len   = str->len,
    };
}

CGS_API CGS_StrView cgs__strv_strv1(CGS_StrView str)
{
    return str;
}

CGS_API CGS_StrView cgs__strv_zstrv1(CGS_ZStrView str)
{
    return *(CGS_StrView *)&str;
}

CGS_API CGS_StrView cgs__strv_strbuf1(const CGS_StrBuf str)
{
    return (CGS_StrView) {
        .chars = str.chars,
        .len   = str.len,
    };
}

CGS_API CGS_StrView cgs__strv_strbuf_ptr1(const CGS_StrBuf *str)
{
    return (CGS_StrView) {
        .chars = str->chars,
        .len   = str->len,
    };
}

CGS_API CGS_StrView cgs__strv_mutstr_ref1(const CGS_MutStrRef str)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__strv_dstr_ptr1(str.str.dstr);
        case CGS__STRBUF_TY:
            return cgs__strv_strbuf_ptr1(str.str.strbuf);
        case CGS__BUF_TY:
            return cgs__strv_cstr1(str.str.buf.ptr);
        default:
            CGS_unreachable();
    }
}

CGS_API CGS_StrView cgs__strv_cstr2(const char *str, unsigned int begin)
{
#ifndef CGS_NDEBUG
    unsigned int len = (unsigned int)strlen(str);
    if (begin > len)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {
        .len   = len - begin,
        .chars = (char *)str + begin,
    };
}

CGS_API CGS_StrView cgs__strv_ucstr2(const unsigned char *str, unsigned int begin)
{
    return cgs__strv_cstr2((char *)str, begin);
}

CGS_API CGS_StrView cgs__strv_dstr_ptr2(const CGS_DStr *str, unsigned int begin)
{
    if (begin > str->len)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }

    return (CGS_StrView) {
        .len   = str->len - begin,
        .chars = str->chars + begin,
    };
}

CGS_API CGS_StrView cgs__strv_dstr2(const CGS_DStr str, unsigned int begin)
{
    return cgs__strv_dstr_ptr2(&str, begin);
}

CGS_API CGS_StrView cgs__strv_strv2(CGS_StrView str, unsigned int begin)
{
#ifndef CGS_NDEBUG
    if (begin > str.len)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {
        .len   = str.len - begin,
        .chars = str.chars + begin,
    };
}

CGS_API CGS_StrView cgs__strv_zstrv2(CGS_ZStrView str, unsigned int begin)
{
    CGS_StrView strv = *(CGS_StrView *)&str;
    return cgs__strv_strv2(strv, begin);
}

CGS_API CGS_StrView cgs__strv_strbuf_ptr2(const CGS_StrBuf *str, unsigned int begin)
{
#ifndef CGS_NDEBUG
    if (begin > str->len)
    {
        return (CGS_StrView) {.len = 0, .chars = NULL};
    }
#endif

    return (CGS_StrView) {
        .len   = str->len - begin,
        .chars = str->chars + begin,
    };
}

CGS_API CGS_StrView cgs__strv_strbuf2(const CGS_StrBuf str, unsigned int begin)
{
    return cgs__strv_strbuf_ptr2(&str, begin);
}

CGS_API CGS_StrView cgs__strv_mutstr_ref2(const CGS_MutStrRef str, unsigned int begin)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__strv_dstr_ptr2(str.str.dstr, begin);
        case CGS__STRBUF_TY:
            return cgs__strv_strbuf_ptr2(str.str.strbuf, begin);
        case CGS__BUF_TY:
            return cgs__strv_fmutstr_ref2(cgs__buf_as_fmutstr_ref(str.str.buf, &(unsigned int) {0}), begin);
        default:
            CGS_unreachable();
    }
}

CGS_PRIVATE CGS_StrView cgs__strv_fmutstr_ref2(const CGS__FixedMutStrRef str, unsigned int begin)
{
    unsigned int len = *str.len;

#ifndef CGS_NDEBUG
    if (begin > len)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {
        .len   = len - begin,
        .chars = str.chars + begin,
    };
}

CGS_PRIVATE CGS_StrView cgs__strv_fmutstr_ref3(const CGS__FixedMutStrRef str, unsigned int begin, unsigned int end)
{
    unsigned int len = *str.len;

#ifndef CGS_NDEBUG
    if (begin > len || end > len || begin > end)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {
        .len   = end - begin,
        .chars = str.chars + begin,
    };
}

CGS_API CGS_StrView cgs__strv_cstr3(const char *str, unsigned int begin, unsigned int end)
{
#ifndef CGS_NDEBUG

    const char *found_nul = memchr(str + begin, 0, end - begin);
    if (found_nul)
    {
        CGS_debug_break();
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {
        .len   = end - begin,
        .chars = (char *)str + begin,
    };
}

CGS_API CGS_StrView cgs__strv_ucstr3(const unsigned char *str, unsigned int begin, unsigned int end)
{
    return cgs__strv_cstr3((char *)str, begin, end);
}

CGS_API CGS_StrView cgs__strv_dstr_ptr3(const CGS_DStr *str, unsigned int begin, unsigned int end)
{
#ifndef CGS_NDEBUG
    if (begin > str->len || end > str->len || begin > end)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {.len = end - begin, .chars = str->chars + begin};
}

CGS_API CGS_StrView cgs__strv_strbuf_ptr3(const CGS_StrBuf *str, unsigned int begin, unsigned int end)
{
#ifndef CGS_NDEBUG
    if (begin > str->len || end > str->len || begin > end)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }
#endif

    return (CGS_StrView) {.len = end - begin, .chars = str->chars + begin};
}

CGS_API CGS_StrView cgs__strv_mutstr_ref3(CGS_MutStrRef str, unsigned int begin, unsigned int end)
{
    switch (str.ty)
    {
        case CGS__DSTR_TY:
            return cgs__strv_dstr_ptr3(str.str.dstr, begin, end);
        case CGS__STRBUF_TY:
            return cgs__strv_strbuf_ptr3(str.str.strbuf, begin, end);
        case CGS__BUF_TY:
            return cgs__strv_fmutstr_ref3(cgs__buf_as_fmutstr_ref(str.str.buf, &(unsigned int) {0}), begin, end);
        default:
            CGS_unreachable();
    }
}

CGS_API CGS_StrView cgs__strv_dstr3(CGS_DStr str, unsigned int begin, unsigned int end)
{
    return cgs__strv_dstr_ptr3(&str, begin, end);
}

CGS_API CGS_StrView cgs__strv_strv3(CGS_StrView str, unsigned int begin, unsigned int end)
{
    if (begin > str.len || end > str.len || begin > end)
    {
        return (CGS_StrView) {
            .len   = 0,
            .chars = NULL,
        };
    }

    return (CGS_StrView) {.len = end - begin, .chars = str.chars + begin};
}

CGS_API CGS_StrView cgs__strv_zstrv3(CGS_ZStrView str, unsigned int begin, unsigned int end)
{
    CGS_StrView strv = *(CGS_StrView *)&str;
    return cgs__strv_strv3(strv, begin, end);
}

CGS_API CGS_StrView cgs__strv_strbuf3(CGS_StrBuf str, unsigned int begin, unsigned int end)
{
    return cgs__strv_strbuf_ptr3(&str, begin, end);
}

CGS_PRIVATE CGS_ZStrView cgs__strv_to_zstrv(CGS_StrView sv)
{
    return *(CGS_ZStrView *)&sv;
}

CGS_API CGS_ZStrView cgs__zstrv_cstr1(const char *str)
{
    return cgs__strv_to_zstrv(cgs__strv_cstr1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_ucstr1(const unsigned char *str)
{
    return cgs__strv_to_zstrv(cgs__strv_ucstr1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_dstr1(CGS_DStr str)
{
    return cgs__strv_to_zstrv(cgs__strv_dstr1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_dstr_ptr1(const CGS_DStr *str)
{
    return cgs__strv_to_zstrv(cgs__strv_dstr_ptr1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_strv1(CGS_ZStrView str)
{
    return cgs__strv_to_zstrv(cgs__strv_zstrv1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_strbuf1(CGS_StrBuf str)
{
    return cgs__strv_to_zstrv(cgs__strv_strbuf1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_strbuf_ptr1(const CGS_StrBuf *str)
{
    return cgs__strv_to_zstrv(cgs__strv_strbuf_ptr1(str));
}
CGS_API CGS_ZStrView cgs__zstrv_mutstr_ref1(CGS_MutStrRef str)
{
    return cgs__strv_to_zstrv(cgs__strv_mutstr_ref1(str));
}

CGS_API CGS_ZStrView cgs__zstrv_cstr2(const char *str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_cstr2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_ucstr2(const unsigned char *str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_ucstr2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_dstr2(CGS_DStr str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_dstr2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_dstr_ptr2(const CGS_DStr *str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_dstr_ptr2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_strv2(CGS_ZStrView str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_zstrv2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_strbuf2(CGS_StrBuf str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_strbuf2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_strbuf_ptr2(const CGS_StrBuf *str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_strbuf_ptr2(str, begin));
}
CGS_API CGS_ZStrView cgs__zstrv_mutstr_ref2(CGS_MutStrRef str, unsigned int begin)
{
    return cgs__strv_to_zstrv(cgs__strv_mutstr_ref2(str, begin));
}

CGS_API CGS_Error cgs__dstr_fread_until(CGS_DStr *dstr, FILE *stream, int delim)
{
    dstr->len = 0;
    if (dstr->cap > 0)
    {
        dstr->chars[0] = '\0';
    }

    return cgs__dstr_append_fread_until(dstr, stream, delim);
}

CGS_API CGS_Error cgs__dstr_append_fread_until(CGS_DStr *dstr, FILE *stream, int delim)
{
    assert(delim == EOF || (delim >= 0 && delim <= 255));
    CGS_Error err = {CGS_OK};
    int c         = 256;
    while (c != EOF && c != delim)
    {
        err = cgs__dstr_maybe_grow(dstr, 64);
        if (err.ec != CGS_OK)
            return err;

        unsigned int count = 0;
        while (c != delim && count < 64 && (c = fgetc(stream)) != EOF)
        {
            char as_char                   = (char)c;
            dstr->chars[dstr->len + count] = as_char;
            count += 1;
        }
        dstr->len += count;
    }
    dstr->chars[dstr->len] = '\0';

    bool io_err          = ferror(stream);
    bool delim_not_found = c != delim;

    if (io_err)
        return (CGS_Error) {CGS_IO_ERROR};
    else if (delim_not_found)
        return (CGS_Error) {CGS_NOT_FOUND};
    else
        return err;
}

CGS_API CGS_Error cgs__fmutstr_ref_fread_until(CGS__FixedMutStrRef dst, FILE *stream, int delim)
{
    assert(delim == EOF || (delim >= 0 && delim <= 255));

    // TODO delim may be EOF, so dont ret here?
    if (dst.cap == 0)
    {
        return (CGS_Error) {CGS_DST_TOO_SMALL};
    }

    unsigned int len = 0;

    int c = 256;
    while (len < dst.cap - 1 && c != delim && (c = fgetc(stream)) != EOF)
    {
        dst.chars[len] = (char)c;
        len += 1;
    }

    dst.chars[len] = '\0';
    *dst.len       = len;

    bool io_err          = ferror(stream);
    bool dst_too_small   = (len == dst.cap - 1) && (c != delim) && (c != EOF);
    bool delim_not_found = c != delim;

    if (dst_too_small)
        return (CGS_Error) {CGS_DST_TOO_SMALL};
    else if (io_err)
        return (CGS_Error) {CGS_IO_ERROR};
    else if (delim_not_found)
        return (CGS_Error) {CGS_NOT_FOUND};
    else
        return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__mutstr_ref_fread_until(CGS_MutStrRef dst, FILE *stream, int delim)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_fread_until(dst.str.dstr, stream, delim);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_fread_until(cgs__strbuf_ptr_as_fmutstr_ref(dst.str.strbuf), stream, delim);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_fread_until(cgs__buf_as_fmutstr_ref(dst.str.buf, &(unsigned int) {0}), stream, delim);
        default:
            CGS_unreachable();
    };
}

CGS_API CGS_Error cgs__fmutstr_ref_append_fread_until(CGS__FixedMutStrRef dst, FILE *stream, int delim)
{
    if (dst.cap == 0 || dst.cap - 1 <= *dst.len)
        return (CGS_Error) {CGS_DST_TOO_SMALL};

    unsigned int appended_len = 0;

    CGS__FixedMutStrRef right = {.cap = dst.cap - *dst.len, .len = &appended_len, .chars = dst.chars + *dst.len};

    CGS_Error err = cgs__fmutstr_ref_fread_until(right, stream, delim);

    *dst.len += *right.len;

    dst.chars[*dst.len] = '\0';

    return err;
}

CGS_API CGS_Error cgs__mutstr_ref_append_fread_until(CGS_MutStrRef dst, FILE *stream, int delim)
{
    switch (dst.ty)
    {
        case CGS__DSTR_TY:
            return cgs__dstr_append_fread_until(dst.str.dstr, stream, delim);
        case CGS__STRBUF_TY:
            return cgs__fmutstr_ref_append_fread_until(cgs__strbuf_ptr_as_fmutstr_ref(dst.str.strbuf), stream, delim);
        case CGS__BUF_TY:
            return cgs__fmutstr_ref_append_fread_until(cgs__buf_as_fmutstr_ref(dst.str.buf, &(unsigned int) {0}), stream, delim);
        default:
            CGS_unreachable();
    };
}

CGS_API unsigned int cgs__fprint_strv(FILE *stream, CGS_StrView str)
{
    if (str.chars == NULL && str.len == 0)
        return 0;
    return (unsigned int)fwrite(str.chars, sizeof(unsigned char), str.len, stream);
}

CGS_API unsigned int cgs__fprintln_strv(FILE *stream, CGS_StrView str)
{
    unsigned int written = (unsigned int)fwrite(str.chars, sizeof(unsigned char), str.len, stream);
    int err              = fputc('\n', stream);

    return written + (err != EOF);
}

CGS_API CGS_Error cgs__idstr_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_DStr *dstr = ((CGS_DStrWriter *)dst)->dstr;
    return cgs__dstr_append(dstr, str);
}

CGS_API CGS_Error cgs__istrbuf_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_StrBuf *strbuf = ((CGS_StrBufWriter *)dst)->strbuf;
    return cgs__fmutstr_ref_append(cgs__fmutstr_ref(strbuf), str);
}

CGS_API CGS_Error cgs__ibuf_append(CGS_Writer *dst, CGS_StrView str)
{
    CGS_Buffer buf = ((CGS_CStrWriter *)dst)->buf;
    return cgs__fmutstr_ref_append(cgs__fmutstr_ref(buf), str);
}

CGS_API CGS_Error cgs__file_append(CGS_Writer *dst, CGS_StrView str)
{
    FILE *f          = ((CGS_FileWriter *)dst)->file;
    unsigned int ret = cgs__fprint_strv(f, str);
    if (ret != str.len)
        return (CGS_Error) {CGS_IO_ERROR};
    return (CGS_Error) {CGS_OK};
}

CGS_PRIVATE CGS_Error cgs__parse_optional_paren_grouping(CGS_ZStrView *fmt_walk);

CGS_PRIVATE CGS_Error cgs__parse_optional_square_grouping(CGS_ZStrView *fmt_walk)
{
    while (fmt_walk->len != 0 && fmt_walk->chars[0] != ']')
    {
        if (fmt_walk->chars[0] == '(')
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
            CGS_Error err = cgs__parse_optional_paren_grouping(fmt_walk);
            if (err.ec != CGS_OK)
                return err;
        }
        else if (fmt_walk->chars[0] == '[')
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
            CGS_Error err = cgs__parse_optional_square_grouping(fmt_walk);
            if (err.ec != CGS_OK)
                return err;
        }
        else if (fmt_walk->chars[0] == ')')
        {
            return (CGS_Error) {CGS_BAD_FORMAT};
        }
        else
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
        }
    }
    if (fmt_walk->len == 0)
    {
        return (CGS_Error) {CGS_BAD_FORMAT};
    }
    else
    {
        fmt_walk->chars += 1;
        fmt_walk->len -= 1;
    }
    return (CGS_Error) {CGS_OK};
}

CGS_PRIVATE CGS_Error cgs__parse_optional_paren_grouping(CGS_ZStrView *fmt_walk)
{
    while (fmt_walk->len != 0 && fmt_walk->chars[0] != ')')
    {
        if (fmt_walk->chars[0] == '(')
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
            CGS_Error err = cgs__parse_optional_paren_grouping(fmt_walk);
            if (err.ec != CGS_OK)
                return err;
        }
        else if (fmt_walk->chars[0] == '[')
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
            CGS_Error err = cgs__parse_optional_square_grouping(fmt_walk);
            if (err.ec != CGS_OK)
                return err;
        }
        else if (fmt_walk->chars[0] == ']')
        {
            return (CGS_Error) {CGS_BAD_FORMAT};
        }
        else
        {
            fmt_walk->chars += 1;
            fmt_walk->len -= 1;
        }
    }
    if (fmt_walk->len == 0)
    {
        return (CGS_Error) {CGS_BAD_FORMAT};
    }
    else
    {
        fmt_walk->chars += 1;
        fmt_walk->len -= 1;
    }
    return (CGS_Error) {CGS_OK};
}

CGS_PRIVATE CGS_Error cgs__parse_optional_format_string(CGS_ZStrView *fmt_walk, CGS_StrView *fmt_arg)
{
    assert(fmt_walk->chars[0] == '(');
    fmt_walk->chars += 1;
    fmt_walk->len -= 1;

    fmt_arg->chars = (char *)fmt_walk->chars;
    CGS_Error err  = cgs__parse_optional_paren_grouping(fmt_walk);
    if (err.ec != CGS_OK)
        return err;
    fmt_arg->len = (unsigned int)((fmt_walk->chars - 1) - fmt_arg->chars);
    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__append_fmt(
    CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error (*tostr_p_funcs[])(CGS_Writer *, const void *, CGS_StrView fmt_arg)
)
{
    // This should proably be:
    // do first format specififer, determine mode from it,
    // and have one outer branch.
    enum
    {
        UNKNOWN_INDEXING,
        AUTO_INDEX,
        SPECIFY_INDEX
    } index_mode = UNKNOWN_INDEXING; // SPECIFY_INDEX is "%index" (e.g. "%0" is the first arg). AUTO_INDEX requires "%?", cannot mix

    CGS_ZStrView fmt_walk = fmt;

    size_t how_many_formatted = 0;
    CGS_Error err             = {CGS_OK};
    while (fmt_walk.len != 0 && err.ec == CGS_OK)
    {
        const char *found   = memchr(fmt_walk.chars, '%', fmt_walk.len);
        CGS_StrView fmt_arg = {};
        if (found)
        {
            unsigned int index = (unsigned int)(found - fmt_walk.chars);
            CGS_StrView chunk  = {.chars = (char *)fmt_walk.chars, .len = index};
            fmt_walk.chars += (index + 1);
            fmt_walk.len -= (index + 1);

            // '%' without espace "%%" or without format "%?" is rejected
            if (fmt_walk.len == 0)
            {
                CGS_debug_break();
                err.ec = CGS_BAD_FORMAT;
                break;
            }

            if (fmt_walk.len > 0 && found[1] == '?')
            {
                // skip the '?'
                fmt_walk.chars += 1;
                fmt_walk.len -= 1;

            auto_arg:
                if (index_mode == SPECIFY_INDEX)
                {
                    CGS_debug_break(); // cannot change arg indexing mode. either all formats use index, or all automatic index
                    err.ec = CGS_BAD_FORMAT;
                    break;
                }
                index_mode = AUTO_INDEX;

                err = cgs__invoke_writer(dst, chunk);

                if (how_many_formatted >= nargs)
                {
                    CGS_debug_break(); // not enough format args
                    err.ec = CGS_NOT_ENOUGH_ARGS;
                    break;
                }
                err = tostr_p_funcs[how_many_formatted](dst, args[how_many_formatted], fmt_arg);
                how_many_formatted += 1;
            }
            else if (fmt_walk.len > 0 && found[1] == '[')
            {
                if (index_mode == AUTO_INDEX)
                {
                    CGS_debug_break(); // cannot change arg indexing mode. either all formats use index, or all automatic index
                    err.ec = CGS_BAD_FORMAT;
                    break;
                }
                index_mode = SPECIFY_INDEX;

                char *end               = NULL;
                unsigned long arg_index = strtoul(found + 2, &end, 10); // we can assume fmt is null terminated

                unsigned int end_idx = (unsigned int)(end - fmt_walk.chars);
                fmt_walk.chars += end_idx;
                fmt_walk.len -= end_idx;

                if (fmt_walk.len == 0)
                {
                    CGS_debug_break();
                    err.ec = CGS_BAD_FORMAT;
                    break;
                }

                if (fmt_walk.chars[0] == '(')
                {
                    err = cgs__parse_optional_format_string(&fmt_walk, &fmt_arg);
                    if (err.ec != CGS_OK)
                        break;
                }

                // skip the ]
                fmt_walk.len -= 1;
                fmt_walk.chars += 1;

                if (arg_index >= nargs)
                {
                    CGS_debug_break(); // not enough format args
                    err.ec = CGS_INDEX_OUT_OF_BOUNDS;
                    break;
                }

                err = cgs__invoke_writer(dst, chunk);
                err = tostr_p_funcs[arg_index](dst, args[arg_index], fmt_arg);
            }
            else if (fmt_walk.len > 0 && found[1] == '%')
            {
                chunk.len += 1; // to include the first %
                fmt_walk.chars += 1;
                fmt_walk.len -= 1;
                err = cgs__invoke_writer(dst, chunk);
            }
            else if (fmt_walk.len > 0 && found[1] == '(')
            {
                err = cgs__parse_optional_format_string(&fmt_walk, &fmt_arg);
                if (err.ec != CGS_OK)
                    break;

                goto auto_arg;
            }
            else
            {
                CGS_debug_break(); // lone percent
                err.ec = CGS_BAD_FORMAT;
                break;
            }
        }
        else
        {
            err = cgs__invoke_writer(dst, cgs__strv_zstrv1(fmt_walk));
            fmt_walk.chars += fmt_walk.len;
            fmt_walk.len = 0;
        }
    }

    if (err.ec == CGS_OK && how_many_formatted < nargs && index_mode != SPECIFY_INDEX)
    {
        return (CGS_Error) {CGS_TOO_MANY_ARGS};
    }
    return err;
}

CGS_API CGS_Error cgs__appendln_fmt_(
    CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error (*tostr_p_funcs[])(CGS_Writer *, const void *, CGS_StrView fmt_arg)
)
{
    CGS_Error err = cgs__append_fmt(dst, fmt, nargs, args, tostr_p_funcs);
    if (err.ec == CGS_OK)
        err = cgs__invoke_writer(dst, cgs_strv("\n"));

    return err;
}

CGS_API CGS_DStr cgs__asprintf(
    CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error (*tostr_p_funcs[])(CGS_Writer *, const void *, CGS_StrView fmt_arg)
)
{
    cgs__append_fmt(dst, fmt, nargs, args, tostr_p_funcs);
    return *((CGS_DStrWriter *)dst)->dstr;
}

CGS_API CGS_DStr cgs__asprintf_with_allocator(
    CGS_Writer *dst, CGS_ZStrView fmt, size_t nargs, void **args, CGS_Error (*tostr_p_funcs[])(CGS_Writer *, const void *, CGS_StrView fmt_arg)
)
{
    // shifting the pointers by 1 to skip the fmt arg
    cgs__append_fmt(dst, fmt, nargs - 1, args + 1, tostr_p_funcs + 1);
    return *((CGS_DStrWriter *)dst)->dstr;
}

CGS_PRIVATE unsigned int cgs__numstr_len(unsigned long long num)
{
    unsigned int len = 1;
    for (unsigned int i = 1; i < CGS__CARR_LEN(cgs__ten_pows_ull) && num >= cgs__ten_pows_ull[i]; len++)
        i++;
    return len;
}

#define cgs__sinteger_min(ty) _Generic((ty) {0}, signed char: SCHAR_MIN, short: SHRT_MIN, int: INT_MIN, long: LONG_MIN, long long: LLONG_MIN)

#define cgs__min_tostr(ty)                \
    _Generic(                             \
        (ty) {0},                         \
        signed char: cgs__schar_min_into, \
        short: cgs__short_min_into,       \
        int: cgs__int_min_into,           \
        long: cgs__long_min_into,         \
        long long: cgs__llong_min_into    \
    )

CGS_PRIVATE CGS_Error cgs__schar_min_into(CGS_Writer *dst)
{
    if (SCHAR_MIN == (-127 - 1))
    {
        const char *numstr = "-128";
        CGS_StrView s      = {.chars = (char *)numstr, .len = (unsigned int)strlen(numstr)};
        return cgs__invoke_writer(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len       = snprintf(temp, sizeof(temp), "%hhd", SCHAR_MIN);
        return cgs__invoke_writer(
            dst,
            (CGS_StrView) {
                .chars = (char *)temp,
                .len   = (unsigned int)len,
            }
        );
    }
}

CGS_PRIVATE CGS_Error cgs__short_min_into(CGS_Writer *dst)
{
    if (SHRT_MIN == (-32767 - 1))
    {
        const char *numstr = "-32768";
        CGS_StrView s      = {.chars = (char *)numstr, .len = (unsigned int)strlen(numstr)};
        return cgs__invoke_writer(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len       = snprintf(temp, sizeof(temp), "%hd", SHRT_MIN);
        return cgs__invoke_writer(
            dst,
            (CGS_StrView) {
                .chars = (char *)temp,
                .len   = (unsigned int)len,
            }
        );
    }
}

CGS_PRIVATE CGS_Error cgs__int_min_into(CGS_Writer *dst)
{
    if (INT_MIN == (-2147483647 - 1))
    {
        const char *numstr = "-2147483648";
        CGS_StrView s      = {.chars = (char *)numstr, .len = (unsigned int)strlen(numstr)};
        return cgs__invoke_writer(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len       = snprintf(temp, sizeof(temp), "%d", INT_MIN);
        return cgs__invoke_writer(
            dst,
            (CGS_StrView) {
                .chars = (char *)temp,
                .len   = (unsigned int)len,
            }
        );
    }
}

CGS_PRIVATE CGS_Error cgs__long_min_into(CGS_Writer *dst)
{
    if (LONG_MIN == INT_MIN)
    {
        return cgs__int_min_into(dst);
    }
    else if (LONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        CGS_StrView s      = {.chars = (char *)numstr, .len = (unsigned int)strlen(numstr)};
        return cgs__invoke_writer(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len       = snprintf(temp, sizeof(temp), "%ld", LONG_MIN);
        return cgs__invoke_writer(
            dst,
            (CGS_StrView) {
                .chars = (char *)temp,
                .len   = (unsigned int)len,
            }
        );
    }
}

CGS_PRIVATE CGS_Error cgs__llong_min_into(CGS_Writer *dst)
{
    if (LLONG_MIN == LONG_MIN)
    {
        return cgs__long_min_into(dst);
    }
    else if (LLONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        CGS_StrView s      = {.chars = (char *)numstr, .len = (unsigned int)strlen(numstr)};
        return cgs__invoke_writer(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len       = snprintf(temp, sizeof(temp), "%lld", LLONG_MIN);
        return cgs__invoke_writer(
            dst,
            (CGS_StrView) {
                .chars = (char *)temp,
                .len   = (unsigned int)len,
            }
        );
    }
}

#define cgs__buf_size_for_integer_type(ty) _Generic((char (*)[sizeof(ty)])0, char (*)[1]: 4, char (*)[2]: 8, char (*)[4]: 16, char (*)[8]: 32)

#define cgs__sinteger_tostr()                                                   \
    do                                                                          \
    {                                                                           \
        if (obj == cgs__sinteger_min(__typeof__(obj)))                          \
        {                                                                       \
            return cgs__min_tostr(__typeof__(obj))(dst);                        \
        }                                                                       \
        bool isneg = false;                                                     \
        if (obj < 0)                                                            \
        {                                                                       \
            isneg = true;                                                       \
            obj *= -1;                                                          \
        }                                                                       \
        unsigned int numlen = cgs__numstr_len((unsigned long long)obj);         \
        char cgs__tmp_buf[cgs__buf_size_for_integer_type(__typeof__(obj))];     \
        if (numlen >= sizeof(cgs__tmp_buf))                                     \
            CGS_unreachable();                                                  \
                                                                                \
        if (isneg)                                                              \
        {                                                                       \
            cgs__tmp_buf[0] = '-';                                              \
        }                                                                       \
                                                                                \
        for (unsigned int i = 0; i < numlen; i++)                               \
        {                                                                       \
            unsigned char rem                      = (unsigned char)(obj % 10); \
            obj                                    = obj / 10;                  \
            cgs__tmp_buf[isneg + numlen - (i + 1)] = (char)(rem + '0');         \
        }                                                                       \
        return cgs__invoke_writer(                                              \
            dst,                                                                \
            (CGS_StrView) {                                                     \
                .chars = cgs__tmp_buf,                                          \
                .len   = numlen + isneg,                                        \
            }                                                                   \
        );                                                                      \
    } while (0)

#define cgs__uinteger_tostr()                                               \
    do                                                                      \
    {                                                                       \
        unsigned int numlen = cgs__numstr_len(obj);                         \
        char cgs__tmp_buf[cgs__buf_size_for_integer_type(__typeof__(obj))]; \
        if (numlen >= sizeof(cgs__tmp_buf))                                 \
            CGS_unreachable();                                              \
                                                                            \
        for (unsigned int i = 0; i < numlen; i++)                           \
        {                                                                   \
            unsigned char rem = (unsigned char)(obj % 10);                  \
            obj               = obj / 10;                                   \
                                                                            \
            cgs__tmp_buf[numlen - (i + 1)] = (char)(rem + '0');             \
        }                                                                   \
        return cgs__invoke_writer(                                          \
            dst,                                                            \
            (CGS_StrView) {                                                 \
                .chars = cgs__tmp_buf,                                      \
                .len   = numlen,                                            \
            }                                                               \
        );                                                                  \
    } while (0)

CGS_API CGS_Error cgs__bool_tostr(CGS_Writer *dst, bool obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    CGS_StrView res = obj ? cgs__strv_cstr1("true") : cgs__strv_cstr1("false");
    return cgs__invoke_writer(dst, res);
}

CGS_API CGS_Error cgs__cstr_tostr(CGS_Writer *dst, const char *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = (char *)obj,
            .len   = (unsigned int)strlen(obj),
        }
    );
}

CGS_API CGS_Error cgs__ucstr_tostr(CGS_Writer *dst, const unsigned char *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = (char *)obj,
            .len   = (unsigned int)strlen((char *)obj),
        }
    );
}

CGS_API CGS_Error cgs__char_tostr(CGS_Writer *dst, char obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = &obj,
            .len   = 1,
        }
    );
}

CGS_API CGS_Error cgs__schar_tostr(CGS_Writer *dst, signed char obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, cgs__sc_to_string[(unsigned char)obj]);
}

CGS_API CGS_Error cgs__uchar_tostr(CGS_Writer *dst, unsigned char obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = (char *)&obj,
            .len   = 1,
        }
    );
}

CGS_API CGS_Error cgs__short_tostr(CGS_Writer *dst, short obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__sinteger_tostr();
}

CGS_API CGS_Error cgs__ushort_tostr(CGS_Writer *dst, unsigned short obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__uinteger_tostr();
}

CGS_API CGS_Error cgs__int_tostr(CGS_Writer *dst, int obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__sinteger_tostr();
}

CGS_API CGS_Error cgs__uint_tostr(CGS_Writer *dst, unsigned int obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__uinteger_tostr();
}

CGS_API CGS_Error cgs__long_tostr(CGS_Writer *dst, long obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__sinteger_tostr();
}

CGS_API CGS_Error cgs__ulong_tostr(CGS_Writer *dst, unsigned long obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__uinteger_tostr();
}

CGS_API CGS_Error cgs__llong_tostr(CGS_Writer *dst, long long obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__sinteger_tostr();
}

CGS_API CGS_Error cgs__ullong_tostr(CGS_Writer *dst, unsigned long long obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    cgs__uinteger_tostr();
}

CGS_API CGS_Error cgs__float_tostr(CGS_Writer *dst, float obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    char tmp[32]    = {0};
    int len         = snprintf(tmp, sizeof(tmp), "%g", obj);
    CGS_StrView str = {.chars = (char *)tmp, .len = (unsigned int)len};
    return cgs__invoke_writer(dst, str);
}

CGS_API CGS_Error cgs__double_tostr(CGS_Writer *dst, double obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    char tmp[32]    = {0};
    int len         = snprintf(tmp, sizeof(tmp), "%g", obj);
    CGS_StrView str = {.chars = (char *)tmp, .len = (unsigned int)len};
    return cgs__invoke_writer(dst, str);
}

CGS_API CGS_Error cgs__error_tostr(CGS_Writer *dst, CGS_Error obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, cgs__error_to_string[obj.ec]);
}

CGS_API CGS_Error cgs__array_fmt_tostr(CGS_Writer *dst, CGS_ArrayFmt obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    CGS_Error err;

    cgs__invoke_writer(dst, obj.open);

    const uint8_t *arr = obj.array;

    if (obj.nb > 0)
    {
        for (size_t i = 0; i < obj.nb - 1; i++)
        {
            obj.elm_tostr(dst, arr + (obj.elm_size * i), fmt_arg);
            cgs__invoke_writer(dst, obj.separator);
        }

        obj.elm_tostr(dst, arr + (obj.elm_size * (obj.nb - 1)), fmt_arg);

        cgs__invoke_writer(dst, obj.trailing_separator);
    }

    err = cgs__invoke_writer(dst, obj.close);

    return err;
}

CGS_API CGS_Error cgs__align_fmt_tostr(CGS_Writer *dst, CGS__AlignFmt obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    // TODO for better perf, have a local 32 byte array so it can be filled fill char, to make as least dst invokes as possible.
    unsigned int len = cgs__invoke_tostr_len(obj.tostr_p, obj.obj);

    if (len < obj.width)
    {
        const CGS_StrView fill_strv = {&obj.fill_char, .len = 1};
        unsigned int diff           = obj.width - len;
        CGS_Error err               = {CGS_OK};

        switch (obj.align_mode.align_mode)
        {
            case CGS__ALIGNMODE_CENTER:
            {
                unsigned int half_diff = diff / 2;
                for (unsigned int i = 0; i < half_diff && err.ec == CGS_OK; i++)
                {
                    err = cgs__invoke_writer(dst, fill_strv);
                }

                if (err.ec != CGS_OK)
                    return err;

                err = obj.tostr_p(dst, obj.obj, fmt_arg);
                if (err.ec != CGS_OK)
                    return err;

                unsigned int remaining_diff = diff - half_diff;
                for (unsigned int i = 0; i < remaining_diff && err.ec == CGS_OK; i++)
                {
                    err = cgs__invoke_writer(dst, fill_strv);
                }

                return err;
            }
            case CGS__ALIGNMODE_LEFT:
            {
                err = obj.tostr_p(dst, obj.obj, fmt_arg);
                if (err.ec != CGS_OK)
                    return err;

                for (unsigned int i = 0; i < diff && err.ec == CGS_OK; i++)
                {
                    err = cgs__invoke_writer(dst, fill_strv);
                }

                return err;
            }
            case CGS__ALIGNMODE_RIGHT:
            {
                for (unsigned int i = 0; i < diff && err.ec == CGS_OK; i++)
                {
                    err = cgs__invoke_writer(dst, fill_strv);
                }

                if (err.ec != CGS_OK)
                    return err;

                err = obj.tostr_p(dst, obj.obj, fmt_arg);

                return err;
            }

            default:
                CGS_unreachable();
        }
    }
    else
    {
        return obj.tostr_p(dst, obj.obj, fmt_arg);
    }
}

CGS_API CGS_Error cgs__repeat_fmt_tostr(CGS_Writer *dst, CGS__RepeatFmt obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    for (unsigned int i = 0; i < obj.n; i++)
    {
        CGS_Error err;
        if ((err = obj.tostr_p(dst, obj.obj, fmt_arg), err.ec != CGS_OK))
            return err;
    }
    return (CGS_Error) {CGS_OK};
}

CGS_API CGS_Error cgs__dstr_tostr(CGS_Writer *dst, const CGS_DStr obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = obj.chars,
            .len   = obj.len,
        }
    );
}

CGS_API CGS_Error cgs__dstr_ptr_tostr(CGS_Writer *dst, const CGS_DStr *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = obj->chars,
            .len   = obj->len,
        }
    );
}

CGS_API CGS_Error cgs__strv_tostr(CGS_Writer *dst, CGS_StrView obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, obj);
}

CGS_API CGS_Error cgs__zstrv_tostr(CGS_Writer *dst, CGS_ZStrView obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, cgs__strv_zstrv1(obj));
}

CGS_API CGS_Error cgs__strbuf_tostr(CGS_Writer *dst, const CGS_StrBuf obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = obj.chars,
            .len   = obj.len,
        }
    );
}

CGS_API CGS_Error cgs__strbuf_ptr_tostr(CGS_Writer *dst, const CGS_StrBuf *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(
        dst,
        (CGS_StrView) {
            .chars = obj->chars,
            .len   = obj->len,
        }
    );
}

CGS_API CGS_Error cgs__mutstr_ref_tostr(CGS_Writer *dst, const CGS_MutStrRef obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, cgs__strv_mutstr_ref2(obj, 0));
}

CGS_PRIVATE CGS_Error cgs__uchar_d_tostr(CGS_Writer *dst, unsigned char obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__invoke_writer(dst, cgs__uc_to_string[obj]);
}

#define cgs__if_else(cond, then, else) _Generic((char (*)[(cond) + 1])0, char (*)[1]: else, char (*)[2]: (then))

#define cgs__unsigned_of_size(sz) \
    __typeof__(_Generic((char (*)[sz])0, char (*)[1]: (uint8_t)0, char (*)[2]: (uint16_t)0, char (*)[4]: (uint32_t)0, char (*)[8]: (uint64_t)0))

#define cgs__integer_d_Fmt_tostr(dst, num, fmt_arg)                                                                                                                                                                                                                                                                                                                                                             \
    return _Generic(num, char: cgs__if_else(CHAR_MIN < 0, cgs__schar_tostr, cgs__uchar_d_tostr), signed char: cgs__schar_tostr, unsigned char: cgs__uchar_d_tostr, short: cgs__short_tostr, unsigned short: cgs__ushort_tostr, int: cgs__int_tostr, unsigned int: cgs__uint_tostr, long: cgs__long_tostr, unsigned long: cgs__ulong_tostr, long long: cgs__llong_tostr, unsigned long long: cgs__ullong_tostr)( \
        dst, num, fmt_arg                                                                                                                                                                                                                                                                                                                                                                                       \
    )

#define cgs__integer_x_Fmt_tostr(dst, num, byte2hex)                                      \
    do                                                                                    \
    {                                                                                     \
        CGS_Error err      = {CGS_OK};                                                    \
        size_t sz          = sizeof(num);                                                 \
        uint8_t *num_bytes = ((uint8_t *)&(num)) + sizeof(num) - 1;                       \
        bool zero_pad      = true;                                                        \
        while (sz--)                                                                      \
        {                                                                                 \
            if (!zero_pad || num_bytes == (uint8_t *)&(num) || *num_bytes != 0)           \
            {                                                                             \
                CGS_StrView hex_sv = {.chars = (char *)(byte2hex)[*num_bytes], .len = 2}; \
                if (zero_pad && hex_sv.chars[0] == '0')                                   \
                {                                                                         \
                    hex_sv.chars += 1;                                                    \
                    hex_sv.len -= 1;                                                      \
                }                                                                         \
                zero_pad = false;                                                         \
                err      = cgs__invoke_writer(dst, hex_sv);                               \
            }                                                                             \
            num_bytes -= 1;                                                               \
        }                                                                                 \
        return err;                                                                       \
    } while (0)

#define cgs__highest_3bits(n) ((__typeof__(n))((n) & (((__typeof__(n))0b111) << ((sizeof(n) * 8) - 3))))

#define cgs__highest_3bits_as_u8(n) (uint8_t)(cgs__highest_3bits(n) >> ((sizeof(n) - 1) * 8))

#define cgs__integer_o_Fmt_tostr(dst, num)                                                       \
    do                                                                                           \
    {                                                                                            \
        cgs__unsigned_of_size(sizeof(num)) unum = (__typeof__(unum))(num);                       \
        CGS_Error err                           = {CGS_OK};                                      \
        const int bits                          = (int)(sizeof(unum) * 8);                       \
        int iters                               = bits / 3;                                      \
        uint8_t extra_bits                      = 3 - ((sizeof(unum) * 8) % 3);                  \
        uint8_t high3                           = cgs__highest_3bits_as_u8(unum);                \
        uint8_t first_3bits                     = high3 >> extra_bits;                           \
        first_3bits                             = first_3bits >> 5;                              \
        bool zero_pad                           = true;                                          \
        if (first_3bits != 0)                                                                    \
        {                                                                                        \
            zero_pad             = false;                                                        \
            CGS_StrView octal_sv = {                                                             \
                .chars = &(char) {(char)('0' + first_3bits)},                                    \
                .len   = 1,                                                                      \
            };                                                                                   \
            err = cgs__invoke_writer(dst, octal_sv);                                             \
        }                                                                                        \
        unum = (__typeof__(unum))(unum << (3 - extra_bits));                                     \
                                                                                                 \
        for (int i = 0; i < iters; i++)                                                          \
        {                                                                                        \
            high3       = cgs__highest_3bits_as_u8(unum);                                        \
            first_3bits = high3 >> (8 - 3);                                                      \
            if (i == (iters - 1) || !zero_pad || (first_3bits != 0))                             \
            {                                                                                    \
                zero_pad             = false;                                                    \
                CGS_StrView octal_sv = {.chars = &(char) {(char)('0' + first_3bits)}, .len = 1}; \
                err                  = cgs__invoke_writer(dst, octal_sv);                        \
            }                                                                                    \
            unum = (__typeof__(unum))(unum << 3);                                                \
        }                                                                                        \
        return err;                                                                              \
    } while (0)

#define cgs__integer_b_Fmt_tostr(dst, num)                                          \
    do                                                                              \
    {                                                                               \
        CGS_Error err                           = {CGS_OK};                         \
        cgs__unsigned_of_size(sizeof(num)) unum = (__typeof__(unum))(num);          \
        size_t sz                               = sizeof(unum) * 8;                 \
        unsigned int written                    = 0;                                \
        unsigned int counter                    = (unsigned int)(sizeof(unum) * 8); \
        bool zero_pad                           = true;                             \
        while (written < sz && counter != 0)                                        \
        {                                                                           \
            bool bit = unum & (((__typeof__(unum))1) << ((sizeof(unum) * 8) - 1));  \
            if (bit)                                                                \
            {                                                                       \
                zero_pad            = false;                                        \
                CGS_StrView bit_str = {.chars = &(char) {'1'}, .len = 1};           \
                err                 = cgs__invoke_writer(dst, bit_str);             \
            }                                                                       \
            else if (!zero_pad || counter == 1)                                     \
            {                                                                       \
                CGS_StrView bit_str = {.chars = &(char) {'0'}, .len = 1};           \
                err                 = cgs__invoke_writer(dst, bit_str);             \
            }                                                                       \
            unum = (__typeof__(unum))(unum << 1);                                   \
            counter -= 1;                                                           \
        }                                                                           \
        return err;                                                                 \
    } while (0)

// clang-format off
#define CGS__X(ty, extra)                                                                                                \
    CGS_API CGS_Error cgs__Integer_d_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_d_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        cgs__integer_d_Fmt_tostr(dst, obj.obj, fmt_arg);                                                                 \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_x_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_x_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        cgs__integer_x_Fmt_tostr(dst, obj.obj, cgs__byte_to_hex);                                                        \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_o_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_o_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        cgs__integer_o_Fmt_tostr(dst, obj.obj);                                                                          \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_b_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_b_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        cgs__integer_b_Fmt_tostr(dst, obj.obj);                                                                          \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_X_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Integer_X_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        cgs__integer_x_Fmt_tostr(dst, obj.obj, cgs__byte_to_heX);                                                        \
    }                                                                                                                    \
                                                                                                                         \
    CGS_API CGS_Error cgs__Integer_d_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)           \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        return cgs__Integer_d_Fmt_##ty##_tostr(dst, *(CGS__Integer_d_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_x_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)           \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        return cgs__Integer_x_Fmt_##ty##_tostr(dst, *(CGS__Integer_x_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_o_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)           \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        return cgs__Integer_o_Fmt_##ty##_tostr(dst, *(CGS__Integer_o_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_b_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)           \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        return cgs__Integer_b_Fmt_##ty##_tostr(dst, *(CGS__Integer_b_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                    \
    CGS_API CGS_Error cgs__Integer_X_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)           \
    {                                                                                                                    \
        (void)fmt_arg;                                                                                                   \
        return cgs__Integer_X_Fmt_##ty##_tostr(dst, *(CGS__Integer_X_Fmt_##ty *)obj, fmt_arg);                           \
    }
// clang-format on

CGS__INTEGER_TYPES(CGS__X, ignore)

#undef CGS__X

#define cgs__floating_fmt(fmt_strlit)                                \
    char buf[64];                                                    \
    int len = snprintf(buf, 64, fmt_strlit, obj.precision, obj.obj); \
    if (len < 0)                                                     \
        return (CGS_Error) {CGS_ENCODING_ERROR};                     \
    return cgs__invoke_writer(                                       \
        dst,                                                         \
        (CGS_StrView) {                                              \
            .chars = buf,                                            \
            .len   = (unsigned int)len,                              \
        }                                                            \
    );

// clang-format off
#define CGS__X(ty, extra)                                                                                                  \
    CGS_API CGS_Error cgs__Floating_f_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_f_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*f")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_g_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_g_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*g")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_e_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_e_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*e")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_a_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_a_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*a")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_F_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_F_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*F")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_G_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_G_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*G")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_E_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_E_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*E")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_A_Fmt_##ty##_tostr(CGS_Writer *dst, CGS__Floating_A_Fmt_##ty obj, CGS_StrView fmt_arg) \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        cgs__floating_fmt("%.*A")                                                                                          \
    }                                                                                                                      \
                                                                                                                           \
    CGS_API CGS_Error cgs__Floating_f_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_f_Fmt_##ty##_tostr(dst, *(CGS__Floating_f_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_g_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_g_Fmt_##ty##_tostr(dst, *(CGS__Floating_g_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_e_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_e_Fmt_##ty##_tostr(dst, *(CGS__Floating_e_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_a_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_a_Fmt_##ty##_tostr(dst, *(CGS__Floating_a_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_F_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_F_Fmt_##ty##_tostr(dst, *(CGS__Floating_F_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_G_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_G_Fmt_##ty##_tostr(dst, *(CGS__Floating_G_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_E_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_E_Fmt_##ty##_tostr(dst, *(CGS__Floating_E_Fmt_##ty *)obj, fmt_arg);                           \
    }                                                                                                                      \
    CGS_API CGS_Error cgs__Floating_A_Fmt_##ty##_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)            \
    {                                                                                                                      \
        (void)fmt_arg;                                                                                                     \
        return cgs__Floating_A_Fmt_##ty##_tostr(dst, *(CGS__Floating_A_Fmt_##ty *)obj, fmt_arg);                           \
    }
// clang-format on

CGS__FLOATING_TYPES(CGS__X, ignore, CGS__X)

#undef CGS__X

CGS_API CGS_Error cgs__bool_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__bool_tostr(dst, *(bool *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__cstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__cstr_tostr(dst, *(char **)obj, fmt_arg);
}
CGS_API CGS_Error cgs__ucstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__ucstr_tostr(dst, *(unsigned char **)obj, fmt_arg);
}
CGS_API CGS_Error cgs__char_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__char_tostr(dst, *(char *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__schar_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__schar_tostr(dst, *(signed char *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__uchar_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__uchar_tostr(dst, *(unsigned char *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__short_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__short_tostr(dst, *(short *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__ushort_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__ushort_tostr(dst, *(unsigned short *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__int_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__int_tostr(dst, *(int *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__uint_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__uint_tostr(dst, *(unsigned int *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__long_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__long_tostr(dst, *(long *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__ulong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__ulong_tostr(dst, *(unsigned long *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__llong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__llong_tostr(dst, *(long long *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__ullong_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__ullong_tostr(dst, *(unsigned long long *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__float_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__float_tostr(dst, *(float *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__double_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__double_tostr(dst, *(double *)obj, fmt_arg);
}

CGS_API CGS_Error cgs__dstr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__dstr_tostr(dst, *(CGS_DStr *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__dstr_ptr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__dstr_ptr_tostr(dst, *(CGS_DStr **)obj, fmt_arg);
}
CGS_API CGS_Error cgs__strv_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__strv_tostr(dst, *(CGS_StrView *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__zstrv_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__zstrv_tostr(dst, *(CGS_ZStrView *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__strbuf_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__strbuf_tostr(dst, *(CGS_StrBuf *)obj, fmt_arg);
}
CGS_API CGS_Error cgs__strbuf_ptr_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__strbuf_ptr_tostr(dst, *(CGS_StrBuf **)obj, fmt_arg);
}
CGS_API CGS_Error cgs__mutstr_ref_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__mutstr_ref_tostr(dst, *(CGS_MutStrRef *)obj, fmt_arg);
}

CGS_API CGS_Error cgs__error_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__error_tostr(dst, *(CGS_Error *)obj, fmt_arg);
}

CGS_API CGS_Error cgs__array_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__array_fmt_tostr(dst, *(CGS_ArrayFmt *)obj, fmt_arg);
}

CGS_API CGS_Error cgs__align_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__align_fmt_tostr(dst, *(CGS__AlignFmt *)obj, fmt_arg);
}

CGS_API CGS_Error cgs__repeat_fmt_tostr_p(CGS_Writer *dst, const void *obj, CGS_StrView fmt_arg)
{
    (void)fmt_arg;
    return cgs__repeat_fmt_tostr(dst, *(CGS__RepeatFmt *)obj, fmt_arg);
}

#endif // CGS__STR_C_INCLUDED

#endif // CGS_IMPL
#undef CGS_IMPL
