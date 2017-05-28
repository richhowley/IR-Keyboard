/*
    IR_KEYBOARD.H
*/

const long  PRE_DATA = 0x20df0000;    // prefix for remote command

enum COMMAND {
            up,
            down,
            left,
            right,
            ok
};

int tv_codes[] = {
                    0x02FD,       // up
                    0x827D,       // down
                    0xE01F,       // left
                    0x609F,       // right
                    0x22DD        // ok 

                    
};
