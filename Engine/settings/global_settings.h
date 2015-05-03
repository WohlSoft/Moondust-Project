#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H


class GlobalSettings
{
public:
    GlobalSettings();
    ~GlobalSettings();
    void load();
    void save();
    void resetDefaults();
    void apply();

    bool debugMode;
    bool interprocessing;
};

extern GlobalSettings AppSettings;

#endif // GLOBALSETTINGS_H
