#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	LVGL::Initialize();
	LVGL::Adapter lvglAdapter;
	lvglAdapter.EnableDoubleBuff().AttachDisplay(display).AttachKeyboard(Stdio::GetKeyboard());
	::lv_example_keyboard_1();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
	}
}
