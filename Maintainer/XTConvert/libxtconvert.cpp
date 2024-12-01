#include <map>
#include <array>

#include <QDebug>

#include <QString>
#include <QDir>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QDirIterator>
#include <QSettings>
#include <QProcess>

#include <fcntl.h>

#include <FreeImageLite.h>

#include <SDL2/SDL_mixer_ext.h>
#include <PGE_File_Formats/file_formats.h>

#include "archive.h"
#include "archive_entry.h"

#include "lz4_pack.h"
#include "libromfs3ds.h"
#include "libtex3ds.h"
#include "spc2it.h"

#include "graphics_load.h"
#include "texconv.h"
#include "export_tpl.h"
#include "export_audio.h"

#include "libxtconvert.h"

namespace XTConvert
{

static bool validate_image_filename(const QString& filename, QString& type, QString& index)
{
    QString basename = filename.section(".", 0, 0);
    type = basename.section("-", 0, 0);
    index = basename.section("-", 1);

    bool index_valid = false;
    index.toUInt(&index_valid);

    if(!index_valid)
        return false;

    static const std::array<const char*, 17> valid_types = {
        "background", "background2", "block", "effect", "level",
        "link", "luigi", "mario", "npc", "path",
        "peach", "player", "scene", "tile", "toad",
        "yoshib", "yoshit"
    };

    for(const char* valid_type : valid_types)
    {
        if(type == valid_type)
            return true;
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

        if(spec.target_platform == TargetPlatform::TPL || spec.target_platform == TargetPlatform::DSG)
            as.freq = 32000;
        // else if(spec.target_platform == TargetPlatform::DSG)
        //     as.freq = 24000;

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
        else if(m_spec.package_type == PackageType::AssetPack && rel_path.startsWith("graphics/"))
        {
            m_cur_dir.make_size_files = DirInfo::SIZEFILES_PREVIEW_ASSETS;
            m_cur_dir.make_fallback_masks = (m_spec.target_platform != TargetPlatform::TPL);
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

    bool save_mask(FIBITMAP* mask, const QString& mask_path)
    {
        // convert to 8-bit paletted mask
        FIBITMAP* mask_2 = FreeImage_ColorQuantizeEx(mask, FIQ_LFPQUANT, 2);
        if(mask_2)
        {
            // the above call actually makes an 8-bit image, and we want a 1-bit image if possible
            FreeImage_Unload(mask_2);

            // use black and white
            mask_2 = FreeImage_Threshold(mask, 127);
        }
        else
            mask_2 = FreeImage_ConvertTo8Bits(mask);

        if(mask_2)
        {
            FreeImage_Save(FIF_GIF, mask_2, mask_path.toUtf8());
            return true;
        }

        return false;
    }

    bool convert_image(const QString& filename, const QString& in_path, const QString& out_path)
    {
        FIBITMAP* image = GraphicsLoad::loadImage(in_path);
        if(!image)
        {
            qInfo() << "Warning: invalid graphics file at [" << in_path << "], skipping...";
            return true;
        }

        QString filename_type, filename_index;
        bool standard_filename_format = validate_image_filename(filename, filename_type, filename_index);

        FIBITMAP* mask = nullptr;
        QString mask_path;
        if(filename.endsWith(".gif") && filename_type != "background2" && filename_type != "tile")
        {
            // find the mask!!
            QString filename_stem = filename.chopped(4);
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

                if(i == 0)
                    dir_to_check = m_cur_dir.dir.path() + s + ".." + s;
                else if(i == 1)
                    dir_to_check = m_assets_dir.path() + s + "graphics" + s + "fallback" + s;
                else
                    dir_to_check = m_assets_dir.path() + s + "graphics" + s + filename.split('-')[0] + s;

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

            // Okay, finding a mask failed. Imagine that image is fully opaque in that case, and ignore the mask logic.
            if(!mask)
                mask_path.clear();
            // qInfo() << "Failed to find mask for GIF " << in_path;
        }

        TargetPlatform output_format = m_spec.target_platform;

        // find out whether mask is actually needed
        if(m_spec.convert_gifs != ConvertGIFs::All && GraphicsLoad::validateBitmaskRequired(image, mask))
            output_format = TargetPlatform::Desktop;
        else if(mask)
        {
            // merge with mask and free
            GraphicsLoad::mergeWithMask(image, mask);
            FreeImage_Unload(mask);
            mask = nullptr;
        }

        QString used_out_path = out_path;

        // save mask fallback here if we are in the global graphics dir
        if(m_cur_dir.make_fallback_masks && output_format != TargetPlatform::Desktop)
        {
            FIBITMAP* fallback_mask = nullptr;

            // make sure that the texture is valid and should have transparency
            if(standard_filename_format && filename_type != "background2" && filename_type != "tile")
                fallback_mask = FreeImage_Clone(image);

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
                    save_mask(fallback_mask, fallback_mask_path);

                FreeImage_Unload(fallback_mask);
            }
        }


        // scale the image as needed

        // save these if it becomes necessary
        // int orig_w = FreeImage_GetWidth(image);
        // int orig_h = FreeImage_GetHeight(image);

        // 2x downscale by default
        if(output_format != TargetPlatform::Desktop && !m_cur_dir.textures_1x.contains(filename))
        {
            FIBITMAP* scaled = GraphicsLoad::fast2xScaleDown(image);
            FreeImage_Unload(image);
            if(!scaled)
            {
                qInfo() << "Scaling failed at [" << in_path << "], aborting...";
                return false;
            }

            image = scaled;
        }

        int image_h = FreeImage_GetHeight(image);
        int image_w = FreeImage_GetWidth(image);

        // save the image!
        bool save_success = false;
        QString size_text;
        if(output_format == TargetPlatform::Desktop)
        {
            qInfo() << "mask required for" << in_path;
            save_success = QFile::copy(in_path, out_path);

            if(save_success && mask)
            {
                QString mask_out_path = out_path.chopped(4) + "m.gif";

                if(!mask_path.isEmpty())
                    save_success = QFile::copy(mask_path, mask_out_path);
                else
                    save_success = save_mask(mask, mask_out_path);

                FreeImage_Unload(mask);
            }

            // set the size text here...!
            size_text = QString("%1\n%2\n").arg(image_w, 4).arg(image_h, 4);
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
                qInfo() << "Using RGBA8888 for " << in_path;
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
            }

            if(save_success)
                size_text = QString("%1\n%2\n").arg(image_w * 2, 4).arg(image_h * 2, 4);
        }
        else if(output_format == TargetPlatform::DSG)
        {
            used_out_path = out_path.chopped(4) + ".dsg";

            int flags = 0;

            // FIXME: move this out of the DSG-specific code
            if(filename.startsWith("mario-") || filename.startsWith("luigi-") || filename.startsWith("peach-") || filename.startsWith("toad-") || filename.startsWith("link-"))
            {
                if(shrink_player_texture(&image, filename.startsWith("link-")))
                {
                    image_w = FreeImage_GetWidth(image);
                    image_h = FreeImage_GetHeight(image);
                }
            }

            // downscale image further, and threshold alpha, here.
            int container_w = next_power_of_2(image_w);
            int container_h = next_power_of_2(image_h);

            while((container_w * container_h) / 2 > 131072)
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
                    qInfo() << "Scaling failed at [" << in_path << "], aborting...";
                    return false;
                }

                image = scaled;
            }

            // palettize texture
            int input_w = FreeImage_GetWidth(image);
            int input_h = FreeImage_GetHeight(image);
            PaletteTex p(FreeImage_GetBits(image), input_w, input_h);
            if(!p.convert(PaletteTex::HALF, 0))
            {
                FreeImage_Unload(image);
                qInfo() << "Palettizing failed at [" << in_path << "], aborting...";
                return false;
            }

            // save the image here
            QByteArray dsg_data;
            dsg_data.resize(32 + container_w * container_h / 2);

            // save palette
            for(int i = 0; i < 16; ++i)
            {
                liq_color c = p.palette()[i];
                uint8_t* dest = reinterpret_cast<unsigned char*>(&dsg_data.data()[i * 2]);

                write_uint16_le(dest, color_to_rgb5a1(c));
            }

            bool fully_opaque = (p.palette()[0].a == 255);
            if(fully_opaque)
                flags |= 16;

            // save image indexes
            const uint8_t* src = p.indexes();
            uint8_t* out_base = reinterpret_cast<unsigned char*>(&dsg_data.data()[32]);
            int out_stride = container_w / 2;
            for(int row = 0; row < input_h; row++)
            {
                uint8_t* dest = out_base + (input_h - 1 - row) * out_stride;

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
                // special case: yoshib and yoshit stored in yoshi
                if(filename_type.startsWith('y'))
                {
                    if(!m_cur_dir.dir.path().endsWith("yoshi"))
                        valid = false;
                }
                else if(!m_cur_dir.dir.path().endsWith(filename_type))
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

        if(output_format == TargetPlatform::Desktop)
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
            qInfo() << "Saving failed at [" << in_path << "], aborting...";

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
        qInfo() << "ogg2wav" << out_path;

        Mix_Chunk* ch = Mix_LoadWAV(in_path.toUtf8().data());
        if(!ch)
            return false;

        qInfo() << "length" << ch->alen;

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
        qInfo() << "mus2qoa" << out_path;

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
            qInfo() << "Episode file must be in top-level directory, not subdirectory:", in_path;
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
    {}

    bool convert_file(const QString& filename, const QString& in_path, const QString& out_path)
    {
        sync_cur_dir(in_path);

        if(filename.endsWith("m.gif") && in_path.contains("/graphics/fallback/"))
        {
            // want to clobber auto-generated masks
            if(QFile::exists(out_path))
                QFile::remove(out_path);

            return QFile::copy(in_path, out_path);
        }
        else if(filename.endsWith("m.gif"))
            return true;
        else if(filename.endsWith(".xcf") || filename.endsWith(".odt") || filename.endsWith(".pdf"))
            return true;
        else if(m_spec.target_platform != TargetPlatform::Desktop && (filename.endsWith(".png") || filename.endsWith(".gif")) && !in_path.contains("/graphics/fallback/"))
            return convert_image(filename, in_path, out_path);
        else if(filename.endsWith(".ini") && m_cur_dir.convert_font_inis)
            return convert_font_ini(filename, in_path, out_path);
        else if(m_spec.target_platform != TargetPlatform::Desktop && filename.endsWith(".ogg") && in_path.contains("/sound/"))
            return convert_sfx(filename, in_path, out_path);
        else if(m_spec.target_platform == TargetPlatform::DSG && (filename.endsWith(".mp3") || filename.endsWith(".ogg")))
            return convert_music_16m(filename, in_path, out_path);
        else if(filename.endsWith(".spc") && m_spec.target_platform == TargetPlatform::T3X)
        {
            qInfo() << "spc2it" << out_path;
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
            qInfo() << "copying" << out_path;
            return QFile::copy(in_path, out_path);
        }
        else
        {
            qInfo() << "copying" << out_path;
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

        m_temp_dir.setPath(m_temp_dir_owner.path());

        m_input_dir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

        QDirIterator it(m_input_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while(it.hasNext())
        {
            QString abs_path = it.next();
            QString rel_path = m_input_dir.relativeFilePath(abs_path);
            QString temp_path = m_temp_dir.filePath(rel_path);

            QString filename = it.fileName().toLower();

            if(it.fileInfo().isDir())
            {
                qInfo() << "making" << temp_path;
                if(!m_temp_dir.exists(temp_path) && !m_temp_dir.mkdir(temp_path))
                {
                    m_error = "Failed to create directory ";
                    m_error += temp_path;
                    return false;
                }

                if(rel_path == "graphics")
                {
                    qInfo() << "Initializing primary graphics.list file";

                    m_main_graphics_list.setFileName(temp_path + QDir::separator() + "graphics.list");
                    m_main_graphics_list.open(QIODevice::WriteOnly);
                }

                continue;
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
            m_cur_dir.graphics_list->close();

        // do post-processing steps
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

        if(!pack_id.isEmpty())
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
        return true;
    }

    bool create_package_iso_lz4()
    {
        if(m_spec.destination.size() == 0)
            return false;

        QTemporaryFile temp_iso;
        if(!temp_iso.open())
            return false;

        QString iso_path = temp_iso.fileName();
        temp_iso.close();

        if(iso_path.isEmpty())
            return false;

        bool success = false;
        int r;

        struct archive* package = nullptr;
        struct archive_entry* entry = nullptr;

        package = archive_write_new();
        if(!package)
            goto cleanup;

        archive_write_set_format_iso9660(package);
        archive_write_set_format_option(package, "iso9660", "rockridge", nullptr);
        archive_write_set_format_option(package, "iso9660", "pad", nullptr);
        archive_write_set_format_option(package, "iso9660", "iso-level", "1");
        archive_write_set_format_option(package, "iso9660", "limit-depth", nullptr);
        archive_write_open_filename(package, iso_path.toUtf8().data());

        {
            m_temp_dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

            QDirIterator it(m_temp_dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

            while(it.hasNext())
            {
                QString abs_path = it.next();
                QString rel_path = m_temp_dir.relativeFilePath(abs_path);

                QFile file(abs_path);
                if(!file.open(QIODevice::ReadOnly))
                    goto cleanup;

                QByteArray got = file.readAll();

                entry = archive_entry_new();
                archive_entry_set_pathname(entry, rel_path.toUtf8().data());
                archive_entry_set_size(entry, got.size());
                archive_entry_set_filetype(entry, AE_IFREG);

                r = archive_write_header(package, entry);
                if(r < ARCHIVE_OK)
                {
                    // think about whether to do anything here
                }

                if(r == ARCHIVE_FATAL)
                    goto cleanup;

                if(r > ARCHIVE_FAILED)
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

        if(success)
        {
            FILE* inf = fopen(iso_path.toUtf8().data(), "rb");
            FILE* outf = fopen(m_spec.destination.toUtf8().data(), "wb");

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

        if(LibRomFS3DS::MkRomfs(m_spec.destination.toUtf8().data(), m_temp_dir.path().toUtf8().data()) != 0)
            return false;

        return true;
    }

    bool create_package()
    {
        if(m_spec.target_platform == TargetPlatform::T3X)
            return create_package_romfs3ds();
        else
            return create_package_iso_lz4();
    }

    bool process()
    {
        GraphicsLoad::FreeImage_Sentinel fs;
        MixerX_Sentinel ms(m_spec);

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

        m_input_dir.setPath(m_spec.input_dir);

        if(m_spec.package_type == PackageType::Episode)
            m_assets_dir.setPath(m_spec.use_assets_dir);
        else
            m_assets_dir.setPath(m_spec.input_dir);

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
    return c.process();
}

} // namespace XTConvert
