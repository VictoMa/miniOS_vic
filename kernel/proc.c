#include "type.h"
#include "global.h"
#include "const.h"
#include "proto.h"

void schedule()
{
    PCB *p;

    int greatestTick = 0;
    
    if (!no_schedule)
    {
        
        //
        while (!greatestTick)
        {
            for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
            {
 //               printf("sheduleing  times:%d\n",((p-procTable)));
  //              printf("sheduleing  p->:%x\n",p->regs.eip);
                if (p->p_flags == 0)
                {
                    if (p->ticks > greatestTick)
                    {
                        greatestTick = p->ticks;
                        p_proc_ready = p;
                        
                    }
                }
            }

            if (!greatestTick)
                //for (p = &procTable[0]; p <= &procTable[NR_TASK + NR_PROCESS - 1]; p++)
                for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
                    if (p->p_flags == 0)
                        p->ticks = p->priority;
            
        }
    }
    else
    {
     //   DispStr("shedule else\n");
     //printf("no_shc\n");
     if (0)
     {
         printf("before shedule  initialEIP_taskTTY->%x\n", taskTable[0].initialEip);
         printf("before shedule  initialEIP_taskSYS->%x\n", taskTable[1].initialEip);
         printf("before shedule  initialEIP_testA->%x\n", userProcessTable[0].initialEip);
         printf("before shedule  initialEIP_testB->%x\n", userProcessTable[1].initialEip);
         printf("before shedule  initialEIP_testC->%x\n", userProcessTable[2].initialEip);
         printf("before shedule  procTable_taskTTY->%x\n", procTable[0].regs.eip);
         printf("before shedule  procTable_taskSYS->%x\n", procTable[1].regs.eip);
         printf("before shedule  procTable_testA->%x\n", procTable[2].regs.eip);
         printf("before shedule  procTable_testB->%x\n", procTable[3].regs.eip);
         printf("before shedule  procTable_testC->%x\n", procTable[4].regs.eip);
     }
     no_schedule = 0;
    }

}
