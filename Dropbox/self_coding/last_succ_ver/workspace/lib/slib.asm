
;;are these comments?  to modified!!!!!!!!
;/*******************
; * in this file we define these functions
; * 
; * PUBLIC       char*   itoa(char* str, int num)
; * PUBLIC       void    DispInt(int number)
; * PUBLIC       void    delay(int time)
; * 
; * 
; * and we define these global variables
; * 
; * 
; * *****************/


%include "sconst.inc"



;-------------------导出----------------
global MemCopy
global MemSet
global strCpy

global DispStr
global outByte
global inByte
global DispColorStr

global enableIRQ
global disableIRQ

global enableINT
global disableINT

;-------------------导入----------------

extern Cursor

[SECTION .text]

;**************************************************************
;void* MemCopy(void* es:pdest ,void* ds:psrc, int size);
;**************************************************************


MemCopy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
;**************************************************************




;**************************************************************
;void     MemSet(void* pDest,char ch,int size);
;**************************************************************
MemSet:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	edx, [ebp + 12]	; Char to be putted
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	byte [edi], dl		; ┓
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
;**************************************************************










;**************************************************************
;   void DispStr(char* str);
;**************************************************************
DispStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; p2str
	mov	edi, [Cursor]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[Cursor], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret

;**************************************************************



;**************************************************************
;   void DispColorStr(char* str,int color);
;**************************************************************
DispColorStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; p2str
	mov	edi, [Cursor]
	mov	ah, [ebp+12]	;color
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[Cursor], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret

;**************************************************************



;**************************************************************
;void outByte(u16 port,u8 value);
;port->dx
;value->ax
;then use the 'out' instruction
;**************************************************************
outByte:
	mov	edx,[esp+4]
	mov	al,[esp+8]
	out	dx,al
	nop					;delay
	nop
	ret

;**************************************************************
;u8 inByte(u16 port);
;port->dx
;value->al
;take it as return value
;use the 'in' instruction
;**************************************************************
inByte:

	mov	edx,[esp+4]		;port
	xor	eax,eax
	in	al,dx
	nop					;delay
	nop
	ret



; ------------------------------------------------------------------------
; char* strCpy(char* p_dst, char* p_src);
; ------------------------------------------------------------------------
strCpy:
	push    ebp
	mov     ebp, esp

	mov     esi, [ebp + 12] ; Source
	mov     edi, [ebp + 8]  ; Destination

.1:
	mov     al, [esi]               ; ┓
	inc     esi                     ; ┃
					; ┣ 逐字节移动
	mov     byte [edi], al          ; ┃
	inc     edi                     ; ┛

	cmp     al, 0           ; 是否遇到 '\0'
	jnz     .1              ; 没遇到就继续循环，遇到就结束

	mov     eax, [ebp + 8]  ; 返回值

	pop     ebp
	ret                     ; 函数结束，返回
; strcpy 结束-------------------------------------------------------------






; ========================================================================
;                  void disableIRQ(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code:
;	if(irq < 8){
;		out_byte(INT_M_CTL21, in_byte(INT_M_CTL21) | (1 << irq));
;	}
;	else{
;		out_byte(INT_S_CTLA1, in_byte(INT_S_CTLA1) | (1 << irq));
;	}
disableIRQ:
        mov     ecx, [esp + 4]          ; irq
        pushf
        cli
        mov     ah, 1
        rol     ah, cl                  ; ah = (1 << (irq % 8))
        cmp     cl, 8
        jae     disable_8               ; disable irq >= 8 at the slave 8259
disable_0:
        in      al, INT_M_CTL21
        test    al, ah
        jnz     dis_already           ; already disabled?
        or      al, ah
        out     INT_M_CTL21, al       ; set bit at master 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
disable_8:
        in      al, INT_S_CTLA1
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     INT_S_CTLA1, al       ; set bit at slave 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
dis_already:
        popf
        xor     eax, eax                ; already disabled
        ret

; ========================================================================
;                  void enableIRQ(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;       if(irq < 8){
;               out_byte(INT_M_CTL21, in_byte(INT_M_CTL21) & ~(1 << irq));
;       }
;       else{
;               out_byte(INT_S_CTLA1, in_byte(INT_S_CTLA1) & ~(1 << irq));
;       }
;
enableIRQ:
        mov     ecx, [esp + 4]          ; irq
        pushf
        cli
        mov     ah, ~1
        rol     ah, cl                  ; ah = ~(1 << (irq % 8))
        cmp     cl, 8
        jae     enable_8                ; enable irq >= 8 at the slave 8259
enable_0:
        in      al, INT_M_CTL21
        and     al, ah
        out     INT_M_CTL21, al       ; clear bit at master 8259
        popf
        ret
enable_8:
        in      al, INT_S_CTLA1
        and     al, ah
        out     INT_S_CTLA1, al       ; clear bit at slave 8259
        popf
        ret


enableINT:
	sti
	ret


disableINT:
	cli
	ret