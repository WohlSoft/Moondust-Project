#include "xml_cpack_list.h"
#include "config_packs.h"

#include <QMessageBox>

XMLCpackList::XMLCpackList() : QXmlDefaultHandler()
{
    reset();
}

XMLCpackList::~XMLCpackList()
{}

void XMLCpackList::reset()
{
    updTime = 0;
    metXbelTag=true;
}

bool XMLCpackList::startElement(const QString & /* namespaceURI */,
                               const QString & /* localName */,
                               const QString &qName,
                               const QXmlAttributes &attributes)
{
    if (!metXbelTag && qName != "cpack-index-sheet") {
        errorStr = QObject::tr("The file is not an cpack-index-sheet file.");
        return false;
    } else metXbelTag=true;

    if (qName == "cpack") {
        cpackName = attributes.value("name");
        updTime = attributes.value("upd").toInt();
    }

    currentText.clear();
    return true;
}

bool XMLCpackList::endElement(const QString & /* namespaceURI */,
                             const QString & /* localName */,
                             const QString &qName)
{
    if (qName == "cpack")
    {
        ConfigPackInfo pack;
        pack.url=currentText;
        pack.lastupdate=updTime;
        pack.name=cpackName;
        cpacks_List.push_back(pack);
    }
    return true;
}

bool XMLCpackList::characters(const QString &str)
{
    currentText += str;
    return true;
}

bool XMLCpackList::fatalError(const QXmlParseException &exception)
{
    QMessageBox::information(NULL, QObject::tr("Config Pack index table"),
                             QObject::tr("Parse error at line %1, column %2:\n"
                                         "%3")
                             .arg(exception.lineNumber())
                             .arg(exception.columnNumber())
                             .arg(exception.message()));
    return false;
}

QString XMLCpackList::errorString() const
{
    return errorStr;
}

