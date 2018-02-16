#include "type.h"
#include "global.h"
#include "const.h"
#include "proto.h"




void schedule()
{
    PCB* p;
    int greatestTick = 0;
    if (!no_schedule)
    {
        while(!greatestTick)
        {
            for(p=procTable;p<procTable+NR_TASK;p++)
            {
                if(p->ticks>greatestTick)
                {
                    greatestTick=p->ticks;
                    p_proc_ready=p;
                }
            }
            //比较指针的值，对特定的进程赋值，也可以吧
            if(!greatestTick)
            {
                for(p = procTable;p<procTable+NR_TASK;p++)
                {
                    p->ticks = p->priority;
                }
            }
        }
    }
    else
    {
        no_schedule = 0;
    }
}