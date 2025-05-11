#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/USBHost.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/Font/shinonome12.h"

using namespace jxglib;

ShellCmd(argtest, "tests command line arguments")
{
    for (int i = 0; i < argc; i++) {
        out.Printf("argv[%d] \"%s\"\n", i, argv[i]);
    }
    return 0;
}

int main()
{
    ::stdio_init_all();
    //-------------------------------------------------------------------------
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    terminal.Println("Shell on Serial Terminal");
    //-------------------------------------------------------------------------
    for (;;) {
        // any jobs
        Tickable::Tick();
    }
}
