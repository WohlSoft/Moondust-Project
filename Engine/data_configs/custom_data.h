#ifndef CUSTOM_DATA_H
#define CUSTOM_DATA_H

#include <QString>

///
/// \brief The CustomDirManager class This is a capturer of custom files from level/world custom directories
///
class CustomDirManager
{
public:
    CustomDirManager();
    CustomDirManager(QString path, QString name, QString stuffPath);
    ///
    /// \brief getCustomFile Get custom file path if exist.
    /// \param name Target file name which need to found
    /// \return empty string if not exist
    ///
    QString getCustomFile(QString name);
    void setCustomDirs(QString path, QString name, QString stuffPath);

private:
    QString dirEpisode;
    QString dirCustom;
    QString mainStuffFullPath;

};

#endif // CUSTOM_DATA_H
