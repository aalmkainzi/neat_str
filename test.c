#include <stdbool.h>
#define NEAT_STR_IMPL
#include "neat_str.h"
#include <string.h>
#include <stdio.h>


// TODO possible problem with my approach: a user can have 2 of my libs with different versions, and if he includes the old first, the new won't define its commons
// TODO add useful string functions, like concat, append, substring, etc.

/*
str_ prefix means generic (any string, sometimes, String_View sometimes not allowed cuz read-only)
API:
// Generic
size_t str_len(any_str)
size_t str_cap(any_str)
size_t str_copy(any_str, any_str)
bool str_equals(any_str, any_str)
size_t str_concat(any_str, any_str)
String_View str_find(any_str, seq)
int str_replace(any_str, any_str target, any_str replacement)
String_View_Array* str_split(any_str, delim)
String_View_Array* str_split(any_str, delim, allocator)
DString str_join(delim, ...strs)
DString str_join(delim, allocator, ...strs)

// *super* basic IO. just calls fread and fwrite
size_t str_read(any_str, stream) // reads up to any_str cap
size_t str_read(any_str, stream, nb) // reads up to nb

size_t str_read_concat(any_str, stream, nb)
size_t str_read_concat(any_str, stream)

size_t str_write(any_str, stream)

// SString_Ref
sstr_ref(sstr_ptr)
// SString
sstr(sstr_ref, any_str)
// String_Buffer
strbuf(any_str)
// String_View
strv(any_str)
strv(any_str, start)
strv(any_str, start, end)
// DString
dstr()
dstr(cap)
dstr(allocator)
dstr(cap, allocator)

dstr_deinit(dstr)

dstr_append(dstr, any_str)
dstr_append_tostr(dstr, Stringable)
dstr_append_tostrptr(dstr, Stringable*)

dstr_prepend(dstr, any_str)
dstr_prepend_tostr(dstr, Stringable)
dstr_prepend_tostrptr(dstr, Stringable*)

dstr_insert(dstr, any_str, idx)
dstr_insert_tostr(dstr, Stringable, idx)
dstr_insert_tostrptr(dstr, Stringable*, idx)


AND ALSO ALL THE PRINT AND tostr STUFF
*/

NEAT_DECL_SSTRING(10);

int main()
{
    SString(10) a;
    SString_Ref ref = sstr_ref(&a);
    
    str_copy(ref, "hello");
    
    
    printf("%s\n", ref.sstring->chars);
}

