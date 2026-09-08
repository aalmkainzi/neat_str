#include "cgs.h"

int main()
{
    char buf[64];
    
    cgs_fmt(buf, "%? + %? = %?\n", 2, 3, 5);
    
    cgs_appendf(buf, "%[1] + %[0] = %[2]", 2, 3, 5);
    
    cgs_printfln("%?", buf);
    
    char *a = malloc(14);
    strcpy(a, "  ");
    cgs_fmt(a, "%?", 123);
    cgs_writeln(a);
}
