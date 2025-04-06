#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    GPIO15.init().set_dir_OUT();
    while (true) {
        GPIO15.put(true);
        sleep_ms(1500);
        GPIO15.put(false);
        sleep_ms(500);
    }
}
