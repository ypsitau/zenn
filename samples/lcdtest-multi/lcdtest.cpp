#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome16-japanese-level1.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display1(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ST7789 display2(spi1, 240, 320, {RST: GPIO18, DC: GPIO19, CS: GPIO20, BL: GPIO21});
	display1.Initialize(Display::Dir::Rotate0);
	display2.Initialize(Display::Dir::Rotate90);
	display1.DrawImage(0, 0, image_cat_240x320);
	display2.SetFont(Font::shinonome16);
	display2.DrawStringWrap(0, 0, Text_Botchan);
}
