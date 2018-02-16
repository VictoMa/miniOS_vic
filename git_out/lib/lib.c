#include "const.h"
#include "type.h"
#include "proto.h"




/*******************
 * in this file we define these functions
 * 
 * PUBLIC       char*   itoa(char* str, int num)
 * PUBLIC       void    DispInt(int number)
 * PUBLIC       void    delay(int time)
 * 
 * 
 * and we define these global variables
 * 
 * 
 * *****************/



PUBLIC char* itoa(char* str, int num)
{
    char* p = str;
    char ch;
    int i;
    int flag = 0;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0)
    {
        *p++ = '0';
    }
    else
    {
        for (i = 28; i >= 0; i -= 4)
        {
            ch = (num >> i) & 0xF;
            if (flag || (ch > 0))
            {
                flag = 1;
                ch += '0';
                if (ch > '9')
                {
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }

    *p = 0;

    return str;
}

PUBLIC void DispInt(int number)
{
    char strOut[16];
    itoa(strOut, number);
    DispStr(strOut);
}

PUBLIC void delay(int time)
{
    int i, j, k;
    for (k = 0; k < time; k++)
    {
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 100; j++)
            {
                ;
            }
        }
    }
}



