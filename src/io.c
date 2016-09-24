#include <ioport.h>
#include <stdarg.h>
#include <stddef.h>

static char basefor(char symbolic)
{
    if (symbolic == 'u')
    {
        return 10;
    }
    if (symbolic == 'o')
    {
        return 8;
    } 
    if (symbolic == 'x')
    {
        return 16;
    }
    return 0;
}

static void print_char(char buffered, char* buffer, size_t buf_size, size_t* ps, char c)
{
    (*ps)++;
    if (buffered)
    {
        if (*ps + 1 >= buf_size) 
        {
            return;
        }  
        buffer[*ps] = c;
    } 
    else
    {
        serial_port_char(c);
    }
}

static void print_uint(char buffered, char* buffer, size_t buf_size, size_t* ps, 
                       uint64_t x, char base)
{
    char a[100];
    int i = 0;
    while (x > 0)
    {
        a[i++] = x % base;
        x /= base;   
    }
    if (i == 0)
    {
        a[i++] = 0;
    }
    do
    {
        i--;
        if (a[i] < 10)
        {
            print_char(buffered, buffer, buf_size, ps, '0' + a[i]);
        }
        else
        {
            print_char(buffered, buffer, buf_size, ps, 'a' + a[i] - 10);
        }
    } while (i > 0);
}

static void print_int(char buffered, char* buffer, size_t buf_size, size_t* ps,
                      int64_t x, char base)
{
    if (x < 0)
    {
        print_char(buffered, buffer, buf_size, ps, '-');
        x = -x;
    }
    print_uint(buffered, buffer, buf_size, ps, (uint64_t)x, base);
}

static int generic_printf(char buffered, char* buffer, size_t buf_size, 
                          const char* fmt, va_list args)
{
    size_t ps = 0;
    for (const char* p = fmt; *p != 0; p++)
    {
        if (*p != '%')
        {
            print_char(buffered, buffer, buf_size, &ps, *p);
            continue;   
        }

        p++;
        if (*p == 0)
        {
            break;
        }
        if (*p == '%')
        {
            print_char(buffered, buffer, buf_size, &ps, '%');
            continue;
        } 
        if (*p == 'c')
        {
            print_char(buffered, buffer, buf_size, &ps, *p);    
            continue;
        }
        if (*p == 's')
        {
            for (char* mes = va_arg(args, char*); *mes != 0; mes++)
            {
                print_char(buffered, buffer, buf_size, &ps, *mes);
            }
            continue;
        }
        if (*p == 'd' || *p == 'i')
        {
            int arg = va_arg(args, int);
            print_int(buffered, buffer, buf_size, &ps, arg, 10);
            continue;            
        }
        if (basefor(*p) != 0)
        {
            unsigned int arg = va_arg(args, unsigned int);
            print_uint(buffered, buffer, buf_size, &ps, arg, basefor(*p));
            continue;
        }
        
        if (*p == 'h')
        {
            p++;
            if (*p == 0)
            {
                break;
            }
            if (*p == 'd' || *p == 'i')
            {
                short arg = (short) va_arg(args, int);
                print_int(buffered, buffer, buf_size, &ps, arg, 10);
                continue;
            }
            if (basefor(*p) != 0)
            {
                unsigned short arg = (unsigned short) va_arg(args, unsigned int);
                print_uint(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                continue;
            }   
            
            if (*p == 'h')
            {
                p++;
                if (*p == 0)
                {
                    break;
                }
                if (*p == 'd' || *p == 'i')
                {
                    signed char arg = (signed char) va_arg(args, signed int);
                    print_int(buffered, buffer, buf_size, &ps, arg, 10);
                    continue;
                }
                if (basefor(*p) != 0)
                {
                    unsigned char arg = (unsigned char) va_arg(args, unsigned int);
                    print_uint(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                    continue;
                }   
            }   
            
            continue;
        }
    
        if (*p == 'l')
        {
            p++;
            if (*p == 0)
            {
                break;
            }
            if (*p == 'd' || *p == 'i')
            {
                long arg = va_arg(args, long);
                print_int(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                continue;
            }
            if (basefor(*p) != 0)
            {
                unsigned long arg = va_arg(args, unsigned long);
                print_uint(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                continue;
            }
            
            if (*p == 'l')
            {
                p++;
                if (*p == 0)
                {
                    break;
                }
                if (*p == 'd' || *p == 'i')
                {
                    long long arg = va_arg(args, long long);
                    print_int(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                    continue;
                }
                if (basefor(*p) != 0)
                {
                    unsigned long long arg = va_arg(args, unsigned long long);
                    print_uint(buffered, buffer, buf_size, &ps, arg, basefor(*p));
                    continue;
                }
            }

            continue;
        }
    }
 
    if (buffered)
    {
        buffer[ps] = 0;
    }
    return (int)ps;
}

int vprintf(const char* fmt, va_list args)
{
    return generic_printf(0, 0, 0, fmt, args);
}

int vsnprintf(char* buffer, size_t buf_size, const char* fmt, va_list args)
{
    return generic_printf(1, buffer, buf_size, fmt, args);
}

int printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int printed = vprintf(fmt, args);
    va_end(args);

    return printed;
}

int snprintf(char* buffer, size_t buf_size, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int printed = vsnprintf(buffer, buf_size, fmt, args);
    va_end(args);

    return printed;
}

