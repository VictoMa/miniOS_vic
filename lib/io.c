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

PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *p;
    char tmp[256];

    //save the arg list pointer
    va_list p_next_arg = args;

    //if the fmt not point to '%',it is to print
    //else if fmt point to '%',the following is the format

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
        //convert to strings
            itoa(tmp, *((int *)p_next_arg));
            strCpy(p, tmp);

            p_next_arg += 4;
            //how many chars should print
            p += strLen(tmp);
            break;
        case 's':
            break;
        default:
            break;
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