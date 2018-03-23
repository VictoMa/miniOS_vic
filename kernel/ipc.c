#include "type.h"
#include "const.h"
#include "global.h"
#include "proto.h"



PRIVATE void block(PCB *p);
PRIVATE void unblock(PCB *p);
PRIVATE int deadlock(int src, int dest);
PRIVATE int msg_send(PCB *current, int dest, Message *m);
PRIVATE int msg_receive(PCB *current, int src, Message *m);


/*****************************************************************************
 *                                sys_sendrec
 *****************************************************************************/
/**
 * <Ring 0> The core routine of system call `sendrec()'.
 * 
 * @param function SEND or RECEIVE
 * @param src_dest To/From whom the message is transferred.
 * @param m        Ptr to the Message body.
 * @param p        The caller proc.
 * 
 * @return Zero if success.
 *****************************************************************************/
PUBLIC int sys_sendrec(int function, int src_dest, Message *m, PCB *p)
{
    assert(k_reenter == 0); /* make sure we are not in ring0 */
    assert((src_dest >= 0 && src_dest < NR_TASK + NR_PROCESS) ||
           src_dest == ANY ||
           src_dest == INTERRUPT);

    int ret = 0;
    int caller = proc2pid(p);
    //printf("caller_pid:%d",caller);
    Message *mla = (Message *)va2la(caller, m);
    mla->source = caller;

    assert(mla->source != src_dest);

    /**
	 * Actually we have the third message type: SEND_RECV. However, it is not
	 * allowed to be passed to the kernel directly. Kernel doesn't know
	 * it at all. It is transformed into a SEND followed by a RECEIVE
	 * by `send_recv()'.
	 */
    if (function == SEND)
    {
        ret = msg_send(p, src_dest, m);
        if (ret != 0)
            return ret;
    }
    else if (function == RECEIVE)
    {
        ret = msg_receive(p, src_dest, m);
        if (ret != 0)
            return ret;
    }
    else
    {
        panic("{sys_sendrec} invalid function: "
              "%d (SEND:%d, RECEIVE:%d).",
              function, SEND, RECEIVE);
    }

    return 0;
}

/*****************************************************************************
 *                                send_recv
 *****************************************************************************/
/**
 * <Ring 1~3> IPC syscall.
 *
 * It is an encapsulation of `sendrec',
 * invoking `sendrec' directly should be avoided
 *
 * @param function  SEND, RECEIVE or SEND_RECV
 * @param src_dest  The caller's proc_nr
 * @param msg       Pointer to the Message struct
 * 
 * @return always 0.
 *****************************************************************************/
PUBLIC int send_recv(int function, int src_dest, Message *msg)
{
    int ret = 0;

    if (function == RECEIVE)
        MemSet(msg, 0, sizeof(Message));

    switch (function)
    {
    case SEND_RECV:
        ret = sendrec(SEND, src_dest, msg);
        if (ret == 0)
            ret = sendrec(RECEIVE, src_dest, msg);
        break;
    case SEND:
    case RECEIVE:
        ret = sendrec(function, src_dest, msg);
        break;
    default:
        assert((function == SEND_RECV) ||
               (function == SEND) || (function == RECEIVE));
        break;
    }

    return ret;
}



/*****************************************************************************
 *                                reset_msg
 *****************************************************************************/
/**
 * <Ring 0~3> Clear up a Message by setting each byte to 0.
 * 
 * @param p  The message to be cleared.
 *****************************************************************************/
PUBLIC void reset_msg(Message *p)
{
    MemSet(p, 0, sizeof(Message));
}

/*****************************************************************************
 *                                block
 *****************************************************************************/
/**
 * <Ring 0> This routine is called after `p_flags' has been set (!= 0), it
 * calls `schedule()' to choose another proc as the `proc_ready'.
 *
 * @attention This routine does not change `p_flags'. Make sure the `p_flags'
 * of the proc to be blocked has been set properly.
 * 
 * @param p The proc to be blocked.
 *****************************************************************************/
PRIVATE void block(PCB *p)
{
    assert(p->p_flags);
    schedule();
}

/*****************************************************************************
 *                                unblock
 *****************************************************************************/
/**
 * <Ring 0> This is a dummy routine. It does nothing actually. When it is
 * called, the `p_flags' should have been cleared (== 0).
 * 
 * @param p The unblocked proc.
 *****************************************************************************/
PRIVATE void unblock(PCB *p)
{
    assert(p->p_flags == 0);
}

/*****************************************************************************
 *                                deadlock
 *****************************************************************************/
/**
 * <Ring 0> Check whether it is safe to send a message from src to dest.
 * The routine will detect if the messaging graph contains a cycle. For
 * instance, if we have procs trying to send messages like this:
 * A -> B -> C -> A, then a deadlock occurs, because all of them will
 * wait forever. If no cycles detected, it is considered as safe.
 * 
 * @param src   Who wants to send message.
 * @param dest  To whom the message is sent.
 * 
 * @return Zero if success.
 *****************************************************************************/
PRIVATE int deadlock(int src, int dest)
{
    PCB *p = procTable + dest;
    while (1)
    {
        if (p->p_flags & SENDING)
        {
            if (p->p_sendto == src)
            {
                /* print the chain */
                p = procTable + dest;
                printf("=_=%s", p->pname);
                do
                {
                    assert(p->p_msg);
                    p = procTable + p->p_sendto;
                    printf("->%s", p->pname);
                } while (p != procTable + src);
                printf("=_=");

                return 1;
            }
            p = procTable + p->p_sendto;
        }
        else
        {
            break;
        }
    }
    return 0;
}

/*****************************************************************************
 *                                msg_send
 *****************************************************************************/
/**
 * <Ring 0> Send a message to the dest proc. If dest is blocked waiting for
 * the message, copy the message to it and unblock dest. Otherwise the caller
 * will be blocked and appended to the dest's sending queue.
 * 
 * @param current  The caller, the sender.
 * @param dest     To whom the message is sent.
 * @param m        The message.
 * 
 * @return Zero if success.
 *****************************************************************************/
PRIVATE int msg_send(PCB *current, int dest, Message *m)
{
    PCB *sender = current;
    PCB *p_dest = procTable + dest; /* proc dest */

    assert(proc2pid(sender) != dest);

    /* check for deadlock here */
    if (deadlock(proc2pid(sender), dest))
    {
        panic(">>DEADLOCK<< %s->%s", sender->pname, p_dest->pname);
    }

    if ((p_dest->p_flags & RECEIVING) && /* dest is waiting for the msg */
        (p_dest->p_recvfrom == proc2pid(sender) ||
         p_dest->p_recvfrom == ANY))
    {
        assert(p_dest->p_msg);
        assert(m);

        MemCopy(va2la(dest, p_dest->p_msg),
                  va2la(proc2pid(sender), m),
                  sizeof(Message));
        p_dest->p_msg = 0;
        p_dest->p_flags &= ~RECEIVING; /* dest has received the msg */
        p_dest->p_recvfrom = NO_TASK;
        unblock(p_dest);

        assert(p_dest->p_flags == 0);
        assert(p_dest->p_msg == 0);
        assert(p_dest->p_recvfrom == NO_TASK);
        assert(p_dest->p_sendto == NO_TASK);
        assert(sender->p_flags == 0);
        assert(sender->p_msg == 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == NO_TASK);
    }
    else
    { /* dest is not waiting for the msg */
        sender->p_flags |= SENDING;
        assert(sender->p_flags == SENDING);
        sender->p_sendto = dest;
        sender->p_msg = m;

        /* append to the sending queue */
        PCB *p;
        if (p_dest->q_sending)
        {
            p = p_dest->q_sending;
            while (p->next_sending)
                p = p->next_sending;
            p->next_sending = sender;
        }
        else
        {
            p_dest->q_sending = sender;
        }
        sender->next_sending = 0;

        block(sender);

        assert(sender->p_flags == SENDING);
        assert(sender->p_msg != 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == dest);
    }

    return 0;
}

/*****************************************************************************
 *                                msg_receive
 *****************************************************************************/
/**
 * <Ring 0> Try to get a message from the src proc. If src is blocked sending
 * the message, copy the message from it and unblock src. Otherwise the caller
 * will be blocked.
 * 
 * @param current The caller, the proc who wanna receive.
 * @param src     From whom the message will be received.
 * @param m       The message ptr to accept the message.
 * 
 * @return  Zero if success.
 *****************************************************************************/
PRIVATE int msg_receive(PCB *current, int src, Message *m)
{
    PCB *p_who_wanna_recv = current; /**
						  * This name is a little bit
						  * wierd, but it makes me
						  * think clearly, so I keep
						  * it.
						  */
    PCB *p_from = 0;                 /* from which the message will be fetched */
    PCB *prev = 0;
    int copyok = 0;

    assert(proc2pid(p_who_wanna_recv) != src);

    if ((p_who_wanna_recv->has_int_msg) &&
        ((src == ANY) || (src == INTERRUPT)))
    {
        /* There is an interrupt needs p_who_wanna_recv's handling and
		 * p_who_wanna_recv is ready to handle it.
		 */

        Message msg;
        reset_msg(&msg);
        msg.source = INTERRUPT;
        msg.type = HARD_INT;
        assert(m);
        MemCopy(va2la(proc2pid(p_who_wanna_recv), m), &msg,
                  sizeof(Message));

        p_who_wanna_recv->has_int_msg = 0;

        assert(p_who_wanna_recv->p_flags == 0);
        assert(p_who_wanna_recv->p_msg == 0);
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);

        return 0;
    }

    /* Arrives here if no interrupt for p_who_wanna_recv. */
    if (src == ANY)
    {
        /* p_who_wanna_recv is ready to receive messages from
		 * ANY proc, we'll check the sending queue and pick the
		 * first proc in it.
		 */
        if (p_who_wanna_recv->q_sending)
        {
            p_from = p_who_wanna_recv->q_sending;
            copyok = 1;

            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);
            assert(p_who_wanna_recv->q_sending != 0);
            assert(p_from->p_flags == SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
        }
    }
    else
    {
        /* p_who_wanna_recv wants to receive a message from
		 * a certain proc: src.
		 */
        p_from = &procTable[src];

        if ((p_from->p_flags & SENDING) &&
            (p_from->p_sendto == proc2pid(p_who_wanna_recv)))
        {
            /* Perfect, src is sending a message to
			 * p_who_wanna_recv.
			 */
            copyok = 1;

            PCB *p = p_who_wanna_recv->q_sending;
            assert(p); /* p_from must have been appended to the
				    * queue, so the queue must not be NULL
				    */
            while (p)
            {
                assert(p_from->p_flags & SENDING);
                if (proc2pid(p) == src)
                { /* if p is the one */
                    p_from = p;
                    break;
                }
                prev = p;
                p = p->next_sending;
            }

            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);
            assert(p_who_wanna_recv->q_sending != 0);
            assert(p_from->p_flags == SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
        }
    }

    if (copyok)
    {
        /* It's determined from which proc the message will
		 * be copied. Note that this proc must have been
		 * waiting for this moment in the queue, so we should
		 * remove it from the queue.
		 */
        if (p_from == p_who_wanna_recv->q_sending)
        { /* the 1st one */
            assert(prev == 0);
            p_who_wanna_recv->q_sending = p_from->next_sending;
            p_from->next_sending = 0;
        }
        else
        {
            assert(prev);
            prev->next_sending = p_from->next_sending;
            p_from->next_sending = 0;
        }

        assert(m);
        assert(p_from->p_msg);
        /* copy the message */
        MemCopy(va2la(proc2pid(p_who_wanna_recv), m),
                  va2la(proc2pid(p_from), p_from->p_msg),
                  sizeof(Message));

        p_from->p_msg = 0;
        p_from->p_sendto = NO_TASK;
        p_from->p_flags &= ~SENDING;
        unblock(p_from);
    }
    else
    { /* nobody's sending any msg */
        /* Set p_flags so that p_who_wanna_recv will not
		 * be scheduled until it is unblocked.
		 */
        p_who_wanna_recv->p_flags |= RECEIVING;

        p_who_wanna_recv->p_msg = m;

        if (src == ANY)
            p_who_wanna_recv->p_recvfrom = ANY;
        else
            p_who_wanna_recv->p_recvfrom = proc2pid(p_from);

        block(p_who_wanna_recv);

        assert(p_who_wanna_recv->p_flags == RECEIVING);
        assert(p_who_wanna_recv->p_msg != 0);
        assert(p_who_wanna_recv->p_recvfrom != NO_TASK);
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);
    }

    return 0;
}

/*****************************************************************************
 *                                dump_proc
 *****************************************************************************/
PUBLIC void dump_proc(PCB *p)
{
    char info[STR_DEFAULT_LEN];
    int i;
    int text_color = MAKE_COLOR(GREEN, RED);

    int dump_len = sizeof(PCB);
    
    outByte(CRTC_ADDR_REG, START_ADDR_H);
    outByte(CRTC_DATA_REG, 0);
    outByte(CRTC_ADDR_REG, START_ADDR_L);
    outByte(CRTC_DATA_REG, 0);

    sprintf(info, "byte dump of procTable[%d]:\n", p - procTable);
    DispColorStr(info, text_color);
    for (i = 0; i < dump_len; i++)
    {
        sprintf(info, "%x.", ((unsigned char *)p)[i]);
        DispColorStr(info, text_color);
    }

    /* printf("^^"); */

    DispColorStr("\n\n", text_color);
    sprintf(info, "ANY: 0x%x.\n", ANY);
    DispColorStr(info, text_color);
    sprintf(info, "NO_TASK: 0x%x.\n", NO_TASK);
    DispColorStr(info, text_color);
    DispColorStr("\n", text_color);

    sprintf(info, "ldt_sel: 0x%x.  ", p->ldtSel);
    DispColorStr(info, text_color);
    sprintf(info, "ticks: 0x%x.  ", p->ticks);
    DispColorStr(info, text_color);
    sprintf(info, "priority: 0x%x.  ", p->priority);
    DispColorStr(info, text_color);
    sprintf(info, "pid: 0x%x.  ", p->pid);
    DispColorStr(info, text_color);
    sprintf(info, "name: %s.  ", p->pname);
    DispColorStr(info, text_color);
    DispColorStr("\n", text_color);
    sprintf(info, "p_flags: 0x%x.  ", p->p_flags);
    DispColorStr(info, text_color);
    sprintf(info, "p_recvfrom: 0x%x.  ", p->p_recvfrom);
    DispColorStr(info, text_color);
    sprintf(info, "p_sendto: 0x%x.  ", p->p_sendto);
    DispColorStr(info, text_color);
    sprintf(info, "nr_tty: 0x%x.  ", p->nr_tty);
    DispColorStr(info, text_color);
    DispColorStr("\n", text_color);
    sprintf(info, "has_int_msg: 0x%x.  ", p->has_int_msg);
    DispColorStr(info, text_color);
}

/*****************************************************************************
 *                                dump_msg
 *****************************************************************************/
PUBLIC void dump_msg(const char *title, Message *m)
{
    int packed = 0;
    printf("{%s}<0x%x>{%ssrc:%s(%d),%stype:%d,%s(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)%s}%s", //, (0x%x, 0x%x, 0x%x)}",
           title,
           (int)m,
           packed ? "" : "\n        ",
           procTable[m->source].pname,
           m->source,
           packed ? " " : "\n        ",
           m->type,
           packed ? " " : "\n        ",
           m->u.m3.m3i1,
           m->u.m3.m3i2,
           m->u.m3.m3i3,
           m->u.m3.m3i4,
           (int)m->u.m3.m3p1,
           (int)m->u.m3.m3p2,
           packed ? "" : "\n",
           packed ? "" : "\n" /* , */
    );
}