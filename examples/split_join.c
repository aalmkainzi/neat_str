#include "../include/neat_str.h"

int main()
{
    String_View_Array splitted = str_split("1 XX 2 XX 3 XX 4 XX 5 XX 6", " XX ");

    char c[128];
    String_Buffer dst = strbuf(c);
    str_join(&dst, splitted, ", ");

    println("the joined string: \n", dst);

    free(splitted.strs);
}
