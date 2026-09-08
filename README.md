# CGS - C Generic Strings

## Quick Examples
```C
#include "cgs.h"

int main()
{
    CGS_StrView s     = cgs_strv("hello, world");
    CGS_StrView hello = cgs_strv(s, 0, 5);
    CGS_StrView world = cgs_strv(s, 7);
    
    cgs_printf("%? %?", hello, world);
}

```
```C
#include "cgs.h"

int main()
{
    CGS_DStr str = cgs_dstr_init();
    
    cgs_append(&str, "world");
    cgs_prepend(&str, "hello, ");
    
    cgs_writeln(str);
    
    cgs_dstr_deinit(&str);
}

```
## Features
The library exposes multiple string types for different use cases:
- [CGS_DStr](#CGS_DStr)
- [CGS_StrBuf](#CGS_StrBuf)
- [CGS_StrView](#CGS_StrView)
- [CGS_MutStrRef](#CGS_MutStrRef)

All of which are null terminated, except for `CGS_StrView`.

There are three categories of string types:
- `anystr_t`: any of `char*`, `unsigned char*`, `char[]`, `unsigned char[]`, `CGS_StrView`, `CGS_DStr`, `CGS_DStr*`, `CGS_StrBuf`, `CGS_StrBuf*`, `CGS_MutStrRef`
- `mutstr_t`: any of `char*`, `unsigned char*`, `char[]`, `unsigned char[]`, `CGS_DStr*`, `CGS_StrBuf*`, `CGS_MutStrRef`
- `writer_t`: any of `char[]`, `unsigned char[]`, `CGS_DStr*`, `CGS_StrBuf*`, `CGS_MutStrRef`, `FILE*`, `unsigned int*`, and any `CGS_<T>Writer*` type

This is a list of all the utility macros CGS provides:
```C++
CGS_StrView             cgs_strv(anystr_t str, unsigned int from = 0, unsigned int to_exclusive = cgs_len(str));
                  
CGS_StrBuf              cgs_strbuf_init_from_cstr([unsigned] char *cstr, unsigned int cap = strlen(cstr) + 1);
CGS_StrBuf              cgs_strbuf_init_from_cstr([unsigned] char cstr[], unsigned int cap = sizeof(cstr));
CGS_StrBuf              cgs_strbuf_init_from_buf([unsigned] char *buf, unsigned int cap);
CGS_StrBuf              cgs_strbuf_init_from_buf([unsigned] char buf[], unsigned int cap = sizeof(buf));

CGS_DStr                cgs_dstr_init(unsigned int initial_cap = 0, CGS_Allocator *allocator = cgs_get_default_allocator());
CGS_DStr                cgs_dstr_init_from(anystr_t src, CGS_Allocator *allocator = cgs_get_default_allocator());
void                    cgs_dstr_deinit(CGS_DStr *dstr);
CGS_Error               cgs_dstr_shrink_to_fit(CGS_DStr *dstr);
CGS_Error               cgs_dstr_ensure_cap(CGS_DStr *dstr, unsigned int at_least);

CGS_MutStrRef           cgs_mutstr_ref(mutstr_t str);
CGS_MutStrRef           cgs_mutstr_ref(cstr, cap);


CGS_<T>Writer           cgs_writer(writer_t writer); // returns the writer object. e.g. if passed a `FILE*` it returns `CGS_FileWriter`, if passed `CGS_DStr*` it returns `CGS_DStrWriter`, etc.. And if passed a writer object, it returns it as-is
CGS_ChainWriter         cgs_writer(writer_t a, writer_t b); // returns a writer object that writes to both `a` and `b`

CGS_Writer*             cgs_writer_ptr(writer_t writer); // same as `cgs_writer`, but returns pointer to writer object (address of compound literal). And if passed a writer pointer, it returns it as-is
CGS_Writer*             cgs_writer_ptr(writer_t a, writer_t b); // return a CGS_Writer pointer to a CGS_ChainWriter stored in current scope

CGS_LenWriter           cgs_len_writer(); // returns a writer that only counts how many bytes were to be written.

CGS_StrViewArray        cgs_strv_arr(...anystr_t); // lifetime ends at end of current scope
CGS_StrViewArray        cgs_strv_arr_from(CGS_StrView strs[N]);
CGS_StrViewArray        cgs_strv_arr_from(CGS_StrView *strs, unsigned int len);

unsigned int            cgs_len(anystr_t);
unsigned int            cgs_cap(anystr_t);
bool                    cgs_equal(anystr_t a, anystr_t b);
char*                   cgs_chars(anystr_t);

CGS_StrView             cgs_find(anystr_t hay, anystr_t needle);
unsigned int            cgs_count(anystr_t hay, anystr_t needle);
CGS_Error               cgs_clear(mutstr_t);
bool                    cgs_starts_with(anystr_t hay, anystr_t needle);
bool                    cgs_ends_with(anystr_t hay, anystr_t needle);

CGS_StrView             cgs_trim_view(anystr_t str);
CGS_Error               cgs_trim(mutstr_t str);

CGS_StrView             cgs_skip(anystr_t str, anystr_t delim); // returns view of after leading delims
CGS_StrView             cgs_skip_any(anystr_t str, anystr_t delim_set);

CGS_StrView             cgs_spn(anystr_t src, anystr_t charset); // returns view of first chunk that contains characters only found in charset
CGS_StrView             cgs_cspn(anystr_t src, anystr_t charset); // returns view of first chunk that contains characters not found in charset

CGS_Result(CGS_StrView) cgs_next_tok(CGS_StrView *base, anystr_t delim); // skips leading delims, returns next token. modifies base such that it's after the returned token
CGS_Result(CGS_StrView) cgs_next_tok_any(CGS_StrView *base, anystr_t delim_set);

CGS_Error               cgs_map_chars(mutstr_t, bool(*map_func)(char *c, void *arg), void *arg = NULL); // iterates each char and calls map_func on it. If false is returned from map_func, iteration is stopped and CGS_CALLBACK_EXIT error code is returned
void                    cgs_tolower(mutstr_t);
void                    cgs_toupper(mutstr_t);

CGS_Error               cgs_copy(mutstr_t dst, anystr_t src);
CGS_DStr                cgs_dup(anystr_t src, CGS_Allocator *allocator = cgs_get_default_allocator()); // alias for cgs_dstr_init_from

CGS_Error               cgs_putc(writer_t dst, char c);
CGS_Error               cgs_append(writer_t dst, anystr_t src);
CGS_Error               cgs_appendln(writer_t dst, anystr_t src);
CGS_Error               cgs_insert(mutstr_t dst, anystr_t src, unsigned int pos);
CGS_Error               cgs_prepend(mutstr_t dst, anystr_t src);
CGS_Error               cgs_del(mutstr_t, unsigned int from, unsigned int to_exclusive);

CGS_Result(int)         cgs_replace(mutstr_t dst, anystr_t target, anystr_t replacement);
CGS_Error               cgs_replace_first(mutstr_t dst, anystr_t target, anystr_t replacement);
CGS_Error               cgs_replace_range(mutstr_t dst, unsigned int from, unsigned int to_exclusive, anystr_t replacement);

CGS_StrViewArray        cgs_split(anystr_t str, anystr_t delim, CGS_Allocator *allocator = cgs_get_default_allocator());
CGS_Error               cgs_split_iter(anystr_t str, anystr_t delim, bool(*callback)(CGS_StrView found, void *arg), void *arg = NULL);
CGS_Error               cgs_join(mutstr_t dst, CGS_StrViewArray arr, anystr_t delim);

CGS_Error               cgs_fwrite(FILE *stream, anystr_t src); // identical to cgs_append, but restricted to FILE*
CGS_Error               cgs_fwriteln(FILE *stream, anystr_t src); // identical to cgs_appendln, but restricted to FILE*
CGS_Error               cgs_write(anystr_t src); // calls cgs_fwrite on stdout
CGS_Error               cgs_writeln(anystr_t src); // calls cgs_fwriteln on stdout

CGS_Error               cgs_fread_until(mutstr_t dst, FILE *stream, int delim); // clears dst, writes to dst from stream until delim is reached. If delim is reached before EOF, it is written to dst
CGS_Error               cgs_append_fread_until(mutstr_t dst, FILE *stream, int delim); // appends to dst from stream until delim is reached. If delim is reached before EOF, it is written to dst
CGS_Error               cgs_read_until(mutstr_t dst, int delim); // calls cgs_fread_until on stdin
CGS_Error               cgs_append_read_until(mutstr_t dst, int delim); // calls cgs_append_fread_until on stdin
CGS_Error               cgs_fread_line(mutstr_t dst, FILE *stream); // calls cgs_fread_until with '\n' delim
CGS_Error               cgs_append_fread_line(mutstr_t dst, FILE *stream); // cgs_append_fread_until with '\n' delim
CGS_Error               cgs_read_line(mutstr_t dst); // calls cgs_fread_line on stdin
CGS_Error               cgs_append_read_line(mutstr_t dst); // calls cgs_append_fread_line on stdin

CGS_MutStrRef           cgs_appender(mutstr_t owner, CGS_AppenderState *state); // returns a string reference that appends to owner. must call cgs_commit_appender to update the length of owner. If owner is a CGS_DStr, the appender may make it grow
CGS_Error               cgs_commit_appender(mutstr_t owner, CGS_MutStrRef appender); // commit the appender to the owning string. The owner must not have had its capacity or length modified between cgs_appender and cgs_commit_appender

CGS_Error               cgs_tostr(mutstr_t dst, T val);
CGS_Error               cgs_append_tostr(writer_t dst, T val);
CGS_Error               cgs_tostr_p(mutstr_t dst, T *val);
bool                    cgs_has_tostr(T);
unsigned int            cgs_tostr_len(T val);
unsigned int            cgs_tostr_p_len(T *val);

CGS_Error               cgs_fmt(mutstr_t dst, const char *fmt, ...args with tostr); // clears dst, then writes the formatted string to it. fmt syntax is "%?", or "%[arg_index]" for positional arguments, cannot mix and match
CGS_Error               cgs_appendf(writer_t dst, const char *fmt, ...args with tostr); // identical to cgs_fmt, but appends
CGS_Error               cgs_appendfln(writer_t dst, const char *fmt, ...args with tostr); // cgs_appendf + '\n'

CGS_Error               cgs_fprintf(FILE *stream, const char *fmt, ...args with tostr); // identical to cgs_appendf, but restricted to FILE*
CGS_Error               cgs_fprintfln(FILE *stream, const char *fmt, ...args with tostr); // identical to cgs_appendfln, but restricted to FILE*

CGS_Error               cgs_printf(const char *fmt, ...args with tostr); // calls cgs_fprintf on stdout
CGS_Error               cgs_printfln(const char *fmt, ...args with tostr); // calls cgs_fprintfln on stdout

CGS_Error               cgs_sprintf(mutstr_t dst, const char *fmt, ...args with tostr); // alias for cgs_fmt
CGS_Error               cgs_sprintfln(mutstr_t dst, const char *fmt, ...args with tostr); // cgs_sprintf + '\n'

CGS_DStr                cgs_asprintf(CGS_Allocator *allocator = cgs_get_default_allocator(), const char *fmt, ...args with tostr); // allocates a CGS_DStr and calls cgs_fmt on it

                        cgs_tostr_many(mutstr_t dst, ...args with tostr); // clears dst, calls the tostr of each ... arg, and appends them to dst
                        cgs_append_tostr_many(writer_t dst, ...args with tostr); // calls the tostr of each ... arg, and appends them to dst

cgs_nfmt_t(integer_T, fmt_char)    cgs_nfmt(integer_T value, int fmt_char); // a number format object that has a tostr
cgs_nfmt_t(float/double, fmt_char) cgs_nfmt(float/double value, int fmt_char, int precision = (fmt_char == 'a' ? -1 : 6)); // a number format object that has a tostr
CGS_ArrayFmt                       cgs_arrfmt(T *array, size_t len); // an array format object that has a tostr, prints in this format `{1, 2, 3}`
CGS_ArrayFmt                       cgs_arrfmt(T *array, size_t len, anystr_t open, anystr_t close, anystr_t delim, anystr_t trailing_delim = "");
CGS__AlignFmt                      cgs_alignfmt(T obj, align_mode, int width, char fill_char = ' '); // an align format object that has a tostr, align_mode must be one of CENTER, LEFT, RIGHT
CGS__RepeatFmt                     cgs_repeatfmt(T obj, unsigned int nb); // a format object that has a tostr that prints obj nb times
```

## CGS_DStr

Dynamic String.

Constructed with:
```C
CGS_DStr cgs_dstr_init(unsigned int initial_cap = 0, CGS_Allocator *allocator = cgs_get_default_allocator());
CGS_DStr cgs_dstr_init_from(anystr_t src, CGS_Allocator *allocator = cgs_get_default_allocator());
```

## CGS_StrBuf

Used as a general purpose string buffer, it's defined like this:
```C
typedef struct CGS_StrBuf
{
    char *chars;
    unsigned int cap; // including the nul
    unsigned int len;
} CGS_StrBuf;
```

You can construct it by calling `cgs_strbuf_init_from_*` macros, or by constructing it yourself.

## CGS_StrView

Used to view into other strings.

To initialize:
```C
CGS_StrView cgs_strv(anystr_t str, unsigned int from = 0, unsigned int to_exclusive = cgs_len(str));
```

## CGS_MutStrRef

This type can be used as a reference to any `mutstr_t` type.

To initialize:
```C
CGS_MutStrRef cgs_mutstr_ref(mutstr_t);
CGS_MutStrRef cgs_mutstr_ref(cstr, cap);
```

For example:
```C
void set_to_bar(CGS_MutStrRef str)
{
    cgs_copy(str, "bar");
}

void f(CGS_DStr *s1, CGS_StrBuf *s2, char *s3)
{
    set_to_bar( cgs_mutstr_ref(s1) );
    set_to_bar( cgs_mutstr_ref(s2) );
    set_to_bar( cgs_mutstr_ref(s3) );
}
```

## tostr

Used to convert values to string:

```C
CGS_Error cgs_tostr(mutstr_t dst, T val);
CGS_Error cgs_append_tostr(writer_t dst, T val);
```
Example:
```C
#include "cgs.h"

int main()
{
    CGS_DStr num = cgs_dstr_init();
    cgs_tostr(&num, 10);
}
```

You can add your own tostr functions for types by defining `ADD_TOSTR (Ty, Ty2str)` and re-including the `cgs.h` header:
```C
#include "cgs.h"

struct FOO {
    char n;
};

CGS_Error foo_to_str(CGS_Writer *dst, struct FOO f, CGS_StrView fmt_arg)
{
    CGS_Error err = cgs_appendf(dst, "%? :: %?", f.n, fmt_arg);
    return err;
}

#define ADD_TOSTR \
(struct FOO, foo_to_str)

#include "cgs.h"

int main()
{
    struct FOO f = {'a'};
    cgs_printf("%?", f); // can now use `struct FOO` in contexts that require a type with tostr
    
    cgs_printf("%(this is the fmt arg)", f); // prints "a :: this is the fmt arg"
    
    // syntax to using the fmt_arg with positional arguments:
    struct FOO f2 = {'b'};
    cgs_printf("%[0(for apple)], %[1(for banana)]", f, f2); // prints "a :: for apple, b :: for banana"
}
```

## cgs_fmt

Types that have a `tostr` defined can be used in `cgs_fmt` and its variants:

```C++
CGS_Error cgs_fmt(mutstr_t dst, const char *fmt, ...args with tostr);
CGS_Error cgs_appendf(writer_t dst, const char *fmt, ...args with tostr);
CGS_Error cgs_appendfln(writer_t dst, const char *fmt, ...args with tostr);

CGS_Error cgs_fprintf(FILE *stream, const char *fmt, ...args with tostr);
CGS_Error cgs_fprintfln(FILE *stream, const char *fmt, ...args with tostr);

CGS_Error cgs_printf(const char *fmt, ...args with tostr);
CGS_Error cgs_printfln(const char *fmt, ...args with tostr);

CGS_Error cgs_sprintf(mutstr_t dst, const char *fmt, ...args with tostr);
CGS_Error cgs_sprintfln(mutstr_t dst, const char *fmt, ...args with tostr);
```

e.g.
```C
#include "cgs.h"

int main()
{
    char buf[64];
    
    cgs_fmt(buf, "%? + %? = %?\n", 2, 3, 5);
    
    cgs_appendf(buf, "%[1] + %[0] = %[2]", 2, 3, 5); // positional arguments
    
    cgs_writeln(buf);
}
```

## cgs_nfmt, cgs_arrfmt, cgs_alignfmt, cgs_repeatfmt

These macros return a format object that has a `tostr`. For example:

```C
cgs_printf("%?", cgs_nfmt(15, 'X')); // prints "F"
```

Note that `nfmt` is type-safe, you cannot use chars other than:
- For integers: `'d'`, `'x'`, `'o'`, `'b'`, `'X'`
- For float/double: `'f'`, `'g'`, `'e'`, `'a'`, `'F'`, `'G'`, `'E'`, `'A'`

## Including
If you want external linking, your options are either:
- Add cgs.c to your build, and only include cgs.h
or
- Include cgs.c in only one file, and cgs.h in other files

You can also always include cgs.c, if you define `#define CGS_API static` before including:
```C
#define CGS_API static inline
#include "cgs.c"
```

## Try it
[godbolt link to try it out](https://godbolt.org/z/ozYT3TGWs)
