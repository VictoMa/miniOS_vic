#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

void kernel_main()
{
    DispStr("______ kernel main starts ________\n");

    Task *p_task_proc = taskTable;
    PCB *p_proc = procTable;
    char *p_task_stack = taskStack + STACK_SIZE_TOTAL; //base of the stack space
                                                       //start to allocate stack for each process here
    u16 selector_ldt = SELECTOR_LDT_FIRST;

    int i;
    u8 rpl;
    u8 privilege;
    int eflags;

    for (i = 0; i < NR_TASK + NR_PROCESS; i++)
    {
        if (i < NR_TASK)
        {
            p_task_proc = taskTable + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202;
        }
        else
        {
            p_task_proc = userProcessTable - NR_TASK + i;
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202;
        }
        strCpy(p_proc->pname, p_task_proc->name);
        p_proc->pid = i;
        p_proc->nr_tty = 0;

        p_proc->ldtSel = selector_ldt;

        //use gdt[kernel_code_seg] to init ldt[0]
        MemCopy(&(p_proc->ldt[0]), &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p_proc->ldt[0].attr1 = DA_C | privilege << 5; // change the DPL
        //and the ldt[1]
        MemCopy(&(p_proc->ldt[1]), &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p_proc->ldt[1].attr1 = DA_DRW | privilege << 5; // change the DPL

        //ldt[0] -> cs
        //0 is ldtIndex << 3
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        //ldt[1] -> other regs
        //8 is ldtIndex << 3
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task_proc->initialEip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags; // IF=1, IOPL=1, bit 2 is always 1.

        p_task_stack -= p_task_proc->stackSize;
        p_proc++;
        p_task_proc++;
        selector_ldt += 1 << 3;
    }

    procTable[0].ticks = procTable[0].priority = 100;
    procTable[1].ticks = procTable[1].priority = 10;
    procTable[2].ticks = procTable[2].priority = 10;
    procTable[3].ticks = procTable[3].priority = 10;

    procTable[1].nr_tty = 0;
    procTable[2].nr_tty = 1;
    procTable[3].nr_tty = 1;

    //debug here
    testCount = 0;

    k_reenter = 0;
    ticks = 0;
    no_schedule = -1;

    p_proc_ready = procTable;

    //    DispStr("GOING!\n");

    init8253();
    //   initKeyboard();

    restart();

    while (1)
    {
        ;
    }
}

void TestA()
{
    //int i = 0;
    while (1)
    {
       // printf("<Ticks:%x>", sys_getTicks());
        milliDelay(2000);
    }
}

void TestB()
{
    //int i = 1000;
    while (1)
    {

        printf("B");
        milliDelay(20000);
    }
}

void TestC()
{
    //int i = 2000;
    while (1)
    {
        assert(0);
        printf("C");
        
        milliDelay(20000);
    }
}
