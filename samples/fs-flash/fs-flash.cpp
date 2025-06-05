#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive", 0x0004'0000);  // 256kB
    FS::File* pFile = FS::OpenFile("Drive:/test.txt", "r");
    if (pFile) {
        char line[256];
        while (pFile->ReadLine(line, sizeof(line)) > 0) {
            ::printf("%s", line);      
        }
        pFile->Close();
        delete pFile;
    }
}
