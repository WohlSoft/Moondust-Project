#ifndef EDITOR_PIPE_H
#define EDITOR_PIPE_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_thread.h>
#include <QVector>
#include <atomic>
#include <mutex>
#include <PGE_File_Formats/file_formats.h>

class EditorPipe
{
        friend class EditorPipe_std;
        SDL_Thread *m_thread;
        bool        m_thread_isAlive;
    public:
        EditorPipe();
        ~EditorPipe();
        void shut();
        bool        m_isWorking;

        static int run(void *self);
        void start();
        void stop();

        // SEND_LVLX: /some/path/to/level file\n\n
        QString     m_accepted_lvl_path; // Send to client the "READY\n\n" before accent raw data
        bool        m_doAcceptLevelData;
        QString     m_acceptedRawData;  // accept any raw data before will be accepted '\n\n'
        bool        m_doParseLevelData;
        LevelData   m_acceptedLevel;    // When accepted PARSE_LVLX\n\n, parse data and call signal

        bool levelIsLoad();

        void sendMessage(const QString &msg);
        void sendMessage(const std::string &in);
        void sendMessage(const char *in);

    private:
        std::atomic_bool    m_levelAccepted;
        std::mutex          m_levelAccepted_lock;

    private slots:
        void icomingData(std::string &in);
};

#endif // EDITOR_PIPE_H
