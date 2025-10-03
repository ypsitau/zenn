#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/Font/shinonome16-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	GPIO21.init().pull_up();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	terminal.AttachDisplay(display);
    terminal.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal.Suppress();
	terminal.Print(Text_Botchan);
	terminal.Suppress(false);
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 13; j++) terminal.Printf("%3d", i * j);
		terminal.Println();
	}
	for (;;) {
		if (!GPIO18.get()) terminal.RollUp();
		if (!GPIO19.get()) terminal.RollDown();
		if (!GPIO20.get()) terminal.Dump.BytesPerRow(8).Ascii()(reinterpret_cast<const void*>(0x10000000), 64);
		if (!GPIO21.get()) terminal.CreateReader().WriteTo(stdout);
		::sleep_ms(100);
	}
}
