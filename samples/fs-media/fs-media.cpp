#include "pico/stdlib.h"
#include "jxglib/FAT/SDCard.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    GPIO2.set_function_SPI0_SCK();
    GPIO3.set_function_SPI0_TX();
    GPIO4.set_function_SPI0_RX();
    FAT::SDCard drive("Drive:", spi0, 10'000'000, {CS: GPIO5});
    bool connectedFlag = false;
    for (;;) {
        if (connectedFlag) {
            if (!drive.CheckMounted()) {
                ::printf("SD Card disconnected.\n");
                connectedFlag = false;
            }
        } else if (drive.Mount()) {
            ::printf("SD Card connected.\n");
            connectedFlag = true;
            FS::Dir* pDir = FS::OpenDir("Drive:/");
            if (pDir) {
                FS::FileInfo* pFileInfo;
                while (pFileInfo = pDir->Read()) {
                    ::printf("%-16s%d\n", pFileInfo->GetName(), pFileInfo->GetSize());
                    delete pFileInfo;
                }
                pDir->Close();
                delete pDir;
            }
        }
        Tickable::Tick();
    }
}