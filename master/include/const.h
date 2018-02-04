#ifndef __CONST_H__
#define __CONST_H__


/****************************  scope ****************************/

#define PUBLIC
#define PRIVATE static
#define EXTERN  extern


/*********************GDT size & GDT selector********************/

#define GDT_SIZE    128

//index
#define INDEX_DUMMY 0
#define INDEX_FLAT_RW    1
#define INDEX_FLAT_C     2
#define INDEX_VIDEO 3
#define	INDEX_TSS		4
#define	INDEX_LDT_FIRST		5

//selector

#define SELECTOR_DUMMY  0x0
#define SELECTOR_FLAT_RW    0x08
#define SELECTOR_FLAT_C     0x10
#define SELECTOR_VIDEO  (0x18+0x3)
#define	SELECTOR_TSS		0x20	/* TSS                       */
#define SELECTOR_LDT_FIRST	0x28


#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO

/****************************************************************/




/***************** attr for descriptor&selector *****************/

//descriptor

#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/

//存储段描述符类型值说明
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
//系统段描述符类型值说明 
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/



//selector
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

/****************************************************************/



/************************** privilege ***************************/

//different rings
#define PRIVILEGE_KERNEL    0
#define PRIVILEGE_TASK      1
#define PRIVILEGE_USER      3


//RPL
#define RPL_KERNEL  0
#define RPL_TASK    1
#define RPL_USER    3

/****************************************************************/





/*********************IDT size & IDT selector********************/
#define IDT_SIZE    256


/****************************************************************/






/*************************  LDT size ****************************/
#define LDT_SIZE    2


/****************************************************************/



/***************************i8259********************************/

//io port 

#define INT_M_CTL20   0x20
#define INT_M_CTL21   0x21
#define INT_S_CTLA0   0xA0
#define INT_S_CTLA1   0xA1


//int vector
//hardware
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

//8259
#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28



//syscall

#define INT_VECTOR_SYSCALL      0x90



//about irq table =====> hardware int
#define NR_IRQ      16

#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7


#define	AT_WINI_IRQ	14	/* at winchester */


/****************************************************************/


/***************************i8253********************************/
#define TIMER0          0x40
#define TIMER_MODE      0x43
#define RATE_GEN        0x34
#define FREQ_IN         200000L     //questioning whether it can be changed
#define HZ              5



/****************************************************************/

/*****************************  macros **************************/

//this macro convert vir addr to phy addr
//simply add the seg_base and the offset together ???
#define vir2phys(seg_base, vir)	(u32)(((u32)seg_base) + (u32)(vir))

/****************************************************************/



/*****************************  logic **************************/
//logic 
#define TRUE 1
#define FALSE 0

/****************************************************************/



/************************** process ****************************/
//about process

#define NR_TASK 3           //number of tasks

//about testX
#define STACK_SIZE_TESTA    0x8000
#define STACK_SIZE_TESTB    0x8000
#define STACK_SIZE_TESTC    0x8000


#define STACK_SIZE_TOTAL    (STACK_SIZE_TESTA*NR_TASK)







/****************************************************************/





/************************ syscall *******************************/
#define NR_SYSCALL  1


/****************************************************************/

#endif