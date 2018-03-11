

times	100h	db	0
jmp	LABEL_START
%include "header_setup.inc"


LABEL_GDT:          	Descriptor 0,              0, 0          
LABEL_DESC_FLAT_RW: 	Descriptor 0,         0fffffh, DA_DRW  | DA_32 | DA_LIMIT_4K       
LABEL_DESC_FLAT_C32:  	Descriptor 0,         0fffffh, DA_CR  | DA_32 | DA_LIMIT_4K   
LABEL_DESC_VIDEO:   	Descriptor 0B8000h,   0ffffh, DA_DRW  | DA_DPL3  

GdtLen		equ		$-LABEL_GDT
GdtPtr		dw		GdtLen-1
			dd		BaseOfSetup*10h+LABEL_GDT


SelectorFlatCode32		equ LABEL_DESC_FLAT_C32-LABEL_GDT
SelectorFlatRW			equ LABEL_DESC_FLAT_RW-LABEL_GDT
SelectorVideo			equ	LABEL_DESC_VIDEO-LABEL_GDT+ SA_RPL3



;[SECTION .stack]
;[BITS 32]
;LABLE_STACK:
;times 512 db 0

;TopOfStack equ $-LABEL_DESC_STACK-1






LABEL_START:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,BaseOfStack


;显示字符串
	mov		dh,0
	call	DispStrInRealMode
	


;软驱复位
	xor	ah,ah
	xor	dl,dl
	int	13h



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;加载Kernel
	push	ax
	push	es
	push	di
	
;设置ES:BX为缓冲区地址

	mov		ax,BaseOfKernelFile
	mov		es,ax
	mov		bx,OffsetOfKernelFile

	call	ReadKernel
	
	pop		di
	pop		es
	pop		ax	



	call	KillMotor

	mov		dh,1
	call	DispStrInRealMode	

	mov		dh,3
	call	DispStrInRealMode	





;;get memory size here
	mov		ebx,0
	mov		di,_MemInfoBuff
.getMemorySize:
	mov 	eax,0e820h
	mov		ecx,20
	mov		edx,0534d4150h
	int		15h
	jc		.getMemoryFail
	add		di,20
	inc		dword [_MemSegNumber]
	cmp		ebx,0
	jne		.getMemorySize
	jmp		.getMemoryFinish
.getMemoryFail:
	mov		dword [_MemSegNumber],0
.getMemoryFinish:





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;进入保护模式

	lgdt	[GdtPtr]
	cli

	in	al, 92h
	or	al, 00000010b
	out	92h, al	

	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax
	
	jmp		dword SelectorFlatCode32:(BaseOfSetupPhy+PM_START)






Message:	db	"LOADING  "
Message1	db	"READY    "
Message2	db	"ERROR    "
Message3	db	"REALMODE "



DispStrInRealMode:
	mov	ax,MessageLength
	mul	dh
	add	ax,Message
	mov	bp,ax
	mov	ax,ds
	mov	es,ax
	mov	cx,MessageLength
	mov	ax,01301h
	mov	bx,0007h
	mov	dl,0
	int	10h
	ret


KillMotor:
	push	dx
	mov	dx, 03F2h
	mov	al, 0
	out	dx, al
	pop	dx
	ret




;Boot     在0面0道1扇区	大小1
;Loader   在0面0道2扇区	大小17

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;假设Kernel大小不超过126KB
;则共需要读取252个扇区	软盘扇区是磁头交替编号，注意计算公式
;读Kernel    读252个扇区  -> es:bx
;循环14次  = 252sector = 126KB


ReadKernel:
	push	ax
	push	cx
	push	dx

	push	bp
	mov		bp,sp
	sub		esp,2

;前7次读63K
;
;第八次循环时，读取第三个扇区时，读取超过64K，改变es
;第一次，后面六次只需修改dh,ch与bx         读 1盘面 0柱面 1扇区 共计18个扇区
	mov		ah,02h		;读扇区的功能
	mov		al,12h		;读取的数目
	mov		ch,00h		;磁道号（柱面）
	mov		cl,01h		;起始扇区号
	mov		dh,1h		;磁头号（盘面）
	mov		dl,0h		;驱动器号
	int		13h
	add		bx,2400h

	mov		ah,02h		;第二次 读 0盘面 1柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,01h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第三次 读 1盘面 1柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,01h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第四次 读 0盘面 2柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,02h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第五次 读 1盘面 2柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,02h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第六次 读 0盘面 3柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,03h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第七次 读 1盘面 3柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,03h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第八次-1 读 0盘面 4柱面 1扇区 共计2个扇区
	mov		al,02h		
	mov		ch,04h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,400h

	push	ax				;改变es
	mov		ax,es
	add		ax,1000h
	mov		es,ax
	pop		ax

	mov		ah,02h		;第八次-2 读 0盘面 4柱面 3扇区 共计16个扇区
	mov		al,10h		
	mov		ch,04h
	mov		cl,03h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2000h

	mov		ah,02h		;第九次 读 1盘面 4柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,04h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第十次 读 0盘面 5柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,05h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第十一次 读 1盘面 5柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,05h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第十二次 读 0盘面 6柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,06h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第十三次 读 1盘面 6柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,06h
	mov		cl,01h
	mov		dh,1h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	mov		ah,02h		;第十四次 读 0盘面 7柱面 1扇区 共计18个扇区
	mov		al,12h		
	mov		ch,07h
	mov		cl,01h
	mov		dh,0h		
	mov		dl,0h		
	int		13h
	add		bx,2400h

	add		esp,2
	pop		bp

	pop		dx
	pop		cx
	pop		ax


	ret
[SECTION .s32]
[BITS 32]
PM_START:
	mov	ax, SelectorVideo
	mov	gs, ax
	mov	ax, SelectorFlatRW
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, TopOfStack


	call	GetMemSize

	call	DispMemInfo
;	push	dword [MemSize]
;	call	DispInt
;	add		esp,4

	call	SetupPaging

	mov	ah, 0Fh				; 0000: 黑底    1111: 白字
	mov	al, 'P'
	mov	[gs:((80 * 0 + 39) * 2)], ax	; 屏幕第 0 行, 第 39 列。

;	jmp $
;;move the kernel

	call	initKernel
;;jmp to kernel

	jmp	SelectorFlatCode32:PhyAddrOfKernelEntryPoint
;;;;;;;;init kernel

;;get memorysize
GetMemSize:
	push	esi
	push	edi
	push	ecx

	mov		esi,MemInfoBuff
	mov		ecx,[MemSegNumber]
	mov		eax,[MemSize]
.countLoop:
	add		esi,16			;TYPE
	cmp		dword[esi],1
	je		.sumMem
	add		esi,4			;next
	dec		ecx
	jnz		.countLoop
	jmp		.getOver
.sumMem:
	dec		ecx
	sub		esi,8			;LengthLow
	add		eax,[esi]
	add		esi,12			;next
	cmp		ecx,0
	jnz		.countLoop
.getOver:
	mov		[MemSize],eax

	pop		ecx
	pop		edi
	pop		esi

	ret



;;;;;setup paging
;;;		eax		total memory size
;;;		ebx		memory per page
;;;		ecx		page number
;;;		edx		keep the remainder

;;所有页表连续存放，页目录也连续存放
SetupPaging:
	xor	edx,edx
	mov	eax,[MemSize]
	mov	ebx,400000h		;4M/page

	div	ebx		

	mov	ecx,eax
	test edx,edx
	jz	.no_remainder
	inc	ecx


.no_remainder:
	push	ecx



;;use 'stosd' to init page table & page dir
;prepare to init page dir

	mov	ax,SelectorFlatRW
	mov	es,ax
	mov	edi,PageDirBase
	xor	eax,eax
	mov	eax,PageTableBase|PG_P|PG_USU|PG_RWW    ;present,user,r/w/x

;loop to init page dir
.initPageDir:
	stosd
	add	eax,4096		;next page table
	loop .initPageDir



;prepare to init page table
	pop	eax
	mov	ebx,1024
	mul	ebx		;eax * ebx -> eax
	mov	ecx,eax
	mov	edi,PageTableBase			;flat
	xor	eax,eax
	mov	eax,PG_P|PG_USU|PG_RWW


.initPageTable:
	stosd
	add eax,4096			;next page
	loop .initPageTable


	mov	eax,PageDirBase
	mov	cr3,eax
	mov	eax,cr0
	or	eax,80000000h
	mov	cr0,eax
	jmp	short .startPgFin

.startPgFin:
	nop
	ret





;;some useful functions
;memcopy
;void* MemCopy(void* es:pdest ,void* ds:psrc, int size)
;


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

; ------------------------------------------------------------------------
; 显示 AL 中的数字
; ------------------------------------------------------------------------
DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [Cursor]

	mov	ah, 0Fh			; 0000b: 黑底    1111b: 白字
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.begin:
	and	al, 01111b
	cmp	al, 9
	ja	.1
	add	al, '0'
	jmp	.2
.1:
	sub	al, 0Ah
	add	al, 'A'
.2:
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	.begin
	;add	edi, 2

	mov	[Cursor], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
; DispAL 结束-------------------------------------------------------------


; ------------------------------------------------------------------------
; 显示一个整形数
; ------------------------------------------------------------------------
DispInt:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 16
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 8
	call	DispAL

	mov	eax, [esp + 4]
	call	DispAL

	mov	ah, 07h			; 0000b: 黑底    0111b: 灰字
	mov	al, 'h'
	push	edi
	mov	edi, [Cursor]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[Cursor], edi
	pop	edi

	ret
; DispInt 结束------------------------------------------------------------

; ------------------------------------------------------------------------
; 显示一个字符串
; ------------------------------------------------------------------------
DispStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; pszInfo
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
; DispStr 结束------------------------------------------------------------

; ------------------------------------------------------------------------
; 换行
; ------------------------------------------------------------------------
DispReturn:
	push	strReturn
	call	DispStr			;printf("\n");
	add	esp, 4

	ret
; DispReturn 结束---------------------------------------------------------


;-------------------------------------------------------------------------
;
;DispMemoryInfo
;
;-------------------------------------------------------------------------
DispMemInfo:
	push	esi
	push	edi
	push	ecx

	push	strMemTitle		;
	call	DispStr			;printf("RAM size:");
	add	esp, 4			;

	mov	esi, MemInfoBuff
	mov	ecx, [MemSegNumber]	;for(int i=0;i<[MCRNumber];i++) // 每次得到一个ARDS(Address Range Descriptor Structure)结构
.loop:					;{
	mov	edx, 5			;	for(int j=0;j<5;j++)	// 每次得到一个ARDS中的成员，共5个成员
	mov	edi, structARD		;	{			// 依次显示：BaseAddrLow，BaseAddrHigh，LengthLow，LengthHigh，Type
.1:					;
	push	dword [esi]		;
	call	DispInt			;		DispInt(MemChkBuf[j*4]); // 显示一个成员
	pop	eax			;
	stosd				;		ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4			;
	dec	edx			;
	cmp	edx, 0			;
	jnz	.1			;	}
	call	DispReturn		;	printf("\n");
	cmp	dword [Type], 1	;	if(Type == AddressRangeMemory) // AddressRangeMemory : 1, AddressRangeReserved : 2
	jne	.2			;	{
	mov	eax, [BaseAddrLow]	;
	add	eax, [LengthLow]	;
	cmp	eax, [MemSize]	;		if(BaseAddrLow + LengthLow > MemSize)
	jb	.2			;
	mov	[MemSize], eax	;			MemSize = BaseAddrLow + LengthLow;
.2:					;	}
	loop	.loop			;}
					;
	call	DispReturn		;printf("\n");
	push	strRAMSize		;
	call	DispStr			;printf("RAM size:");
	add	esp, 4			;
					;
	push	dword [MemSize]	;
	call	DispInt			;DispInt(MemSize);
	add	esp, 4			;

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------
;;
;;;;;;;;;;;;;;;;;;;;;;;;;
;;initKernel
initKernel:
	xor	esi,esi
	mov	cx,word [BaseOfKernelFilePhy+2ch];program header number -> cx
	movzx	ecx,cx
	mov	esi,[BaseOfKernelFilePhy+1ch];
	add	esi,BaseOfKernelFilePhy;  esi-> program header table
								; in first loop means proHeaderTable[0]
.CopyLoop:
	mov	eax,[esi+0]
	cmp	eax,0
	jz	.NullType		;if type==0 do nothing
	push	dword	[esi+10h]; the size of current seg   "p_filesize"

	mov	eax,[esi+04h]			;offset of this seg to the begining of elf file
	add	eax,BaseOfKernelFilePhy	
	push	eax			;the src

	push	dword	[esi+08h]	;virtual addr
								;for it is a flat seg 
								;virtual addr == liner addr == phy addr
								;the dst
	
	call MemCopy
	add	esp,12			;clean the stack
.NullType:
	add	esi,020h;esi->next program header
	dec	ecx
	jnz	.CopyLoop ;still have program header

	ret






[SECTION .data1]
;;use _VARNAME in real mode       VARNAME in protect mode
;;string end with '\0'
;;;;;;;;;Strings	to display

LABEL_DATA:

_strRAMSize: db "RAMSIZE:",0
_strReturn:	db 0ah,0
_strMemTitle:	db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0

;;;;;;;;;Vars & structs
_MemSegNumber:	dd	0
_MemSize:	dd	0
_MemInfo:	dd	0
_Cursor:		dd	(80*6+0)*2		;row 6, column 0

_structARD:
	_BaseAddrLow:	dd	0
	_BaseAddrHigh:	dd	0
	_LengthLow:		dd	0
	_LengthHigh:	dd	0
	_Type:			dd	0

_MemInfoBuff:	times	100h	db	0


;;in protect mode
strRAMSize	equ	BaseOfSetupPhy+_strRAMSize
strReturn	equ BaseOfSetupPhy+_strReturn
strMemTitle equ BaseOfSetupPhy+_strMemTitle
MemSegNumber	equ	BaseOfSetupPhy+_MemSegNumber
MemSize		equ BaseOfSetupPhy+_MemSize
MemInfo		equ BaseOfSetupPhy+_MemInfo
Cursor		equ BaseOfSetupPhy+_Cursor
structARD	equ BaseOfSetupPhy+_structARD
	BaseAddrLow	equ BaseOfSetupPhy+_BaseAddrLow
	BaseAddrHigh	equ BaseOfSetupPhy+_BaseAddrHigh
	LengthLow		equ BaseOfSetupPhy+_LengthLow
	LengthHigh	equ BaseOfSetupPhy+_LengthHigh
	Type			equ BaseOfSetupPhy+_Type
MemInfoBuff		equ BaseOfSetupPhy+_MemInfoBuff


;put stack at the end of the data segment
times	1000h	db	0
TopOfStack equ BaseOfSetupPhy+$

;;here is a magic number to fill setup.bin to size 17*512 = 8704 B 

times	2784	db	0