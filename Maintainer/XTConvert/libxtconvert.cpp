#include <QDebug>

#include <QString>
#include <QDir>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QDirIterator>
#include <QProcess>

#include <fcntl.h>

#include "archive.h"
#include "archive_entry.h"

#include "lz4_pack.h"
#include "libromfs3ds.h"
#include "libtex3ds.h"

#include "libxtconvert.h"

namespace XTConvert
{

class Converter
{
public:
    QDir m_input_dir;
    QTemporaryDir m_temp_dir_owner;
    QDir m_temp_dir;

    Spec m_spec;

    QString m_error;

    bool convert_file(const QString& filename, const QString& in_path, const QString& out_path)
    {
        if(filename.endsWith("m.gif"))
            return true;
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

            success = LZ4Pack::compress(outf, inf, 4096, m_spec.target_big_endian);

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
        if(m_spec.target_3ds)
            return create_package_romfs3ds();
        else
            return create_package_iso_lz4();
    }

    bool process()
    {
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
