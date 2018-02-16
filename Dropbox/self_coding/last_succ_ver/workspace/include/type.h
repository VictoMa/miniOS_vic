#include "const.h"


#ifndef __TYPE_H__
#define __TYPE_H__


/*********************good names********************/
typedef void (*intHandler) ();      //intHandler p-> void* fun()
typedef void (*taskEnterPoint) ();	//taskEnterPoint p -> void* taskEnter()
typedef void (*irqHandler) (int irq); //irqHandler p -> void* intHandler(int)
typedef void* sysCall;			//sysCall p -> void    
								//which refers to any type of function

/*********************unsigned int********************/
typedef	unsigned int		u32;    //32bit int
typedef	unsigned short		u16;    //16bit
typedef	unsigned char		u8;     //8bit


/*********************struct in protect mode********************/
typedef struct s_desc           //descriptor
{
    u16 limitLow;
    u16 baseLow;
    u8  baseMid;
    u8  attr1;
    u8  limitHigh_attr2;
    u8  baseHigh;
}Descriptor;

typedef struct s_gate       //gate
{
    u16 offsetLow;
    u16 selector;
    u8  dcount;
    u8  attr;
    u16 offsetHigh;
}Gate;

typedef struct s_stackFrame		//stackFrame defines the stack while switching task
{								//we save the status of current process in this form
    u32 gs;
    u32	fs;		/* |                                    */
	u32	es;		/* |                                    */
	u32	ds;		/* |                                    */
	u32	edi;		/* |                                    */
	u32	esi;		/* | pushed by save()                   */
	u32	ebp;		/* |                                    */
	u32	kernel_esp;	/* <- 'popad' will ignore it            */
	u32	ebx;		/* |                                    */
	u32	edx;		/* |                                    */
	u32	ecx;		/* |                                    */
	u32	eax;		/* /                                    */
	u32	retaddr;	/* return addr for kernel.asm::save()   */
	u32	eip;		/* \                                    */
	u32	cs;		/* |                                    */
	u32	eflags;		/* | pushed by CPU during interrupt     */
	u32	esp;		/* |                                    */
	u32	ss;		/* /                                    */
}StackFrame;

typedef struct s_pcb  //PCB
{
    StackFrame regs;
    u16 ldtSel;
    Descriptor ldt[LDT_SIZE];


    u32 pid;
    char pname[16];
	int ticks;
	int priority;
} PCB;


typedef struct s_tss //TSS
{
	u32	backlink;
	u32	esp0;	/* stack pointer to use during interrupt */
	u32	ss0;	/*   "   segment  "  "    "        "     */
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
}TSS;



typedef struct s_task
{
	taskEnterPoint initialEip;
	int	stackSize;
	char name[32];
}Task;


typedef struct s_kb 
{
	char*	p_head;			/* 指向缓冲区中下一个空闲位置 */
	char*	p_tail;			/* 指向键盘任务应处理的字节 */
	int		count;			/* 缓冲区中共有多少字节 */
	char	buf[KB_IN_BYTES];	/* 环形缓冲区 */
}KB_INPUT;



//console & tty

//a console
typedef struct s_console
{
	u32 startAddr;		//the console start from this add in gs mem
	u32 memoryAddr;    //gs mem
	u32 memLimit;		//mem size
	u32 currentCursor;	//cursor in current console
	
}Console;

typedef struct s_tty
{
	char*	p_in_head;			/* 指向缓冲区中下一个空闲位置 */
	char*	p_in_tail;			/* 指向应处理的字节 */
	int		in_buf_count;			/* 缓冲区中共有多少字节 */
	char	in_buf[TTY_IN_BYTES];	/* 环形缓冲区 */

	Console* p_console; //current console
}TTY;

#endif