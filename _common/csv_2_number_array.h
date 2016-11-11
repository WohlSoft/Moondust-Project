
#include <QString>
#include <QVector>
#include <QList>

#include <string>
#include <sstream>
#include <cstdlib>

template<class TList>
inline void CSV2NumArray(const QString& source, TList& dest, const typename TList::value_type& def)
{
    typedef typename TList::value_type T;
    if(!source.isEmpty())
    {
        bool ok;
        QStringList tmlL = source.split(',', QString::SkipEmptyParts);
        foreach(QString fr, tmlL)
        {
            if(std::is_same<T, int>::value)
                dest.push_back(fr.toInt(&ok));
            else
            if(std::is_same<T, long>::value)
                dest.push_back(fr.toInt(&ok));
            else
            if(std::is_same<T, unsigned int>::value)
                dest.push_back(fr.toUInt(&ok));
            else
            if(std::is_same<T, unsigned long>::value)
                dest.push_back(fr.toUInt(&ok));
            else
                dest.push_back(fr.toDouble(&ok));
            if(!ok) dest.pop_back();
        }
        if(dest.isEmpty()) dest.push_back(def);
    }
    else
    {
        dest.push_back(def);
    }
}

template<class TList>
inline void CSV2NumArraySTL(const std::string& source, TList& dest, const typename TList::value_type& def)
{
    typedef typename TList::value_type T;

    if(!source.empty())
    {
        std::stringstream ss(source);
        std::string item;
        std::vector<std::string> splittedStrings;

        while(std::getline(ss, item, ','))
        {
            try
            {
                if(std::is_same<T, int>::value)
                    dest.push_back(static_cast<int>(std::strtoul(item.c_str(), NULL, 0)));
                else if(std::is_same<T, long>::value)
                    dest.push_back(std::atol(item.c_str()));
                else if(std::is_same<T, unsigned int>::value)
                    dest.push_back(static_cast<unsigned int>(std::strtoul(item.c_str(), NULL, 0)));
                else if(std::is_same<T, unsigned long>::value)
                    dest.push_back(std::strtoul(item.c_str(), NULL, 0));
                else if(std::is_same<T, float>::value)
                    dest.push_back(std::strtof(item.c_str(), NULL));
                else
                    dest.push_back(std::strtod(item.c_str(), NULL));
            }
            catch(...)
            {
                dest.pop_back();
            }
        }

        if(dest.empty())
            dest.push_back(def);
    }
    else
        dest.push_back(def);
}

