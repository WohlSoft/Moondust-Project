/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

/*!
 * \brief This is a capturer of custom files from level/world custom directories
 */
class CustomDirManager
{
public:
    /*!
     * \brief Constructor
     */
    CustomDirManager() = default;
    /*!
     * \brief Constructor with pre-defined parameters
     * \param path Full path to episode directory
     * \param name Base file name of current file
     * \param stuffPath Full path to default data folder
     */
    CustomDirManager(std::string path, std::string name, std::string stuffPath);
    /*!
     * \brief Get custom file path if exist.
     * \param name Target file name which need to find
     * \return empty string if not exist
     */
    std::string getCustomFile(std::string name, bool *isDefault=0);
    /**
     * @brief Get the file from default storage of config pack
     * @param name Target file name which need to find
     * @return empty string if not exist
     */
    std::string getDefaultFile(std::string name);
    /**
     * @brief Get the mask fallback file if default is PNG
     * @param name Target file name which need to find
     * @return empty string if not exist
     */
    std::string getMaskFallbackFile(std::string name);
    /*!
     * \brief Sets paths where look for a requested files
     * \param path Full path to episode directory
     * \param name Base file name of current file
     * \param stuffPath Full path to default data folder
     */
    void setCustomDirs(std::string path, std::string name, std::string stuffPath);

private:
    //! Full path to episode directory
    std::string m_dirEpisode;
    //! Full path to file's custom stuff directory
    std::string m_dirCustom;
    //! Full path to default data folder
    std::string m_mainStuffFullPath;

};

#endif // CUSTOM_DATA_H
