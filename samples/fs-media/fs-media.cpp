#include "pico/stdlib.h"
#include "jxglib/FAT/USBMSC.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    USBHost::Initialize();
    FAT::USBMSC drive("Drive:");
    bool connectedFlag = false;
    for (;;) {
        if (connectedFlag) {
            if (!drive.CheckMounted()) {
                connectedFlag = false;
                ::printf("USB storage disconnected.\n");
            }
        } else if (drive.Mount()) {
            ::printf("USB storage connected.\n");
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