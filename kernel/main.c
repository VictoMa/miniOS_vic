#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"




void kernel_main()
{
    DispStr("______ kernel main starts ________\n");

    Task*   p_task          = taskTable;
    PCB*    p_proc          = procTable;
    char*   p_task_stack    = taskStack+STACK_SIZE_TOTAL;       //base of the stack space
                                                                //start to allocate stack for each process here
    u16     selector_ldt    = SELECTOR_LDT_FIRST;               

    int     i;
    for(i=0;i<NR_TASK;i++)
    {
        strCpy(p_proc->pname,p_task->name);
        p_proc->pid = i;

        p_proc->ldtSel = selector_ldt;


        //use gdt[kernel_code_seg] to init ldt[0]
        MemCopy(&(p_proc->ldt[0]), &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p_proc->ldt[0].attr1 = DA_C | PRIVILEGE_TASK << 5; // change the DPL
        //and the ldt[1]
        MemCopy(&(p_proc->ldt[1]), &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p_proc->ldt[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5; // change the DPL

        //ldt[0] -> cs
        //0 is ldtIndex << 3
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        //ldt[1] -> other regs
        //8 is ldtIndex << 3
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;


        p_proc->regs.eip = (u32)p_task->initialEip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.





        p_task_stack -= p_task->stackSize;
        p_proc++;
        p_task++;
        selector_ldt += 1<<3;
    }

    procTable[0].ticks = procTable[0].priority = 100;
	procTable[1].ticks = procTable[1].priority =  10;
	procTable[2].ticks = procTable[2].priority =  10;
    procTable[3].ticks = procTable[3].priority =  10;

    //debug here
    testCount = 0;

    k_reenter = 0;
    ticks = 0;
    no_schedule = -1;

    p_proc_ready=procTable;

//    DispStr("GOING!\n");

    
    init8253();
 //   initKeyboard();


    restart();

    while(1)
    {
        ;
    }

}



void TestA()
{
	//int i = 0;
	while(1)
    {
		//DispStr("A.");
       // delay(10);
        milliDelay(10);
	}
}

void TestB()
{
	//int i = 1000;
	while(1)
    {
		//DispStr("B.");
		//DispInt(i++);
		//DispStr(".");
        //delay(10);
        milliDelay(10);
	}
}

void TestC()
{
	//int i = 2000;
	while(1)
    {
		//DispStr("C.");
		//DispInt(i++);
		//DispStr(".");
        //delay(10);
        milliDelay(10);
	}
}

