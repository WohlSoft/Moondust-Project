/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef CUSTOM_DATA_H
#define CUSTOM_DATA_H

#include <string>
#include <vector>
#include <Utils/dir_list_ci.h>

/*!
 * \brief This is a capturer of custom files from level/world custom directories
 */
class CustomDirManager
{
public:
    /*!
     * @brief Constructor
     */
    CustomDirManager() = default;
    /*!
     * @brief Constructor with pre-defined parameters
     * @param path Full path to episode directory
     * @param name Base file name of current file
     * @param stuffPath Full path to default data folder
     */
    CustomDirManager(const std::string &path, const std::string &name, const std::string &stuffPath);
    /*!
     * @brief Get custom file path if exist.
     * @param name Target file name which need to find
     * @return empty string if not exist
     */
    std::string getCustomFile(const std::string &name, bool *isDefault = nullptr);
    /*!
     * @brief Get the file from default storage of config pack
     * @param name Target file name which need to find
     * @return empty string if not exist
     */
    std::string getDefaultFile(const std::string &name);
    /*!
     * @brief Get the mask fallback file if default is PNG
     * @param name Target file name which need to find
     * @return empty string if not exist
     */
    std::string getMaskFallbackFile(const std::string &name);
    /*!
     * @brief Sets paths where look for a requested files
     * @param path Full path to episode directory
     * @param name Base file name of current file
     * @param stuffPath Full path to default data folder
     */
    void setCustomDirs(const std::string &path,
                       const std::string &name,
                       const std::string &stuffPath,
                       const std::vector<std::string> &extraPaths = std::vector<std::string>()
    );
    /*!
     * @brief Add extra custom data directories
     * @param dPaths Lost of paths to the custom data directories
     */
    void addExtraDirs(const std::vector<std::string> &dPaths);
    /*!
     * @brief Remove all custom data directories from a list
     */
    void clearExtraDirs();
    /*!
     * @brief Find a file through extra custom data directories
     * @param fPath
     * @return
     */
    std::string findFileInExtraDirs(const std::string &fPath);

private:
    DirListCI m_dirCiEpisode;
    DirListCI m_dirCiCustom;
    //! Full path to episode directory
    std::string m_dirEpisode;
    //! Full path to file's custom stuff directory
    std::string m_dirCustom;
    //! Extra directories for resources search until search in the config pack
    std::vector<std::string> m_dirsExtra;
    //! Full path to default data folder
    std::string m_mainStuffFullPath;

};

#endif // CUSTOM_DATA_H
