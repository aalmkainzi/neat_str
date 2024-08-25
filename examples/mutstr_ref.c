#define NEAT_STR_IMPL
#include "../neat_str.h"

void replace_dash(Mut_String_Ref str)
{
    str_replace(str, "-", " ");
}

NEAT_DECL_SSTRING(127);

int main()
{
    String_Buffer sb = strbuf((char[]){"string-buffer"});

    SString_Ref ssr = sstr_ref(&(SString(127)){0});
    str_copy(ssr, "my-sstring");

    char *cstr = calloc(128, sizeof(char));
    strcpy(cstr, "c-string");

    replace_dash(mutstr_ref(&sb));
    replace_dash(mutstr_ref(ssr));
    replace_dash(mutstr_ref(cstr));

    println(sb);
    println(ssr);
    println(cstr);

    free(cstr);
}
