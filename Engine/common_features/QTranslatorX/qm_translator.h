/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** Copyright (C) 2016-2019 Vitaliy Novichkov <admin@wohlnet.ru>
**
** This file use a part of the QtCore module of the Qt Toolkit,
** ported into pure STL to allow support of qm translations in the non-Qt projects.
**
** To use this code statically linked with the non-GPL projects
** you must have commercial license from the Qt rightholder
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Qt License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMTRANSLATORX_H
#define QMTRANSLATORX_H

#include <string>
#include <vector>

class QmTranslatorX
{
    uint8_t  *m_fileData;
    size_t    m_fileLength;

    // Pointers and offsets into FileData[FileLength] array, or user
    // provided data array
    uint8_t  *m_messageArray;
    uint8_t  *m_offsetArray;
    uint8_t  *m_contextArray;
    uint8_t  *m_numerusRulesArray;
    uint32_t  m_messageLength;
    uint32_t  m_offsetLength;
    uint32_t  m_contextLength;
    uint32_t  m_numerusRulesLength;
    std::vector<QmTranslatorX *> m_subTranslators;

public:
    QmTranslatorX();
    virtual ~QmTranslatorX();

    //Return UTF-8 string
    std::string    do_translate8(const char *context, const char *sourceText,
                                 const char *comment = nullptr, int32_t n = -1);

    //Return UTF-16 string (equal wchar_t on Windows)
    std::u16string do_translate(const char *context, const char *sourceText,
                                const char *comment = nullptr, int32_t n = -1);

    //Return UTF-32 string (equal wchar_t on Unix-like operating systems)
    std::u32string do_translate32(const char *context, const char *sourceText,
                                  const char *comment = nullptr, int32_t n = -1);

    bool loadFile(const char *filePath, uint8_t *directory = nullptr);
    bool loadData(const uint8_t *data, size_t len, uint8_t *directory = nullptr);
    bool isEmpty();
    void close();

private:
    bool loadDataPrivate(uint8_t *data, size_t len, uint8_t *directory = nullptr);
};

#endif // QMTRANSLATORX_H
