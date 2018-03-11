%include "sconst.inc"


;use in file syscall.asm


INT_VECTOR_SYS_CALL     equ 0x90
                                            ;these number of syscalls
_NR_syscall_get_ticks   equ 0               ;must keep with global.c
_NR_syscall_write       equ 1
_NR_syscall_sendRecv    equ 2


;-------------------导入函数----------------
extern save

;-------------------导入全局变量----------------

extern syscallTable
extern p_proc_ready

;-------------------导出----------------
global getTicks
global write

global sys_Call



bits 32
[SECTION .text]




getTicks:
    mov     eax,_NR_syscall_get_ticks
    int     INT_VECTOR_SYS_CALL
    ret

write:
    mov     eax,_NR_syscall_write
    mov     ebx,[esp+4]
    mov     ecx,[esp+8]
    int     INT_VECTOR_SYS_CALL
    ret

sendrec:
	mov	    eax, _NR_syscall_sendRecv
	mov	    ebx, [esp + 4]	; function
	mov	    ecx, [esp + 8]	; src_dest
	mov	    edx, [esp + 12]	; p_msg
	int	    INT_VECTOR_SYS_CALL
	ret









sys_Call:
    call save
    push dword [p_proc_ready]
    sti

    push ecx
    push ebx
    call [syscallTable+eax*4]
    add  esp,4*3

    mov [esi+EAXREG-P_STACKBASE],eax
    cli
    ret
    
