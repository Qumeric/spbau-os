#ifndef __IO_H_
#define __IO_H_

#include <stddef.h>
#include <stdarg.h>

int printf (const char* fmt, ...);
int vprintf(const char* fmt, va_list args);

int snprintf (char* buffer, size_t buf_size, const char* fmt, ...);
int vsnprintf(char* buffer, size_t buf_size, const char* fmt, va_list args);

#endif /* IO_H */
