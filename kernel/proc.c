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
        while (!greatestTick)
        {
            for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
            {
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
                for (p = &FIRST_PROC; p <= &LAST_PROC; p++)
                    if (p->p_flags == 0)
                        p->ticks = p->priority;

                        
        }
    }
    else
    {
        no_schedule = 0;
    }
}

PUBLIC int sys_sendrec(int function, int src_dest, Message *m, PCB *p)
{
    ;
}