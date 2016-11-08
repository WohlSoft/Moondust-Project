/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PGE_FILEMAPPER_H
#define PGE_FILEMAPPER_H

#include <string>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#if defined(__APPLE__) || defined(__x86_64__) || !defined(__USE_FILE_OFFSET64)
#define PGEFileManSize off_t
#else
#define PGEFileManSize off64_t
#endif
#else
#define PGEFileManSize long
#endif

/*!
 * \brief Provides cross-platform file memory mapping interface
 */
class PGE_FileMapper
{
#ifdef _WIN32
        void       *m_File;
        void       *m_Map;
        void       *m_Address;
#endif
        //! Full path to opened file
        std::string m_path;
        //! Recent occouped error description
        std::string m_error;

    public:
        //! Pointer to mapped file data
        void           *data;
        //! Size of mapped file
        PGEFileManSize  size;

        /*!
         * \brief Constructor
         */
        PGE_FileMapper();

        /*!
         * \brief Constructor with pre-opened file
         */
        PGE_FileMapper(std::string file);

        /*!
         * \brief Copy Constructor
         */
        PGE_FileMapper(const PGE_FileMapper &fm);

        /*!
         * \brief Opens file
         * \param path to file
         * \return True if success. False if error occouped
         */
        bool open_file(std::string path);

        /*!
         * \brief Closes opened file
         * \return True if no errors
         */
        bool close_file();

        /*!
         * \brief Destructor
         */

        virtual ~PGE_FileMapper();
        /*!
         * \brief Returns recent occouped error info
         * \return recent occouped error info
         */
        std::string error();
};

#endif // PGE_FILEMAPPER_H
