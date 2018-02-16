org	07c00h		;加载到07c00h

BaseOfStack		equ     07c00h                             
                                                          
BaseOfSetup		equ	09000h
OffsetOfSetup		equ	0h
                
MessageLength		equ	9




;定义的常量
;采用写二进制代码的形式硬拷贝到磁盘映像上面去



;磁盘的映像形式如下
;boot.bin | loader.bin | kernel.bin
;   1		17          剩余大小
;以 512B 大小的块为单位			





jmp	short LABEL_START
nop


LABEL_START:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,BaseOfStack

    push dword 012345678h

    jmp $

times	510-($-$$)	db	0
dw	0xaa55
