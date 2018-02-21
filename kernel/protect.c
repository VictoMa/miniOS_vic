#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

/*******************
 * in this file we define these functions
 * 
 * PUBLIC	void	initProtect()
 * PUBLIC   void    exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
 * PUBLIC   u32     seg2phys(u16 seg)
 * 
 * PRIVATE  void    initIdtDescriptor(u8 intVec,u8 descType,intHandler handler,u8 privilege)
 * PRIVATE  void    initDescriptor(Descriptor *pDesc,u32 Base,u32 limit,u16 attribute);
 * 
 * 
 * 
 * *****************/


PRIVATE void initIdtDescriptor(u8 intVec,u8 descType,intHandler handler,u8 privilege);
PRIVATE void initDescriptor(Descriptor *pDesc,u32 Base,u32 limit,u16 attribute);




PRIVATE void initIdtDescriptor(u8 intVec,u8 descType,intHandler handler,u8 privilege)
{
    Gate *pGate = &idt[intVec];
    u32 base = (u32)handler;
    pGate->offsetLow = base & 0xffff;
    pGate->offsetHigh = (base >> 16) & 0xffff;

    pGate->dcount = 0;

    pGate -> attr = descType | (privilege << 5);

    pGate -> selector = SELECTOR_KERNEL_CS;


}

PRIVATE void initDescriptor(Descriptor *pDesc,u32 Base,u32 limit,u16 attribute)
{
	pDesc->limitLow = limit & 0xffff;
	pDesc->limitHigh_attr2 = (((limit >> 16) & 0x0f) | ((attribute >>8) & 0xf0));

	pDesc->baseHigh = (Base >> 24) & 0x0ff;
	pDesc->baseMid = (Base >> 16) & 0x0ff;
	pDesc->baseLow = Base & 0xffff;

	pDesc->attr1 = attribute & 0xff;
}

PUBLIC	void	initProtect()
{
	init8259A();

	//init all the IDT descriptor to IGate （Hardware int)
	initIdtDescriptor(INT_VECTOR_DIVIDE,DA_386IGate,divide_error,PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_DEBUG,		DA_386IGate, single_step_exception,	PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_NMI,		DA_386IGate, nmi,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpoint_exception,	PRIVILEGE_USER);
	initIdtDescriptor(INT_VECTOR_OVERFLOW,	DA_386IGate, overflow,			PRIVILEGE_USER);
	initIdtDescriptor(INT_VECTOR_BOUNDS,	DA_386IGate, bounds_check,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_INVAL_OP,	DA_386IGate, inval_opcode,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_COPROC_NOT,	DA_386IGate, copr_not_available,	PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate, double_fault,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_COPROC_SEG,	DA_386IGate, copr_seg_overrun,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_INVAL_TSS,	DA_386IGate, inval_tss,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_SEG_NOT,	DA_386IGate, segment_not_present,	PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_STACK_FAULT,	DA_386IGate, stack_exception,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_PROTECTION,	DA_386IGate, general_protection,	PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_PAGE_FAULT,	DA_386IGate, page_fault,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_COPROC_ERR,	DA_386IGate, copr_error,		PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 0,	DA_386IGate, hwint00,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 1,	DA_386IGate, hwint01,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 2,	DA_386IGate, hwint02,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 3,	DA_386IGate, hwint03,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 4,	DA_386IGate, hwint04,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 5,	DA_386IGate, hwint05,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 6,	DA_386IGate, hwint06,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ0 + 7,	DA_386IGate, hwint07,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 0,	DA_386IGate, hwint08,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 1,	DA_386IGate, hwint09,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 2,	DA_386IGate, hwint10,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 3,	DA_386IGate, hwint11,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 4,	DA_386IGate, hwint12,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 5,	DA_386IGate, hwint13,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 6,	DA_386IGate, hwint14,			PRIVILEGE_KERNEL);
	initIdtDescriptor(INT_VECTOR_IRQ8 + 7,	DA_386IGate, hwint15,			PRIVILEGE_KERNEL);


	//syscall
	initDescriptor(INT_VECTOR_SYSCALL,DA_386IGate,sys_Call,PRIVILEGE_USER);


	MemSet(&tss,0,sizeof(tss));			//all bit to 0
	tss.ss0 = SELECTOR_KERNEL_DS;
	initDescriptor(&gdt[INDEX_TSS],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS),&tss),
			sizeof(tss)-1,
			DA_386TSS);
	tss.iobase=sizeof(tss);




	//f
	int i=0;
	PCB* p_proc = procTable;
	u16 selector_ldt = INDEX_LDT_FIRST <<3 ;

	for(i=0;i<NR_TASK+NR_PROCESS;i++)
	{
		initDescriptor(&gdt[selector_ldt>>3],
		vir2phys(seg2phys(SELECTOR_KERNEL_DS), procTable[i].ldt),
		LDT_SIZE * sizeof(Descriptor) - 1,
		DA_LDT);

		p_proc++;
		selector_ldt+= 1<<3 ;
	}



}


PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
	int i;
	int text_color = 0x74; /* 灰底红字 */
	char err_description[][64] = {	"#DE Divide Error",
					"#DB RESERVED",
					"—  NMI Interrupt",
					"#BP Breakpoint",
					"#OF Overflow",
					"#BR BOUND Range Exceeded",
					"#UD Invalid Opcode (Undefined Opcode)",
					"#NM Device Not Available (No Math Coprocessor)",
					"#DF Double Fault",
					"    Coprocessor Segment Overrun (reserved)",
					"#TS Invalid TSS",
					"#NP Segment Not Present",
					"#SS Stack-Segment Fault",
					"#GP General Protection",
					"#PF Page Fault",
					"—  (Intel reserved. Do not use.)",
					"#MF x87 FPU Floating-Point Error (Math Fault)",
					"#AC Alignment Check",
					"#MC Machine Check",
					"#XF SIMD Floating-Point Exception"
				};

	/* 通过打印空格的方式清空屏幕的前五行，并把 cursor 清零 */
	Cursor = 0;
	for(i=0;i<80*5;i++){
		DispStr(" ");
	}
	Cursor = 0;

	DispColorStr("Exception! --> ", text_color);
	DispColorStr(err_description[vec_no], text_color);
	DispColorStr("\n\n", text_color);
	DispColorStr("EFLAGS:", text_color);
	DispInt(eflags);
	DispColorStr("CS:", text_color);
	DispInt(cs);
	DispColorStr("EIP:", text_color);
	DispInt(eip);

	if(err_code != 0xFFFFFFFF){
		DispColorStr("Error code:", text_color);
		DispInt(err_code);
	}
}



PUBLIC u32 seg2phys(u16 seg)
{
	Descriptor* p_dest = &gdt[seg >> 3];
	return (p_dest->baseHigh<<24 | p_dest->baseMid<<16 | p_dest->baseLow);
}
