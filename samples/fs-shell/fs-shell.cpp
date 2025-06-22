#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash driveA("A:",  0x1010'0000, 0x0004'0000); // Flash address and size 256kB
    LFS::Flash driveB("B:",  0x1014'0000, 0x0004'0000); // Flash address and size 256kB
    LFS::Flash driveC("*C:", 0x1018'0000, 0x0004'0000); // Flash address and size 256kB
    FAT::Flash driveD("D:",  0x101c'0000, 0x0004'0000); // Flash address and size 256kB
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    for (;;) {
        // :
        // any other jobs
        // :
        Tickable::Tick();
    }
}
