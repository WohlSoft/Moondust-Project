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
};

#endif // GLOBALSETTINGS_H
