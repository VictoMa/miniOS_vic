










/*********************lib/slb.asm********************/

PUBLIC void*    MemCopy(void* pDest,void* pSrc,int size);
PUBLIC void     MemSet(void* pDest,char ch,int size);

PUBLIC void     DispStr(char* str);
PUBLIC u8       inByte(u16 port);
PUBLIC void     outByte(u16 port,u8 value);
PUBLIC void     DispColorStr(char* str, int color);

PUBLIC char*    strCpy(char* pDest, char* pSrc);
PUBLIC void     disableIRQ(int irq);
PUBLIC void     enableIRQ(int irq);




/********************* kernel/protect.c ******************/
PUBLIC void     initProtect();
PUBLIC u32      seg2phys(u16 seg);


/********************** kernel/i8259.c *******************/
PUBLIC void     init8259A();
PUBLIC void     exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);
PUBLIC void     setIrqHandler(int irq,irqHandler handler);
PUBLIC void     clockHandler(int irq);
PUBLIC void     spuriousIrq(int irq);

PUBLIC void     sys_getTicks();

/**************** lib/lib.c ***************/
PUBLIC void     delay(int time);
PUBLIC void     DispInt(int number);

/********************** kernel/kernel.asm *******************/
void    restart();

void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();

/********************** kernel/main.c *******************/
void TestA();
void TestB();
void TestC();


void init8253();

/********************** kernel/proc.c *******************/

void schedule();



/********************** kernel/syscall.asm *******************/

void sys_Call();
void getTicks();
