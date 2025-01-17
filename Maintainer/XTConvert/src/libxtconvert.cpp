/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
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

#include <map>
#include <array>
#include <utility>

#include <QDebug>

#include <QString>
#include <QDir>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QDirIterator>
#include <QDateTime>
#include <QSettings>

#include <fcntl.h>

#include <FreeImageLite.h>

#include <SDL2/SDL_mixer_ext.h>
#include <PGE_File_Formats/file_formats.h>

#include <json.hpp>

#include "archive.h"
#include "archive_entry.h"

#include "lz4_pack.h"
#include "libromfs3ds.h"
#include "libtex3ds.h"
#include "spc2it.h"
#include "mmutil.h"

#include "graphics_load.h"
#include "texconv.h"
#include "export_tpl.h"
#include "export_audio.h"
#include "has_no_mask.h"
#include "extract_archive.h"

#include "libxtconvert.h"

static void log_file_callback_default(void*, XTConvert::LogCategory cat, const std::string& str)
{
    printf("%s - %s\n", XTConvert::log_category[(int)cat], str.c_str());
}

static int progress_callback_default(void*, int, int, const std::string&, int, int, const std::string&) { return 0; }

static QByteArray s_get_default_masks()
{
    QByteArray ret;

    Q_INIT_RESOURCE(libxtconvert);

    {
        QFile file(":/default-fallback-masks.7z");
        if(file.open(QIODevice::ReadOnly))
            ret = file.readAll();
    }

    Q_CLEANUP_RESOURCE(libxtconvert);

    return ret;
}

namespace XTConvert
{

const char* const log_category[(int)LogCategory::Category_Count] =
{
    "Bitmask image",
    "Lost bitmask",
    "Transparent image",
    "Lost transparency",
    "Image scaled down",
    "Image cropped",
    "Image not 2x",
    "Oversized player image",
    "QOA music",
    "spc2it music",
    "38A content",
    "Unknown (copied)",
    "Unused (skipped)",
    "Invalid (skipped)",
    "Custom PNG (skipped)",
    "Assets GIF (skipped)",
    "No mask (skipped)",
    "Error Message",
};

const LogLevel log_level[(int)LogCategory::Category_Count] =
{
    LogLevel::Warning,
    LogLevel::Warning,
    LogLevel::Notice,
    LogLevel::Warning,
    LogLevel::Warning,
    LogLevel::Error,
    LogLevel::Warning,
    LogLevel::Notice,
    LogLevel::Info,
    LogLevel::Info,
    LogLevel::Error,
    LogLevel::Notice,
    LogLevel::Info,
    LogLevel::Error,
    LogLevel::Warning,
    LogLevel::Warning,
    LogLevel::Warning,
    LogLevel::Error,
};

enum Stage
{
    STAGE_EXTRACT = 0,
    STAGE_CONVERT,
    STAGE_POST_PROCESS,
    STAGE_PACK,
    STAGE_COMPRESS,
    STAGE_COUNT,
};

const char* const stage_names[(int)STAGE_COUNT] =
{
    "Extracting",
    "Converting",
    "Finishing",
    "Packaging",
    "Compressing",
};

static bool validate_image_filename(const QString& filename, QString& type, QString& index, QString& dir, bool& exceeds_max_GIF)
{
    QString basename = filename.section(".", 0, 0);
    type = basename.section("-", 0, 0);
    index = basename.section("-", 1);

    bool index_valid = false;
    unsigned int index_value = index.toUInt(&index_valid);

    if(!index_valid)
        return false;

    static const std::array<std::pair<const char*, unsigned int>, 17> valid_types = {{
        {"background", 200}, {"background2", 100}, {"block", 700}, {"effect", 200}, {"level", 100},
        {"link", 7}, {"luigi", 7}, {"mario", 7}, {"npc", 302}, {"path", 100},
        {"peach", 7}, {"player", 5}, {"scene", 100}, {"tile", 400}, {"toad", 7},
        {"yoshib", 10}, {"yoshit", 10}
    }};

    dir = type;

    // special case: yoshib and yoshit stored in yoshi
    if(type.startsWith('y'))
        dir = "yoshi";

    for(const auto& valid_type : valid_types)
    {
        if(type == valid_type.first)
        {
            exceeds_max_GIF = (index_value > valid_type.second);
            return true;
        }
    }

    return false;
}

struct MixerX_Sentinel
{
    bool valid = false;

    MixerX_Sentinel(const Spec& spec)
    {
        {
            int freq;
            Uint16 fmt;
            int chn;

            if(Mix_QuerySpec(&freq, &fmt, &chn))
                return;
        }

        if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID) < 0)
            return;

        SDL_AudioSpec as;

        as.channels = 2;
        as.format = AUDIO_S16LSB;
        as.freq = 44100;
        as.silence = 0;
        as.samples = 8192;
        as.size = as.samples * 4;
        as.callback = nullptr;
        as.userdata = nullptr;

        if(spec.target_platform == TargetPlatform::TPL)
            as.freq = 32000;
        else if(spec.target_platform == TargetPlatform::DSG)
            as.freq = 16384;

        Mix_InitMixer(&as, SDL_FALSE);

        {
            int freq;
            Uint16 fmt;
            int chn;

            if(!Mix_QuerySpec(&freq, &fmt, &chn) || freq != as.freq || chn != as.channels || fmt != as.format)
            {
                Mix_FreeMixer();
                Mix_Quit();

                return;
            }
        }

        valid = true;
    }

    ~MixerX_Sentinel()
    {
        if(valid)
        {
            Mix_FreeMixer();
            Mix_Quit();
        }

        valid = true;
    }
};

class Converter
{
    struct DirInfo
    {
        QDir dir;

        QSet<QString> textures_1x;

        std::unique_ptr<QFile> graphics_list; // open file to the directory's own graphics list (if any)
        bool graphics_list_empty = true;

        enum MakeSizeFiles_t
        {
            SIZEFILES_NONE = 0,
            SIZEFILES_PREVIEW_EPISODE,
            SIZEFILES_PREVIEW_ASSETS,
            SIZEFILES_ALL,
        };

        MakeSizeFiles_t make_size_files = SIZEFILES_NONE;
        bool make_fallback_masks = false;
        bool convert_font_inis = false;
    };

    struct EpisodeInfo
    {
        QString path;
        QString title;
        std::map<QString, QString> title_translations;
        bool playstyle_classic = false;
        QString char_block;
    };

    QTemporaryDir m_temp_input_dir_owner;
    QTemporaryDir m_temp_assets_dir_owner;

    QDir m_input_dir;
    QDir m_assets_dir;

    QTemporaryDir m_temp_dir_owner;
    QDir m_temp_dir;

    DirInfo m_cur_dir;
    QFile m_main_graphics_list;

    EpisodeInfo m_episode_info;

    void sync_cur_dir(const QString& in_file)
    {
        QFileInfo fi(in_file);
        QDir parent = fi.dir();

        if(parent.path() == m_cur_dir.dir.path())
            return;

        // remove old graphics list if empty
        if(m_cur_dir.graphics_list_empty && m_cur_dir.graphics_list)
            m_cur_dir.graphics_list->remove();

        // reset m_cur_dir
        m_cur_dir = DirInfo();
        m_cur_dir.dir = std::move(parent);

        QString rel_path = m_input_dir.relativeFilePath(m_cur_dir.dir.path());
        QStringList path_parts = QDir::toNativeSeparators(m_cur_dir.dir.path()).toLower().split(QDir::separator());

        // check for 1x textures: fonts
        if(path_parts[path_parts.size() - 1] == "fonts")
        {
            for(QFileInfo f : m_cur_dir.dir.entryInfoList({"*.ini"}))
            {
                // qInfo() << "Scanning" << f.absoluteFilePath();

                QSettings ini(f.absoluteFilePath(), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                ini.setIniCodec("UTF-8");
#endif

                ini.beginGroup("font-map");
                int texture_scale = ini.value("texture-scale", 1).toInt();
                QString texture_filename = ini.value("texture", "").toString().toLower();

                // qInfo() << "scale" << texture_scale << "fn" << texture_filename;

                if(texture_scale != 1 && !texture_filename.isEmpty())
                    m_cur_dir.textures_1x.insert(texture_filename);
            }

            // trigger font INI conversion
            if(m_spec.target_platform != TargetPlatform::Desktop)
                m_cur_dir.convert_font_inis = true;
        }

        // prepare size file policy and graphics list
        if(m_spec.target_platform == TargetPlatform::Desktop)
            m_cur_dir.make_size_files = DirInfo::SIZEFILES_NONE;
        else if(m_spec.package_type == PackageType::AssetPack && (rel_path == "graphics" || rel_path.startsWith("graphics/")))
        {
            m_cur_dir.make_size_files = DirInfo::SIZEFILES_PREVIEW_ASSETS;
            m_cur_dir.make_fallback_masks = (m_spec.target_platform != TargetPlatform::DSG);
        }
        else
        {
            // find path of temp dir's graphics list
            auto s = QDir::separator();
            QString graphics_list_path = m_temp_dir.filePath(rel_path) + s + "graphics.list";

            m_cur_dir.graphics_list.reset(new QFile(graphics_list_path));

            if(m_cur_dir.graphics_list->open(QIODevice::WriteOnly | QIODevice::Append))
            {
                m_cur_dir.make_size_files = DirInfo::SIZEFILES_PREVIEW_EPISODE;
                m_cur_dir.graphics_list_empty = (m_cur_dir.graphics_list->pos() == 0);
            }
            else
                m_cur_dir.make_size_files = DirInfo::SIZEFILES_ALL;
        }
    }

public:
    Spec m_spec;

    QString m_error;

    void log_file(LogCategory log_category, const QString& filename)
    {
        m_spec.log_file_callback(m_spec.callback_userdata, log_category, m_input_dir.relativeFilePath(filename).toStdString());
    }

    int progress(int cur_stage, int cur_file, int file_count, const QString& file_name)
    {
        const auto& rel_dir = (cur_stage <= STAGE_CONVERT) ? m_input_dir : m_temp_dir;
        return m_spec.progress_callback(m_spec.callback_userdata, cur_stage, STAGE_COUNT, stage_names[cur_stage], cur_file, file_count, rel_dir.relativeFilePath(file_name).toStdString());
    }

    bool save_gif(FIBITMAP* image, const QString& out_path)
    {
        FIBITMAP* image_2 = FreeImage_ColorQuantizeEx(image, FIQ_LFPQUANT);

        if(image_2)
        {
            bool success = FreeImage_Save(FIF_GIF, image_2, out_path.toUtf8());
            FreeImage_Unload(image_2);
            return success;
        }

        return false;
    }

    bool save_mask(FIBITMAP* mask, const QString& mask_path, bool force_bw)
    {
        // check for B/W support
        // first, try converting to 8-bit paletted mask with 2 colors
        FIBITMAP* mask_2 = FreeImage_ColorQuantizeEx(mask, FIQ_LFPQUANT, 2);
        bool is_bw = (mask_2 != nullptr);

        // check if the obtained mask really is black and white
        for(int i = 0; i < 2; i++)
        {
            if(!mask_2 || !is_bw)
                break;

            RGBQUAD color = FreeImage_GetPalette(mask_2)[i];
            if(color.rgbRed <  0x10 && color.rgbGreen <  0x10 && color.rgbBlue <  0x10)
                continue;

            if(color.rgbRed >= 0xf0 && color.rgbGreen >= 0xf0 && color.rgbBlue >= 0xf0)
                continue;

            is_bw = false;
        }

        // free temporary mask
        if(mask_2)
        {
            FreeImage_Unload(mask_2);
            mask_2 = nullptr;
        }

        // if black and white, make mask by thresholding (we want a 1-bit image)
        if(is_bw || force_bw)
        {
            mask_2 = FreeImage_Threshold(mask, 127);
        }
        // otherwise, first try making a 256-color image, then grayscale
        else
        {
            // try to make a 256-color image
            mask_2 = FreeImage_ColorQuantizeEx(mask, FIQ_LFPQUANT);

            // fallback to grayscale
            if(!mask_2)
                mask_2 = FreeImage_ConvertTo8Bits(mask);
        }

        if(mask_2)
        {
            bool success = FreeImage_Save(FIF_GIF, mask_2, mask_path.toUtf8());
            FreeImage_Unload(mask_2);
            return success;
        }

        return false;
    }

    bool convert_image(const QString& filename, const QString& in_path, const QString& out_path)
    {
        FIBITMAP* image = GraphicsLoad::loadImage(in_path);
        if(!image)
        {
            log_file(LogCategory::SkippedInvalid, in_path);
            return true;
        }

        QString filename_stem = filename.chopped(4);

        QString filename_type, filename_index, filename_dir;
        bool exceeds_max_GIF = false;
        bool standard_filename_format = validate_image_filename(filename, filename_type, filename_index, filename_dir, exceeds_max_GIF);

        // decide whether to look for mask
        bool may_have_mask = standard_filename_format;
        if(filename_type == "background2")
            may_have_mask = false;
        else if(filename_type == "tile")
            may_have_mask = false;
        if(filename_type == "block" && check_BlockHasNoMask(filename_index.toUInt()))
            may_have_mask = false;
        if(filename_type == "background" && check_BackgroundHasNoMask(filename_index.toUInt()))
            may_have_mask = false;
        if(exceeds_max_GIF)
            may_have_mask = false;

        if(filename.endsWith(".png") && standard_filename_format && !m_cur_dir.dir.entryInfoList({filename_stem + ".gif"}).isEmpty())
        {
            FreeImage_Unload(image);
            log_file(LogCategory::SkippedPNG, in_path);
            return true;
        }

        if(filename.endsWith(".gif") && !standard_filename_format && !m_cur_dir.dir.entryInfoList({filename_stem + ".png"}).isEmpty())
        {
            FreeImage_Unload(image);
            log_file(LogCategory::SkippedGIF, in_path);
            return true;
        }

        FIBITMAP* mask = nullptr;
        QString mask_path;
        if(filename.endsWith(".gif") && may_have_mask)
        {
            // find the mask!!
            auto found = m_cur_dir.dir.entryInfoList({filename_stem + "m.gif"});
            if(!found.isEmpty())
            {
                mask_path = found[0].absoluteFilePath();
                mask = GraphicsLoad::loadImage(mask_path);
            }

            // keep trying to find mask, in fallback dir and in appropriate graphics dir
            for(int i = 0; i < 3; i++)
            {
                if(mask)
                    break;

                auto s = QDir::separator();

                QString dir_to_check;

                // episode custom graphics
                if(i == 0)
                    dir_to_check = m_cur_dir.dir.path() + s + ".." + s;
                // fallback graphics
                else if(i == 1)
                    dir_to_check = m_assets_dir.path() + s + "graphics" + s + "fallback" + s;
                // normal graphics
                else
                    dir_to_check = m_assets_dir.path() + s + "graphics" + s + filename_dir + s;

                // look for mask GIF
                mask_path = dir_to_check + filename_stem + "m.gif";
                mask = GraphicsLoad::loadImage(mask_path);

                if(mask)
                    break;

                // look for PNG
                mask_path.clear();
                mask = GraphicsLoad::loadImage(dir_to_check + filename_stem + ".png");

                // convert PNG to mask
                if(mask)
                    GraphicsLoad::RGBAToMask(mask);
            }

            // Okay, finding a mask failed. So ignore the image.
            if(!mask)
            {
                log_file(LogCategory::SkippedNoMask, in_path);
                FreeImage_Unload(image);
                return false;
            }
        }

        TargetPlatform output_format = m_spec.target_platform;

        bool bitmask_required = GraphicsLoad::validateBitmaskRequired(image, mask);

        // find out whether mask is actually needed
        if(m_spec.convert_gifs != ConvertGIFs::All && bitmask_required)
            output_format = TargetPlatform::Desktop;
        else if(mask)
        {
            if(bitmask_required)
                log_file(LogCategory::ImageBitmaskLost, in_path);

            // merge with mask and free
            GraphicsLoad::mergeWithMask(image, mask);
            FreeImage_Unload(mask);
            mask = nullptr;
        }

        QString used_out_path = out_path;

        // save mask fallback here if we are in the global graphics dir
        if(m_cur_dir.make_fallback_masks && output_format != TargetPlatform::Desktop && may_have_mask)
        {
            FIBITMAP* fallback_mask = FreeImage_Clone(image);

            if(fallback_mask)
            {
                GraphicsLoad::RGBAToMask(fallback_mask);

                QString fallback_mask_path = m_temp_dir.filePath("graphics/fallback/");

                // make fallback folder if it isn't present
                if(!QFileInfo::exists(fallback_mask_path))
                    m_temp_dir.mkdir(fallback_mask_path);

                // append filename
                fallback_mask_path += filename.chopped(4) + "m.gif";

                // don't clobber a version copied from the original fallback folder
                if(!QFileInfo::exists(fallback_mask_path))
                    save_mask(fallback_mask, fallback_mask_path, filename.endsWith(".png"));

                FreeImage_Unload(fallback_mask);
            }
        }


        // scale the image as needed

        // save these if it becomes necessary
        int orig_w = FreeImage_GetWidth(image);
        int orig_h = FreeImage_GetHeight(image);

        // 2x downscale by default
        if(m_spec.target_platform != TargetPlatform::Desktop && !m_cur_dir.textures_1x.contains(filename))
        {
            FIBITMAP* scaled = GraphicsLoad::fast2xScaleDown(image);
            FreeImage_Unload(image);
            if(!scaled)
            {
                m_error = "Scaling failed at [";
                m_error += in_path;
                m_error += "], aborting...";
                return false;
            }

            image = scaled;

            if(mask)
            {
                FIBITMAP* scaled_mask = GraphicsLoad::fast2xScaleDown(mask);
                FreeImage_Unload(mask);
                if(!scaled_mask)
                {
                    m_error = "Scaling failed at [";
                    m_error += in_path;
                    m_error += "], aborting...";
                    return false;
                }

                mask = scaled_mask;
            }
        }

        int image_h = FreeImage_GetHeight(image);
        int image_w = FreeImage_GetWidth(image);

        if(filename.startsWith("mario-") || filename.startsWith("luigi-") || filename.startsWith("peach-") || filename.startsWith("toad-") || filename.startsWith("link-"))
        {
            // try to handle mask eventually
            // FIBITMAP* mask_bak = FreeImage_Clone(mask);
            if(!mask && shrink_player_texture(&image, filename.startsWith("link-")))
            {
                image_w = FreeImage_GetWidth(image);
                image_h = FreeImage_GetHeight(image);
            }
            else
                log_file(LogCategory::ImagePlayerCompressFailed, in_path);
        }

        // save the image!
        bool save_success = false;
        QString size_text;
        if(output_format == TargetPlatform::Desktop)
        {
            log_file(LogCategory::ImageBitmask, in_path);

            if(image_w == orig_w)
                save_success = QFile::copy(in_path, out_path);
            else
                save_success = save_gif(image, out_path);

            if(save_success && mask)
            {
                QString mask_out_path = out_path.chopped(4) + "m.gif";

                if(!mask_path.isEmpty() && image_w == orig_w)
                    save_success = QFile::copy(mask_path, mask_out_path);
                else
                    save_success = save_mask(mask, mask_out_path, false);

                FreeImage_Unload(mask);
            }

            // set the size text here...!
            size_text = QString("%1\n%2\n").arg(orig_w, 4).arg(orig_h, 4);
        }
        else if(output_format == TargetPlatform::T3X)
        {
            // break these into their own functions

            used_out_path = out_path.chopped(4) + ".t3x";

            FreeImage_FlipVertical(image);

            Tex3DS::Params params;
            params.compression_format = Tex3DS::COMPRESSION_LZ11;
            params.process_format = Tex3DS::RGBA5551;

            // check for transparency
            if(!GraphicsLoad::validateForDepthTest(image))
            {
                log_file(LogCategory::ImageTransparent, in_path);
                params.process_format = Tex3DS::RGBA8888;
            }

            // image parts
            save_success = true;
            for(int i = 0; i < 3; i++)
            {
                int start_y = i * 1024;
                int h = image_h - start_y;
                if(h <= 0)
                    break;
                if(h > 1024)
                    h = 1024;

                int w = image_w;
                if(w > 1024)
                    w = 1024;

                int stride = FreeImage_GetPitch(image);

                params.input_img = Tex3DS::Image(w, h);

                if(w * 4 == stride)
                    memcpy(params.input_img.pixels.data(), FreeImage_GetBits(image) + stride * 1024 * i, params.input_img.pixels.size() * sizeof(Tex3DS::RGBA));
                else
                {
                    for(int row = 0; row < h; row++)
                        memcpy(params.input_img.pixels.data() + params.input_img.stride * row, FreeImage_GetBits(image) + stride * (1024 * i + row), params.input_img.w * sizeof(Tex3DS::RGBA));
                }

                params.output_path = (const char*)used_out_path.toUtf8();
                if(i > 0)
                    params.output_path.push_back('0' + i);

                save_success &= Tex3DS::Process(params);
            }

            if(image_w > 1024 || image_h > 1024 * 3)
                log_file(LogCategory::ImageCropped, in_path);

            size_text = QString("%1\n%2\n").arg(image_w * 2, 4).arg(image_h * 2, 4);
        }
        else if(output_format == TargetPlatform::TPL)
        {
            used_out_path = out_path.chopped(4) + ".tpl";

            FreeImage_FlipVertical(image);

            // palettize texture
            int input_w_full = FreeImage_GetWidth(image);
            int input_h_full = FreeImage_GetHeight(image);

            // do PaletteTex conversion on the whole thing
            PaletteTex p(FreeImage_GetBits(image), input_w_full, input_h_full);

            save_success = true;

            PaletteTex::PaletteSize used_size = PaletteTex::HALF;
            if(!p.convert(used_size, 90))
            {
                used_size = PaletteTex::BYTE;

                if(!p.convert(used_size, 0))
                    save_success = false;
            }

            if(save_success)
            {
                // decide whether to do horizontal mode here
                bool horiz_mode = (input_w_full > 1024 && input_h_full <= 1024);

                save_success = false;

                for(int i = 0; i < 3; i++)
                {
                    // resolve correct portion of image to save
                    int start_x = 0;
                    int start_y = 0;

                    int input_w = input_w_full;
                    int input_h = input_h_full;

                    if(horiz_mode)
                    {
                        start_x = i * 1024;
                        input_w = input_w_full - start_x;
                        if(input_w <= 0)
                            break;
                    }
                    else
                    {
                        start_y = i * 1024;
                        input_h = input_h_full - start_y;
                        if(input_h <= 0)
                            break;
                    }

                    if(input_w > 1024)
                        input_w = 1024;
                    if(input_h > 1024)
                        input_h = 1024;

                    // fill TPL data
                    auto input_data = p.indexes() + input_w_full * start_y + start_x;

                    QByteArray tpl_data;
                    tpl_data.resize(ExportTPL::data_size(used_size, input_w, input_h));
                    ExportTPL::fill_data(reinterpret_cast<uint8_t*>(tpl_data.data()), used_size, input_w, input_h, p.palette(), input_data, input_w_full);

                    auto tpl_path = used_out_path;
                    if(i > 0)
                        tpl_path += char('0' + i);

                    QFile file(tpl_path);
                    if(!file.open(QIODevice::WriteOnly))
                        break;
                    else
                    {
                        file.write(tpl_data);
                        file.close();
                        save_success = true;
                    }
                }

                if(image_h > 1024 * 3 || image_w > 1024 * 3 || (!horiz_mode && image_w > 1024))
                    log_file(LogCategory::ImageCropped, in_path);
            }

            if(save_success)
                size_text = QString("%1\n%2\n").arg(image_w * 2, 4).arg(image_h * 2, 4);
        }
        else if(output_format == TargetPlatform::DSG)
        {
            used_out_path = out_path.chopped(4) + ".dsg";

            int flags = 0;

            // downscale image further, and threshold alpha, here.
            int container_w = next_power_of_2(image_w);
            int container_h = next_power_of_2(image_h);

            while((container_w * container_h) / 2 > 131072 || container_w > 1024 || container_h > 1024)
            {
                // log a warning here
                flags++;
                container_w = next_power_of_2(image_w / (1 << flags));
                container_h = next_power_of_2(image_h / (1 << flags));
            }

            if(flags)
            {
                FIBITMAP* scaled = FreeImage_Rescale(image, image_w / (1 << flags), image_h / (1 << flags), FILTER_BILINEAR);

                FreeImage_Unload(image);

                if(!scaled)
                {
                    m_error = "Scaling failed at [";
                    m_error += in_path;
                    m_error += "], aborting...";
                    return false;
                }

                image = scaled;

                log_file(LogCategory::ImageScaledDown, in_path);
            }

            // palettize texture
            int input_w = FreeImage_GetWidth(image);
            int input_h = FreeImage_GetHeight(image);
            PaletteTex p(FreeImage_GetBits(image), input_w, input_h);
            if(!p.convert(PaletteTex::HALF, 0))
            {
                FreeImage_Unload(image);
                m_error = "Palettizing failed at [";
                m_error += in_path;
                m_error += "], aborting...";
                return false;
            }

            bool fully_opaque = (p.palette()[0].a >= 224);
            if(fully_opaque)
                flags |= 16;

            int pixels_per_byte = 2;

            bool semi_trans = (p.palette()[0].a >= 32) && (p.palette()[0].a < 224);
            bool multi_trans = (p.palette_used() > 1) && (p.palette()[1].a < 224);
            bool rgb32_a3 = (semi_trans || multi_trans) && (container_w * container_h) <= 131072;
            if(rgb32_a3)
            {
                log_file(LogCategory::ImageTransparent, in_path);
                flags |= 32;
                flags |= 16; // use pixel alpha values, not palette entry 1 color key
                pixels_per_byte = 1;
            }
            else if(semi_trans || multi_trans)
                log_file(LogCategory::ImageTransparencyLost, in_path);

            // save the image here
            QByteArray dsg_data;
            dsg_data.resize(32 + container_w * container_h / pixels_per_byte);

            // save palette
            for(int i = 0; i < 16; ++i)
            {
                liq_color c = p.palette()[i];
                uint8_t* dest = reinterpret_cast<unsigned char*>(&dsg_data.data()[i * 2]);

                write_uint16_le(dest, color_to_rgb5a1(c));
            }

            // save image indexes
            const uint8_t* src = p.indexes();
            uint8_t* out_base = reinterpret_cast<unsigned char*>(&dsg_data.data()[32]);
            int out_stride = container_w / pixels_per_byte;
            for(int row = 0; row < input_h; row++)
            {
                uint8_t* dest = out_base + (input_h - 1 - row) * out_stride;

                if(!rgb32_a3)
                {
                    int col;
                    for(col = 0; col < input_w - 1; col += 2)
                    {
                        *(dest++) = (src[0] << 0) | (src[1] << 4);
                        src += 2;
                    }

                    if(col < input_w)
                    {
                        *(dest++) = (src[0] << 4);
                        src += 1;
                    }
                }
                else
                {
                    for(int col = 0; col < input_w; col++)
                    {
                        int idx = *(src++);
                        *(dest++) = idx | ((p.palette()[idx].a >> 5) << 5);
                    }
                }
            }

            QFile file(used_out_path);
            if(!file.open(QIODevice::WriteOnly))
                save_success = false;
            else
            {
                file.write(dsg_data);
                file.close();
                save_success = true;

                size_text = QString("%1\n%2\n%3\n").arg(image_w * 2, 4).arg(image_h * 2, 4).arg(flags);
            }
        }

        // cleanup the image
        FreeImage_Unload(image);

        // check if graphics list present
        QFile* write_graphics_list = nullptr;

        if(m_cur_dir.graphics_list)
            write_graphics_list = m_cur_dir.graphics_list.get();
        else if(m_main_graphics_list.isOpen())
            write_graphics_list = &m_main_graphics_list;

        // write graphics list
        if(write_graphics_list && save_success && !size_text.isEmpty())
        {
            bool valid = standard_filename_format;

            // only allow valid subdirectories for main graphics list
            if(valid && write_graphics_list == &m_main_graphics_list)
            {
                if(!m_cur_dir.dir.path().endsWith(filename_dir))
                    valid = false;
            }

            if(valid)
            {
                // make path relative to graphics list
                QFileInfo fi(write_graphics_list->fileName());
                QDir graphics_list_parent = fi.dir();
                QString relative_out_path = graphics_list_parent.relativeFilePath(used_out_path);

                write_graphics_list->write(filename_type.toUtf8());
                write_graphics_list->write(" ");
                write_graphics_list->write(filename_index.toUtf8());
                write_graphics_list->write("\n");
                write_graphics_list->write(relative_out_path.toUtf8());
                write_graphics_list->write("\n");
                write_graphics_list->write(size_text.toUtf8());
                write_graphics_list->write("\n");

                if(write_graphics_list == m_cur_dir.graphics_list.get())
                    m_cur_dir.graphics_list_empty = false;
            }
            else
                write_graphics_list = nullptr;
        }

        // check if size file needed
        bool make_size_file = false;

        if(m_cur_dir.make_size_files == DirInfo::SIZEFILES_ALL)
            make_size_file = true;
        else if(m_cur_dir.make_size_files != DirInfo::SIZEFILES_NONE && !write_graphics_list)
        {
            // always write if filename doesn't follow graphics.list format
            make_size_file = true;
        }
        else if(m_cur_dir.make_size_files == DirInfo::SIZEFILES_PREVIEW_ASSETS)
        {
            // UI and background sprites
            make_size_file = in_path.contains("/graphics/ui/")
                || in_path.contains("/graphics/background2/");
        }
        else if(m_cur_dir.make_size_files == DirInfo::SIZEFILES_PREVIEW_EPISODE)
        {
            // player and mount sprites
            make_size_file = filename_type == "mario"
                || filename_type == "luigi"
                || filename_type == "peach"
                || filename_type == "toad"
                || filename_type == "link"
                || filename.contains("yoshi")
                || filename.contains("mount");
        }

        if(m_spec.target_platform == TargetPlatform::Desktop)
            make_size_file = false;

        // write to size file
        if(make_size_file && save_success && !size_text.isEmpty())
        {
            QFile file(used_out_path + ".size");
            if(!file.open(QIODevice::WriteOnly))
                save_success = false;

            file.write(size_text.toUtf8());
            file.close();
        }

        if(!save_success)
        {
            m_error = "Saving failed at [";
            m_error += in_path;
            m_error += "], aborting...";
        }

        return save_success;
    }

    bool convert_font_ini(const QString&, const QString& in_path, const QString& out_path)
    {
        if(!QFile::copy(in_path, out_path))
            return false;

        QSettings ini(out_path, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ini.setIniCodec("UTF-8");
#endif

        ini.beginGroup("font-map");
        QString texture_filename = ini.value("texture", "").toString();

        if(!texture_filename.isEmpty() && (texture_filename.toLower().endsWith(".png") || texture_filename.toLower().endsWith(".gif")))
        {
            texture_filename.chop(4);
            if(m_spec.target_platform == TargetPlatform::T3X)
                texture_filename += ".t3x";
            else if(m_spec.target_platform == TargetPlatform::TPL)
                texture_filename += ".tpl";
            else if(m_spec.target_platform == TargetPlatform::DSG)
                texture_filename += ".dsg";

            ini.setValue("texture", texture_filename);
        }

        ini.endGroup();

        ini.sync();

        if(ini.status() != QSettings::NoError)
            return false;

        return true;
    }

    bool convert_sfx(const QString&, const QString& in_path, const QString& out_path)
    {
        // qInfo() << "ogg2wav" << out_path;

        Mix_Chunk* ch = Mix_LoadWAV(in_path.toUtf8().data());
        if(!ch)
            return false;

        // qInfo() << "length" << ch->alen;

        QByteArray wav_data;

        ExportAudio::export_wav(wav_data, ch);

        Mix_FreeChunk(ch);

        QFile file(out_path);
        if(!file.open(QIODevice::WriteOnly))
            return false;
        else
        {
            file.write(wav_data);
            file.close();
            return true;
        }
    }

    bool convert_music_16m(const QString&, const QString& in_path, const QString& out_path)
    {
        // qInfo() << "mus2qoa" << out_path;
        log_file(LogCategory::AudioQoa, in_path);

        Mix_Chunk* ch = Mix_LoadWAV(in_path.toUtf8().data());
        if(!ch)
            return false;

        QByteArray qoa_data;

        ExportAudio::export_qoa(qoa_data, ch);

        Mix_FreeChunk(ch);

        QFile file(out_path + ".qoa");
        if(!file.open(QIODevice::WriteOnly))
            return false;
        else
        {
            file.write(qoa_data);
            file.close();
            return true;
        }
    }

    bool scan_episode(const QString& in_path)
    {
        QString rel_path = m_input_dir.relativeFilePath(in_path);

        if(!m_episode_info.path.isEmpty())
        {
            qInfo() << "Input directory includes multiple episodes: [" << m_episode_info.path << "] and [" << rel_path << "]";
            return false;
        }

        if(rel_path.contains(QDir::separator()))
        {
            qInfo() << "Episode file must be in top-level directory, not subdirectory:" << in_path;
            return false;
        }

        m_episode_info.path = rel_path;

        WorldData head;

        if(!FileFormats::OpenWorldFileHeader(in_path, head))
        {
            qInfo() << "Episode [" << rel_path << "] could not be opened.";
            return false;
        }

        bool is_wldx = (head.meta.RecentFormat == WorldData::PGEX);
        bool is_wld38a = (head.meta.RecentFormat == WorldData::SMBX38A);
        if(is_wldx && head.meta.configPackId == "SMBX2")
        {
            qInfo() << "Episode [" << rel_path << "] targets SMBX2. Cannot proceed.";
            return false;
        }

        if(is_wld38a)
        {
            qInfo() << "Episode [" << rel_path << "] targets SMBX-38A. Cannot proceed.";
            return false;
        }

        // set episode title
        m_episode_info.title = head.EpisodeTitle;

        if(m_episode_info.title.isEmpty())
            m_episode_info.title = rel_path;

        // set playstyle
        if(is_wldx)
            m_episode_info.playstyle_classic = false;
        else
            m_episode_info.playstyle_classic = true;

        // set character block
        m_episode_info.char_block = "00000";

        if(head.nocharacter1)
            m_episode_info.char_block[0] = '1';

        if(head.nocharacter2)
            m_episode_info.char_block[1] = '1';

        if(head.nocharacter3)
            m_episode_info.char_block[2] = '1';

        if(head.nocharacter4)
            m_episode_info.char_block[3] = '1';

        if(head.nocharacter5)
            m_episode_info.char_block[4] = '1';

        return true;
    }

    void scan_episode_translation(const QString& in_path)
    {
        QString lang_name = in_path.split('_').last().split('.').first().toLower();
        if(lang_name == "metadata")
            return;

        QFile file(in_path);
        if(!file.open(QIODevice::ReadOnly))
            return;

        QByteArray got = file.readAll();
        QString translated_title;

        try
        {
            const nlohmann::json translation_data = nlohmann::json::parse(got.constData());
            translated_title = QString::fromStdString(translation_data.value(" episode_title", ""));
        }
        catch(const std::exception& e)
        {
            qInfo() << "Error when parsing" << in_path << e.what();
            return;
        }

        if(translated_title.isEmpty())
        {
            qInfo() << "Translation file" << in_path << "missing episode title";
            return;
        }

        // qInfo() << "Episode title in language" << lang_name << "is" << translated_title;
        m_episode_info.title_translations[lang_name] = translated_title;
    }

    bool convert_file(const QString& filename, const QString& in_path, const QString& out_path)
    {
        sync_cur_dir(in_path);

        bool is_non_tracker_music = filename.endsWith(".mp3") || filename.endsWith(".ogg")
            || filename.endsWith(".vgm") || filename.endsWith(".vgz") || filename.endsWith(".mid")
            || filename.endsWith(".nsf") || filename.endsWith(".hes")
            || filename.endsWith(".pttune") || filename.endsWith(".ptcop")
            || filename.endsWith(".wma") || filename.endsWith(".gbs");

        bool is_font = filename.endsWith(".ttf") || filename.endsWith(".otf") || filename.endsWith(".pcf") || filename.endsWith(".woff") || filename.endsWith(".woff2");

        if(filename.endsWith("m.gif") && in_path.contains("/graphics/fallback/"))
        {
            // want to clobber auto-generated masks
            if(QFile::exists(out_path))
                QFile::remove(out_path);

            return QFile::copy(in_path, out_path);
        }
        else if(m_spec.target_platform != TargetPlatform::Desktop && filename.endsWith("m.gif"))
            return true;
        // defer these until the end
        else if(m_spec.package_type == PackageType::AssetPack && m_spec.target_platform == TargetPlatform::DSG && (in_path.contains("/sound/") || in_path.contains("/music/")))
            return true;
        else if(m_spec.target_platform != TargetPlatform::Desktop && (filename.endsWith(".png") || filename.endsWith(".gif")) && !in_path.contains("/graphics/fallback/"))
            return convert_image(filename, in_path, out_path);
        else if(filename.endsWith(".ini") && m_cur_dir.convert_font_inis)
            return convert_font_ini(filename, in_path, out_path);
        else if(m_spec.target_platform != TargetPlatform::Desktop && filename.endsWith(".ogg") && in_path.contains("/sound/"))
            return convert_sfx(filename, in_path, out_path);
        else if(m_spec.target_platform == TargetPlatform::DSG && (filename.endsWith(".wav") || is_non_tracker_music))
        {
            return convert_music_16m(filename, in_path, out_path);
        }
        else if(filename.endsWith(".spc") && m_spec.target_platform == TargetPlatform::T3X)
        {
            log_file(LogCategory::AudioSpc2It, in_path);
            return spc2it_convert(in_path.toUtf8().data(), out_path.toUtf8().data()) == 0;
        }
        else if(m_spec.package_type == PackageType::Episode && (filename.endsWith(".wld") || filename.endsWith(".wldx")))
        {
            // load the episode information, or complain if there are multiple WLD(X) files
            if(!scan_episode(in_path))
                return false;

            return QFile::copy(in_path, out_path);
        }
        else if(m_spec.package_type == PackageType::Episode && filename.startsWith("translation_") && filename.endsWith(".json"))
        {
            // load the translated episode name
            scan_episode_translation(in_path);
            return QFile::copy(in_path, out_path);
        }
        else if(filename.endsWith(".xcf") || filename.endsWith(".db") || filename == ".DS_Store"
            || filename.endsWith(".psg") || filename.endsWith(".ps") || filename == "progress.json"
            || filename.endsWith("tst") || filename.endsWith(".tileset.ini") || filename.endsWith(".sav")
            || filename.endsWith(".savx") || filename.endsWith(".pdn") || filename.endsWith(".jpg")
            || filename.endsWith(".jpeg") || filename.endsWith(".ico") || filename.endsWith(".icns")
            || filename.endsWith(".bmp") || filename.endsWith(".pal")
            || filename.endsWith(".dll") || filename.endsWith(".exe")
            || filename.endsWith(".rar") || filename.endsWith(".zip") || filename.endsWith(".7z")
            || filename.endsWith(".xte") || filename.endsWith(".xta")
            || filename.endsWith(".odt") || filename.endsWith(".pdf") || filename.endsWith(".md"))
        {
            // banned filenames
            log_file(LogCategory::SkippedUnused, in_path);
            return true;
        }
        // skip TTFs on DSG
        else if(m_spec.target_platform == TargetPlatform::DSG && is_font)
            return true;
        else
        {
            if(!filename.endsWith(".txt") && !filename.endsWith(".lvl") && !filename.endsWith(".lvlx")
                && !filename.endsWith(".wld") && !filename.endsWith(".wldx")
                && !is_non_tracker_music && !filename.endsWith(".spc")
                && !filename.endsWith(".it") && !filename.endsWith(".mod") && !filename.endsWith(".xm") && !filename.endsWith(".s3m")
                && !(filename.startsWith("translation_") && filename.endsWith(".json"))
                && !(filename.startsWith("assets_") && filename.endsWith(".json"))
                && !(filename.startsWith("thextech_") && filename.endsWith(".json"))
                && !filename.endsWith(".ini")
                && !filename.endsWith(".png") && !filename.endsWith(".gif")
                && !is_font)
            {
                log_file(LogCategory::CopiedUnknown, in_path);
            }

            return QFile::copy(in_path, out_path);
        }
    }

    bool build_temp_dir()
    {
        m_input_dir.makeAbsolute();

        if(!m_temp_dir_owner.isValid())
        {
            m_error = "Failed to create temporary directory";
            return false;
        }

        // check that this isn't already converted for a non-desktop platform
        {
            QSettings meta(m_input_dir.filePath("_meta.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            meta.setIniCodec("UTF-8");
#endif

            meta.beginGroup("content");
            QString prev_target = meta.value("platform", "main").toString();
            meta.endGroup();
            if(prev_target != "main")
            {
                m_error = "Provided content is already converted for use on [" + prev_target + "]";
                return false;
            }
        }

        m_temp_dir.setPath(m_temp_dir_owner.path());

        bool asset_pack_for_non3ds = (m_spec.package_type == PackageType::AssetPack && m_spec.target_platform != TargetPlatform::T3X);

        if(m_spec.legacy_archives && (m_spec.package_type == PackageType::Episode || asset_pack_for_non3ds))
        {
            QString subfolder_name = QFileInfo(QString::fromStdString(m_spec.destination)).baseName();
            if(subfolder_name.isEmpty())
            {
                m_error = "Empty destination path";
                return false;
            }

            m_temp_dir.mkdir(subfolder_name);
            m_temp_dir.setPath(m_temp_dir.filePath(subfolder_name));
        }

        m_input_dir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

        if(m_spec.package_type == PackageType::AssetPack)
        {
            if(!m_temp_dir.exists("graphics") && !m_temp_dir.mkdir("graphics"))
            {
                m_error = "Failed to create graphics directory";
                return false;
            }

            m_main_graphics_list.setFileName(m_temp_dir.filePath(QString("graphics") + QDir::separator() + "graphics.list"));
            m_main_graphics_list.open(QIODevice::WriteOnly);

            if(!m_main_graphics_list.isOpen())
            {
                m_error = "Failed to create main graphics.list";
                return false;
            }
        }

        // check count of files
        size_t file_count = 0;
        QDirIterator count_it(m_input_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        while(count_it.hasNext())
        {
            file_count++;
            count_it.next();
        }

        // iterate over all directories
        size_t files_done = 0;
        QDirIterator it(m_input_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while(it.hasNext())
        {
            files_done++;

            QString abs_path = it.next();
            QString rel_path = m_input_dir.relativeFilePath(abs_path);
            QString temp_path = m_temp_dir.filePath(rel_path);

            if(m_spec.package_type == PackageType::AssetPack)
            {
                if(rel_path.startsWith("screenshots") || rel_path.startsWith("logs")
                    || rel_path.startsWith("settings") || rel_path.startsWith("gif-recordings")
                    || rel_path.startsWith("gameplay-records"))
                {
                    log_file(LogCategory::SkippedUnused, abs_path);
                    continue;
                }
            }

            QString filename = it.fileName().toLower();

            if(it.fileInfo().isDir())
            {
                // qInfo() << "making" << temp_path;
                if(!m_temp_dir.exists(temp_path) && !m_temp_dir.mkdir(temp_path))
                {
                    m_error = "Failed to create directory ";
                    m_error += temp_path;
                    return false;
                }

                continue;
            }

            if(progress(STAGE_CONVERT, files_done, file_count, abs_path) < 0)
            {
                m_error = "Conversion canceled by user";
                return false;
            }

            if(!convert_file(filename, abs_path, temp_path))
            {
                m_error = "Failed to convert file ";
                m_error += temp_path;
                return false;
            }
        }

        m_main_graphics_list.close();

        if(m_cur_dir.graphics_list)
        {
            m_cur_dir.graphics_list->close();

            if(m_cur_dir.graphics_list_empty)
                m_cur_dir.graphics_list->remove();
        }

        // do post-processing steps
        if(progress(STAGE_POST_PROCESS, 0, 1, "") < 0)
        {
            m_error = "Conversion canceled by user";
            return false;
        }

        bool post_process_success = true;

        if(m_spec.package_type == PackageType::Episode)
            post_process_success &= update_meta_episode();
        else
            post_process_success &= update_meta_asset_pack();

        if(m_spec.target_platform == TargetPlatform::DSG && m_spec.package_type == PackageType::AssetPack)
            post_process_success &= build_soundbank();

        return post_process_success;
    }

    bool update_meta_episode()
    {
        if(m_episode_info.path.isEmpty())
        {
            qInfo() << "No episode found";
            return false;
        }

        // load ID from asset pack's gameinfo
        QSettings gameinfo(m_assets_dir.filePath("gameinfo.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        gameinfo.setIniCodec("UTF-8");
#endif

        gameinfo.beginGroup("game");
        QString pack_id = gameinfo.value("id", "").toString();
        gameinfo.endGroup();


        // save info to _meta.ini
        QSettings meta(m_temp_dir.filePath("_meta.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        meta.setIniCodec("UTF-8");
#endif

        // contents
        meta.beginGroup("content");

        meta.setValue("engine", "TheXTech");
        meta.setValue("platform", platform_name());
        meta.setValue("type", "episode");
        meta.setValue("filename", m_episode_info.path);

        if(!pack_id.isEmpty() && meta.value("pack-id", "").toString().isEmpty())
            meta.setValue("pack-id", pack_id);

        meta.endGroup();

        // properties
        meta.beginGroup("properties");

        meta.setValue("title", m_episode_info.title);
        meta.setValue("is-classic", m_episode_info.playstyle_classic);
        meta.setValue("char-block", m_episode_info.char_block);

        for(const auto& i : m_episode_info.title_translations)
        {
            QString key = "title-";
            key += i.first;
            meta.setValue(key, i.second);
        }

        meta.endGroup();

        // finish and save
        meta.sync();

        if(meta.status() != QSettings::NoError)
        {
            qInfo() << "Failed: could not update _meta.ini";
            return false;
        }

        return true;
    }

    const char* platform_name() const
    {
        switch(m_spec.target_platform)
        {
        case TargetPlatform::Desktop:
            return "main";
        case TargetPlatform::TPL:
            return "wii";
        case TargetPlatform::T3X:
            return "3ds";
        case TargetPlatform::DSG:
            return "dsi";
        default:
            return "unknown";
        }
    }

    bool update_meta_asset_pack()
    {
        // load several values from gameinfo
        QSettings gameinfo(m_temp_dir.filePath("gameinfo.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        gameinfo.setIniCodec("UTF-8");
#endif

        gameinfo.beginGroup("game");
        QString pack_id = gameinfo.value("id", "").toString();
        QString version = gameinfo.value("version", "").toString();
        bool show_id = gameinfo.value("show-id", false).toBool();
        gameinfo.endGroup();

        gameinfo.beginGroup("android");
        QString gfx_logo = gameinfo.value("logo", "").toString();
        QString gfx_background = gameinfo.value("background", "").toString();
        int gfx_background_frames = gameinfo.value("background-frames", 0).toInt();
        int gfx_background_delay = gameinfo.value("background-delay", 0).toInt();
        gameinfo.endGroup();

        // validate:
        if(pack_id.isEmpty())
        {
            qInfo() << "Failed: gameinfo.ini does not have a pack id";
            return false;
        }


        // save info to _meta.ini
        QSettings meta(m_temp_dir.filePath("_meta.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        meta.setIniCodec("UTF-8");
#endif

        // contents
        meta.beginGroup("content");

        meta.setValue("engine", "TheXTech");
        meta.setValue("platform", platform_name());
        meta.setValue("type", "asset-pack");
        meta.setValue("pack-id", pack_id);

        if(!version.isEmpty())
            meta.setValue("version", version);

        meta.endGroup();

        // properties
        meta.beginGroup("properties");

        if(show_id)
            meta.setValue("show-id", show_id);

        if(!gfx_logo.isEmpty())
            meta.setValue("logo", gfx_logo);

        if(!gfx_background.isEmpty())
            meta.setValue("background", gfx_background);

        if(gfx_background_frames)
            meta.setValue("background-frames", gfx_background_frames);

        if(gfx_background_delay)
            meta.setValue("background-delay", gfx_background_delay);

        meta.endGroup();

        // finish and save
        meta.sync();

        if(meta.status() != QSettings::NoError)
        {
            qInfo() << "Failed: could not update _meta.ini";
            return false;
        }

        return true;
    }

    bool build_soundbank()
    {
        // do something about "discovered SPC files", and eventually "discovered music / sound"
        std::map<QString, QString> music_filenames;
        std::map<QString, QString> sound_filenames;

        QSettings music_ini(m_temp_dir.filePath("music.ini"), QSettings::IniFormat);
        QSettings sound_ini(m_temp_dir.filePath("sounds.ini"), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        music_ini.setIniCodec("UTF-8");
        sound_ini.setIniCodec("UTF-8");
#endif

        for(int i = 0; i < 2; i++)
        {
            auto& filenames = (i) ? music_filenames : sound_filenames;
            auto& ini = (i) ? music_ini : sound_ini;

            for(const QString& group : ini.childGroups())
            {
                ini.beginGroup(group);
                QString filename = ini.value("file", "").toString();
                ini.endGroup();

                filenames[group] = filename;
            }
        }

        for(auto& mus : music_filenames)
        {
            QString real_fn = mus.second;
            QString mus_args = "";

            if(mus.second.contains('|'))
            {
                real_fn = mus.second.section('|', 0, -2);
                mus_args = mus.second.section('|', -1, -1);
            }

            QString use_fn_in = m_input_dir.filePath("music" + (QDir::separator() + real_fn));
            QString use_fn_out = m_temp_dir.filePath("music" + (QDir::separator() + real_fn));

            if(QFile::exists(use_fn_in + ".spc"))
            {
                use_fn_in += ".spc";
                use_fn_out += ".spc";
            }

            if(QFile::exists(use_fn_in + ".it"))
            {
                use_fn_in += ".it";
                use_fn_out += ".it";
            }

            bool success = false;

            if(!QFileInfo(use_fn_in).isFile())
            {
                // just skip it
                use_fn_out = "";
            }
            else if(use_fn_in.endsWith(".spc"))
            {
                log_file(LogCategory::AudioSpc2It, use_fn_in);
                use_fn_out += ".it";
                success = (spc2it_convert(use_fn_in.toUtf8().data(), use_fn_out.toUtf8().data()) == 0);
            }
            else if(use_fn_in.endsWith(".it") || use_fn_in.endsWith(".mod") || use_fn_in.endsWith(".s3m") || use_fn_in.endsWith(".xm"))
                success = QFile::copy(use_fn_in, use_fn_out);
            else
            {
                // currently can't use mus_args
                success = convert_music_16m(use_fn_in, use_fn_in, use_fn_out);
                use_fn_out += ".qoa";
            }

            mus.second = use_fn_out;

            if(!success)
            {
                // warn the user
            }
        }

        for(auto& snd : sound_filenames)
        {
            QString use_fn_in = m_input_dir.filePath("sound" + (QDir::separator() + snd.second));
            QString use_fn_out = m_temp_dir.filePath("sound" + (QDir::separator() + snd.second));

            use_fn_out += ".wav";

            bool success = convert_sfx(use_fn_in, use_fn_in, use_fn_out);

            snd.second = use_fn_out;

            if(!success)
            {
                // warn the user
            }
        }

        QString soundbank_ini_path = m_temp_dir.filePath("soundbank.ini");

        std::vector<QByteArray> mmutil_args_str;

        for(const auto& mus : music_filenames)
        {
            if(mus.second.endsWith(".qoa") || !QFileInfo(mus.second).isFile())
                continue;

            mmutil_args_str.push_back(mus.second.toUtf8());
        }

        for(const auto& snd : sound_filenames)
        {
            if(!QFileInfo(snd.second).isFile())
                continue;

            mmutil_args_str.push_back(snd.second.toUtf8());
        }

        std::vector<char*> mmutil_args;
        for(auto& arg : mmutil_args_str)
            mmutil_args.push_back(reinterpret_cast<char*>(arg.data()));

        int mmutil_ret = MMUTIL_Create(mmutil_args.data(), mmutil_args.size(),
            m_temp_dir.filePath("soundbank.bin").toUtf8().data(),
            nullptr,
            m_temp_dir.filePath("soundbank.ini").toUtf8().data(),
            false,
            0, 0);

        bool mmutil_success = (mmutil_ret == 0);

        // use the resulting soundbank to update sound and music inis!
        {
            QFile soundbank_ini_file(soundbank_ini_path);
            soundbank_ini_file.open(QIODevice::ReadOnly | QIODevice::Text);

            QTextStream soundbank_ini(&soundbank_ini_file);
            soundbank_ini.setCodec("UTF-8");

            bool in_samples = false;
            std::map<QString, int> resolved_modules;
            std::map<QString, int> resolved_samples;
            while(!soundbank_ini.atEnd())
            {
                QString line = soundbank_ini.readLine();

                if(line.startsWith('[') && line == "[samples]")
                    in_samples = true;

                if(!line.contains(" = "))
                    continue;

                QString path = line.section(" = ", 0, -2);
                int resolved = line.section(" = ", -1, -1).toInt();

                auto& dest = (in_samples) ? resolved_samples : resolved_modules;

                dest[path] = resolved;
            }

            for(const auto& mus : music_filenames)
            {
                if(mus.second.endsWith(".qoa"))
                {
                    music_ini.beginGroup(mus.first);
                    music_ini.setValue("file-qoa", mus.second.section(QDir::separator(), -1, -1));
                    music_ini.endGroup();
                    continue;
                }

                auto found = resolved_modules.find(mus.second);

                if(found == resolved_modules.end())
                    continue;

                int mod_index = found->second;

                music_ini.beginGroup(mus.first);
                music_ini.setValue("resolved-mod", mod_index);
                music_ini.endGroup();
            }

            for(const auto& snd : sound_filenames)
            {
                auto found = resolved_samples.find(snd.second);

                if(found == resolved_samples.end())
                    continue;

                int sample_index = found->second;

                sound_ini.beginGroup(snd.first);
                sound_ini.setValue("resolved-sfx", sample_index);
                sound_ini.endGroup();
            }
        }

        // save inis
        music_ini.sync();
        sound_ini.sync();

        // clean up inputs to maxmod
        for(const auto& mus : music_filenames)
        {
            if(mus.second.endsWith(".qoa") || !QFileInfo(mus.second).isFile())
                continue;

            QFile::remove(mus.second);
        }

        for(const auto& snd : sound_filenames)
        {
            if(!QFileInfo(snd.second).isFile())
                continue;

            QFile::remove(snd.second);
        }

        QFile::remove(soundbank_ini_path);

        return mmutil_success;
    }

    static bool find_assets_root(QDir& assets_dir)
    {
        QDirIterator it(assets_dir.path(), QStringList() << "gameinfo.ini", QDir::NoFilter, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        bool found_root = false;

        while(it.hasNext())
        {
            it.next();
            if(!it.fileInfo().isFile())
                continue;

            if(found_root)
            {
                qInfo() << "Multiple gameinfo.ini entries";
                return false;
            }

            assets_dir = it.fileInfo().dir();
            found_root = true;
        }

        if(!found_root)
        {
            qInfo() << "Could not find gameinfo.ini";
            return false;
        }

        return true;
    }

    static bool find_episode_root(QDir& episode_dir)
    {
        QDirIterator it(episode_dir.path(), QStringList{"*.wld", "*.wldx"}, QDir::NoFilter, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        bool found_root = false;

        while(it.hasNext())
        {
            it.next();
            if(!it.fileInfo().isFile())
                continue;

            if(found_root)
            {
                qInfo() << "Multiple WLD/WLDX files";
                return false;
            }

            episode_dir = it.fileInfo().dir();
            found_root = true;
        }

        if(!found_root)
        {
            qInfo() << "Could not find WLD/WLDX file";
            return false;
        }

        return true;
    }

    bool create_archive(const QString& dest_path, bool type_is_iso)
    {
        struct archive* package = nullptr;
        struct archive_entry* entry = nullptr;
        bool success = false;

        package = archive_write_new();
        if(!package)
            goto cleanup;

        if(type_is_iso)
        {
            archive_write_set_format_iso9660(package);
            archive_write_set_format_option(package, "iso9660", "rockridge", nullptr);
            archive_write_set_format_option(package, "iso9660", "pad", nullptr);
            archive_write_set_format_option(package, "iso9660", "iso-level", "1");
            archive_write_set_format_option(package, "iso9660", "limit-depth", nullptr);
            archive_write_set_format_option(package, "iso9660", "joliet", "long");
        }
        else
        {
            archive_write_set_format_7zip(package);
        }

        if(archive_write_open_filename(package, dest_path.toUtf8().data()) != ARCHIVE_OK)
            goto cleanup;

        {
            m_temp_dir.setPath(m_temp_dir_owner.path());
            m_temp_dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

            // check count of files
            size_t file_count = 0;
            QDirIterator count_it(m_temp_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
            while(count_it.hasNext())
            {
                file_count++;
                count_it.next();
            }

            // iterate over all directories
            size_t files_done = 0;
            QDirIterator it(m_temp_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

            while(it.hasNext())
            {
                files_done++;

                QString abs_path = it.next();
                QString rel_path = m_temp_dir.relativeFilePath(abs_path);

                if(progress(STAGE_PACK, files_done, file_count, abs_path) < 0)
                {
                    m_error = "Conversion canceled by user";
                    goto cleanup;
                }

                QFile file(abs_path);
                if(!file.open(QIODevice::ReadOnly))
                    goto cleanup;

                QByteArray got = file.readAll();

                entry = archive_entry_new();
                archive_entry_set_pathname(entry, rel_path.toUtf8().data());
                archive_entry_set_size(entry, got.size());
                archive_entry_set_filetype(entry, AE_IFREG);
                archive_entry_set_mtime(entry, file.fileTime(QFileDevice::FileModificationTime).toSecsSinceEpoch(), 0);

                int r = archive_write_header(package, entry);
                if(r != ARCHIVE_OK)
                {
                    // think about whether to do anything here
                }

                if(r == ARCHIVE_FATAL)
                    goto cleanup;

                if(r != ARCHIVE_FAILED)
                    archive_write_data(package, got.data(), got.size());

                archive_entry_free(entry);
                entry = nullptr;
            }
        }

        success = true;

cleanup:
        if(entry)
            archive_entry_free(entry);

        if(package)
        {
            archive_write_close(package);
            archive_write_free(package);
        }

        return success;
    }

    bool create_package_iso_lz4()
    {
        if(m_spec.destination.empty())
            return false;

        QTemporaryFile temp_iso;
        if(!temp_iso.open())
            return false;

        QString iso_path = temp_iso.fileName();
        temp_iso.close();

        if(iso_path.isEmpty())
            return false;

        bool success = create_archive(iso_path, true);

        if(success)
        {
            if(progress(STAGE_COMPRESS, 0, 1, "") < 0)
            {
                m_error = "Conversion canceled by user";
                return false;
            }

            FILE* inf = fopen(iso_path.toUtf8().data(), "rb");
            FILE* outf = fopen(m_spec.destination.c_str(), "wb");

            success = LZ4Pack::compress(outf, inf, 4096, m_spec.target_platform == TargetPlatform::TPL);

            if(inf)
                fclose(inf);

            if(outf)
                fclose(outf);

            return success;
        }

        return false;
    }

    bool create_package_romfs3ds()
    {
        if(m_spec.destination.size() == 0)
            return false;

        if(LibRomFS3DS::MkRomfs(m_spec.destination.c_str(), m_temp_dir_owner.path().toUtf8().data()) != 0)
            return false;

        return true;
    }

    bool create_package()
    {
        if(progress(STAGE_PACK, 0, 1, "") < 0)
        {
            m_error = "Conversion canceled by user";
            return false;
        }

        if(m_spec.legacy_archives)
        {
            if(m_spec.target_platform == TargetPlatform::T3X)
                return create_package_romfs3ds();
            else
                return create_archive(QString::fromStdString(m_spec.destination), false);
        }
        else
            return create_package_iso_lz4();
    }

    bool process()
    {
        GraphicsLoad::FreeImage_Sentinel fs;
        MixerX_Sentinel ms(m_spec);

        init_mask_arrays();

        if(!ms.valid)
        {
            qInfo() << "Error: MixerX could not be (re)initialized";
            return false;
        }

        if(m_spec.target_platform == TargetPlatform::DSG && m_spec.convert_gifs != ConvertGIFs::All)
        {
            qInfo() << "Note: converting all GIFs to DSG";
            m_spec.convert_gifs = ConvertGIFs::All;
        }

        {
            QString input_path = QString::fromStdString(m_spec.input_path);
            QFileInfo fi(input_path);

            if(fi.isDir())
                m_input_dir.setPath(input_path);
            else
            {
                if(!m_temp_input_dir_owner.isValid())
                {
                    m_error = "Failed to create temporary directory for input archive";
                    return false;
                }

                m_input_dir.setPath(m_temp_input_dir_owner.path());

                if(!fi.isFile())
                {
                    m_error = "Cannot find input file/directory " + input_path;
                    return false;
                }

                if(progress(STAGE_EXTRACT, 0, 1, "") < 0)
                {
                    m_error = "Conversion canceled by user";
                    return false;
                }

                if(!extract_archive_file(m_input_dir.path().toUtf8().data(), input_path.toUtf8().data()))
                {
                    m_error = "Could not extract input file";
                    return false;
                }
            }

            if(m_spec.package_type == PackageType::AssetPack && !find_assets_root(m_input_dir))
            {
                m_error = "Invalid input provided (must contain exactly one gameinfo.ini file)";
                return false;
            }

            if(m_spec.package_type == PackageType::Episode && !find_episode_root(m_input_dir))
            {
                m_error = "Invalid input provided (must contain exactly one .wld/.wldx file)";
                return false;
            }
        }

        if(m_spec.package_type == PackageType::Episode)
        {
            QString base_assets_path = QString::fromStdString(m_spec.base_assets_path);
            QFileInfo fi(base_assets_path);

            if(fi.isDir())
                m_assets_dir.setPath(base_assets_path);
            else
            {
                if(!m_temp_assets_dir_owner.isValid())
                {
                    m_error = "Failed to create temporary directory for base assets";
                    return false;
                }

                m_assets_dir.setPath(m_temp_assets_dir_owner.path());

                if(fi.isFile())
                {
                    if(!extract_archive_file(m_assets_dir.path().toUtf8().data(), base_assets_path.toUtf8().data()))
                    {
                        m_error = "Could not extract base assets";
                        return false;
                    }
                }
                else if(base_assets_path.isEmpty())
                {
                    QByteArray default_masks = s_get_default_masks();
                    if(default_masks.size() == 0)
                    {
                        m_error = "Failed to load embedded default masks";
                        return false;
                    }

                    if(!extract_archive_data(m_assets_dir.path().toUtf8().data(), (const uint8_t*)default_masks.data(), default_masks.size()))
                    {
                        m_error = "Could not extract embedded default masks";
                        return false;
                    }
                }
                else
                {
                    m_error = "Cannot find base assets file/directory " + base_assets_path;
                    return false;
                }
            }

            if(!find_assets_root(m_assets_dir))
            {
                m_error = "Invalid base assets provided (must contain exactly one gameinfo.ini file)";
                return false;
            }
        }
        else
            m_assets_dir.setPath(m_input_dir.path());

        m_episode_info = EpisodeInfo();

        if(!build_temp_dir())
            return false;

        if(!create_package())
            return false;

        return true;
    }
};

bool Convert(const Spec& spec)
{
    Converter c;
    c.m_spec = spec;

    if(!c.m_spec.log_file_callback)
        c.m_spec.log_file_callback = log_file_callback_default;

    if(!c.m_spec.progress_callback)
        c.m_spec.progress_callback = progress_callback_default;

    if(!c.process())
    {
        c.m_spec.log_file_callback(c.m_spec.callback_userdata, LogCategory::ErrorMessage, c.m_error.toStdString());
        return false;
    }

    return true;
}

} // namespace XTConvert
