#include "cgs.h"

int main()
{
    CGS_DStr str = cgs_dstr_init();
    
    cgs_append(&str, "world");
    cgs_prepend(&str, "hello, ");
    
    cgs_writeln(str);
    
    cgs_dstr_deinit(&str);
}
