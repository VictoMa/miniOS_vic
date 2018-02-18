#include "type.h"
#include "const.h"
#include "proto.h"
#include "keymap.h"
#include "global.h"

PRIVATE u8 getByteFromKeyboardIn();

//these vars can be alse defined in type 'BOOLEAN'

PRIVATE int code_with_E0;
PRIVATE int shift_l;     /* l shift state */
PRIVATE int shift_r;     /* r shift state */
PRIVATE int alt_l;       /* l alt state	 */
PRIVATE int alt_r;       /* r left state	 */
PRIVATE int ctrl_l;      /* l ctrl state	 */
PRIVATE int ctrl_r;      /* l ctrl state	 */
PRIVATE int caps_lock;   /* Caps Lock	 */
PRIVATE int num_lock;    /* Num Lock	 */
PRIVATE int scroll_lock; /* Scroll Lock	 */
PRIVATE int column;

PUBLIC void initKeyboard()
{
    keyboardIn.count = 0;
    keyboardIn.p_head = keyboardIn.p_tail = keyboardIn.buf;

    shift_l = shift_r = 0;
    alt_l = alt_r = 0;
    ctrl_l = ctrl_r = 0;

    setIrqHandler(KEYBOARD_IRQ, keyboardHandler);
    enableIRQ(KEYBOARD_IRQ);
}

PUBLIC void keyboardHandler(int irq)
{
    u8 scanCode = inByte(KB_DATA);

    if (keyboardIn.count < KB_IN_BYTES)
    {
        *(keyboardIn.p_head) = scanCode;
        keyboardIn.p_head++;
        if (keyboardIn.p_head == keyboardIn.buf + KB_IN_BYTES)
        {
            keyboardIn.p_head = keyboardIn.buf;
        }
        keyboardIn.count++;
    }

    //DispInt(keyboardIn.count);
}

PUBLIC void keyboardRead(TTY *p_tty)
{
    u8 scanCode;
    //MemSet(outPut,0,2);

    int makeORbreak; /* TRUE: make;  FALSE: break. */
    int isPauseBreak = 0;

    u32 *keymapRow;  //point to one row of the keymap
    int realKey = 0; //the realkey    must be reset to 0 each time

    int i; //counter to read the buf

    u8 pausebrks_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5}; //pausebreak
    u8 prtScr_make_code[] = {0xE0, 0x2A, 0xE0, 0x37};
    u8 prtScr_break_code[] = {0xE0, 0xB7, 0xE0, 0xAA};

    // if(keyboardIn.count!=0)
    //  {
    // DispInt(keyboardIn.count);
    //}
    // DispStr("count:");
    //DispInt(keyboardIn.count);
    // DispStr("\n");

    if (keyboardIn.count > 0) //still have character to do with
    {
        //read the char
        // DispStr("ScanCode:");
        //DispInt(scanCode);
        //DispStr("\n");

        code_with_E0 = 0;
        //DispStr("count:");DispInt(keyboardIn.count);
        scanCode = getByteFromKeyboardIn();

 //       DispStr("ScanCode:");
 //       DispInt(scanCode);
  //      DispStr("    ");
        //process the already read char

        if (scanCode == 0xE1) //control char  => pausebreak
        {
            isPauseBreak = 1;
            for (i = 1; i < 6; i++) //check pausebreak
            {
                if (getByteFromKeyboardIn() != pausebrks_code[i])
                {
                    isPauseBreak = 0;
                    break;
                }
            }
            if (isPauseBreak)
            {
                realKey = PAUSEBREAK;
            }
        }

        //here may be the problem
        if (scanCode == 0xE0) //control char PrtScr / others
        {
            //
            scanCode = getByteFromKeyboardIn();
            // DispStr("E0 code:");DispInt(scanCode);DispStr("    ");

            if (scanCode == prtScr_make_code[1])
            {
                if (getByteFromKeyboardIn() == prtScr_make_code[2])
                {
                    if (getByteFromKeyboardIn() == prtScr_make_code[3])
                    {
                        //all passed

                        realKey = PRINTSCREEN; //printscreen pressed
                        makeORbreak = 1;       //make code
                    }
                }
            }

            //it's wired that only when i release the prtscr key,it works

            //!!!!!!!!!!!!!!!!!!!!!!!!!  I TAKE THE ASSIGN '=' AS EQUAL '=='
            //but nothing to do with the upper bug
            if (scanCode == prtScr_break_code[1])
            {
                if (getByteFromKeyboardIn() == prtScr_break_code[2])
                {
                    if (getByteFromKeyboardIn() == prtScr_break_code[3])
                    {
                        //all passed

                        realKey = PRINTSCREEN; //printscreen released
                        //DispStr("count");DispInt(keyboardIn.count);DispStr("\n");
                        makeORbreak = 0; //break code
                    }
                }
            }

            //if not prtscr or pausebrk , then must be others key with E0
            if (realKey == 0)
            {
                // DispStr("E0");
                code_with_E0 = 1;
            }
        }

        // DispStr("Now:");
        // DispInt(realKey);
        // DispStr("stand:");
        // DispInt(PRINTSCREEN);




        // another if condition!!,logic mistake

        if ((realKey != PAUSEBREAK) && (realKey != PRINTSCREEN)) //deal with the simpler ones
        {
            // DispStr("debug //\\\\");
            //make code or break code
            //            DispStr("sc:");DispInt(scanCode);DispStr("   ");
            makeORbreak = (scanCode & FLAG_BREAK ? FALSE : TRUE);
            //           DispStr("mb:");DispInt(makeORbreak);DispStr("   ");
            //look up the keymap

            //the row is in fact the index of the char in the keymap
            //when use in this way => keymapRow[column]
            //it is compiled into the form 'pointer+offset'
            //which equals to the 2 dim way:  keymap[row][column]
            keymapRow = &keymap[(scanCode & KEYMAP_MASK) * MAP_COLS]; //row
                                                                      //           DispStr("row:");DispInt((keymapRow-keymap)/3);DispStr("   ");
            column = 0;                                               //column

            //shift ones in column 1
            if (shift_l || shift_r)
            {
                column = 1;
            }
            //E0 ones in column 2
            if (code_with_E0)
            {
                column = 2;
                code_with_E0 = 0; //clear the flag for the next input
            }

            //            DispStr("col:");DispInt(column);DispStr("   ");
            realKey = keymapRow[column];

            //           DispStr("rlk:");DispInt(realKey);
            //           DispStr("\n");
            //control keys
            switch (realKey)
            {
            case SHIFT_L:
                shift_l = makeORbreak; //whether X is pressed
                break;
            case SHIFT_R:
                shift_r = makeORbreak;
                break;
            case CTRL_L:
                ctrl_l = makeORbreak;
                break;
            case CTRL_R:
                ctrl_r = makeORbreak;
                break;
            case ALT_L:
                alt_l = makeORbreak;
                break;
            case ALT_R:
                alt_l = makeORbreak;
                break;
            default:
                break;
            }

            if (makeORbreak) //make , display
            {
                int rawCode = realKey & MASK_RAW;


                //DispStr("UP:");DispInt(UP);DispStr("rawcode:");DispInt(rawCode);DispStr("realkey:");DispInt(realKey);DispStr("    ");

                realKey |= shift_l ? FLAG_SHIFT_L : 0;
                realKey |= shift_r ? FLAG_SHIFT_R : 0;
                realKey |= ctrl_l ? FLAG_CTRL_L : 0;
                realKey |= ctrl_r ? FLAG_CTRL_R : 0;
                realKey |= alt_l ? FLAG_ALT_L : 0;
                realKey |= alt_r ? FLAG_ALT_R : 0;

                //char output[2]={0,0};
                //output[0]=realKey;
                // DispStr(output);
                //DispInt(realKey);

                rawCode = realKey & MASK_RAW;

                //               DispStr("UP:");DispInt(UP);DispStr("rawcode:");DispInt(rawCode);DispStr("    ");
                //               DispStr("process     ");
                vga_processKey(p_tty, realKey);
            }
        }
        else
        {
            DispStr("cannot hand");
            //getByteFromKeyboardIn();
        }
    }
}

//it WILL return a char
PRIVATE u8 getByteFromKeyboardIn()
{
    u8 scanCode;

    //wait
    while (keyboardIn.count <= 0)
    {
        ;
    }
    disableINT();
    scanCode = *(keyboardIn.p_tail);
    keyboardIn.p_tail++; //next char to process
    keyboardIn.count--;  //count - 1
    if (keyboardIn.p_tail == keyboardIn.buf + KB_IN_BYTES)
    {
        keyboardIn.p_tail = keyboardIn.buf;
    } //roll the tail
    enableINT();

    return scanCode;
}