#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"

//when call printf => we use vsprintf to handle the parameter
//=>then use the syscall 'write' to print to the certain(caller) console
//the syscall write calls ttywrite,in this way we protect the iostream

//use in this way : printf("%fmt1%fmt2,....",arg1,arg2,....)

PUBLIC int printf(const char *fmt, ...)
{
    int i;
    char buf[256];

    //skip the fmt para in stack
    //fmt is 4B in stack
    //then take the head addr of var list
    va_list arg = (va_list)((char *)(&fmt) + 4);

    //vsprintf write args in defined format to buf
    //then return the length of writen contents (in byte)
    i = vsprintf(buf, fmt, arg);
    write(buf, i);

    return i;
}

PRIVATE char* i2a(int val, int base, char **ps)
{
    int m = val % base;
    int q = val / base;
    if (q)
    {
        i2a(q, base, ps);
    }
    *(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

    return *ps;
}

PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p;
    char tmp[256];

    //save the arg list pointer
    va_list p_next_arg = args;

    int m, k;
    char cs;
    char inner_buf[STR_DEFAULT_LEN];
    int nr_align;

    //if the fmt not point to '%',it is to print
    //else if fmt point to '%',the following is the format

    for (p = buf; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
            *p++ = *fmt;
            continue;
        }
        else //here is one format
        {
            nr_align = 0;
        }
        fmt++;

        if (*fmt == '%')
        {
            *p++ = *fmt;
            continue;
        }
        else if (*fmt == '0')
        {
            cs = '0';
            fmt++;
        }
        else
        {
            cs = ' ';
        }

        while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9'))
        {
            nr_align *= 10;
            nr_align += *fmt - '0';
            fmt++;
        }

        char *q = inner_buf;
        MemSet(q, 0, sizeof(inner_buf));

        switch (*fmt)
        {
        case 'c':
            *q++ = *((char *)p_next_arg);
            p_next_arg += 4;
            break;
        case 'x':
            m = *((int *)p_next_arg);
            i2a(m, 16, &q);
            p_next_arg += 4;
            break;
        case 'd':
            m = *((int *)p_next_arg);
            if (m < 0)
            {
                m = m * (-1);
                *q++ = '-';
            }
            i2a(m, 10, &q);
            p_next_arg += 4;
            break;
        case 's':
            strCpy(q, (*((char **)p_next_arg)));
            q += strLen(*((char **)p_next_arg));
            p_next_arg += 4;
            break;
        default:
            break;
        }

        for (k = 0; k < ((nr_align > strLen(inner_buf)) ? (nr_align - strLen(inner_buf)) : 0); k++)
        {
            *p++ = cs;
        }
        q = inner_buf;
        while (*q)
        {
            *p++ = *q++;
        }
    }

    //without format indicater,how may chars to print
    return (p - buf);
}

//write to tty of a process
//start from 'buf' , for 'len' chars
PUBLIC int sys_write(char *buf, int len, PCB *p_proc)
{
    ttyWriteChar(&ttyTable[p_proc->nr_tty], buf, len);
    return 0;
}

PUBLIC int sys_printx(int _unused1, int _unused2, char *s, PCB *p_proc)
{
    const char *p;
    char ch;

    char reenter_err[] = "? k_reenter is incorrect for unknown reason";
    reenter_err[0] = MAG_CH_PANIC;

    /**
	 * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
	 * If this happens in Ring 0, no linear-physical address mapping
	 * is needed.
	 *
	 * @attention The value of `k_reenter' is tricky here. When
	 *   -# printx() is called in Ring 0
	 *      - k_reenter > 0. When code in Ring 0 calls printx(),
	 *        an `interrupt re-enter' will occur (printx() generates
	 *        a software interrupt). Thus `k_reenter' will be increased
	 *        by `kernel.asm::save' and be greater than 0.
	 *   -# printx() is called in Ring 1~3
	 *      - k_reenter == 0.
	 */
    if (k_reenter == 0) /* printx() called in Ring<1~3> */
        p = va2la(proc2pid(p_proc), s);
    else if (k_reenter > 0) /* printx() called in Ring<0> */
        p = s;
    else /* this should NOT happen */
        p = reenter_err;

    /**
	 * @note if assertion fails in any TASK, the system will be halted;
	 * if it fails in a USER PROC, it'll return like any normal syscall
	 * does.
	 */
    if ((*p == MAG_CH_PANIC) ||
        (*p == MAG_CH_ASSERT && p_proc_ready < &procTable[NR_TASK]))
    {
        disableINT();
        char *v = (char *)V_MEM_BASE;
        const char *q = p + 1; /* +1: skip the magic char */

        while (v < (char *)(V_MEM_BASE + V_MEM_SIZE))
        {
            *v++ = *q++;
            *v++ = RED_CHAR;
            if (!*q)
            {
                while (((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16))
                {
                    /* *v++ = ' '; */
                    v++;
                    *v++ = GRAY_CHAR;
                }
                q = p + 1;
            }
        }

        __asm__ __volatile__("hlt");
    }

    while ((ch = *p++) != 0)
    {
        if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
            continue; /* skip the magic char */

        outChar(ttyTable[p_proc->nr_tty].p_console, ch);
    }

    return 0;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list arg = (va_list)((char *)(&fmt) + 4); /* 4 是参数 fmt 所占堆栈中的大小 */
    return vsprintf(buf, fmt, arg);
}