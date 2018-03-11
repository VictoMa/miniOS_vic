

/*********************lib/slb.asm********************/

PUBLIC void *MemCopy(void *pDest, void *pSrc, int size);
PUBLIC void MemSet(void *pDest, char ch, int size);

PUBLIC void DispStr(char *str);
PUBLIC u8 inByte(u16 port);
PUBLIC void outByte(u16 port, u8 value);
PUBLIC void DispColorStr(char *str, int color);

PUBLIC char *strCpy(char *pDest, char *pSrc);
PUBLIC void disableIRQ(int irq);
PUBLIC void enableIRQ(int irq);

PUBLIC void disableINT();
PUBLIC void enableINT();
PUBLIC int strLen(char* p_str);

/********************* kernel/protect.c ******************/
PUBLIC void initProtect();
PUBLIC u32 seg2phys(u16 seg);

/********************** kernel/i8259.c *******************/
//int about
PUBLIC void init8259A();
PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);
PUBLIC void setIrqHandler(int irq, irqHandler handler);
PUBLIC void clockHandler(int irq);
PUBLIC void spuriousIrq(int irq);

//syscall about
PUBLIC int sys_getTicks();

PUBLIC void milliDelay(int milliSec);

PUBLIC void init8253();

/**************** lib/lib.c ***************/
PUBLIC void delay(int time);
PUBLIC void DispInt(int number);

PUBLIC char* itoa(char* str, int num);

/**************** lib/io.c ***************/
PUBLIC int printf(const char *fmt, ...);
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args);

PUBLIC int sys_write(char *buf, int len, PCB *p_proc);
PUBLIC int sprintf(char *buf, const char *fmt, ...);

/**************** lib/assert.c ***************/
PUBLIC void spin(char *func_name);
PUBLIC void assertion_failure(char *exp,char *file,char* base_file, int line);
PUBLIC void panic(const char* fmt, ... );

/********************** kernel/kernel.asm *******************/
void restart();

void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

/********************** kernel/main.c *******************/
void TestA();
void TestB();
void TestC();

/********************** kernel/proc.c *******************/

void schedule();

/********************** kernel/syscall.asm *******************/

PUBLIC void sys_Call();
PUBLIC void getTicks();
PUBLIC void write(char *buf, int len);

/********************** kernel/keyboard.c *******************/

PUBLIC void initKeyboard();
PUBLIC void keyboardHandler(int irq);
PUBLIC void keyboardRead(TTY *p_tty);

/********************** kernel/tty.c *******************/

PUBLIC void taskTTY();
PUBLIC void vga_processKey(TTY *p_tty, u32 realKey);

PUBLIC void outChar(Console *p_cons, char ch);
PUBLIC int isCurrentConsole(Console *p_cons);

PUBLIC void selectConsole(int indexOfConsole);
PUBLIC void setVideoStartAddr(u32 addr);

PUBLIC void initConsole(TTY *p_tty);

PUBLIC void ttyWriteChar(TTY *p_tty, char *buf, int len);
//PUBLIC void ttyRead();
//PUBLIC void ttyWrite(TTY* p_tty);


/********************** kernel/ipc.c *******************/

PUBLIC void reset_msg(Message *p);


PUBLIC void *va2la(int pid, void *va);
PUBLIC int ldt_seg_linear(PCB *p, int idx);


PUBLIC void dump_proc(PCB *p);
PUBLIC void dump_msg(const char *title, Message *m);


PUBLIC int send_recv(int function, int src_dest, Message *msg);
PUBLIC int sys_sendrec(int function, int src_dest, Message *m, PCB *p);
/********************** kernel/systask.c *******************/


PUBLIC void taskSYS();