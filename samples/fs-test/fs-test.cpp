#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
//#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    //FAT::Flash fat("C", 0x0004'0000); // 256kB
    for (;;) {
        // any job
        Tickable::Tick();
    }
}
