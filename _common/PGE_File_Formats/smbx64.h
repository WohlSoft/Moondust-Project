/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file smbx64.h
 *  \brief Contains static class with SMBX64 standard file format fields validation and generation functions
 */

#ifndef SMBX64_H
#define SMBX64_H

#include "pge_file_lib_globs.h"

/*!
 * \brief SMBX64 Standard validation and raw data conversion functions
 */
#ifdef PGE_FILES_QT
class SMBX64 : public QObject
{
    Q_OBJECT
#else
class SMBX64
{
#endif

public:
    inline SMBX64() {}

    /*********************Validations**********************/
    /*!
     * \brief Validate Unsigned Integer value
     * \param in raw value
     * \return true if value is valid
     */
    static bool IsUInt(PGESTRING in);
    /*!
     * \brief Validate Signed Integer value
     * \param in raw value
     * \return true if value is valid
     */
    static bool IsSInt(PGESTRING in);
    /*!
     * \brief Validate Floating Point value
     * \param in raw value
     * \return true if value is valid
     */
    static bool IsFloat(PGESTRING &in);
    /*!
     * \brief Validate quoted string value
     * \param in raw value
     * \return true if value is valid
     */
    static bool IsQuotedString(PGESTRING in);
    /*!
     * \brief Validate CSV-boolean value (#TRUE# or #FALSE#)
     * \param in raw value
     * \return true if value is INVALID
     */
    static bool IsCSVBool(PGESTRING in);
    /*!
     * \brief Validate digital boolean value (0 or 1)
     * \param in raw value
     * \return true if value is INVALID
     */
    static bool IsBool(PGESTRING in);

    /*!
     * \brief Converts CVS-bool raw string into boolean value
     * \param in raw value
     * \return boolean equivalent
     */
    static bool wBoolR(PGESTRING in);

    /******************RAW to Internal**********************/
    /*!
     * \brief Convert from SMBX64 string to internal with damage correction
     * \param in raw value
     * \return fixed string vale
     */
    static PGESTRING StrToStr(PGESTRING in);

    /******************Internal to RAW**********************/
    /*!
     * \brief Generate raw string from integer value
     * \param input Source integer value
     * \return ASCII encoded integer value
     */
    static PGESTRING IntS(long input);

    /*!
     * \brief Generate raw CVS-bool string from boolean value
     * \param input Source boolean value
     * \return ASCII encoded CVS-bool value
     */
    static PGESTRING BoolS(bool input);

    /*!
     * \brief Convert string into valid CVS string line
     * \param input Source string
     * \return Valid CVS string
     */
    static PGESTRING qStrS(PGESTRING input);

    /*!
     * \brief Convert string into valid CVS string line (with line feeds keeping)
     * \param input Source string
     * \return Valid CVS string
     */
    static PGESTRING qStrS_multiline(PGESTRING input);

    /*!
     * \brief Generate raw string from floating point value
     * \param input Source floating point vale
     * \return ASCII encoded floating point value
     */
    static PGESTRING FloatS(float input);


    /******************Units converters**********************/
    /*!
     * \brief Convert 1/65 seconds into milliseconds
     * \param t65 1/65 time value
     * \return millisecond time equivalent
     */
    static double t65_to_ms(double t65);

    /*!
     * \brief Convert milliseconds into 1/65 seconds
     * \param ms time in milliseconds
     * \return 1/65 second time equivalent
     */
    static double ms_to_65(double ms);
};


#endif // SMBX64_H

