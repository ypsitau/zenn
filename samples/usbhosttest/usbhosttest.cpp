#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
    GPIO18.init().set_dir_OUT();
    GPIO19.init().set_dir_OUT();
    GPIO20.init().set_dir_OUT();
    GPIO21.init().set_dir_OUT();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    for (;;) {
        uint8_t keyCode;
        bool rtn = keyboard.GetKeyCodeNB(&keyCode);
        GPIO18.put(rtn && keyCode == VK_V);
        GPIO19.put(rtn && keyCode == VK_C);
        GPIO20.put(rtn && keyCode == VK_X);
        GPIO21.put(rtn && keyCode == VK_Z);
        Tickable::Sleep(50);
    }
}
