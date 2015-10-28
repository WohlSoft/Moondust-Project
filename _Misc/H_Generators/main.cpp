#include <QCoreApplication>
#include <QString>
#include <QSettings>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSettings inix(a.applicationDirPath()+"/configs/SMBX/lvl_bgo.ini", QSettings::IniFormat);

    QTextStream x(stdout);

    inix.beginGroup("background-main");
    int total=inix.value("total", 0).toInt();
    inix.endGroup();
    x << "int bgo_sort_priorities["+QString::number(total)+"] = {\n";
    for(int i=1;i<=total;i++)
    {
        inix.beginGroup("background-"+QString::number(i));
        x << inix.value("smbx64-sort-priority", 0).toInt() << (i==total?"":",")<<(i%30==0?"\n":"");
        inix.endGroup();
    }

    x<< "};\n\n";

    return 0;

    //junk code will be never ran. xD
    a.exec();
    return 0;
}

