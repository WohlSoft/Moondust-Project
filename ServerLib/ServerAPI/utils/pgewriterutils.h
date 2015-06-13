#ifndef PGEWRITERUTILS_H
#define PGEWRITERUTILS_H

#include <QDataStream>

static inline void writeStreamData(QDataStream& stream, const QByteArray& data){
    int lengthOfData = data.length();
    stream.writeRawData(reinterpret_cast<const char*>(&lengthOfData), sizeof(decltype(lengthOfData)));
    stream.writeRawData(data.data(), data.length());
}

static inline void writeStreamData(QDataStream& stream, const QString& data){
    int lengthOfData = data.length();
    stream.writeRawData(reinterpret_cast<const char*>(&lengthOfData), sizeof(decltype(lengthOfData)));
    stream.writeRawData(data.toStdString().c_str(), data.length());
}

static inline QString readStreamDataStr(QDataStream& stream){
    int lengthOfData;
    stream.readRawData(reinterpret_cast<char*>(&lengthOfData), sizeof(decltype(lengthOfData)));
    char* buf = new char[lengthOfData + 1];

    stream.readRawData(buf, lengthOfData);

    QString retData = buf;
    delete buf;
    return retData;
}

static inline QByteArray readStreamDataByteArray(QDataStream& stream){
    int lengthOfData;
    stream.readRawData(reinterpret_cast<char*>(&lengthOfData), sizeof(decltype(lengthOfData)));
    char* buf = new char[lengthOfData + 1];

    stream.readRawData(buf, lengthOfData);

    QByteArray retData(buf, lengthOfData);
    delete buf;
    return retData;
}



#endif // PGEWRITERUTILS_H
