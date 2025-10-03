#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Display/SSD1306.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
    ::stdio_init_all();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
#if 0
    ::i2c_init(i2c0, 400 * 1000);
    GPIO4.set_function_I2C0_SDA().pull_up();
    GPIO5.set_function_I2C0_SCL().pull_up();
    SSD1306 display(i2c0, 0x3c);
#else
    // compatible with http://www.ze.em-net.ne.jp/~kenken/picogames/index.html
	::spi_init(spi0, 125 * 1000 * 1000);	// for displays
	GPIO18.set_function_SPI0_SCK();
	GPIO19.set_function_SPI0_TX();
	//ILI9341 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
	ST7789 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
#endif
    display.Initialize();
    terminal.SetFont(Font::shinonome16)
        .AttachDisplay(display).AttachKeyboard(keyboard);
    terminal.Println("ReadLine Test Program");
    for (;;) {
        char* str = terminal.ReadLine(">");
        terminal.Printf("%s\n", str);
    }
}
