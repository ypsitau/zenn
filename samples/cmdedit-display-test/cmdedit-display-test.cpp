#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
    ::stdio_init_all();
    GPIO::KeyboardMatrix keyboard;
    const Keyboard::KeySet keySetTbl[] = {
        VK_1,    VK_2, VK_3,     VK_BACK,
        VK_4,    VK_5, VK_6,     VK_UP,
        VK_7,    VK_8, VK_9,     VK_DOWN,
        VK_LEFT, VK_0, VK_RIGHT, VK_RETURN,
    };
    const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
    const GPIO::KeyCol keyColTbl[] = {
        GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up()
    };
    keyboard.Initialize(keySetTbl,
        keyRowTbl, count_of(keyRowTbl),
        keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate0);
    terminal.SetFont(Font::shinonome16)
        .AttachDisplay(display).AttachKeyboard(keyboard);
    terminal.Println("ReadLine Test Program");
    for (;;) {
        char* str = terminal.ReadLine(">");
        terminal.Printf("%s\n", str);
    }
}
