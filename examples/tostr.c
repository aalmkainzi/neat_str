#include "cgs.h"

struct FOO {
    char n;
};

CGS_Error foo_to_str(CGS_Writer *dst, struct FOO f, CGS_StrView fmt_arg)
{
    CGS_Error err = cgs_appendf(dst, "%? :: <%?>", f.n, fmt_arg);
    return err;
}

#define ADD_TOSTR \
(struct FOO, foo_to_str)

#include "cgs.h"

int main()
{
    struct FOO f = {'a'};
    cgs_printfln("%?", f); // can now use `struct FOO` in contexts that require a type with tostr
    
    cgs_printfln("%(this is the fmt arg)", f); // prints "a :: <this is the fmt arg>"
    
    // syntax to using the fmt_arg with positional arguments:
    struct FOO f2 = {'b'};
    cgs_printfln("%[0(for apple)], %[1(for banana)]", f, f2); // prints "a :: <for apple>, b :: <for banana>"
}
