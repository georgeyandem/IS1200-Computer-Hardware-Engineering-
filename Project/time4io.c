#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 

//Alexander och George (lab)

int getsw(void)
{
    int switches = (PORTD>>8) & 0x000f; //last 4 LSB ska vara 1 (16-bits) 8 till 11 genom shift
                                    // 0000 1111 1110 0000 >>> 0000 0000 0000 1111
    return switches; 
}

int getbtns(void) 
{
    int button = (PORTD>>5) & 0x0007; //0000 1111 1110 0000 --- 0000 0111 1111 --- 0000 0000 0111 // 7,6,5 bits
                                        //7 eftersom vi använder 3 button
    return button;
}
