#include <stdarg.h>
#include <utils.h>
#include <ints.h>

void halt(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    disable_ints();
    __asm__("hlt");
}

unsigned long min(unsigned long x, unsigned long y)
{
    return x < y ? x : y;
}

unsigned long max(unsigned long x, unsigned long y) 
{
    return x > y ? x : y;
}

