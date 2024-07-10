# neat_str
A library to make dealing with strings more intuitive in C.

It exposes multiple string types for different use cases:
- [DString](#DString)
- [String_Buffer](#String_Buffer)
- [SString](#SString)
- [String_View](#String_View)
- [Mut_String_Ref](#Mut_String_Ref)

This is a list of the functions (macros) that work with all string types:
```C
unsigned int      str_len(any_str);
unsigned int      str_cap(any_str);
bool              str_equal(any_str1, any_str2);
char*             str_cstr(any_str);

// Returns a String_View of arg1 where arg2 was found
String_View       str_find(any_str_hay, any_str_needle);

// Returns how many times arg2 was found in arg1
unsigned int      str_count(any_str_hay, any_str_needle);

// Copies arg2 into arg1. If it doesn't fit, it copies as many chars as can fit. Returns how many chars were copied
unsigned int      str_copy(mut_str_dst, any_str_src);

// Concats arg2 into arg1. If it doesn't fit, it concats as many chars as can fit. Returns how many chars were concated
unsigned int      str_concat(cap_str_dst, any_str_src);

// Insert into arg1 the string arg2 at a specific index. If it doesn't fit, it inserts as many chars as can fit. Returns how many chars were inserted
unsigned int      str_insert(cap_str_dst, any_str_src, idx);

// Same as calling the above with index 0
unsigned int      str_prepend(cap_str_dst, any_str_src);

// Replaces all occurrence of arg2 inside arg1 with the replacement arg3. Returns how many replaced
unsigned int      str_replace(mut_str, any_str_target, any_str_replacement);

// Replaces the first occurance of arg2 inside arg1 with the replacement arg3. Returns whether it was found
bool              str_replace_first(mut_str, any_str_target, any_str_replacement);

// Deletes the characters specified by the range [begin, end). Returns false if the range is invalid, true otherwise
bool              str_del(mut_str, begin, end);

// Returns a new String_View_Array containing arg2 splitted using the delimiter arg1
String_View_Array str_split(any_str_delim, any_str);

// Same as above except specify the allocator in arg3
String_View_Array str_split(any_str_delim, any_str, allocator);

// Joins the String_View_Array in arg3 using the delimiter in arg2, store the resulting string in arg1. Returns how many chars were copied
unsigned int      str_join(mut_str_dst, any_str_delim, strv_arr);

// Same as above except returns a new DString containing the joined string
DString           str_join_new(any_str_delim, strv_arr);

// Same as above except specify the allocator in arg3
DString           str_join_new(any_str_delim, strv_arr, allocator);

// sprintf replacement. Store into arg1 the tostr_into of all the va args (e.g. `str_print( mystr, 10, "\n", "hello", "world" );`
void              str_print(mut_str, ...);

// Same as above except return a new DString containing the tostr of all args
DString           str_print_new(...);

// Same as above except specify the allocator in arg1
DString           str_print_new(allocator, ...);

// Reads a line from file stream arg2 into string arg1. If it doesn't fit, it reads as many chars as can fit. Returns how many chars were read and copied
unsigned int      str_fread_line(mut_str, stream);

// Same as above except concats into string arg1 rather than copy
unsigned int      str_concat_fread_line(cap_str, stream);

// Same as str_fread_line(any_str, stdio)
unsigned int      str_read_line(mut_str);

// Same as str_concat_fread_line(any_str, stdio)
unsigned int      str_concat_read_line(cap_str);
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
void         dstr_append(dstr, any_str);
void         dstr_append_tostr(dstr, stringable);
void         dstr_append_tostr_p(dstr, stringable*);
void         dstr_prepend(dstr, any_str);
void         dstr_prepend_tostr(dstr, stringable);
void         dstr_prepend_tostr_p(dstr, stringable*);
bool         dstr_insert(dstr, any_str, idx);
bool         dstr_insert_tostr(dstr, stringable, idx);
bool         dstr_insert_tostr_p(dstr, stringable*, idx);
unsigned int dstr_fread_line(any_str, stream);
unsigned int dstr_read_line(dstr);
unsigned int dstr_append_fread_line(dstr, stream);
unsigned int dstr_append_read_line(dstr);
void         dstr_shrink_to_fit(dstr);
void         dstr_ensure_cap(dstr, new_cap);
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
String_Buffer strbuf(carr, cap);
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

An array of `String_View`. This type is returned from `str_split` and is passed to `str_join`

to initialize:
```C
String_View_Array strv_arr(...any_str);

// arg1 is String_View[N]
String_View_Array strv_arr_carr(carr);

// arg 1 can either be String_View[N] or String_View*. arg2 is how many elements in the array
String_View_Array strv_arr_carr(carr_or_ptr, nb);
```

## Mut_String_Ref

This type can be used as a mutable reference to any mutable string type (all string types except `String_View`).

to initialize:
```C
Mut_String_Ref mutstr_ref(mut_str);
Mut_String_Ref mutstr_ref(carr, cap);
```

for example:
```C
void concat_comma(Mut_String_Ref str)
{
    str_concat(str, ",");
}

void foo(SString_Ref s1, String_Buffer *s2)
{
    concat_comma( mutstr_ref(s1) );
    concat_comma( mutstr_ref(s2) );
}
```

## tostr

You can convert any type to string by using `tostr`:

```C
DString tostr(stringable);
DString tostr_p(stringable*);
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
    DString ret = dstr();
    dstr_append_tostr(&ret, f->n);
    return ret;
}

#define ADD_TOSTR struct FOO, foo_to_str
#include "neat_str.h"
```

## tostr_into

Similar to `tostr`, except it writes to a string instead of returning a new `DString`:
```C
void tostr_into(mut_str, stringable);
void tostr_into_p(mut_str, stringable*);
```

Example to add your own ```tostr_into```
```C
#include "neat_str.h"

typedef struct {
    char c;
    float f;
} FOO;

void foo_to_str_into(Mut_String_Ref dst, FOO *foo)
{
    str_print(dst, "FOO{", ".c=", foo->c, ", .f=", foo->f, "}");
}

#define ADD_TOSTR_INTO FOO, foo_to_str_into
#include "neat_str.h"
```

now that `FOO` has a `tostr_into` it can be used in `str_print` like this:
```C
FOO foo = {.c = 'X', .f = 1.5f};

str_print(&mystr, foo);

println(mystr); // prints 'FOO{.c=X, .f=1.5}'
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

## Namespacing

You can choose to prefix the entire lib like this:
```C
#define NEAT_STR_PREFIX
#include "neat_str.h"
```
this will prefix all macro definitions with `neat_` and all types with `Neat_`

## Building and Including

1) Run:
```
cd build
cmake .
cmake --build .
```
After which you'll find the static library inside `./bin`.

2) Copy both headers from `./include` to your project.
