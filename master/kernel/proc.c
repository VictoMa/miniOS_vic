#include "type.h"
#include "global.h"
#include "const.h"
#include "proto.h"




void schedule()
{
    p_proc_ready++;
    if(p_proc_ready>=procTable+NR_TASK)
    {
        p_proc_ready=procTable;
    }
}