
#include <QString>
#include <QVector>
#include <QList>

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

