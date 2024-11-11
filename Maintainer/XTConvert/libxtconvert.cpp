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

#include "archive.h"
#include "archive_entry.h"

#include "lz4_pack.h"
#include "libromfs3ds.h"
#include "libtex3ds.h"
#include "spc2it.h"

#include "graphics_load.h"
#include "texconv.h"
#include "export_tpl.h"
#include "export_wav.h"

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

        if(spec.target_platform == TargetPlatform::TPL)
            as.freq = 32000;
        else if(spec.target_platform == TargetPlatform::DSG)
            as.freq = 24000;

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

    QDir m_input_dir;
    QDir m_assets_dir;

    QTemporaryDir m_temp_dir_owner;
    QDir m_temp_dir;

    DirInfo m_cur_dir;
    QFile m_main_graphics_list;

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
        else if(m_spec.use_assets_dir.isEmpty() && rel_path.startsWith("graphics/"))
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

    bool convert_image(const QString& filename, const QString& in_path, const QString& out_path)
    {
        FIBITMAP* image = GraphicsLoad::loadImage(in_path);
        if(!image)
            return false;

        FIBITMAP* mask = nullptr;
        QString mask_path;
        if(filename.endsWith(".gif"))
        {
            // find the mask!!
            QString filename_stem = filename.chopped(4);
            auto found = m_cur_dir.dir.entryInfoList({filename_stem + "m.gif"});
            if(!found.isEmpty())
            {
                mask_path = found[0].absoluteFilePath();
                mask = GraphicsLoad::loadImage(mask_path);
            }

            if(!mask)
                mask_path.clear();

            // keep trying to find mask, in fallback dir and in appropriate graphics dir
            for(int i = 0; i < 2; i++)
            {
                if(mask)
                    break;

                QString dir_piece = (i == 0) ? "fallback" : filename.split('-')[0];
                auto s = QDir::separator();
                QString dir_to_check = m_assets_dir.path() + s + "graphics" + s + dir_piece + s;

                // look for mask GIF
                mask = GraphicsLoad::loadImage(dir_to_check + filename_stem + "m.gif");

                if(mask)
                    break;

                // look for PNG
                mask = GraphicsLoad::loadImage(dir_to_check + filename_stem + ".png");

                // convert PNG to mask
                if(mask)
                    GraphicsLoad::RGBAToMask(mask);
            }

            // Okay, finding a mask failed. Imagine that image is fully opaque in that case, and ignore the mask logic.
        }

        TargetPlatform output_format = m_spec.target_platform;

        // find out whether mask is actually needed
        if(m_spec.preserve_bitmask_appearance && GraphicsLoad::validateBitmaskRequired(image, mask))
            output_format = TargetPlatform::Desktop;
        else if(mask)
            GraphicsLoad::mergeWithMask(image, mask);

        // either way we can free the mask bitmap now
        if(mask)
        {
            FreeImage_Unload(mask);
            mask = nullptr;
        }

        QString used_out_path = out_path;

        // save mask fallback here if we are in the global graphics dir
        if(m_cur_dir.make_fallback_masks && output_format != TargetPlatform::Desktop)
        {
            QString type, index;

            FIBITMAP* fallback_mask = nullptr;


            // make sure that the texture is valid and should have transparency
            if(validate_image_filename(filename, type, index) && type != "background2" && type != "tile")
                fallback_mask = FreeImage_Clone(image);

            // convert to 8-bit paletted mask
            if(fallback_mask)
            {
                GraphicsLoad::RGBAToMask(fallback_mask);

                // should not fail (there are only 256 shades of gray)
                FIBITMAP* fallback_mask_2 = FreeImage_ColorQuantizeEx(fallback_mask, FIQ_LFPQUANT, 2);
                if(fallback_mask_2)
                {
                    FreeImage_Unload(fallback_mask_2);
                    fallback_mask_2 = FreeImage_Threshold(fallback_mask, 127);
                }
                else
                    fallback_mask_2 = FreeImage_ConvertTo8Bits(fallback_mask);

                FreeImage_Unload(fallback_mask);

                fallback_mask = fallback_mask_2;
            }

            if(fallback_mask)
            {
                QString fallback_mask_path = m_temp_dir.filePath("graphics/fallback/");

                // make fallback folder if it isn't present
                if(!QFileInfo::exists(fallback_mask_path))
                    m_temp_dir.mkdir(fallback_mask_path);

                // append filename
                fallback_mask_path += filename.chopped(4) + "m.gif";

                // don't clobber a version copied from the original fallback folder
                if(!QFileInfo::exists(fallback_mask_path))
                    FreeImage_Save(FIF_GIF, fallback_mask, fallback_mask_path.toUtf8());

                FreeImage_Unload(fallback_mask);
            }
        }


        // scale the image as needed

        // save these if it becomes necessary
        // int orig_w = FreeImage_GetWidth(image);
        // int orig_h = FreeImage_GetHeight(image);

        // 2x downscale by default
        if(!m_cur_dir.textures_1x.contains(filename))
        {
            FIBITMAP* scaled = GraphicsLoad::fast2xScaleDown(image);
            FreeImage_Unload(image);
            if(!scaled)
                return false;

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

            if(save_success && !mask_path.isEmpty())
                save_success = QFile::copy(mask_path, out_path.chopped(4) + "m.gif");
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

                // FIXME: check if possible to do 5551 here, or possibly just do 5551 by force (maybe unless there's intermediate opacity?)

                params.output_path = used_out_path.toUtf8();
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
            int input_w = FreeImage_GetWidth(image);
            int input_h_full = FreeImage_GetHeight(image);
            int stride = FreeImage_GetPitch(image);

            save_success = false;

            for(int i = 0; i < 3; i++)
            {
                int start_y = i * 1024;
                int input_h = input_h_full - start_y;
                if(input_h <= 0)
                    break;
                if(input_h > 1024)
                    input_h = 1024;

                PaletteTex p(FreeImage_GetBits(image) + stride * start_y, input_w, input_h);

                PaletteTex::PaletteSize used_size = PaletteTex::HALF;
                if(!p.convert(used_size, 90))
                {
                    used_size = PaletteTex::BYTE;

                    if(!p.convert(used_size, 0))
                        break;
                }

                QByteArray tpl_data;
                tpl_data.resize(ExportTPL::data_size(used_size, input_w, input_h));
                ExportTPL::fill_data(reinterpret_cast<uint8_t*>(&tpl_data[0]), used_size, input_w, input_h, p.palette(), p.indexes());

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
                    return false;

                image = scaled;
            }

            // palettize texture
            int input_w = FreeImage_GetWidth(image);
            int input_h = FreeImage_GetHeight(image);
            PaletteTex p(FreeImage_GetBits(image), input_w, input_h);
            if(!p.convert(PaletteTex::HALF, 0))
            {
                FreeImage_Unload(image);
                return false;
            }

            // save the image here
            QByteArray dsg_data;
            dsg_data.resize(32 + container_w * container_h / 2);

            // save palette
            for(int i = 0; i < 16; ++i)
            {
                liq_color c = p.palette()[i];
                uint8_t* dest = reinterpret_cast<unsigned char*>(&dsg_data[i * 2]);

                write_uint16_le(dest, color_to_rgb5a1(c));
            }

            bool fully_opaque = (p.palette()[0].a == 255);
            if(fully_opaque)
                flags |= 16;

            // save image indexes
            const uint8_t* src = p.indexes();
            uint8_t* out_base = reinterpret_cast<unsigned char*>(&dsg_data[32]);
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

        if(m_cur_dir.graphics_list.get())
            write_graphics_list = m_cur_dir.graphics_list.get();
        else if(m_main_graphics_list.isOpen())
            write_graphics_list = &m_main_graphics_list;

        // write graphics list
        if(write_graphics_list)
        {
            QString type, index;

            bool valid = validate_image_filename(filename, type, index);

            // only allow valid subdirectories for main graphics list
            if(valid && write_graphics_list == &m_main_graphics_list)
            {
                // special case: yoshib and yoshit stored in yoshi
                if(type.startsWith('y'))
                {
                    if(!m_cur_dir.dir.path().endsWith("yoshi"))
                        valid = false;
                }
                else if(!m_cur_dir.dir.path().endsWith(type))
                    valid = false;
            }

            if(valid)
            {
                // make path relative to graphics list
                QFileInfo fi(write_graphics_list->fileName());
                QDir graphics_list_parent = fi.dir();
                QString relative_out_path = graphics_list_parent.relativeFilePath(used_out_path);

                write_graphics_list->write(type.toUtf8());
                write_graphics_list->write(" ");
                write_graphics_list->write(index.toUtf8());
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
            make_size_file = in_path.contains("/ui/")
                || filename.contains("/background2/");
        }
        else if(m_cur_dir.make_size_files == DirInfo::SIZEFILES_PREVIEW_EPISODE)
        {
            // player and mount sprites
            make_size_file = filename.contains("mario")
                || filename.contains("luigi")
                || filename.contains("peach")
                || filename.contains("toad")
                || filename.contains("link")
                || filename.contains("yoshi")
                || filename.contains("mount");
        }

        // write to size file
        if(make_size_file && save_success && !size_text.isEmpty())
        {
            QFile file(used_out_path + ".size");
            if(!file.open(QIODevice::WriteOnly))
                save_success = false;

            file.write(size_text.toUtf8());
            file.close();
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
        qInfo() << "ogg2wav" << out_path;

        Mix_Chunk* ch = Mix_LoadWAV(in_path.toUtf8().data());
        if(!ch)
            return false;

        qInfo() << "length" << ch->alen;

        QByteArray wav_data;

        ExportWAV::export_wav(wav_data, ch);

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
        else if(filename.endsWith(".ogg") && in_path.contains("/sound/"))
            return convert_sfx(filename, in_path, out_path);
        else if(filename.endsWith(".spc") && m_spec.target_platform == TargetPlatform::T3X)
        {
            qInfo() << "spc2it" << out_path;
            return spc2it_convert(in_path.toUtf8().data(), out_path.toUtf8().data()) == 0;
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

        return true;
    }

    bool build_graphics_lists()
    {
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

        QTemporaryFile temp_iso = QTemporaryFile();
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

        m_input_dir.setPath(m_spec.input_dir);

        if(!m_spec.use_assets_dir.isEmpty())
            m_assets_dir.setPath(m_spec.use_assets_dir);
        else
            m_assets_dir.setPath(m_spec.input_dir);

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
