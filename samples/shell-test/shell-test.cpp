#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/USBHost.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    //-------------------------------------------------------------------------
    //Display::Terminal terminal;
    //USBHost::Initialize();
    //USBHost::Keyboard keyboard;
    //::spi_init(spi1, 125 * 1000 * 1000);
    //GPIO14.set_function_SPI1_SCK();
    //GPIO15.set_function_SPI1_TX();
    //ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    //terminal.Initialize().AttachDisplay(display.Initialize(Display::Dir::Rotate90))
    //    .AttachKeyboard(keyboard.SetCapsLockAsCtrl()).SetFont(Font::shinonome16);
    ////    .AttachKeyboard(Stdio::GetKeyboard()).SetFont(Font::shinonome16);
    //Shell::AttachTerminal(terminal);
    //terminal.Println("Shell on ST7789 TFT LCD");
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    //-------------------------------------------------------------------------
    for (;;) {
        // any jobs
        Tickable::Tick();
    }
}