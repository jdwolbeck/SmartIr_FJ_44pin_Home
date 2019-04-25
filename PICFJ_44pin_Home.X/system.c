#include <xc.h> // include processor files - each processor file is guarded.  
#include "system.h"

void delay(int ms)
{
    int i,j;
    for(i = 2*ms; i > 0; i--)
    {
        for(j = 284; j > 0; j--);
    }
}
