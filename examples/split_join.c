#include "../include/neat_str.h"

int main()
{
    String_View_Array splitted = str_split("-", "1-2-3-4-5-6");

    char c[128];
    String_Buffer dst = strbuf(c);
    str_join(&dst, ", ", splitted);

    println("the joined string: \n", dst);

    free(splitted.strs);
}
