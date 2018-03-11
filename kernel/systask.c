#include "type.h"
#include "const.h"
#include "global.h"
#include "proto.h"

PUBLIC void taskSYS()
{
    Message msg;
    while (1)
    {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

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
}

PUBLIC int m_getTicks()
{
    Message msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(SEND_RECV,TASK_SYS,&msg);
    return msg.RETVAL;
}