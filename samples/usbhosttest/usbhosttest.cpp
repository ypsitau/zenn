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
    USBHose::Keyboard keyboard;
    for (;;) {
        GPIO18.put(keyboard.IsPressed(VK_V));
        GPIO19.put(keyboard.IsPressed(VK_C));
        GPIO20.put(keyboard.IsPressed(VK_X));
        GPIO21.put(keyboard.IsPressed(VK_Z));
        Tickable::Sleep(50);
    }
}