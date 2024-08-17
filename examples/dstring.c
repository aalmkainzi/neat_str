#define NEAT_STR_IMPL
#include "../neat_str.h"

int main()
{
    DString str = dstr();
    
    dstr_append(&str, "world");
    dstr_prepend(&str, "hello, ");
    
    println(str);
    
    dstr_deinit(&str);
}
