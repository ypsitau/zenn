#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/Font/shinonome16-japanese-level1.h"

using namespace jxglib;

int main()
{
	// Initialize devices
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	// Display items
	display.DrawImage(20, 20, image_cat_240x320, {20, 20, 200, 200});
	display.SetFont(Font::shinonome16);
	const char* str = "吾輩は猫である";
	Size sizeStr = display.CalcStringSize(str);
	int x = (display.GetWidth() - sizeStr.width) / 2, y = 230;
	display.DrawString(x, y, str);
	display.DrawRect(x - 8, y - 4, sizeStr.width + 8 * 2, sizeStr.height + 4 * 2, Color::white);
	display.DrawRectFill(0, 260, 55, 60, Color::red);
	display.DrawRectFill(60, 260, 55, 60, Color::green);
	display.DrawRectFill(120, 260, 55, 60, Color::blue);
	display.DrawRectFill(180, 260, 55, 60, Color::aqua);
}
