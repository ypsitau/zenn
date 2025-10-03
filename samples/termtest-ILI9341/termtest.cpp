#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/LVGL.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

Display::Terminal terminal;

void OnValueChanged_btnm(lv_event_t* e);

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);		// for touch screens
	::spi_init(spi1, 125 * 1000 * 1000);	// for displays
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	display.Initialize(Display::Dir::Rotate0);
	touchScreen.Initialize(display);
	//-----------------------------------------
	terminal.AttachDisplay(display, {0, 0, 240, 220});
	terminal.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2);
	terminal.Suppress().Print(Text_Botchan);
	terminal.Suppress(false);
	//-----------------------------------------
	LVGL::Initialize();
	LVGL::Adapter lvglAdapter;
	lvglAdapter.AttachDisplay(display, {0, 220, 240, 100});
	lvglAdapter.AttachTouchScreen(touchScreen);
	do {
		static const char* labelTbl[] = {
			"Roll Up", "Dump", "\n",
			"Roll Down", "Print Buffer", "",
		};
		lv_obj_t* btnm = ::lv_buttonmatrix_create(lv_screen_active());
		::lv_obj_set_size(btnm, 230, 90);
		::lv_obj_align(btnm, LV_ALIGN_BOTTOM_MID, 0, -5);
		::lv_obj_add_event_cb(btnm, OnValueChanged_btnm, LV_EVENT_VALUE_CHANGED, nullptr);
		::lv_obj_remove_flag(btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE);
		::lv_buttonmatrix_set_map(btnm, labelTbl);
	} while (0);
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
	}
}

void OnValueChanged_btnm(lv_event_t* e)
{
	enum class Id {
		RollUp, Dump,
		RollDown, PrintBuffer,
	};
	lv_obj_t* btnm = reinterpret_cast<lv_obj_t*>(::lv_event_get_target(e));
	Id id = static_cast<Id>(::lv_buttonmatrix_get_selected_button(btnm));
	if (id == Id::RollUp) terminal.RollUp();
	if (id == Id::RollDown) terminal.RollDown();
	if (id == Id::Dump) terminal.Dump.BytesPerRow(8).Ascii()(reinterpret_cast<const void*>(0x10000000), 64);
	if (id == Id::PrintBuffer) terminal.CreateReader().WriteTo(stdout);
}
