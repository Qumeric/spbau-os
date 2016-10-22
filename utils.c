#include <utils.h>
#include <ints.h>
#include <io.h>

void halt_program(const char* message)
{
    printf("%s", message);
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

