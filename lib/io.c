#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"

PUBLIC int printf(const char *fmt, ...)
{
    int i;
    char buf[256];

    va_list arg = (va_list)((char *)(&fmt) + 4); /*4是参数fmt所占堆栈中的大小*/
    i = vsprintf(buf, fmt, arg);
    write(buf, i);

    return i;
}

PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p;
    char tmp[256];
    va_list p_next_arg = args;

    for (p = buf; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
            *p++ = *fmt;
            continue;
        }

        fmt++;

        switch (*fmt)
        {
        case 'x':
            itoa(tmp, *((int *)p_next_arg));
            strCpy(p, tmp);
            p_next_arg += 4;
            p += strLen(tmp);
            break;
        case 's':
            break;
        default:
            break;
        }
    }

    return (p - buf);
}

PUBLIC int sys_write(char *buf, int len, PCB *p_proc)
{
    ttyWriteChar(&ttyTable[p_proc->nr_tty], buf, len);
    return 0;
}