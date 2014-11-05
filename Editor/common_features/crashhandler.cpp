#include "crashhandler.h"
#include "ui_crashhandler.h"
#include <QDesktopServices>

//temp
#include <QMessageBox>
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#endif

CrashHandler::CrashHandler(QString &crashText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashHandler)
{
    ui->setupUi(this);
    ui->crashData->setText(crashText);
}


CrashHandler::~CrashHandler()
{
    delete ui;
}

void CrashHandler::crashByFlood()
{
    QString crashMsg = QApplication::tr("We're sorry, but PGE has crashed. Reason: Out of memory! :(\n"
                                        "To prevent this, try closing other uneccessary programs to free up more memory.");
#ifdef _WIN32
    unsigned int   i;
    void         * stack[ 100 ];
    unsigned short frames;
    SYMBOL_INFO  * symbol;
    HANDLE         process;

    process = GetCurrentProcess();

    SymInitialize( process, NULL, TRUE );

    frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
    symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

    QString bkTrace("");
    for( i = 0; i < frames; i++ )
    {
        char buf[500];
        SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

        sprintf(buf, "%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address );
        bkTrace += buf;
    }
    crashMsg = crashMsg + QString("\n\n") + bkTrace;
    CrashHandler* crsh = new CrashHandler(crashMsg);
    crsh->exec();
#else
    QMessageBox::warning(nullptr, QApplication::tr("Crash"), crashMsg);


#endif
    std::exit(1);
}

void CrashHandler::crashByUnhandledException()
{
    std::exception_ptr unhandledException = std::current_exception();
    try{
        std::rethrow_exception(unhandledException);
    }
    catch(const std::exception& e){
        std::string s1 = "We're sorry, but PGE has crashed. Reason: ";
        std::string s2 = e.what();
        std::string s3 = " :(\nPlease inform our forum staff so we can try to fix this problem, Thank you\n\nForum link: engine.wohlnet.ru/forum";

        QString crashMsg = (s1 + s2 + s3).c_str();
#ifdef _WIN32
        unsigned int   i;
        void         * stack[ 100 ];
        unsigned short frames;
        SYMBOL_INFO  * symbol;
        HANDLE         process;

        process = GetCurrentProcess();

        SymInitialize( process, NULL, TRUE );

        frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
        symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        QString bkTrace("");
        for( i = 0; i < frames; i++ )
        {
            char buf[500];
            SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

            sprintf(buf, "%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address );
            bkTrace += buf;
        }
        crashMsg = crashMsg + QString("\n\n") + bkTrace;
        CrashHandler* crsh = new CrashHandler(crashMsg);
        crsh->exec();
#else
        QMessageBox::warning(nullptr, QApplication::tr("Crash"), QApplication::tr((s1 + s2 + s3).c_str()));
#endif
    }

    std::exit(2);
}

void CrashHandler::initCrashHandlers()
{
    std::set_new_handler(&crashByFlood);
    std::set_terminate(&crashByUnhandledException);
}

void CrashHandler::on_pgeForumButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://engine.wohlnet.ru/forum/"));
}

void CrashHandler::on_exitButton_clicked()
{
    this->close();
}
