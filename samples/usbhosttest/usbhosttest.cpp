#include <lvgl/examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    USBHost::Mouse mouse;
    ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate90);
    LVGL::Initialize(5);
    LVGL::Adapter lvglAdapter;
    lvglAdapter.AttachDisplay(display)
        .AttachKeyboard(keyboard)
        .AttachMouse(mouse);
    ::lv_example_keyboard_1();
    for (;;) Tickable::Tick();
}