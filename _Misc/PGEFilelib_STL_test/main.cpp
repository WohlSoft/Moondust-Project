#include <iostream>
#include <PGE_File_Formats/file_formats.h>

using namespace std;

int main()
{
    cout << FileFormats::WriteSMBX64LvlFile(FileFormats::dummyLvlDataArray(), 64) << endl;
    return 0;
}

