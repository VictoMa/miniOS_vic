#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"



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


/*****************************************************************************
 *				  ldt_seg_linear
 *****************************************************************************/
/**
 * <Ring 0~1> Calculate the linear address of a certain segment of a given
 * proc.
 * 
 * @param p   Whose (the proc ptr).
 * @param idx Which (one proc has more than one segments).
 * 
 * @return  The required linear address.
 *****************************************************************************/
PUBLIC int ldt_seg_linear(PCB * p, int idx)
{
	Descriptor * d = &p->ldt[idx];

	return d->baseHigh << 24 | d->baseMid << 16 | d->baseLow;
}



/*****************************************************************************
 *				  va2la
 *****************************************************************************/
/**
 * <Ring 0~1> Virtual addr --> Linear addr.
 * 
 * @param pid  PID of the proc whose address is to be calculated.
 * @param va   Virtual address.
 * 
 * @return The linear address for the given virtual address.
 *****************************************************************************/
PUBLIC void* va2la(int pid, void* va)
{
	PCB* p = &(procTable[pid]);

	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;

	if (pid < NR_TASK + NR_PROCESS) {
		assert(la == (u32)va);
	}

	return (void*)la;
}


