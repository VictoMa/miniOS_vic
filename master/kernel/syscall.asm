#include "sconst.inc"





;-------------------导入函数----------------
extern save

;-------------------导入全局变量----------------

extern syscallTable

;-------------------导出----------------
global getTicks
global sys_Call



bits 32
[SECTION .text]




getTicks:
    mov     eax,_NR_syscall_get_ticks
    int     INT_VECTOR_SYS_CALL
    ret


sys_Call:
    call save
    sti
    call [syscallTable+eax*4]
    mov [esi+EAXREG-P_STACKBASE],eax
    cli
    ret
    
