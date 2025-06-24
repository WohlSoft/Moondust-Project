/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "audio_processor.h"
#include <SDL2/SDL.h>


MoondustAudioProcessor::MoondustAudioProcessor()
{}

MoondustAudioProcessor::~MoondustAudioProcessor()
{}

std::string MoondustAudioProcessor::getLastError() const
{
    return m_lastError;
}

const MDAudioFileSpec &MoondustAudioProcessor::getInSpec() const
{
    return m_in_file->getSpec();
}

bool MoondustAudioProcessor::openInFile(const std::string &file, int *detectedFormat)
{
    return false;
}

bool MoondustAudioProcessor::openOutFile(const std::string &file, int dstFormat, const MDAudioFileSpec &dstSpec)
{
    return false;
}

uint32_t MoondustAudioProcessor::numChunks() const
{
    return m_numChunks;
}

uint32_t MoondustAudioProcessor::curChunk() const
{
    return m_curChunk;
}

bool MoondustAudioProcessor::runChunk()
{
    return false;
}

bool MoondustAudioProcessor::done() const
{
    return m_done;
}
