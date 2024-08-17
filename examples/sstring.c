#define NEAT_STR_IMPL
#include "../neat_str.h"

NEAT_DECL_SSTRING(127); // not necessary as of C23

int main()
{
    SString(127) str = {0};
    SString_Ref sref = sstr_ref(&str);

    str_print(sref, 123, "\n", "hello from sstring");

    println(sref);
}
