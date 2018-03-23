#include "type.h"
#include "const.h"
#include "global.h"
#include "proto.h"

PUBLIC void taskSYS()
{
    Message msg;
    //printf("in taskSys");
    if (0)
    {
        DispStr("\n\n\n\n\nin task sys");
    }
    while (1)
    {
 //      printf("going send_recv");
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

//        printf("going switch");
        switch (msg.type)
        {
        case GET_TICKS:
            msg.RETVAL = ticks;
            send_recv(SEND, src, &msg);
            break;
        default:
            panic("unknown msg type");
            break;
        }
    }
    while(1)
        ;
}

PUBLIC int m_getTicks()
{
    Message msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(SEND_RECV,TASK_SYS,&msg);
    return msg.RETVAL;
}