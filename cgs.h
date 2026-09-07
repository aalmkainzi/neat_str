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
cgs__strv_equal(cgs_strv(anystr1), cgs_strv(anystr2))

#define cgs_dup(anystr_src, ...) \
cgs__dstr_init_from(cgs_strv(anystr_src), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

#define cgs_copy(mutstr_dst, anystr_src) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_copy(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs_strv(anystr_src)), \
    CGS_DStr*     : cgs__dstr_copy(cgs__coerce(mutstr_dst, CGS_DStr*), cgs_strv(anystr_src)), \
    default       : cgs__fmutstr_ref_copy(cgs__fmutstr_ref_zero_len(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs_strv(anystr_src)) \
)

#define cgs_putc(writer_dst, c) \
cgs__invoke_writer(cgs_writer_ptr(writer_dst), (CGS_StrView){.chars = &(char){c}, .len = 1})

#define cgs_append(writer_dst, anystr_src) \
cgs__invoke_writer(cgs_writer_ptr(writer_dst), cgs_strv(anystr_src))

#define cgs_appendln(writer_dst, anystr_src) \
cgs__invoke_writer_ln(cgs_writer_ptr(writer_dst), cgs_strv(anystr_src))

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
    CGS_MutStrRef : cgs__mutstr_ref_insert(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs_strv(anystr_src), idx), \
    CGS_DStr*     : cgs__dstr_insert(cgs__coerce(mutstr_dst, CGS_DStr*), cgs_strv(anystr_src), idx), \
    default       : cgs__fmutstr_ref_insert(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs_strv(anystr_src), idx) \
)

#define cgs_prepend(mutstr_dst, anystr_src) \
cgs_insert(mutstr_dst, anystr_src, 0)

#define cgs_find(anystr_hay, anystr_needle) \
cgs__strv_find(cgs_strv(anystr_hay), cgs_strv(anystr_needle))

#define cgs_count(anystr_hay, anystr_needle) \
cgs__strv_count(cgs_strv(anystr_hay), cgs_strv(anystr_needle))

#define cgs_spn(anystr, anystr_charset) \
cgs__strv_spn(cgs_strv(anystr), cgs_strv(anystr_charset))

#define cgs_cspn(anystr, anystr_charset) \
cgs__strv_cspn(cgs_strv(anystr), cgs_strv(anystr_charset))

#define cgs_trim_view(anystr) \
cgs__trim_view(cgs_strv(anystr))

#define cgs_trim(mutstr) \
cgs__trim(cgs__fmutstr_ref(mutstr))

#define cgs_clear(mutstr) \
cgs__fmutstr_ref_clear(cgs__fmutstr_ref_zero_len(mutstr)) \

#define cgs_starts_with(anystr_hay, anystr_needle) \
cgs__strv_starts_with(cgs_strv(anystr_hay), cgs_strv(anystr_needle))

#define cgs_ends_with(anystr_hay, anystr_needle) \
cgs__strv_ends_with(cgs_strv(anystr_hay), cgs_strv(anystr_needle))

#define cgs_map_chars(mutstr, map_func, ...) \
cgs__map_chars(cgs__fmutstr_ref(mutstr), map_func, CGS__VA_OR(NULL, __VA_ARGS__))

#define cgs_tolower(mutstr) \
cgs__chars_tolower(cgs__fmutstr_ref(mutstr))

#define cgs_toupper(mutstr) \
cgs__chars_toupper(cgs__fmutstr_ref(mutstr))

#define cgs_replace(mutstr_dst, anystr_target, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs_strv(anystr_target), cgs_strv(anystr_replacement)), \
    CGS_DStr* : cgs__dstr_replace(cgs__coerce(mutstr_dst, CGS_DStr*), cgs_strv(anystr_target), cgs_strv(anystr_replacement)), \
    default   : cgs__fmutstr_ref_replace(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs_strv(anystr_target), cgs_strv(anystr_replacement)) \
)

#define cgs_replace_first(mutstr_dst, anystr_target, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace_first(cgs__coerce(mutstr_dst, CGS_MutStrRef), cgs_strv(anystr_target), cgs_strv(anystr_replacement)), \
    CGS_DStr*     : cgs__dstr_replace_first(cgs__coerce(mutstr_dst, CGS_DStr*), cgs_strv(anystr_target), cgs_strv(anystr_replacement)), \
    default       : cgs__fmutstr_ref_replace_first(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), cgs_strv(anystr_target), cgs_strv(anystr_replacement)) \
)

#define cgs_replace_range(mutstr_dst, begin, end, anystr_replacement) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__mutstr_ref_replace_range(cgs__coerce(mutstr_dst, CGS_MutStrRef), begin, end, cgs_strv(anystr_replacement)), \
    CGS_DStr*     : cgs__dstr_replace_range(cgs__coerce(mutstr_dst, CGS_DStr*), begin, end, cgs_strv(anystr_replacement)), \
    default       : cgs__fmutstr_ref_replace_range(cgs__fmutstr_ref(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), begin, end, cgs_strv(anystr_replacement)) \
)

#define cgs_split(anystr, anystr_delim, ...) \
cgs__strv_split(cgs_strv(anystr), cgs_strv(anystr_delim), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

#define cgs_split_iter(anystr, anystr_delim, callback, ...) \
cgs__strv_split_iter(cgs_strv(anystr), cgs_strv(anystr_delim), callback, CGS__VA_OR(NULL, __VA_ARGS__))

#define cgs_join(mutstr_dst, strv_arr, anystr_delim) \
_Generic(mutstr_dst, \
    CGS_MutStrRef : cgs__strv_arr_join(cgs__coerce(mutstr_dst, CGS_MutStrRef), strv_arr, cgs_strv(anystr_delim)), \
    CGS_DStr*     : cgs__strv_arr_join_into_dstr(cgs__coerce(mutstr_dst, CGS_DStr*), strv_arr, cgs_strv(anystr_delim)), \
    default       : cgs__strv_arr_join_into_fmutstr_ref(cgs__fmutstr_ref_zero_len(cgs__coerce_not(mutstr_dst, CGS_MutStrRef, CGS_StrBuf*)), strv_arr, cgs_strv(anystr_delim)) \
)

#define cgs_next_tok(strv_ptr, delim) \
cgs__next_tok(strv_ptr, cgs_strv(delim))

#define cgs_next_tok_any(strv_ptr, delim_set) \
cgs__next_tok_any(strv_ptr, cgs_strv(delim_set))

#define cgs_skip(anystr_src, delim) \
cgs__skip(cgs_strv(anystr_src), cgs_strv(delim))

#define cgs_skip_any(anystr_src, delim_set) \
cgs__skip_any(cgs_strv(anystr_src), cgs_strv(delim_set))

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
cgs_strv(anystr),

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
__VA_OPT__(cgs__strbuf_init_from_cstr_2(cstr, __VA_ARGS__)) \
CGS__IF_EMPTY(cgs__strbuf_init_from_cstr_(cstr), __VA_ARGS__)

#define cgs__strbuf_init_from_cstr_(cstr) \
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
__VA_OPT__( cgs__strbuf_init_from_buf_2(buf, __VA_ARGS__) ) \
CGS__IF_EMPTY(cgs__strbuf_init_from_buf_(buf), __VA_ARGS__)

#define cgs__carr_to_buf(carr) \
_Generic(&(__typeof__(carr)){0}, \
char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
unsigned char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
CGS_Buffer*: (carr) \
)

#define cgs__strbuf_init_from_buf_(buf) \
cgs__strbuf_from_buf(cgs__carr_to_buf(buf))

#define cgs__strbuf_init_from_buf_2(buf, cap_) \
cgs__strbuf_from_buf((CGS_Buffer){.ptr = (char*) _Generic(buf,char*:(buf),unsigned char*:(buf),void*:(buf)), .cap = (cap_)})

#define cgs__cstr_to_buf(carr, ...) \
( \
CGS__IF_EMPTY( \
    _Generic((__typeof__(carr)*){0}, \
        char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)}, \
        unsigned char(*)[sizeof(__typeof__(carr))]: (CGS_Buffer){.ptr = (char*) (carr), .cap = sizeof(carr)} \
    ), \
    __VA_ARGS__ \
) \
__VA_OPT__(cgs__cstr_to_buf2((carr), __VA_ARGS__)) \
)

#define cgs__cstr_to_buf2(carr_or_ptr, cap_) \
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

#define cgs__strv_1(anystr)                       \
_Generic(anystr,                                  \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 1) \
    CGS__INCOMPAT: 0                              \
)(cgs__strv_type_erasure(anystr))

#define cgs__strv_2(anystr, begin, ...)           \
_Generic(anystr,                                  \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 2) \
    CGS__INCOMPAT: 0                              \
)(cgs__strv_type_erasure(anystr), begin)

#define cgs__strv_3(anystr, begin, end)           \
_Generic(anystr,                                  \
    CGS__T_ALL_STRINGS(cgs__strv_generic_assoc, 3) \
    CGS__INCOMPAT: 0                              \
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
cgs__dstr_init_from(cgs_strv(anystr_src), CGS__VA_OR(cgs_get_default_allocator(), __VA_ARGS__))

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
    .open = cgs_strv(open_), \
    .close = cgs_strv(close_), \
    .separator = cgs_strv(seperator_), \
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
