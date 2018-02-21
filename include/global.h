


/********************************************************
 * vars below are defined in 'global.c'
 * else
 * they will be taken as extern declaration
 * 
 * 
 * this file defines some vars used in different files
 * such as the IDT pointer / GDT pointer & IDT / GDT
 * also the cursor on current tty , ect.
 * 
 * 
 * 
 * 
 * ******************************************************/
#ifdef GLOBAL_VARS_HERE
#undef EXTERN
#define EXTERN
#endif


//cursor
EXTERN int  Cursor;

//int re_enter 
EXTERN u32  k_reenter;
//gdt & gdt pointer
EXTERN u8 gdt_ptr[6];
EXTERN Descriptor gdt[GDT_SIZE];

//idt & idt pointer
EXTERN u8 idt_ptr[6];
EXTERN Descriptor idt[IDT_SIZE];

//TSS
EXTERN TSS tss;

//process
EXTERN PCB* p_proc_ready;



//syscall

EXTERN int ticks;

EXTERN int no_schedule;

                                    //these are declarations of vars defined in "global.c"
extern PCB  procTable[];
extern char taskStack[];
extern Task taskTable[];
extern Task userProcessTable[];
extern irqHandler irqTable[];
extern sysCall syscallTable[];


//keyboard & tty
EXTERN int indexCurrentConsole;    
extern TTY ttyTable[];
extern Console consoleTable[];

extern KB_INPUT keyboardIn;




//debug

EXTERN int testCount;