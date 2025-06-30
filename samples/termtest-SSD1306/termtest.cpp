#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/Font/naga10-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400 * 1000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	GPIO21.init().pull_up();
	SSD1306 display(i2c0, 0x3c);
	display.Initialize();
	terminal.AttachDisplay(display);
    terminal.SetFont(Font::naga10).SetSpacingRatio(1., 1.).ClearScreen();
	terminal.Suppress();
	terminal.Print(Text_Botchan);
	terminal.Suppress(false);
	for (int i = 1; i < 3; i++) {
		for (int j = 1; j < 8; j++) terminal.Printf("%3d", i * j);
		terminal.Println();
	}
	for (;;) {
		if (!GPIO18.get()) terminal.RollUp();
		if (!GPIO19.get()) terminal.RollDown();
		if (!GPIO20.get()) terminal.Dump.Addr(false).BytesPerRow(8)(reinterpret_cast<const void*>(0x10000000), 32);
		if (!GPIO21.get()) terminal.CreateReader().WriteTo(stdout);
		::sleep_ms(100);
	}
}
