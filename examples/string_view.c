#define NEAT_STR_IMPL
#include "../neat_str.h"

int main()
{
    String_View s     = strv("hello, world");
    String_View hello = strv(s, 0, 5);
    String_View world = strv(s, 7);
    
    println(hello);
    println(world);
}
