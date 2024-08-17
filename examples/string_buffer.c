#define NEAT_STR_IMPL
#include "../neat_str.h"

int main()
{
    // example 1
    char c[128] = {0};
    String_Buffer mystr = strbuf(c);

    str_print(&mystr, "hello, world", "\n", 123);

    str_concat(&mystr, "456");

    println(mystr);
    // example 1 end

    // example 2
    String_Buffer mystr2 = strbuf(127);

    str_copy(&mystr2, "hello,,,,,world");
    str_replace(&mystr2, ",,,,,", ", ");

    println(mystr2);

    free(mystr2.chars);
    // example 2 end
}
