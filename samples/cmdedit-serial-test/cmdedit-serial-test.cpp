#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

int main()
{
    ::stdio_init_all();
    terminal.Initialize();
    terminal.AttachPrintable(Stdio::Instance).AttachKeyboard(Stdio::GetKeyboard());
    terminal.Println("ReadLine Test Program");
    for (;;) {
        char* str = terminal.ReadLine(">");
        terminal.Printf("%s\n", str);
    }
}
