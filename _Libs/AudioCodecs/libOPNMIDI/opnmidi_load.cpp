/*
 * libOPNMIDI is a free MIDI to WAV conversion library with OPN2 (YM2612) emulation
 *
 * MIDI parser and player (Original code from ADLMIDI): Copyright (c) 2010-2014 Joel Yliluoma <bisqwit@iki.fi>
 * OPNMIDI Library and YM2612 support:   Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Library is based on the ADLMIDI, a MIDI player for Linux and Windows with OPL3 emulation:
 * http://iki.fi/bisqwit/source/adlmidi.html
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

#include "opnmidi_private.hpp"

#include "opnmidi_mus2mid.h"
#include "opnmidi_xmi2mid.h"

uint64_t OPNMIDIplay::ReadBEint(const void *buffer, size_t nbytes)
{
    uint64_t result = 0;
    const unsigned char *data = reinterpret_cast<const unsigned char *>(buffer);

    for(unsigned n = 0; n < nbytes; ++n)
        result = (result << 8) + data[n];

    return result;
}

uint64_t OPNMIDIplay::ReadLEint(const void *buffer, size_t nbytes)
{
    uint64_t result = 0;
    const unsigned char *data = reinterpret_cast<const unsigned char *>(buffer);

    for(unsigned n = 0; n < nbytes; ++n)
        result = result + static_cast<uint64_t>(data[n] << (n * 8));

    return result;
}

uint64_t OPNMIDIplay::ReadVarLenEx(size_t tk, bool &ok)
{
    uint64_t result = 0;
    ok = false;

    for(;;)
    {
        if(tk >= TrackData.size())
            return 1;

        if(tk >= CurrentPosition.track.size())
            return 2;

        size_t ptr = CurrentPosition.track[tk].ptr;

        if(ptr >= TrackData[tk].size())
            return 3;

        unsigned char byte = TrackData[tk][CurrentPosition.track[tk].ptr++];
        result = (result << 7) + (byte & 0x7F);

        if(!(byte & 0x80)) break;
    }

    ok = true;
    return result;
}

bool OPNMIDIplay::LoadBank(const std::string &filename)
{
    fileReader file;
    file.openFile(filename.c_str());
    return LoadBank(file);
}

bool OPNMIDIplay::LoadBank(void *data, unsigned long size)
{
    fileReader file;
    file.openData(data, size);
    return LoadBank(file);
}

size_t readU16BE(OPNMIDIplay::fileReader &fr, uint16_t &out)
{
    uint8_t arr[2];
    size_t ret = fr.read(arr, 1, 2);
    out = arr[1];
    out |= ((arr[0] << 8) & 0xFF00);
    return ret;
}

size_t readS16BE(OPNMIDIplay::fileReader &fr, int16_t &out)
{
    uint8_t arr[2];
    size_t ret = fr.read(arr, 1, 2);
    out = *reinterpret_cast<signed char *>(&arr[0]);
    out *= 1 << 8;
    out |= arr[1];
    return ret;
}

int16_t toSint16BE(uint8_t *arr)
{
    int16_t num = *reinterpret_cast<signed char *>(&arr[0]);
    num *= 1 << 8;
    num |= arr[1];
    return num;
}

bool OPNMIDIplay::LoadBank(OPNMIDIplay::fileReader &fr)
{
    #define qqq(x) (void)x
    size_t  fsize;
    qqq(fsize);
    if(!fr.isValid())
    {
        OPN2MIDI_ErrorString = "Invalid data stream!";
        return false;
    }

    char magic[32];
    memset(magic, 0, 32);

    uint16_t count_melodic_banks     = 1;
    uint16_t count_percusive_banks   = 1;

    if(fr.read(magic, 1, 11) != 11)
    {
        OPN2MIDI_ErrorString = "Can't read magic number!";
        return false;
    }

    if(strncmp(magic, "WOPN2-BANK\0", 11) != 0)
    {
        OPN2MIDI_ErrorString = "Invalid magic number!";
        return false;
    }

    opn.dynamic_instruments.clear();
    opn.dynamic_metainstruments.clear();
    if((readU16BE(fr, count_melodic_banks) != 2) || (readU16BE(fr, count_percusive_banks) != 2))
    {
        OPN2MIDI_ErrorString = "Can't read count of banks!";
        return false;
    }

    if(fr.read(&opn.regLFO, 1, 1) != 1)
    {
        OPN2MIDI_ErrorString = "Can't read LFO registry state!";
        return false;
    }

    opn.dynamic_percussion_offset = count_melodic_banks * 128;
    uint16_t total = 128 * count_melodic_banks + 128 * count_percusive_banks;

    for(uint16_t i = 0; i < total; i++)
    {
        opnInstData data;
        opnInstMeta meta;
        uint8_t idata[65];
        /* Junk, delete later */
        meta.flags = 0;
        meta.ms_sound_kon   = 1000;
        meta.ms_sound_koff  = 500;
        meta.fine_tune      = 0.0;
        /* Junk, delete later */

        if(fr.read(idata, 1, 65) != 65)
        {
            OPN2MIDI_ErrorString = "Failed to read instrument data";
            return false;
        }
        data.finetune = toSint16BE(idata + 32);
        //Percussion instrument note number or a "fixed note sound"
        meta.tone  = idata[34];
        data.fbalg = idata[35];
        data.lfosens = idata[36];
        for(size_t op = 0; op < 4; op++)
        {
            size_t off = 37 + op * 7;
            memcpy(data.OPS[op].data, idata + off, 7);
        }
        meta.opnno1 = uint16_t(opn.dynamic_instruments.size());
        meta.opnno2 = uint16_t(opn.dynamic_instruments.size());
        opn.dynamic_instruments.push_back(data);
        opn.dynamic_metainstruments.push_back(meta);
    }

    return true;
    #undef qqq
}

bool OPNMIDIplay::LoadMIDI(const std::string &filename)
{
    fileReader file;
    file.openFile(filename.c_str());

    if(!LoadMIDI(file))
    {
        std::perror(filename.c_str());
        return false;
    }

    return true;
}

bool OPNMIDIplay::LoadMIDI(void *data, unsigned long size)
{
    fileReader file;
    file.openData(data, size);
    return LoadMIDI(file);
}

bool OPNMIDIplay::LoadMIDI(OPNMIDIplay::fileReader &fr)
{
    #define qqq(x) (void)x
    size_t  fsize;
    qqq(fsize);
    //! Temp buffer for conversion
    AdlMIDI_CPtr<uint8_t> cvt_buf;

    //std::FILE* fr = std::fopen(filename.c_str(), "rb");
    if(!fr.isValid())
    {
        OPN2MIDI_ErrorString = "Invalid data stream!";
        return false;
    }

    if(opn.dynamic_instruments.empty())
    {
        OPN2MIDI_ErrorString = "Can't play music without a sound bank!";
        return false;
    }

    /**** Set all properties BEFORE starting of actial file reading! ****/

    config->stored_samples = 0;
    config->backup_samples_size = 0;
    opn.ScaleModulators = config->ScaleModulators;
    opn.LogarithmicVolumes = config->LogarithmicVolumes;
    opn.ChangeVolumeRangesModel(static_cast<OPNMIDI_VolumeModels>(config->VolumeModel));

    if(config->VolumeModel == OPNMIDI_VolumeModel_AUTO)
        opn.m_volumeScale = OPN2::VOLUME_Generic;

    opn.NumCards    = config->NumCards;
    //opn.NumFourOps  = config->NumFourOps;
    cmf_percussion_mode = false;
    opn.Reset();

    trackStart       = true;
    loopStart        = true;
    loopStart_passed = false;
    invalidLoop      = false;
    loopStart_hit    = false;

    bool is_GMF = false; // GMD/MUS files (ScummVM)

    const size_t HeaderSize = 4 + 4 + 2 + 2 + 2; // 14
    char HeaderBuf[HeaderSize] = "";
    size_t DeltaTicks = 192, TrackCount = 1;

riffskip:
    fsize = fr.read(HeaderBuf, 1, HeaderSize);

    if(std::memcmp(HeaderBuf, "RIFF", 4) == 0)
    {
        fr.seek(6l, SEEK_CUR);
        goto riffskip;
    }

    if(std::memcmp(HeaderBuf, "GMF\x1", 4) == 0)
    {
        // GMD/MUS files (ScummVM)
        fr.seek(7 - static_cast<long>(HeaderSize), SEEK_CUR);
        is_GMF = true;
    }
    else if(std::memcmp(HeaderBuf, "MUS\x1A", 4) == 0)
    {
        // MUS/DMX files (Doom)
        fr.seek(0, SEEK_END);
        size_t mus_len = fr.tell();
        fr.seek(0, SEEK_SET);
        uint8_t *mus = (uint8_t*)malloc(mus_len);
        if(!mus)
        {
            OPN2MIDI_ErrorString = "Out of memory!";
            return false;
        }
        fr.read(mus, 1, mus_len);
        //Close source stream
        fr.close();

        uint8_t *mid = NULL;
        uint32_t mid_len = 0;
        int m2mret = OpnMidi_mus2midi(mus, static_cast<uint32_t>(mus_len),
                                      &mid, &mid_len, 0);
        if(mus) free(mus);
        if(m2mret < 0)
        {
            OPN2MIDI_ErrorString = "Invalid MUS/DMX data format!";
            return false;
        }
        cvt_buf.reset(mid);
        //Open converted MIDI file
        fr.openData(mid, static_cast<size_t>(mid_len));
        //Re-Read header again!
        goto riffskip;
    }
    else if(std::memcmp(HeaderBuf, "FORM", 4) == 0)
    {
        if(std::memcmp(HeaderBuf + 8, "XDIR", 4) != 0)
        {
            fr.close();
            OPN2MIDI_ErrorString = fr._fileName + ": Invalid format\n";
            return false;
        }

        fr.seek(0, SEEK_END);
        size_t mus_len = fr.tell();
        fr.seek(0, SEEK_SET);
        uint8_t *mus = (uint8_t*)malloc(mus_len);
        if(!mus)
        {
            OPN2MIDI_ErrorString = "Out of memory!";
            return false;
        }
        fr.read(mus, 1, mus_len);
        //Close source stream
        fr.close();

        uint8_t *mid = NULL;
        uint32_t mid_len = 0;
        int m2mret = OpnMidi_xmi2midi(mus, static_cast<uint32_t>(mus_len),
                                      &mid, &mid_len, XMIDI_CONVERT_NOCONVERSION);
        if(mus) free(mus);
        if(m2mret < 0)
        {
            OPN2MIDI_ErrorString = "Invalid XMI data format!";
            return false;
        }
        cvt_buf.reset(mid);
        //Open converted MIDI file
        fr.openData(mid, static_cast<size_t>(mid_len));
        //Re-Read header again!
        goto riffskip;
    }
    else
    {
        if(std::memcmp(HeaderBuf, "MThd\0\0\0\6", 8) != 0)
        {
InvFmt:
            fr.close();
            OPN2MIDI_ErrorString = fr._fileName + ": Invalid format\n";
            return false;
        }

        /*size_t  Fmt =*/ ReadBEint(HeaderBuf + 8,  2);
        TrackCount = ReadBEint(HeaderBuf + 10, 2);
        DeltaTicks = ReadBEint(HeaderBuf + 12, 2);
    }

    TrackData.clear();
    TrackData.resize(TrackCount, std::vector<uint8_t>());
    CurrentPosition.track.clear();
    CurrentPosition.track.resize(TrackCount);
    InvDeltaTicks = fraction<uint64_t>(1, 1000000l * static_cast<uint64_t>(DeltaTicks));
    //Tempo       = 1000000l * InvDeltaTicks;
    Tempo         = fraction<uint64_t>(1,            static_cast<uint64_t>(DeltaTicks));
    static const unsigned char EndTag[4] = {0xFF, 0x2F, 0x00, 0x00};
    int totalGotten = 0;

    for(size_t tk = 0; tk < TrackCount; ++tk)
    {
        // Read track header
        size_t TrackLength;
        {
            if(is_GMF) // Take the rest of the file
            {
                size_t pos = fr.tell();
                fr.seek(0, SEEK_END);
                TrackLength = fr.tell() - pos;
                fr.seek(static_cast<long>(pos), SEEK_SET);
            }
            else
            {
                fsize = fr.read(HeaderBuf, 1, 8);
                if(std::memcmp(HeaderBuf, "MTrk", 4) != 0)
                    goto InvFmt;
                TrackLength = ReadBEint(HeaderBuf + 4, 4);
            }

            // Read track data
            TrackData[tk].resize(TrackLength);
            fsize = fr.read(&TrackData[tk][0], 1, TrackLength);
            totalGotten += fsize;

            if(is_GMF /*|| is_MUS*/) // Note: CMF does include the track end tag.
                TrackData[tk].insert(TrackData[tk].end(), EndTag + 0, EndTag + 4);

            bool ok = false;
            // Read next event time
            uint64_t tkDelay = ReadVarLenEx(tk, ok);

            if(ok)
                CurrentPosition.track[tk].delay = tkDelay;
            else
            {
                std::stringstream msg;
                msg << fr._fileName << ": invalid variable length in the track " << tk << "! (error code " << tkDelay << ")";
                OPN2MIDI_ErrorString = msg.str();
                return false;
            }
        }
    }

    for(size_t tk = 0; tk < TrackCount; ++tk)
        totalGotten += TrackData[tk].size();

    if(totalGotten == 0)
    {
        OPN2MIDI_ErrorString = fr._fileName + ": Empty track data";
        return false;
    }

    opn.Reset(); // Reset AdLib
    ch.clear();
    ch.resize(opn.NumChannels);
    return true;
    #undef qqq
}
