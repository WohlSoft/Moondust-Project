#include <QDebug>

#include <QString>
#include <QDir>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QDirIterator>
#include <QProcess>

#include <fcntl.h>

#include <FreeImageLite.h>

#include "archive.h"
#include "archive_entry.h"

#include "lz4_pack.h"
#include "libromfs3ds.h"
#include "libtex3ds.h"

#include "graphics_load.h"

#include "libxtconvert.h"

namespace XTConvert
{

class Converter
{
    struct DirInfo
    {
        QDir dir;

        QSet<QString> textures_1x;

        std::unique_ptr<QFile> own_graphics_list; // open file to the directory's own graphics list (if any)
        QFile* graphics_list = nullptr; // pointer to the correct graphics list to write to (if any)

        enum MakeSizeFiles_t
        {
            SIZEFILES_NONE = 0,
            SIZEFILES_PREVIEW_EPISODE,
            SIZEFILES_PREVIEW_ASSETS,
            SIZEFILES_ALL,
        };

        MakeSizeFiles_t make_size_files = SIZEFILES_NONE;
        bool make_mask_gifs = false;
        bool copy_mask_gifs = false;
    };

    DirInfo m_cur_dir;

    void sync_cur_dir(const QString& in_file)
    {
        QFileInfo fi(in_file);
        QDir parent = fi.dir();

        if(parent.path() == m_cur_dir.dir.path())
            return;

        m_cur_dir = DirInfo();
        m_cur_dir.dir = std::move(parent);

        QStringList path_parts = QDir::toNativeSeparators(m_cur_dir.dir.path()).toLower().split(QDir::separator());

        // TODO: check for 1x textures

        m_cur_dir.make_size_files = DirInfo::SIZEFILES_ALL;
    }

public:
    QDir m_input_dir;
    QDir m_assets_dir;

    QTemporaryDir m_temp_dir_owner;
    QDir m_temp_dir;

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
                    GraphicsLoad::PNGToMask(mask);
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

        // scale the image as needed
        int orig_w = FreeImage_GetWidth(image);
        int orig_h = FreeImage_GetHeight(image);

        // 2x downscale by default
        if(!m_cur_dir.textures_1x.contains(filename))
        {
            FIBITMAP* scaled = GraphicsLoad::fast2xScaleDown(image);
            FreeImage_Unload(image);
            if(!scaled)
                return false;

            image = scaled;
        }

        // save the image!
        bool save_success = false;
        if(output_format == TargetPlatform::Desktop)
        {
            qInfo() << "mask required for" << in_path;
            save_success = QFile::copy(in_path, out_path);

            if(save_success && !mask_path.isEmpty())
                save_success = QFile::copy(mask_path, out_path.chopped(4) + "m.gif");
        }
        else if(output_format == TargetPlatform::T3X)
        {
            used_out_path = out_path.chopped(4) + ".t3x";

            FreeImage_FlipVertical(image);

            Tex3DS::Params params;
            params.compression_format = Tex3DS::COMPRESSION_LZ11;

            // image parts
            save_success = true;
            for(int i = 0; i < 3; i++)
            {
                int start_y = i * 1024;
                int h = FreeImage_GetHeight(image) - start_y;
                if(h <= 0)
                    break;
                if(h > 1024)
                    h = 1024;

                int w = FreeImage_GetWidth(image);
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

                params.output_path = used_out_path.toUtf8();
                if(i > 0)
                    params.output_path.push_back('0' + i);

                save_success &= Tex3DS::Process(params);
            }
        }

        // cleanup the image
        FreeImage_Unload(image);

        // finish up by constructing the size information and writing it as needed
        QString size_text = QString("%1\n%2\n").arg(orig_w, 4).arg(orig_h, 4);

        // write to graphics list NOW, write to actual size file after deciding output format
        if(m_cur_dir.make_size_files && save_success && output_format != TargetPlatform::Desktop)
        {
            QFile file(used_out_path + ".size");
            if(!file.open(QIODevice::WriteOnly))
                save_success = false;

            file.write(size_text.toUtf8());
            file.close();
        }

        return save_success;
    }

    bool convert_file(const QString& filename, const QString& in_path, const QString& out_path)
    {
        sync_cur_dir(in_path);

        if(filename.endsWith("m.gif"))
            return true;
        else if(m_spec.target_platform != TargetPlatform::Desktop && (filename.endsWith(".png") || filename.endsWith(".gif")))
            return convert_image(filename, in_path, out_path);
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
                if(!m_temp_dir.mkdir(temp_path))
                {
                    m_error = "Failed to create directory ";
                    m_error += temp_path;
                    return false;
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
        GraphicsLoad::initFreeImage();

        m_input_dir.setPath(m_spec.input_dir);

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
