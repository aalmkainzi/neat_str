#include "../include/neat_str.h"

NEAT_DECL_SSTRING(127);

void replace_dash(Any_String_Ref str)
{
    str_replace(str, "-", " ");
}

int main()
{
    String_Buffer sb = strbuf((char[]){"string-buffer"});

    SString_Ref ssr = sstr_ref(&(SString(127)){0});
    str_copy(ssr, "my-sstring");

    char *cstr = calloc(127, sizeof(char));
    strcpy(cstr, "c-string");

    replace_dash(anystr_ref(&sb));
    replace_dash(anystr_ref(ssr));
    replace_dash(anystr_ref(cstr));

    println(sb);
    println(ssr);
    println(cstr);

    free(cstr);
}
