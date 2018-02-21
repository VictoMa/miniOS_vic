
#define GLOBAL_VARS_HERE

#include "type.h"
#include "const.h"
#include "global.h"
#include "proto.h"




/*******************
 * in this file we define these functions
 * 
 * and we define these global variables
 * 
 * 
 * *****************/


PUBLIC PCB  procTable[NR_TASK+NR_PROCESS];
PUBLIC char taskStack[STACK_SIZE_TOTAL];


//the task table
PUBLIC Task taskTable[NR_TASK] = 
{
    {taskTTY,STACK_SIZE_TASK_TTY,"TTY"}
};


PUBLIC Task userProcessTable[NR_PROCESS] = 
{
    {TestA,STACK_SIZE_TESTA,"TEST___A"},
    {TestB,STACK_SIZE_TESTB,"TEST__B"},
    {TestC,STACK_SIZE_TESTC,"TEST__C"}
};


PUBLIC intHandler irqTable[NR_IRQ];

PUBLIC sysCall syscallTable[NR_SYSCALL] = {sys_getTicks};


PUBLIC KB_INPUT keyboardIn;


PUBLIC TTY ttyTable[NR_TTY];
PUBLIC Console consoleTable[NR_CONSOLES];



