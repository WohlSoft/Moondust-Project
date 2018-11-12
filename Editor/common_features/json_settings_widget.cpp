#include "json_settings_widget.h"

#include <QFile>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QtPropertyBrowser/qtpropertymanager.h>
#include <QtPropertyBrowser/qtvariantproperty.h>
#include <QtPropertyBrowser/qtgroupboxpropertybrowser.h>
#include <QtPropertyBrowser/qtbuttonpropertybrowser.h>
#include <QtPropertyBrowser/qttreepropertybrowser.h>

#ifdef DEBUG_BUILD
#include <QDebug>
#endif

QJsonObject JsonSettingsWidget::SetupStack::rectToArray(QVariant r)
{
    QRect rekt = r.toRect();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::rectfToArray(QVariant r)
{
    QRectF rekt = r.toRectF();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::sizeToArray(QVariant r)
{
    QSize rekt = r.toSize();
    QJsonObject a;
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::sizefToArray(QVariant r)
{
    QSizeF rekt = r.toSizeF();
    QJsonObject a;
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::pointToArray(QVariant r)
{
    QPoint rekt = r.toPoint();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::pointfToArray(QVariant r)
{
    QPointF rekt = r.toPointF();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    return a;
}

QString JsonSettingsWidget::SetupStack::getPropertyId(const QString &name)
{
    QString outPr;
    for(const QString & t : m_setupTree)
    {
        outPr.append(t);
        outPr.append('/');
    }
    outPr.append(name);
    return outPr;
}

bool JsonSettingsWidget::SetupStack::loadSetup(const QByteArray &layoutJson, QString &errorString)
{
    QJsonParseError errCode = QJsonParseError();

    m_setupTree.clear();
    if(layoutJson.isEmpty())
    {
        clear();
        return true;
    }

    m_setupCache = QJsonDocument::fromJson(layoutJson, &errCode);
    if(errCode.error != QJsonParseError::NoError)
    {
        clear();
        errorString = errCode.errorString();
        return false;
    }
    return true;
}

QByteArray JsonSettingsWidget::SetupStack::saveSetup()
{
    return m_setupCache.toJson();
}

void JsonSettingsWidget::SetupStack::clear()
{
    m_setupTree.clear();
    m_setupCache = QJsonDocument();
}

void JsonSettingsWidget::SetupStack::setValue(const QString &propertyId, QVariant value)
{
    QStringList stack = propertyId.split("/");
    QJsonObject o = m_setupCache.object();
    QStack<QJsonObject> stack_o;
    QString top;
    for(int i = 0; i < stack.size(); i++)
    {
        QString &t = stack[i];
        top = t;
        if(i == stack.size() - 1)
            break;
        stack_o.push(o);
        o = o[t].toObject();
    }

    switch(value.type())
    {
    case QVariant::Rect:
        o[top] = rectToArray(value);
        break;
    case QVariant::RectF:
        o[top] = rectfToArray(value);
        break;
    case QVariant::Point:
        o[top] = pointToArray(value);
        break;
    case QVariant::PointF:
        o[top] = pointfToArray(value);
        break;
    case QVariant::Size:
        o[top] = sizeToArray(value);
        break;
    case QVariant::SizeF:
        o[top] = sizefToArray(value);
        break;
    default:
        o[top] = QJsonValue::fromVariant(value);
        break;
    }

    for(int i = stack.size() - 2; i >= 0; i--)
    {
        QString &s = stack[i];
        QJsonObject oo = stack_o.pop();
        oo[s] = o;
        o = oo;
    }

    m_setupCache.setObject(o);
}


JsonSettingsWidget::JsonSettingsWidget(QWidget *parent) :
    QObject(parent)
{}

JsonSettingsWidget::JsonSettingsWidget(const QByteArray &layout, QWidget *parent) :
    QObject(parent)
{
    loadLayout(layout);
}

JsonSettingsWidget::~JsonSettingsWidget()
{
    if(m_browser)
        delete m_browser;
    m_browser = nullptr;
}

bool JsonSettingsWidget::loadSettingsFromFile(const QString &path)
{
    QFile f;
    f.setFileName(path);
    if(!f.open(QIODevice::ReadOnly))
    {
        m_setupStack.clear();
        return false;
    }

    QByteArray layoutJson = f.readAll();
    f.close();

    m_errorString.clear();
    return m_setupStack.loadSetup(layoutJson, m_errorString);
}

bool JsonSettingsWidget::saveSettingsIntoFile(const QString &path)
{
    QFile f;
    f.setFileName(path);
    if(!f.open(QIODevice::WriteOnly))
        return false;

    QByteArray curSetup = m_setupStack.saveSetup();
    f.write(curSetup);
    f.close();

    return true;
}

bool JsonSettingsWidget::loadSettings(const QByteArray &rawData)
{
    m_errorString.clear();
    return m_setupStack.loadSetup(rawData, m_errorString);
}

bool JsonSettingsWidget::loadSettings(const QString &rawData)
{
    m_errorString.clear();
    return m_setupStack.loadSetup(rawData.toUtf8(), m_errorString);
}

QString JsonSettingsWidget::saveSettings()
{
    return m_setupStack.saveSetup();
}

bool JsonSettingsWidget::loadLayout(const QByteArray &layout)
{
    m_browser = loadLayoutDetail(m_setupStack, layout, m_errorString);
    return m_browser != nullptr;
}

bool JsonSettingsWidget::loadLayout(const QByteArray &settings, const QByteArray &layout)
{
    if(!loadSettings(settings))
        return false;
    return loadLayout(layout);
}

bool JsonSettingsWidget::loadLayoutFromFile(const QString &settings_path, const QString &layout_path)
{
    if(!loadSettingsFromFile(settings_path))
        return false;

    QFile f;
    f.setFileName(layout_path);
    if(!f.open(QIODevice::ReadOnly))
    {
        m_errorString = f.errorString();
        return false;
    }

    QByteArray a = f.readAll();
    if(a.isEmpty())
    {
        m_errorString = f.errorString();
        return false;
    }

    f.close();

    return loadLayout(a);
}

bool JsonSettingsWidget::isValid()
{
    return (m_browser != nullptr) && m_errorString.isEmpty();
}

QWidget *JsonSettingsWidget::getWidget()
{
    return m_browser;
}

QString JsonSettingsWidget::errorString()
{
    return m_errorString;
}

QVariant JsonSettingsWidget::retrieve_property(const JsonSettingsWidget::SetupStack &setupTree,
                                               QString prop,
                                               const QVariant &defaultValue)
{
    const QJsonDocument d = setupTree.m_setupCache;
    QJsonObject o = d.object();
    QVariant out;
    QString outPr;

    for(const QString & t : setupTree.m_setupTree)
    {
        outPr.append(t);
        outPr.append(" << ");
        if(!o.contains(t))
        {
#ifdef DEBUG_BUILD
            qDebug() << outPr << prop << defaultValue << "DEFAULT-TREE";
#endif
            return defaultValue;
        }
        o = o[t].toObject();
        out = o[t].toVariant();
    }

    if(!o.contains(prop))
    {
#ifdef DEBUG_BUILD
        qDebug() << outPr << prop << defaultValue << "DEFAULT-PROP";
#endif
        return defaultValue;
    }

    switch(defaultValue.type())
    {
    case QVariant::Size:
    {
        QJsonObject sz = o[prop].toObject();
        out = QSize(sz["w"].toInt(), sz["h"].toInt());
        break;
    }
    case QVariant::SizeF:
    {
        QJsonObject sz = o[prop].toObject();
        out = QSizeF(sz["w"].toDouble(), sz["h"].toDouble());
        break;
    }
    case QVariant::Point:
    {
        QJsonObject sz = o[prop].toObject();
        out = QPoint(sz["x"].toInt(), sz["y"].toInt());
        break;
    }
    case QVariant::PointF:
    {
        QJsonObject sz = o[prop].toObject();
        out = QPointF(sz["x"].toDouble(), sz["y"].toDouble());
        break;
    }
    case QVariant::Rect:
    {
        QJsonObject sz = o[prop].toObject();
        out = QRect(sz["x"].toInt(), sz["y"].toInt(), sz["w"].toInt(), sz["h"].toInt());
        break;
    }
    case QVariant::RectF:
    {
        QJsonObject sz = o[prop].toObject();
        out = QRectF(sz["x"].toDouble(), sz["y"].toDouble(), sz["w"].toDouble(), sz["h"].toDouble());
        break;
    }
    default:
        out = o[prop].toVariant();
        break;
    }

#ifdef DEBUG_BUILD
    qDebug() << outPr << prop << out << "INPUT";
#endif

    return out;
}

static const QHash<QString, bool> loadPropertiesLoayout_requiredTypes =
{
    {"group", false},
    {"checkbox", false},
    {"spinbox", true},
    {"lineedit", false},
};

bool JsonSettingsWidget::entryHasType(const QString &type)
{
    QString l = type.toLower();
    if(!loadPropertiesLoayout_requiredTypes.contains(l))
        return false;
    return loadPropertiesLoayout_requiredTypes[l];
}

void JsonSettingsWidget::loadLayoutEntries(JsonSettingsWidget::SetupStack setupTree,
                                           const QJsonArray &elements,
                                           QtVariantPropertyManager *manager,
                                           QtProperty *target,
                                           QString &err,
                                           QWidget *parent)
{
    for(const QJsonValue &o : elements)
    {
        QString type = o["type"].toString("invalid");
        QString name = o["name"].toString(type);
        QString title = o["title"].toString(name);
        QString control = o["control"].toString();
        QtVariantProperty *item = nullptr;

        if(control.isEmpty())
            continue;//invalid
        if(entryHasType(control) && (type.isEmpty() || type == "invalid"))
            continue;
        if(name.isEmpty())
            continue;//invalid

#ifdef DEBUG_BUILD
        qDebug() << "property" << setupTree.getPropertyId(name);
#endif

        if(!control.compare("spinBox", Qt::CaseInsensitive))
        {
            if(!type.compare("int", Qt::CaseInsensitive))
            {
                int valueDefault = o["value-default"].toInt();
                int valueMin = o["value-min"].toInt(0);
                int valueMax = o["value-max"].toInt(100);
                int singleStep = o["single-step"].toInt(1);
                item = manager->addProperty(QVariant::Int, title);
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
                item->setAttribute(QLatin1String("singleStep"), singleStep);
                item->setPropertyId(setupTree.getPropertyId(name));
                target->addSubProperty(item);
            }
            else if(!type.compare("double", Qt::CaseInsensitive) || !type.compare("float", Qt::CaseInsensitive))
            {
                double valueDefault = o["value-default"].toDouble();
                double valueMin = o["value-min"].toDouble(0.0);
                double valueMax = o["value-max"].toDouble(100.0);
                double singleStep = o["single-step"].toDouble(1);
                int decimals = o["decimals"].toInt(1);
                item = manager->addProperty(QVariant::Double, title);
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
                item->setAttribute(QLatin1String("singleStep"), singleStep);
                item->setAttribute(QLatin1String("decimals"), decimals);
                item->setPropertyId(setupTree.getPropertyId(name));
                target->addSubProperty(item);
            }
        }
        else if(!control.compare("checkBox", Qt::CaseInsensitive))
        {
            bool valueDefault = o["value-default"].toBool();
            item = manager->addProperty(QVariant::Bool, title);
            item->setValue(retrieve_property(setupTree, name, valueDefault));
            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("lineEdit", Qt::CaseInsensitive))
        {
            int maxLength = o["max-length"].toInt(-1);
            QString valueDefault = o["value-default"].toString();
            QString validator = o["validator"].toString();
            item = manager->addProperty(QVariant::String, title);
            item->setValue(retrieve_property(setupTree, name, valueDefault));
            item->setAttribute(QLatin1String("maxlength"), maxLength);
            if(!validator.isEmpty())
                item->setAttribute(QLatin1String("regExp"), QRegExp(validator));
            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("comboBox", Qt::CaseInsensitive))
        {
            item = manager->addProperty(QtVariantPropertyManager::enumTypeId(), title);
            QStringList enumList;
            QVariantList children = o["elements"].toArray().toVariantList();
            for(QVariant &j : children)
                enumList.push_back(j.toString());
            int valueDefault = o["value-default"].toInt();
            item->setAttribute(QLatin1String("enumNames"), enumList);
            item->setValue(retrieve_property(setupTree, name, valueDefault));
            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("flagBox", Qt::CaseInsensitive))
        {
            item = manager->addProperty(QtVariantPropertyManager::flagTypeId(), title);
            QStringList enumList;
            QVariantList children = o["elements"].toArray().toVariantList();
            for(QVariant &j : children)
                enumList.push_back(j.toString());
            int valueDefault = o["value-default"].toInt();
            item->setAttribute(QLatin1String("flagNames"), enumList);
            item->setValue(retrieve_property(setupTree, name, valueDefault));
            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("sizeBox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();
            if(!type.compare("double", Qt::CaseInsensitive))
            {
                item = manager->addProperty(QVariant::SizeF, title);
                QSizeF valueDefault = QSizeF(defArr["w"].toDouble(), defArr["h"].toDouble());
                QSizeF valueMin = QSizeF(defMin["w"].toDouble(), defMin["h"].toDouble());
                QSizeF valueMax = QSizeF(defMax["w"].toDouble(), defMax["h"].toDouble());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
                int decimals = o["decimals"].toInt(2);
                item->setAttribute(QLatin1String("decimals"), decimals);
            }
            else
            {
                item = manager->addProperty(QVariant::Size, title);
                QSize valueDefault = QSize(defArr["w"].toInt(), defArr["h"].toInt());
                QSize valueMin = QSize(defMin["w"].toInt(), defMin["h"].toInt());
                QSize valueMax = QSize(defMax["w"].toInt(), defMax["h"].toInt());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
            }

            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("pointbox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();

            if(!type.compare("double", Qt::CaseInsensitive))
            {
                item = manager->addProperty(QVariant::PointF, title);
                QPointF valueDefault = QPointF(defArr["x"].toDouble(), defArr["y"].toDouble());
                QPointF valueMin = QPointF(defMin["x"].toDouble(), defMin["y"].toDouble());
                QPointF valueMax = QPointF(defMax["x"].toDouble(), defMax["y"].toDouble());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
                int decimals = o["decimals"].toInt(2);
                item->setAttribute(QLatin1String("decimals"), decimals);
            }
            else
            {
                item = manager->addProperty(QVariant::Point, title);
                QPoint valueDefault = QPoint(defArr["x"].toInt(), defArr["y"].toInt());
                QPoint valueMin = QPoint(defMin["x"].toInt(), defMin["y"].toInt());
                QPoint valueMax = QPoint(defMax["x"].toInt(), defMax["y"].toInt());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
            }

            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("rectbox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();

            if(!type.compare("double", Qt::CaseInsensitive))
            {
                item = manager->addProperty(QVariant::RectF, title);
                QRectF valueDefault = QRectF(defArr["x"].toDouble(), defArr["y"].toDouble(),
                        defArr["w"].toDouble(), defArr["h"].toDouble());
                QRectF valueMin     = QRectF(defMin["x"].toDouble(), defMin["y"].toDouble(),
                        defMin["w"].toDouble(), defMin["h"].toDouble());
                QRectF valueMax     = QRectF(defMax["x"].toDouble(), defMax["y"].toDouble(),
                        defMax["w"].toDouble(), defMax["h"].toDouble());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
                int decimals = o["decimals"].toInt(2);
                item->setAttribute(QLatin1String("decimals"), decimals);
            }
            else
            {
                item = manager->addProperty(QVariant::Rect, title);
                QRect valueDefault = QRect(defArr["x"].toInt(), defArr["y"].toInt(),
                        defArr["w"].toInt(), defArr["h"].toInt());
                QRect valueMin     = QRect(defMin["x"].toInt(), defMin["y"].toInt(),
                        defMin["w"].toInt(), defMin["h"].toInt());
                QRect valueMax     = QRect(defMax["x"].toInt(), defMax["y"].toInt(),
                        defMax["w"].toInt(), defMax["h"].toInt());
                item->setValue(retrieve_property(setupTree, name, valueDefault));
                item->setAttribute(QLatin1String("minimum"), valueMin);
                item->setAttribute(QLatin1String("maximum"), valueMax);
            }

            item->setPropertyId(setupTree.getPropertyId(name));
            target->addSubProperty(item);
        }
        else if(!control.compare("group", Qt::CaseInsensitive))
        {
            QJsonArray children = o["children"].toArray();
            if(!children.isEmpty())
            {
                QtProperty *subGroup = manager->addProperty(QtVariantPropertyManager::groupTypeId(), title);
                setupTree.m_setupTree.push(name);
                loadLayoutEntries(setupTree, children, manager, subGroup, err, parent);
                setupTree.m_setupTree.pop();
                target->addSubProperty(subGroup);
            }
        }
    }
}

QtAbstractPropertyBrowser *JsonSettingsWidget::loadLayoutDetail(JsonSettingsWidget::SetupStack &stack,
                                                                const QByteArray &layoutJson,
                                                                QString &err)
{
    QtAbstractPropertyBrowser *gui = nullptr;
    QString style;
    QString title;

    QJsonParseError errCode = QJsonParseError();
    QJsonDocument layout = QJsonDocument::fromJson(layoutJson, &errCode);

    if(errCode.error != QJsonParseError::NoError)
    {
        err = errCode.errorString();
        return nullptr;
    }

    QJsonObject layoutData = layout.object();

    style = layoutData["style"].toString();
    title = layoutData["title"].toString();
    if(style == "groupbox")
        gui = new QtGroupBoxPropertyBrowser(qobject_cast<QWidget*>(parent()));
    else if(style == "button")
        gui = new QtButtonPropertyBrowser(qobject_cast<QWidget*>(parent()));
    else // "tree" is default
    {
        QtTreePropertyBrowser *gui_t = new QtTreePropertyBrowser(qobject_cast<QWidget*>(parent()));
        gui_t->setPropertiesWithoutValueMarked(true);
        gui = gui_t;
    }

    QtVariantPropertyManager *variantManager = new QtVariantPropertyManager(gui);

    QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), title);

    QJsonArray layoutEntries = layoutData["layout"].toArray();
    loadLayoutEntries(stack, layoutEntries, variantManager, topItem, err, qobject_cast<QWidget*>(parent()));

    QtVariantPropertyManager::connect(variantManager, &QtVariantPropertyManager::valueChanged,
                            [this](QtProperty *p,QVariant v)
    {
        QString pid = p->propertyId();
#ifdef DEBUG_BUILD
        qDebug() << "changed:" << pid << v;
#endif
        if(!pid.isEmpty())
        {
            m_setupStack.setValue(p->propertyId(), v);
            emit settingsChanged();
        }
    }
    );

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(gui);

    gui->setFactoryForManager(variantManager, variantFactory);
    gui->addProperty(topItem);

    return gui;
}
