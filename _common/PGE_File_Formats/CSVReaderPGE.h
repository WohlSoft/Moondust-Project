#ifndef CSVREADERQT
#define CSVREADERQT



#include "CSVReader.h"

#ifdef PGE_FILES_QT
#include "pge_file_lib_globs.h"
#include <QString>

namespace CSVReader {
    struct CSVPGEReader
    {
        CSVPGEReader(PGE_FileFormats_misc::TextInput* reader) : _reader(reader) {}

        QString read_line()
        {
            return _reader->readLine();
        }
    private:
        PGE_FileFormats_misc::TextInput* _reader;
    };

    struct CSVQStringUtils
    {
        static bool find(const QString& str, QChar sep, size_t& findIndex)
        {
            int pos = str.indexOf(sep, findIndex);
            if (pos == -1)
                return false;
            findIndex = static_cast<size_t>(pos);
            return true;
        }

        static size_t length(const QString& str)
        {
            return static_cast<size_t>(str.length());
        }

        static QString substring(const QString& str, size_t pos, size_t count)
        {
            return str.mid(pos, count);
        }
    };

    struct CSVQStringConverter
    {
        template<typename T>
        static void Convert(T* out, const QString& field)
        {
            ConvertInternal(out, field, identity<T>());
        }

    private:
        template<typename T>
        static void ConvertInternal(T* out, const QString& field, identity<T>)
        {
            (void)out;
            (void)field;
            static_assert(std::is_integral<T>::value, "No default converter for this type");
        }
        static void ConvertInternal(double* out, const QString& field, identity<double>)
        {
            bool ok = true;
            *out = field.toDouble(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to double");
        }
        static void ConvertInternal(float* out, const QString& field, identity<float>)
        {
            bool ok = true;
            *out = field.toFloat(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to float");
        }
        static void ConvertInternal(int* out, const QString& field, identity<int>)
        {
            bool ok = true;
            *out = field.toInt(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to int");
        }
        static void ConvertInternal(long* out, const QString& field, identity<long>)
        {
            bool ok = true;
            *out = field.toLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long");
        }
        static void ConvertInternal(long long* out, const QString& field, identity<long long>)
        {
            bool ok = true;
            *out = field.toLongLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long long");
        }
        static void ConvertInternal(long double* out, const QString& field, identity<long double>)
        {
            bool ok = true;
            *out = field.toDouble(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long double");
        }
        static void ConvertInternal(unsigned long* out, const QString& field, identity<unsigned long>)
        {
            bool ok = true;
            *out = field.toULong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to unsigned long");
        }
        static void ConvertInternal(unsigned long long* out, const QString& field, identity<unsigned long long>)
        {
            bool ok = true;
            *out = field.toULongLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to unsigned long long");
        }
        static void ConvertInternal(bool* out, const QString& field, identity<bool>)
        {
            if(field == "0" || field == "") // FIXME: Is it correct? Or too hackish?
                *out = false;
            else if(field == "!0" || field == "1") // FIXME: Is it correct? Or too hackish?
                *out = true;
            else
                throw std::invalid_argument(std::string("Could not convert to bool (must be empty, \"0\", \"!0\" or \"1\"), got \"") + field.toStdString() + std::string("\""));
        }
        static void ConvertInternal(QString* out, const QString& field, identity<QString>)
        {
            *out = field;
        }
    };

    template<class Reader>
    constexpr CSVReader<Reader, QString, QChar, CSVQStringUtils, CSVQStringConverter> MakeCSVReaderForPGESTRING(Reader* reader, QChar sep)
    {
        return CSVReader<Reader, QString, QChar, CSVQStringUtils, CSVQStringConverter>(reader, sep);
    }




}
#endif



#endif // CSVREADERQT

