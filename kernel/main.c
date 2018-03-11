#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

void kernel_main()
{
    DispStr("______ kernel main starts ________\n");

    Task *p_task;
    PCB *p_proc = procTable;
    char *p_task_stack = taskStack + STACK_SIZE_TOTAL; //base of the stack space
                                                       //start to allocate stack for each process here
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    u8 privilege;
    u8 rpl;
    int eflags;
    int i;
    int prio;
    for (i = 0; i < NR_TASK + NR_PROCESS; i++)
    {
        if (i < NR_TASK)
        { /* 任务 */
            p_task = taskTable + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
            prio = 15;
        }
        else
        { /* 用户进程 */
            p_task = userProcessTable + (i - NR_TASK);
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202; /* IF=1, bit 2 is always 1 */
            prio = 5;
        }

        strCpy(p_proc->pname, p_task->name); /* name of the process */
        p_proc->pid = i;                     /* pid */

        p_proc->ldtSel = selector_ldt;
        //use gdt[kernel_code_seg] to init ldt[0]
        MemCopy(&p_proc->ldt[0], &gdt[SELECTOR_KERNEL_CS >> 3],
                sizeof(Descriptor));
        p_proc->ldt[0].attr1 = DA_C | privilege << 5;
        MemCopy(&p_proc->ldt[1], &gdt[SELECTOR_KERNEL_DS >> 3],
                sizeof(Descriptor));
        p_proc->ldt[1].attr1 = DA_DRW | privilege << 5;
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

        p_proc->regs.eip = (u32)p_task->initialEip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->nr_tty = 0;

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;

        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stackSize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    procTable[NR_TASK + 0].nr_tty = 0;
    procTable[NR_TASK + 1].nr_tty = 1;
    procTable[NR_TASK + 2].nr_tty = 1;

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
        printf("<Ticks:%x>", m_getTicks());
        //printf("A");
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
        //assert(0);
        printf("C");

        milliDelay(20000);
    }
}
