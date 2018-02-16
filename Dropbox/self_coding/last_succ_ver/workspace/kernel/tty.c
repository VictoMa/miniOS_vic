#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"

#define TTY_FIRST (ttyTable)
#define TTY_END (ttyTable + NR_TTY)

PRIVATE void setCursor(unsigned int position);
PRIVATE void ttyRead(TTY *p_tty);
PRIVATE void ttyWrite(TTY *p_tty);
PRIVATE void initTTY(TTY *p_tty);
PRIVATE void rollScr();

int testCount = 0;

PUBLIC void taskTTY()
{
    TTY *p_tty;
    initKeyboard();

    for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
    {
        initTTY(p_tty);
    }

    //the first tty
    indexCurrentConsole = 0;
    while (1)
    {
        for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++)
        {
            //           DispInt(8);
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
                rollScr();
            }
            break;
        case DOWN:
            if ((realKey & FLAG_SHIFT_L) || (realKey & FLAG_SHIFT_R))
            {
                /* Shift+Down, do nothing */
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
    p_tty->p_console = consoleTable + (int)(p_tty - ttyTable);
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
    if (!testCount)
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

PRIVATE void rollScr()
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
    u8 *p_video_mem = (u8 *)(V_MEM_BASE + Cursor);

    *p_video_mem++ = ch;                 //write raw byte to video mem to display
    *p_video_mem++ = DEFAULT_CHAR_COLOR; //the high byte

    Cursor += 2;

    setCursor(Cursor / 2);
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