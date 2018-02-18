#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

#define TTY_FIRST (ttyTable)
#define TTY_END (ttyTable + NR_TTY)



//static funcs about display
PRIVATE void setCursor(unsigned int position);
PRIVATE void ttyRead(TTY *p_tty);
PRIVATE void ttyWrite(TTY *p_tty);
PRIVATE void initTTY(TTY *p_tty);
PRIVATE void scrollUp();
PRIVATE void scrollScreen();

//int testCount = 0;

PUBLIC void taskTTY()
{
    TTY *p_tty;
    initKeyboard();

    for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
    {
        //DispInt(8);
        initTTY(p_tty);
    }

    //the first tty
    indexCurrentConsole = 0;
    selectConsole(indexCurrentConsole);
    while (1)
    {
        for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
        {
            //DispInt(8);
            ttyRead(p_tty);
            ttyWrite(p_tty);
        }
    }
}

PUBLIC void vga_processKey(TTY *p_tty, u32 realKey)
{
    char outPut[2] = {0, 0}; //string to display
    int rawCode;             //raw code

    if (!(realKey & FLAG_EXT)) //graph char
    {
        // DispInt(realKey);
        // outPut[0] = realKey & 0xFF;
        if (p_tty->in_buf_count < TTY_IN_BYTES)
        {
            *(p_tty->p_in_head) = realKey;
            p_tty->p_in_head++;
            if (p_tty->p_in_head == p_tty->in_buf + TTY_IN_BYTES)
            {
                p_tty->p_in_head = p_tty->in_buf;
            }
            p_tty->in_buf_count++;
        }
    }
    else
    {
        rawCode = realKey & MASK_RAW;

        // DispStr("UP:");DispInt(UP);DispStr("rawcode:");DispInt(rawCode);DispStr("\n");
        switch (rawCode)
        {
        case UP:
            if ((realKey & FLAG_SHIFT_L) || (realKey & FLAG_SHIFT_R))
            {
                //scrollUp();
                scrollScreen(p_tty->p_console,DIRECTION_UP);
            }
            break;
        case DOWN:
            if ((realKey & FLAG_SHIFT_L) || (realKey & FLAG_SHIFT_R))
            {
                scrollScreen(p_tty->p_console,DIRECTION_DOWN);
            }
            break;
        case F1:
        case F2:
            //case F3:
            //case F4:
            //case F5:
            //case F6:
            //case F7:
            //case F8:
            if ((realKey & FLAG_SHIFT_L) || (realKey & FLAG_SHIFT_L))
            {
                //##debug here
                //outChar(p_tty->p_console, rawCode - F6 + '0');
                selectConsole(rawCode - F1);
            }
            break;
        default:
            break;
        }
    }
}

PRIVATE void initTTY(TTY *p_tty)
{
    p_tty->in_buf_count = 0;
    p_tty->p_in_head = p_tty->p_in_tail = p_tty->in_buf;

    initConsole(p_tty);
    //p_tty->p_console = consoleTable + (int)(p_tty - ttyTable);
}

PRIVATE void ttyRead(TTY *p_tty)
{
    if (isCurrentConsole(p_tty->p_console))
    {
        keyboardRead(p_tty);
        //       DispInt(p_tty->in_buf_count);
    }
}

PRIVATE void ttyWrite(TTY *p_tty)
{
    //if (!testCount)
    {
        // DispInt(p_tty->in_buf_count);DispStr("   ");
        char outCh;
        char testOut[2] = {0, 0};
        if (p_tty->in_buf_count)
        {
            outCh = *(p_tty->p_in_tail);
            p_tty->p_in_tail++;
            if (p_tty->p_in_tail == p_tty->in_buf + TTY_IN_BYTES)
            {
                p_tty->p_in_tail = p_tty->in_buf;
            } //roll the tail to head
            p_tty->in_buf_count--;
            testOut[0] = outCh;
            //           DispStr(testOut);DispStr("     ");
            //           DispInt(Cursor);DispStr("     ");
            outChar(p_tty->p_console, outCh);

            if (0)
            {
                u8 *p_video_mem = (u8 *)(V_MEM_BASE + Cursor);

                *p_video_mem++ = outCh;              //write raw byte to video mem to display
                *p_video_mem++ = DEFAULT_CHAR_COLOR; //the high byte

                Cursor += 2;

                disableINT();
                outByte(CRTC_ADDR_REG, CURSOR_H);
                outByte(CRTC_DATA_REG, ((Cursor >> 8) & 0xFF)); //start at BYTE 'position'
                outByte(CRTC_ADDR_REG, CURSOR_L);
                outByte(CRTC_DATA_REG, (Cursor & 0xFF));
                enableINT();
            }
        }

        //DispStr("X");

        //testCount++;
    }
}

//some small func

PUBLIC int isCurrentConsole(Console *p_cons)
{
    return (p_cons == &consoleTable[indexCurrentConsole]);
}

PRIVATE void scrollUp()
{
    disableINT();
    outByte(CRTC_ADDR_REG, START_ADDR_H);
    outByte(CRTC_DATA_REG, (80 * 15 >> 8) & 0xFF);
    outByte(CRTC_ADDR_REG, START_ADDR_L);
    outByte(CRTC_DATA_REG, (80 * 15) & 0xFF);
    enableINT();
}

PUBLIC void outChar(Console *p_cons, char ch)
{
    u8 *p_video_mem = (u8 *)(V_MEM_BASE + p_cons->currentCursor * 2); //the vga op is in WORD

    *p_video_mem++ = ch;                 //write raw byte to video mem to display
    *p_video_mem++ = DEFAULT_CHAR_COLOR; //the high byte

    p_cons->currentCursor++;

    setCursor(p_cons->currentCursor);
}

PUBLIC void selectConsole(int indexOfConsole) //console from 0 ~ NR_CONSOLES-1
{
    if ((indexOfConsole >= 0) && (indexOfConsole < NR_CONSOLES))
    {
        indexCurrentConsole = indexOfConsole;
        setCursor(consoleTable[indexCurrentConsole].currentCursor);
        setVideoStartAddr(consoleTable[indexCurrentConsole].startAddr);
    }
}

PUBLIC void setVideoStartAddr(u32 addr)
{
    disableINT();
    outByte(CRTC_ADDR_REG, START_ADDR_H);
    outByte(CRTC_DATA_REG, ((addr >> 8) & 0xFF)); //start at BYTE 'addr'
    outByte(CRTC_ADDR_REG, START_ADDR_L);
    outByte(CRTC_DATA_REG, (addr & 0xFF));
    enableINT();
}
PRIVATE void setCursor(unsigned int position)
{
    disableINT();
    outByte(CRTC_ADDR_REG, CURSOR_H);
    outByte(CRTC_DATA_REG, ((position >> 8) & 0xFF)); //start at BYTE 'position'
    outByte(CRTC_ADDR_REG, CURSOR_L);
    outByte(CRTC_DATA_REG, (position & 0xFF));
    enableINT();
}

PUBLIC void initConsole(TTY *p_tty)
{
    int indexOfCurrentTTY = p_tty - ttyTable;
    p_tty->p_console = &consoleTable[indexOfCurrentTTY]; //index of console is equal to index of tty

    //divide the video ram into equal size,in WORD
    int vRamPerConsole = (V_MEM_SIZE >> 1) / NR_CONSOLES;
    //allocate the gs memory
    p_tty->p_console->memoryAddr = indexOfCurrentTTY * vRamPerConsole; //from high addr -> low addr
    //the console start from the begining of the gs memory of the current console
    p_tty->p_console->startAddr = indexOfCurrentTTY * vRamPerConsole;
    p_tty->p_console->memLimit = vRamPerConsole;
    p_tty->p_console->currentCursor = p_tty->p_console->startAddr; //start from the begining

    //if it is the first console (console #0)
    //use the old position
    if (indexOfCurrentTTY == 0)
    {
        p_tty->p_console->currentCursor = Cursor / 2;
        Cursor = 0; //set the cursor to the begining
    }
    else
    {
        //output some chars
        outChar(p_tty->p_console, '#');
        outChar(p_tty->p_console, indexOfCurrentTTY + '0');
    }
}


/*
|           |     ->high addr
|start addr |
|     +     |
|     +     |
| --------  |
|           |
|           |
|           |
|           |    ->low addr   =>limit
*/
PRIVATE void scrollScreen(Console* p_cons,int direction)
{
    //when UP,the start addr should move to the lower addr
    if(direction == DIRECTION_UP)
    {
        if(p_cons->startAddr>p_cons->memoryAddr) //still have something , not displayed in current console
        {
            p_cons->startAddr-=SCREEN_WIDTH;
        }
    }
    //when DOWN , move to higher addr
    else if(direction == DIRECTION_DOWN)
    {
        //the bottom of current console is above the limit of the gs mem
        if((p_cons->startAddr+SCREEN_SIZE)<(p_cons->memoryAddr+p_cons->memLimit))
        {
            p_cons->startAddr+=SCREEN_WIDTH;
        }
    }
    else
    {
        ;
    }

    setVideoStartAddr(p_cons->startAddr);
    setCursor(p_cons->currentCursor);
}