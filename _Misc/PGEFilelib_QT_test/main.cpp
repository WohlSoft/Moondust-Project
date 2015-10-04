
#include <QCoreApplication>
#include <QTextStream>
#include <PGE_File_Formats/file_formats.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream(stdout) << FileFormats::WriteSMBX64LvlFile(FileFormats::dummyLvlDataArray(), 64) << "\n";
    a.exit(0);
    return 0;
}

