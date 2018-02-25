#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"

PUBLIC void panic(const char* fmt, ... )
{
    int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printf("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

PUBLIC void assertion_failure(char *exp,char *file,char* base_file, int line)
{
	printf("\n%c  assert(%s) failed: file: %s, base_file: %s, ln:%d\n",
	       MAG_CH_ASSERT,
	       exp, file, base_file, line);

    spin("assertion_failure!");

    __asm__ __volatile__("ud2");
    
}


PUBLIC void spin(char *func_name)
{
    printf("\nspinning in %s ...\n", func_name);
    while(1)
    {
        ;
    }
}

