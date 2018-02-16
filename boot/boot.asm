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


;设置显示样式
	mov	ax,0600h
	mov	bx,0700h
	mov	cx,0
	mov	dx,0184fh
	int	10h

;显示字符串
	mov	dh,0
	call	DispStr
	


;软驱复位
	xor	ah,ah
	xor	dl,dl
	int	13h



;加载Loader
	push	ax
	push	es
	push	di
	
	mov	ax,BaseOfSetup
	mov	es,ax
	mov	bx,OffsetOfSetup

	call	ReadLoader
	
	pop	di
	pop	es
	pop	ax	




	mov	dh,1
	call	DispStr	
	
	jmp	BaseOfSetup:OffsetOfSetup





;Boot     在0面0道1扇区

;读Loader   0面0道2扇区 17个扇区
ReadLoader:
	push	ax
	push	cx
	push	dx

	push	bp
	mov	bp,sp
	sub	esp,2



	mov		ah,02h		;读扇区的功能
	mov		al,11h		;读取的数目


	mov		ch,00h		;磁道号（柱面）
	mov		cl,02h		;起始扇区号


	mov		dh,0h		;磁头号（盘面）
	mov		dl,0h		;驱动器号
	int		13h


	add	esp,2
	pop	bp

	pop	dx
	pop	cx
	pop	ax


	ret










BootMessage:	db	"BOOTING  "
Message1	db	"READY    "
Message2	db	"ERROR    "




DispStr:
	mov	ax,MessageLength
	mul	dh
	add	ax,BootMessage
	mov	bp,ax
	mov	ax,ds
	mov	es,ax
	mov	cx,MessageLength
	mov	ax,01301h
	mov	bx,0007h
	mov	dl,0
	int	10h
	ret





times	510-($-$$)	db	0
dw	0xaa55
