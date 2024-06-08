#include <stdbool.h>
#define NEAT_STR_IMPL
#include "neat_str.h"
#include <string.h>
#include <stdio.h>


// TODO possible problem with my approach: a user can have 2 of my libs with different versions, and if he includes the old first, the new won't define its commons
// TODO add useful string functions, like concat, append, substring, etc.

NEAT_DECL_SSTRING(10);

int main()
{
    SString(10) a = { .chars = "wow", .len = 3};
    SString_Ref ref = sstr_ref(&a);
    
    printf("%s\n", ref.sstring->chars);
}
