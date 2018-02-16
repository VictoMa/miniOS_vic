
#include "type.h"
#include "proto.h"
#include "const.h"
#include "global.h"



/*******************
 * in this file we define these functions
 * 
 * PUBLIC   void    init8259A()
 * PUBLIC   void    spuriousIrq(int irq)
 * 
 * 
 * and we define these global variables
 * 
 * 
 * *****************/




PUBLIC void init8259A()
{
    //ICW1    0001 0001B   => 级联，有ICW4
    outByte(INT_M_CTL20,0x11);
    //ICW2  0002 0000B   =>  设置中断类型码为20H，对应IR0-IR7 中断号为 20H-27H
    outByte(INT_M_CTL21,INT_VECTOR_IRQ0);
    //ICW3   0000 0100B  => 1片从片 连接在第2引脚 IR2
    outByte(INT_M_CTL21,0x4);
    //ICW4  0000 0001B  =>  非自动结束，非缓冲 8086模式
    outByte(INT_M_CTL21,0x1);

    //屏蔽除 IR0=>时钟 外的所有中断
    outByte(INT_M_CTL21,0xFF);



 //   outByte(INT_M_CTL21,0xFF);

    //从片设置
    outByte(INT_S_CTLA0,0x11);
    outByte(INT_S_CTLA1,INT_VECTOR_IRQ8);
    //ICW3 0000 0010B => 连接主片第(10B) 即第2引脚
    outByte(INT_S_CTLA1,0x2);
    outByte(INT_S_CTLA1,0x1);

    outByte(INT_S_CTLA1,0xFF);

    //init irqTable

    int i=0;
//    irqTable[0]=clockHandler;

    for(i=0;i<NR_IRQ;i++)
    {
        irqTable[i]=spuriousIrq;
    }
}


PUBLIC void spuriousIrq(int irq)
{
    DispStr("oh,the spurious irq:");
    DispInt(irq);
    DispStr("\n");
}


PUBLIC void setIrqHandler(int irq,irqHandler handler)
{
    disableIRQ(irq);
    irqTable[irq] = handler;
}



PUBLIC void clockHandler(int irq)
{
  //  DispInt(ticks);
    p_proc_ready->ticks--;
    ticks++;
    if(k_reenter!=0)
    {
        return;
    }
    else if(p_proc_ready->ticks>0)
    {
        return;
    }
    else    
    {
       // DispStr("#");
        schedule();
        //delay(10);
    }
}   

 



PUBLIC int sys_getTicks()
{
    return ticks;
}



PUBLIC void     milliDelay(int milliSec)
{
    int i = sys_getTicks();
    while(((sys_getTicks()-i)*1000/HZ)<milliSec)
    {
        ;
    }
}


PUBLIC void init8253()
{
    outByte(TIMER_MODE,RATE_GEN);
    outByte(TIMER0,(u8)(FREQ_IN/HZ));
    outByte(TIMER0, (u8) ((FREQ_IN/HZ) >> 8));
    setIrqHandler(CLOCK_IRQ,clockHandler);
    enableIRQ(CLOCK_IRQ);
}