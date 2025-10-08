#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Font/naga10.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    //::spi_init(spi0, 125 * 1000 * 1000);
    //GPIO18.set_function_SPI0_SCK();
    //GPIO19.set_function_SPI0_TX();
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    Display::Terminal terminal;
    //Display::ILI9341 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
    //Display::ST7789 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
    Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate270);
    terminal.Initialize().AttachDisplay(display).SetFont(Font::naga10);
    USBHost::Initialize();
    USBHost::GamePad gamePad;
    terminal.Printf("USB GamePad Monitor\n");
    for (;;) {
        if (gamePad.HasReportChanged()) {
            terminal.Printf("%s%s%s%s%s%s%s%s%s%s%s%s%s %X % 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f\n",
                gamePad.Get_Button0()? "0" : ".",
                gamePad.Get_Button1()? "1" : ".",
                gamePad.Get_Button2()? "2" : ".",
                gamePad.Get_Button3()? "3" : ".",
                gamePad.Get_Button4()? "4" : ".",
                gamePad.Get_Button5()? "5" : ".",
                gamePad.Get_Button6()? "6" : ".",
                gamePad.Get_Button7()? "7" : ".",
                gamePad.Get_Button8()? "8" : ".",
                gamePad.Get_Button9()? "9" : ".",
                gamePad.Get_Button10()? "A" : ".",
                gamePad.Get_Button11()? "B" : ".",
                gamePad.Get_Button12()? "C" : ".",
                gamePad.Get_HatSwitch(),
                gamePad.Get_Axis0(),
                gamePad.Get_Axis1(),
                gamePad.Get_Axis2(),
                gamePad.Get_Axis3(),
                gamePad.Get_Axis4(),
                gamePad.Get_Axis5(),
                gamePad.Get_Axis6(),
                gamePad.Get_Axis7(),
                gamePad.Get_Axis8());
        }
        Tickable::Sleep(300);
    }
}
