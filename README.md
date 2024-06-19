# neat_str
A library to make dealing with strings more intuitive in C.

It exposes multiple string types for different use cases.

This is a list of the functions (macros) that work with all string types:
```C
unsigned int      str_len(any_str);
unsigned int      str_cap(any_str);
bool              str_equal(any_str1, any_str2);
String_View       str_find(any_str_hay, any_str_needle);
unsigned int      str_count(any_str_hay, any_str_needle);
unsigned int      str_copy(any_str_dst, any_str_src);
unsigned int      str_concat(cap_str_dst, any_str_src);
unsigned int      str_concat_all(cap_str_dst, strv_arr);
DString           str_concat_new(any_str_1, any_str_2);
DString           str_concat_new(any_str_1, any_str_2, allocator);
DString           str_concat_all_new(strv_arr);
DString           str_concat_all_new(strv_arr, allocator);
unsigned int      str_replace(mut_str, any_str_target, any_str_replacement);
String_View_Array str_split(any_str, any_str_delim);
String_View_Array str_split(any_str, any_str_delim, allocator);
unsigned int      str_join(mut_str_dst, any_str_delim, strv_arr);
DString           str_join_new(any_str_delim, strv_arr);
DString           str_join_new(any_str_delim, strv_arr, allocator);
unsigned int      str_fread_line(stream, any_str);
unsigned int      str_concat_fread_line(stream, any_str);
unsigned int      str_read_line(any_str);
unsigned int      str_concat_read_line(any_str);
unsigned int      str_fprint(stream, any_str);
unsigned int      str_fprintln(stream, any_str);
unsigned int      str_print(any_str);
unsigned int      str_println(any_str);
```
Note some of these macros require a mutable string type (e.g. `str_replace`), that includes all string types except `String_View`.

And some require a string with cap information (e.g. `str_concat`), that includes all string types except `String_View` and `[unsigned] char*`

## DString

Dynamic String.

to initialize / deinit:
```C
DString dstr();
DString dstr(cap);
DString dstr(allocator);
DString dstr(cap, allocator);

void dstr_deinit(dstr);
```
utility:
```C
void dstr_append(dstr, any_str);
void dstr_append_tostr(dstr, stringable);
void dstr_append_tostr_p(dstr, stringable_ptr);
void dstr_prepend(dstr, any_str);
void dstr_prepend_tostr(dstr, stringable);
void dstr_prepend_tostr_p(dstr, stringable_ptr);
bool dstr_insert(dstr, any_str, idx);
bool dstr_insert_tostr(dstr, stringable, idx);
bool dstr_insert_tostr_p(dstr, stringable_ptr, idx);
```

## String_Buffer

Used as a general purpose string buffer, it's defined like this:
```C
typedef struct String_Buffer
{
    unsigned int cap; /* including the nul */
    unsigned int len;
    unsigned char *chars;
} String_Buffer;
```
to initialize:
```C
String_Buffer strbuf(any_str);
String_Buffer strbuf(cap);
String_Buffer strbuf(cap, allocator);
```
## SString

Static/Small String. Can be stored on the stack or inside structs.

To pass it around to functions, you can use:
```C
SString_Ref sstr_ref(sstr_ptr);
```

Used like this:
```C
NEAT_DECL_SSTRING(16); // not necessary as of C23

void foo()
{
    SString(16) mystr = {0};
    SString_Ref ref = sstr_ref(&mystr);
    // can now use ref with any function (macro) that starts with str_
}
```

## String_View

Used to view into other strings.

to initialize:
```C
String_View strv(any_str);
String_View strv(any_str, begin_idx);
String_View strv(any_str, begin_idx, end_idx);
```

## String_View_Array

An array of `String_View`. This type is returned from `str_split` and is passed to `str_join` and `str_concat_all`

to initialize:
```C
String_View_Array strv_arr(...any_str);
String_View_Array strv_arr_from_carr(carr);
String_View_Array strv_arr_from_carr(carr_or_ptr, nb);
```

## Any_String_Ref

This type can be used as a mutable reference to any string type.

to initialize:
```C
Any_String_Ref anystr_ref(any_str);
```

for example:
```C
void concat_comma(Any_String_Ref str)
{
    str_concat(str, ",");
}

void foo(SString_Ref s1, String_Buffer *s2)
{
    concat_comma( anystr_ref(s1) );
    concat_comma( anystr_ref(s2) );
}
```

## tostr

You can convert any type to string by using `tostr`:

```C
DString tostr(Stringable);
DString tostr_p(Stringable*);
```
e.g.
```C
int main()
{
    DString num = tostr(10);
}
```

You can add your own Stringable types by defining `ADD_TOSTR Ty, Ty2str` and re-including the `neat_str.h` header:
```C
#include "neat_str.h"

struct FOO {
    char n;
};

DString foo_to_str( struct FOO *f )
{
    DString ret = neat_dstr();
    neat_dstr_append_tostr(&ret, f->n);
    return ret;
}

#define ADD_TOSTR struct FOO, foo_to_str
#include "neat_str.h"
```

## tostr_into

Similar to `tostr`, except it writes to a string instead of returning a new `DString`:
```C
void tostr_into(mut_str, Stringable);
void tostr_into_p(mut_str, Stringable*);
```

Example to add your own ```tostr_into```
```C
#include "neat_str.h"

struct FOO {
    char n;
};

void foo_to_str_into(Any_String_Ref dst, struct FOO *f)
{
    char tmp[2] = {f->n, '\0'};
    neat_str_copy(dst, tmp);
}

#define ADD_TOSTR_INTO struct FOO, foo_to_str_into
#include "neat_str.h"
```

## print

Any Stringable type can be printed with these macros:
```C
fprint(stream, ...);
fprintln(stream, ...);
print(...);
println(...);
```

e.g:
```C
int main()
{
    println("hello", 123, "\n", 15.3);
}
```

## Including

to include this lib in your project, add `neat_str.c` and `neat_core.c` to your build script.

note that `neat_str.c` includes `neat_str.h` in the same directory, same for `neat_core.c` and `neat_core.h`,
