#include "translator_main.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <common_features/logger.h>
#include <common_features/app_path.h>
#include <Utf8Main/utf8main.h>
#include "version.h"


int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    QStringList args;

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName("Moondust Translator");

    QApplication a(argc, argv);
#ifdef _WIN32
    for(int i = 0; i < argc; i++)
        args.push_back(QString::fromUtf8(argv[i]));
#else
    args    = a.arguments();
#endif

    TranslatorMain w;

    w.show();

    return a.exec();
}
