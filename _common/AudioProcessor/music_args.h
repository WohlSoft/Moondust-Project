#ifndef MUSIC_ARGS_H
#define MUSIC_ARGS_H

#include <string>
#include <map>

class MusicArgs
{
    typedef std::map<std::string, std::string> ArgsMap;
    ArgsMap m_args;
    int     m_argTrack = 0; //!< GME only track number

public:
    explicit MusicArgs(const std::string &args = std::string());
    MusicArgs(const MusicArgs &o);

    int getTrackNumber() const;

    bool setArgs(const std::string &args);

    int getArgI(const std::string &key, int def) const;
    bool getArgB(const std::string &key, bool def) const;
    float getArgF(const std::string &key, float def) const;
    double getArgD(const std::string &key, double def) const;
    std::string getArgS(const std::string &key, const std::string def) const;
};

#endif // MUSIC_ARGS_H
