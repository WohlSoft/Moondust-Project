#ifndef XBELHANDLER_H
#define XBELHANDLER_H

#include <QXmlDefaultHandler>

class XMLCpackList : public QXmlDefaultHandler
{
public:
    XMLCpackList();
    ~XMLCpackList();
    void reset();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes) Q_DECL_OVERRIDE;
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName) Q_DECL_OVERRIDE;
    bool characters(const QString &str) Q_DECL_OVERRIDE;
    bool fatalError(const QXmlParseException &exception) Q_DECL_OVERRIDE;
    QString errorString() const Q_DECL_OVERRIDE;

private:
    QString cpackName;
    QString cpackPath;
    long    updTime;

    QString currentText;
    QString errorStr;
    bool metXbelTag;
};

#endif
