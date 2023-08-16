#include "json_settings_widget.h"

#include <QFile>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QGroupBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QResizeEvent>
#include <pge_qt_compat.h>

#ifndef UNIT_TEST
#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/util.h>
#include <audio/sdl_music_player.h>
#endif

#include "file_list_browser/musicfilelist.h"
#include "file_list_browser/levelfilelist.h"

#ifdef DEBUG_BUILD
#include <QDebug>
#endif


ColorPreview::ColorPreview(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(16, 16);
}

ColorPreview::~ColorPreview()
{}

void ColorPreview::setColor(QColor color)
{
    m_color = color;
    repaint();
}

QColor ColorPreview::color() const
{
    return m_color;
}

QSize ColorPreview::sizeHint() const
{
    QSize s_new = size();
    s_new.setWidth(s_new.height());
    return s_new;
}

void ColorPreview::resizeEvent(QResizeEvent *event)
{
    QSize s_old = event->oldSize();
    QSize s_new = event->size();
    if(s_old != s_new)
    {
        s_new.setWidth(s_new.height());
        resize(s_new);
    }

    QWidget::resizeEvent(event);
}

void ColorPreview::paintEvent(QPaintEvent *)
{
    QSize size = this->size();
    int w = size.width(), h = size.height();

    QBrush b = QBrush(m_color);

    QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);

    QBrush lg = QBrush(Qt::lightGray);
    QBrush dg = QBrush(Qt::darkGray);
    QBrush bl = QBrush(Qt::black);
    QPen noPen = QPen(Qt::NoPen);

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(noPen);
    painter.setBrush(dg);

    painter.fillRect(0, 0, w, h, lg);
    for(int i = 0; i < h / 4; i++)
    {
        for(int j = 0; j < w / 4; j++)
        {
            painter.drawRect(i * 8, j * 8, 4, 4);
            painter.drawRect(i * 8 + 4, j * 8 + 4, 4, 4);
        }
    }

    painter.fillRect(0, 0, w, h, b);

    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::NoBrush));
    painter.drawRect(0, 0, w - 1, h - 1);

    painter.end();
}


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
    QJsonObject o = m_setupCache.object();
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

void JsonSettingsWidget::SetupStack::removeElement(const QString &propertyId)
{

    QStringList stack = propertyId.split("/");
    QJsonObject o = m_setupCache.object();
    QStack<QJsonObject> stack_o;
    QString top;
    int index = -1;
    for(int i = 0; i <= stack.size() - 1; i++)
    {
        QString &t = stack[i];
        top = t;
        if(i == stack.size() - 1) {
            index = t.toInt();
            break;
        }
        stack_o.push(o);
        o = o[t].toObject();
    }

    if (!o.contains(QString::number(index))) {
        return;
    }

    int i = index;
    while (o.contains(QString::number(i + 1))) {
        o[QString::number(i)] = o[QString::number(i + 1)];
        i = i + 1;
    }
    o.remove(QString::number(i));

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
{
    QDir cur(".");
    m_directoryEpisode = cur.absolutePath();
    m_directoryData = m_directoryEpisode;
}

JsonSettingsWidget::JsonSettingsWidget(const QByteArray &layout, QWidget *parent) :
    QObject(parent)
{
    QDir cur(".");
    m_directoryEpisode = cur.absolutePath();
    m_directoryData = m_directoryEpisode;
    loadLayout(layout);
}

JsonSettingsWidget::~JsonSettingsWidget()
{
    if(m_browser)
        delete m_browser;
    m_browser = nullptr;
}

void JsonSettingsWidget::setSearchDirectories(const QString &episode, const QString &data)
{
    m_directoryEpisode = episode;
    if(!m_directoryEpisode.endsWith('/'))
        m_directoryEpisode.append('/');
    m_directoryData = m_directoryEpisode + data;
    if(!m_directoryData.endsWith('/'))
        m_directoryData.append('/');
}

void JsonSettingsWidget::setConfigPack(DataConfig *config)
{
    m_configPack = config;
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

bool JsonSettingsWidget::loadSettings(const QJsonDocument &rawData)
{
    m_errorString.clear();
    m_setupStack.m_setupTree.clear();
    m_setupStack.m_setupCache = rawData;
    return true;
}

QString JsonSettingsWidget::saveSettings()
{
    return m_setupStack.saveSetup();
}

QJsonDocument JsonSettingsWidget::getSettings()
{
    return m_setupStack.m_setupCache;
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

bool JsonSettingsWidget::loadLayout(const QJsonDocument &settings, const QByteArray &layout)
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

bool JsonSettingsWidget::spacerNeeded()
{
    return m_spacerNeeded;
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
    {"itemselect", true}
};

bool JsonSettingsWidget::entryHasType(const QString &type)
{
    QString l = type.toLower();
    if(!loadPropertiesLoayout_requiredTypes.contains(l))
        return false;
    return loadPropertiesLoayout_requiredTypes[l];
}

static QString colorHexValueText(const QColor &c, bool alpha = true)
{
    if(alpha)
        return QString("#%1%2%3%4")
            .arg(c.red(), 2, 16, QChar('0'))
            .arg(c.green(), 2, 16, QChar('0'))
            .arg(c.blue(), 2, 16, QChar('0'))
            .arg(c.alpha(), 2, 16, QChar('0'));
    else
        return QString("#%1%2%3")
            .arg(c.red(), 2, 16, QChar('0'))
            .arg(c.green(), 2, 16, QChar('0'))
            .arg(c.blue(), 2, 16, QChar('0'));
}

static QColor colorFromHexValue(QString color)
{
    QColor newRgba;

    if(color.size() < 7)
        return QColor();

    color.remove(0, 1);

    if(color.size() >= 6)
    {
        newRgba.setRed(color.mid(0, 2).toInt(nullptr, 16));
        newRgba.setGreen(color.mid(2, 2).toInt(nullptr, 16));
        newRgba.setBlue(color.mid(4, 2).toInt(nullptr, 16));
    }

    if(color.size() == 8)
        newRgba.setAlpha(color.mid(6, 2).toInt(nullptr, 16));
    else
        newRgba.setAlpha(255);

    return newRgba;
}

static void setFlagBoxValue(QVector<QCheckBox *> &flagBox, unsigned int value)
{
    for(QCheckBox *b : flagBox)
    {
        b->setChecked(static_cast<bool>(value & 0x01));
        value >>= 1;
    }
}

static unsigned int getFlagBoxValue(const QVector<QCheckBox *> &flagBox)
{
    unsigned int value = 0;
    for(auto it = flagBox.rbegin(); it != flagBox.rend(); it++)
    {
        auto b = *it;
        value <<= 1;
        value |= (b->isChecked() ? 1 : 0);
    }
    return value;
}

void JsonSettingsWidget::loadLayoutEntries(JsonSettingsWidget::SetupStack setupTree,
                                           const QJsonArray &elements,
                                           QWidget *target,
                                           const QString &err,
                                           QWidget *parent)
{
    int row = 0;
    if(!target->layout())
    {
        target->setLayout(new QGridLayout(target));
    }

    QGridLayout *l = qobject_cast<QGridLayout *>(target->layout());
    Q_ASSERT(l);

    l->setColumnStretch(0, 0);
    l->setColumnStretch(1, 1000);
    l->setSizeConstraint(QLayout::SetMinimumSize);

    for(const QJsonValue &ov : elements)
    {
        QJsonObject o = ov.toObject();
        QString type = o["type"].toString("invalid");
        QString name = o["name"].toString(type);
        QString title = o["title"].toString(name);
        QString control = o["control"].toString();
        QString tooltip = o["tooltip"].toString();

        if(control.isEmpty())
            continue;//invalid
        if(entryHasType(control) && (type.isEmpty() || type == "invalid"))
            continue;
        if(name.isEmpty())
            continue;//invalid

#ifdef DEBUG_BUILD
        qDebug() << "property" << setupTree.getPropertyId(name);
#endif

        // Spin box
        if(!control.compare("spinBox", Qt::CaseInsensitive))
        {
            if(!type.compare("int", Qt::CaseInsensitive))
            {
                int valueDefault = o["value-default"].toInt();
                int valueMin = o["value-min"].toInt(0);
                int valueMax = o["value-max"].toInt(100);
                int singleStep = o["single-step"].toInt(1);

                QLabel *label = new QLabel(target);
                label->setText(title);
                label->setToolTip(tooltip);
                l->addWidget(label, row, 0);

                QSpinBox *it = new QSpinBox(target);
                it->setToolTip(tooltip);
                it->setMinimum(valueMin);
                it->setMaximum(valueMax);
                it->setSingleStep(singleStep);
                it->setValue(retrieve_property(setupTree, name, valueDefault).toInt());

                const QString id = setupTree.getPropertyId(name);
                l->addWidget(it, row, 1);
                QObject::connect(it, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [id, this](int val)
                {
#ifdef DEBUG_BUILD
                    qDebug() << "changed:" << id << val;
#endif
                    m_setupStack.setValue(id, val);
                    emit settingsChanged();
                });
                row++;
            }
            else if(!type.compare("double", Qt::CaseInsensitive) || !type.compare("float", Qt::CaseInsensitive))
            {
                double valueDefault = o["value-default"].toDouble();
                double valueMin = o["value-min"].toDouble(0.0);
                double valueMax = o["value-max"].toDouble(100.0);
                double singleStep = o["single-step"].toDouble(1);
                int decimals = o["decimals"].toInt(1);

                QLabel *label = new QLabel(target);
                label->setText(title);
                label->setToolTip(tooltip);
                l->addWidget(label, row, 0);

                QDoubleSpinBox *it = new QDoubleSpinBox(target);
                it->setToolTip(tooltip);
                it->setMinimum(valueMin);
                it->setMaximum(valueMax);
                it->setSingleStep(singleStep);
                it->setDecimals(decimals);
                it->setValue(retrieve_property(setupTree, name, valueDefault).toDouble());

                const QString id = setupTree.getPropertyId(name);
                l->addWidget(it, row, 1);
                QObject::connect(it, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, this](double val)
                {
#ifdef DEBUG_BUILD
                    qDebug() << "changed:" << id << val;
#endif
                    m_setupStack.setValue(id, val);
                    emit settingsChanged();
                });
                row++;
            }
        }

        // Check box
        else if(!control.compare("checkBox", Qt::CaseInsensitive))
        {
            bool valueDefault = o["value-default"].toBool(false);
            bool textAtLeft = true;

            if(o.keys().contains("text-at-left"))
                textAtLeft = o["text-at-left"].toBool();

            if(textAtLeft)
            {
                QLabel *label = new QLabel(target);
                label->setText(title);
                label->setToolTip(tooltip);
                l->addWidget(label, row, 0);
            }

            QCheckBox *it = new QCheckBox(target);
            it->setToolTip(tooltip);
            if(!textAtLeft)
                it->setText(title);
            it->setChecked(retrieve_property(setupTree, name, valueDefault).toBool());

            const QString id = setupTree.getPropertyId(name);
            if(!textAtLeft)
                l->addWidget(it, row, 0, 1, 2);
            else
                l->addWidget(it, row, 1);
            QObject::connect(it, static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked),
            [id, this](bool val)
            {
#ifdef DEBUG_BUILD
                qDebug() << "changed:" << id << val;
#endif
                m_setupStack.setValue(id, val);
                emit settingsChanged();
            });
            row++;
        }

        // Color box
        else if(!control.compare("color", Qt::CaseInsensitive))
        {
            QColor valueDefault = QColor(o["value-default"].toString());
            QColor value = colorFromHexValue(retrieve_property(setupTree, name, valueDefault).toString());
            bool useAlpha = true;
            if(o.keys().contains("alpha-channel"))
                useAlpha = o["alpha-channel"].toBool();

            QLabel *label = new QLabel(target);
            label->setText(title);
            label->setToolTip(tooltip);
            l->addWidget(label, row, 0);

            QFrame *colorBox = new QFrame(target);
            colorBox->setToolTip(tooltip);
            l->addWidget(colorBox, row, 1);
            QHBoxLayout *colorBoxL = new QHBoxLayout(colorBox);
            colorBoxL->setContentsMargins(0, 0, 0, 0);
            colorBox->setLayout(colorBoxL);

            ColorPreview *colorPreview = new ColorPreview(colorBox);
            colorBoxL->addWidget(colorPreview, 0);

            QLineEdit *colorString = new QLineEdit(colorBox);
            QString colorStringExpr = useAlpha ? "#[0-9a-fA-F]{6,8}" : "#[0-9a-fA-F]{6}";
            colorString->setValidator(new Q_QRegExpValidator(Q_QRegExp(colorStringExpr), colorString));
            colorBoxL->addWidget(colorString, 1000);

            QPushButton *colorChoose = new QPushButton(colorBox);
            colorChoose->setText("...");
            colorChoose->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
            colorChoose->setFixedWidth(24);
            colorBoxL->addWidget(colorChoose, 0);

            colorPreview->setColor(value);
            colorString->setText(colorHexValueText(value, useAlpha));

            const QString id = setupTree.getPropertyId(name);

            QObject::connect(colorChoose, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
            [id, target, useAlpha, colorPreview, colorString, this](bool)
            {
                QColor oldRgba = colorFromHexValue(colorString->text());
                QColorDialog::ColorDialogOption useAlphaFlag = (useAlpha ? QColorDialog::ShowAlphaChannel : QColorDialog::ColorDialogOption(0));
                QColor newRgba = QColorDialog::getColor(oldRgba, target,
                                                        QString(),
                                                        useAlphaFlag | QColorDialog::DontUseNativeDialog);
                if(newRgba.isValid() && newRgba != oldRgba)
                {
                    QString val = colorHexValueText(newRgba, useAlpha);
#ifdef DEBUG_BUILD
                    qDebug() << "changed:" << id << colorString->text() << val;
#endif
                    colorString->setText(val);
                    colorPreview->setColor(newRgba);
                    m_setupStack.setValue(id, val);
                    emit settingsChanged();
                }
            });

            QObject::connect(colorString, &QLineEdit::editingFinished,
            [id, colorPreview, colorString, valueDefault, this]()
            {
                QString val = colorString->text();
                QColor newRgba = colorFromHexValue(colorString->text());
                QColor oldRgna = colorPreview->color();
                if(newRgba.isValid() && (oldRgna != newRgba))
                {
#ifdef DEBUG_BUILD
                    qDebug() << "changed:" << id << val;
#endif
                    colorPreview->setColor(newRgba);
                    m_setupStack.setValue(id, val);
                    emit settingsChanged();
                }
            });

            row++;
        }

        // Line edit box
        else if(!control.compare("lineEdit", Qt::CaseInsensitive))
        {
            int maxLength = o["max-length"].toInt(-1);
            QString valueDefault = o["value-default"].toString();
            QString validator = o["validator"].toString();
            QString placeholder = o["placeholder"].toString();
            bool readOnly = false;

            if(o.keys().contains("read-only"))
                readOnly = o["read-only"].toBool();

            QLabel *label = new QLabel(target);
            label->setText(title);
            label->setToolTip(tooltip);
            l->addWidget(label, row, 0);

            QLineEdit *it = new QLineEdit(target);

            it->setText(retrieve_property(setupTree, name, valueDefault).toString());
            it->setToolTip(tooltip);
            it->setPlaceholderText(placeholder);
            it->setMaxLength(maxLength);
            if(!validator.isEmpty())
                it->setValidator(new Q_QRegExpValidator(Q_QRegExp(validator), it));
            it->setReadOnly(readOnly);

            const QString id = setupTree.getPropertyId(name);
            l->addWidget(it, row, 1);
            QObject::connect(it, static_cast<void(QLineEdit::*)(const QString &)>(&QLineEdit::textChanged),
            [id, this](const QString &val)
            {
#ifdef DEBUG_BUILD
                qDebug() << "changed:" << id << val;
#endif
                m_setupStack.setValue(id, val);
                emit settingsChanged();
            });
            row++;
        }

        // Line edit box
        else if(!control.compare("multiLineEdit", Qt::CaseInsensitive))
        {
            QString valueDefault = o["value-default"].toString();
            bool readOnly = false;

            if(o.keys().contains("read-only"))
                readOnly = o["read-only"].toBool();

            QGroupBox *multiText = new QGroupBox(target);
            multiText->setTitle(title);
            multiText->setToolTip(tooltip);
            l->addWidget(multiText, row, 0, 1, 2);
            QVBoxLayout *multiTextL = new QVBoxLayout(multiText);
            multiTextL->setContentsMargins(0, 0, 0, 0);
            multiText->setLayout(multiTextL);

            QPlainTextEdit *it = new QPlainTextEdit(target);

            it->setPlainText(retrieve_property(setupTree, name, valueDefault).toString());
            it->setToolTip(tooltip);
            it->setReadOnly(readOnly);
            multiTextL->addWidget(it, row);

            const QString id = setupTree.getPropertyId(name);
            QObject::connect(it, &QPlainTextEdit::textChanged,
            [id, it, this]()
            {
                QString val = it->toPlainText();
#ifdef DEBUG_BUILD
                qDebug() << "changed:" << id << val;
#endif
                m_setupStack.setValue(id, val);
                emit settingsChanged();
            });
            row++;
        }

        // Music/Sound/Level/Generic file
        else if(
            !control.compare("musicFile", Qt::CaseInsensitive) ||
            !control.compare("soundFile", Qt::CaseInsensitive) ||
            !control.compare("levelFile", Qt::CaseInsensitive) ||
            !control.compare("file", Qt::CaseInsensitive)
        )
        {
            QString valueDefault = o["value-default"].toString();
            QString placeholder = o["placeholder"].toString();
            QString dialogTitle = o["dialog-title"].toString();
            QString dialogDescription = o["dialog-description"].toString();

            QStringList filters;
            QVariantList filtersList = o["filters"].toArray().toVariantList();
            for(QVariant &j : filtersList)
                filters.push_back(j.toString());

            bool isMusic = !control.compare("musicFile", Qt::CaseInsensitive);
            bool isSFX = !control.compare("soundFile", Qt::CaseInsensitive);
            bool isLevel = !control.compare("levelFile", Qt::CaseInsensitive);
            bool isGenericFile = !control.compare("file", Qt::CaseInsensitive);

            bool lookAtEpisode = isLevel;

            if(o.keys().contains("directory"))
                lookAtEpisode = !o["directory"].toString(isLevel ? "data" : "episode").compare("episode", Qt::CaseInsensitive);

            const QString id = setupTree.getPropertyId(name);

            QLabel *label = new QLabel(target);
            label->setText(title);
            label->setToolTip(tooltip);
            l->addWidget(label, row, 0);

            QFrame *fileBox = new QFrame(target);
            QHBoxLayout *fileBoxL = new QHBoxLayout(fileBox);
            fileBoxL->setContentsMargins(0, 0, 0, 0);
            fileBox->setLayout(fileBoxL);
            fileBox->setToolTip(tooltip);

            QLineEdit *it = new QLineEdit(fileBox);
            it->setText(retrieve_property(setupTree, name, valueDefault).toString());
            it->setToolTip(tooltip);
            it->setPlaceholderText(placeholder);
            fileBoxL->addWidget(it, 1000);

            QPushButton *browse = new QPushButton(fileBox);
            browse->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
            browse->setFixedWidth(24);
            browse->setText("...");
            browse->setToolTip(tr("Browse"));
            fileBoxL->addWidget(browse, 0);

            if(isMusic || isSFX)
            {
                QPushButton *preview = new QPushButton(fileBox);
                preview->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
                preview->setFixedWidth(24);
                preview->setText("▶️");
                preview->setToolTip(tr("Play"));
                fileBoxL->addWidget(preview, 0);

                QObject::connect(preview, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                [id, it, isSFX, lookAtEpisode, this](bool)
                {
                    QString root = lookAtEpisode ? m_directoryEpisode : m_directoryData;
                    if(!root.endsWith('/'))
                        root.append('/');
                    QString file = it->text();
                    QString fileNA = file.split('|').first();
                    if(!file.isEmpty() && QFile::exists(root + fileNA))
                    {
                        qDebug() << "Trying to play music" << root + file;
#ifndef UNIT_TEST
                        if(isSFX)
                            PGE_SfxPlayer::playFile(root + file);
                        else
                        {
                            PGE_MusPlayer::openFile(root + file);
                            PGE_MusPlayer::play();
                        }
#endif
                    }
                });
            }

            l->addWidget(fileBox, row, 1);

            QObject::connect(browse, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
            [id, it, isSFX, isLevel, isGenericFile, target, lookAtEpisode, dialogTitle, dialogDescription, filters, this](bool)
            {
                if(isGenericFile)
                {
                    FileListBrowser file(lookAtEpisode ? m_directoryEpisode : m_directoryData, it->text(), target);
                    if(!dialogTitle.isEmpty())
                        file.setWindowTitle(dialogTitle);
                    if(!dialogDescription.isEmpty())
                        file.setDescription(dialogDescription);
                    if(!filters.isEmpty())
                        file.setFilters(filters);
                    file.startListBuilder();
                    if(file.exec() == QDialog::Accepted)
                    {
                        it->setText(file.currentFile());
                        m_setupStack.setValue(id, file.currentFile());
                        emit settingsChanged();
                    }
                }
                else if(isLevel)
                {
                    LevelFileList levels(lookAtEpisode ? m_directoryEpisode : m_directoryData, it->text(), target);
                    if(!dialogTitle.isEmpty())
                        levels.setWindowTitle(dialogTitle);
                    if(!dialogDescription.isEmpty())
                        levels.setDescription(dialogDescription);
                    if(levels.exec() == QDialog::Accepted)
                    {
                        it->setText(levels.currentFile());
                        m_setupStack.setValue(id, levels.currentFile());
                        emit settingsChanged();
                    }
                }
                else
                {
                    MusicFileList muz(lookAtEpisode ? m_directoryEpisode : m_directoryData, it->text(), target, isSFX);
                    if(!dialogTitle.isEmpty())
                        muz.setWindowTitle(dialogTitle);
                    if(!dialogDescription.isEmpty())
                        muz.setDescription(dialogDescription);
                    if(muz.exec() == QDialog::Accepted)
                    {
                        it->setText(muz.currentFile());
                        m_setupStack.setValue(id, muz.currentFile());
                        emit settingsChanged();
                    }
                }
            });

            QObject::connect(it, static_cast<void(QLineEdit::*)(const QString &)>(&QLineEdit::textChanged),
            [id, this](const QString &val)
            {
#ifdef DEBUG_BUILD
                qDebug() << "changed:" << id << val;
#endif
                m_setupStack.setValue(id, val);
                emit settingsChanged();
            });
            row++;
        }

        // Item-ID select dialog
        else if(!control.compare("itemSelect", Qt::CaseInsensitive))
        {
            int valueDefault = o["value-default"].toInt();
            int value = retrieve_property(setupTree, name, valueDefault).toInt();
            const QString id = setupTree.getPropertyId(name);
            QString type = o["type"].toString("invalid");

            QLabel *label = new QLabel(target);
            label->setText(title);
            label->setToolTip(tooltip);
            l->addWidget(label, row, 0);

            auto updateButton = [type](QPushButton *button, int value)
            {
                if(!type.compare("npc", Qt::CaseInsensitive))
                    button->setText(value ? QString("NPC-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("npcOrCoins", Qt::CaseInsensitive))
                    button->setText(value ? (
                                            (value > 0) ? QString("NPC-%1").arg(value) : tr("%1 coins").arg(-value)
                                            ) : tr("[empty]"));
                else if(!type.compare("block", Qt::CaseInsensitive))
                    button->setText(value ? QString("BLOCK-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("bgo", Qt::CaseInsensitive))
                    button->setText(value ? QString("BGO-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("tile", Qt::CaseInsensitive) || !type.compare("terrain", Qt::CaseInsensitive))
                    button->setText(value ? QString("TERRAIN-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("scenery", Qt::CaseInsensitive))
                    button->setText(value ? QString("SCENERY-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("path", Qt::CaseInsensitive))
                    button->setText(value ? QString("PATH-%1").arg(value) : tr("[empty]"));
                else if(!type.compare("level", Qt::CaseInsensitive))
                    button->setText(value ? QString("LEVEL-%1").arg(value) : tr("[empty]"));
                else
                    button->setText("---------");
            };

            QPushButton *it = new QPushButton(target);
            updateButton(it, value);
            it->setToolTip(tooltip);
            it->setDisabled(type == "invalid");
            l->addWidget(it, row, 1);

            QObject::connect(it, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
            [id, it, target, type, valueDefault, updateButton, this](bool)
            {
#ifndef UNIT_TEST
                int value = retrieve_property(m_setupStack, id, valueDefault).toInt();
                ItemSelectDialog *itemsList = nullptr;
                int *itemListValuePtr = nullptr;
                if(!type.compare("block", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_BLOCK,
                                                     0, value, 0, 0, 0, 0, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_BLOCK);
                    itemListValuePtr = &itemsList->blockID;
                }
                else if(!type.compare("bgo", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_BGO,
                                                     0, 0, value, 0, 0, 0, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_BGO);
                    itemListValuePtr = &itemsList->bgoID;
                }
                else if(!type.compare("npc", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_NPC,
                                                     0, 0, 0, value, 0, 0, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_NPC);
                    itemListValuePtr = &itemsList->npcID;
                }
                else if(!type.compare("npcOrCoins", Qt::CaseInsensitive))
                {
                    int npcExtraData = ItemSelectDialog::NPCEXTRA_WITHCOINS;
                    if (value < 0)
                    {
                        value = -value;
                        npcExtraData |= ItemSelectDialog::NPCEXTRA_ISCOINSELECTED;
                    }
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_NPC,
                                                     npcExtraData, 0, 0, value, 0, 0, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_NPC);
                    itemListValuePtr = &itemsList->npcID;
                }
                else if(!type.compare("tile", Qt::CaseInsensitive) || !type.compare("terrain", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_TILE,
                                                     0, 0, 0, 0, value, 0, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_TILE);
                    itemListValuePtr = &itemsList->tileID;
                }
                else if(!type.compare("scenery", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_SCENERY,
                                                     0, 0, 0, 0, 0, value, 0, 0, 0,
                                                     target, ItemSelectDialog::TAB_SCENERY);
                    itemListValuePtr = &itemsList->sceneryID;
                }
                else if(!type.compare("path", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_PATH,
                                                     0, 0, 0, 0, 0, 0, value, 0, 0,
                                                     target, ItemSelectDialog::TAB_PATH);
                    itemListValuePtr = &itemsList->pathID;
                }
                else if(!type.compare("level", Qt::CaseInsensitive))
                {
                    itemsList = new ItemSelectDialog(m_configPack, ItemSelectDialog::TAB_LEVEL,
                                                     0, 0, 0, 0, 0, 0, 0, value, 0,
                                                     target, ItemSelectDialog::TAB_LEVEL);
                    itemListValuePtr = &itemsList->levelID;
                }
                
                if (itemsList)
                {
                    util::DialogToCenter(itemsList, true);
                    if(itemsList->exec() == QDialog::Accepted)
                    {
                        int newValue = *itemListValuePtr;
                        if (itemsList->isCoin)
                        {
                            newValue = -newValue;
                        }
                        updateButton(it, newValue);
                        m_setupStack.setValue(id, newValue);
                        emit settingsChanged();
                    }
                }
#endif
            });
            row++;
        }

        // Description block
        else if(!control.compare("description", Qt::CaseInsensitive))
        {
            QString text = o["text"].toString();
            QLabel *label = new QLabel(target);
            label->setText(text);
            label->setToolTip(tooltip);
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
            label->setOpenExternalLinks(true);
            l->addWidget(label, row, 0, 1, 2);
            row++;
        }

        // Combo-box
        else if(!control.compare("comboBox", Qt::CaseInsensitive))
        {
            QStringList enumList;
            QVariantList children = o["elements"].toArray().toVariantList();
            for(QVariant &j : children)
                enumList.push_back(j.toString());
            int valueDefault = o["value-default"].toInt();

            QLabel *label = new QLabel(target);
            label->setText(title);
            label->setToolTip(tooltip);
            l->addWidget(label, row, 0);

            QComboBox *it = new QComboBox(target);
            it->addItems(enumList);
            it->setCurrentIndex(retrieve_property(setupTree, name, valueDefault).toInt());
            it->setToolTip(tooltip);

            const QString id = setupTree.getPropertyId(name);
            l->addWidget(it, row, 1);
            QObject::connect(it, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [id, this](int val)
            {
#ifdef DEBUG_BUILD
                qDebug() << "changed:" << id << val;
#endif
                m_setupStack.setValue(id, val);
                emit settingsChanged();
            });
            row++;
        }

        // Flag box
        else if(!control.compare("flagBox", Qt::CaseInsensitive))
        {
            QVariantList children = o["elements"].toArray().toVariantList();
            QVector<QCheckBox*> values;
            unsigned int valueDefault = o["value-default"].toVariant().toUInt();

            QGroupBox *sizeBox = new QGroupBox(target);
            sizeBox->setTitle(title);
            sizeBox->setToolTip(tooltip);
            QVBoxLayout *sizeBoxL = new QVBoxLayout(sizeBox);
            sizeBox->setLayout(sizeBoxL);

            const QString id = setupTree.getPropertyId(name);

            for(QVariant &j : children)
            {
                QCheckBox *cb = new QCheckBox(sizeBox);
                cb->setText(j.toString());
                cb->setToolTip(tooltip);
                sizeBoxL->addWidget(cb);
                values.append(cb);
            }

            for(QCheckBox *cb : values)
            {
                QObject::connect(cb, static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked),
                [id, values, this](bool)
                {
                    unsigned int val = getFlagBoxValue(values);
#ifdef DEBUG_BUILD
                    qDebug() << "changed:" << id << val;
#endif
                    m_setupStack.setValue(id, val);
                    emit settingsChanged();
                });
            }

            setFlagBoxValue(values, retrieve_property(setupTree, name, valueDefault).toUInt());

            l->addWidget(sizeBox, row, 0, 1, 2);
            row++;
        }

        // Size box
        else if(!control.compare("sizeBox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();

            QGroupBox *sizeBox = new QGroupBox(target);
            sizeBox->setTitle(title);
            sizeBox->setToolTip(tooltip);
            QHBoxLayout *sizeBoxL = new QHBoxLayout(sizeBox);
            sizeBox->setLayout(sizeBoxL);

            const QString id = setupTree.getPropertyId(name);

            if(!type.compare("double", Qt::CaseInsensitive))
            {
                QSizeF valueDefault = QSizeF(defArr["w"].toDouble(), defArr["h"].toDouble());
                QSizeF valueMin = QSizeF(defMin["w"].toDouble(INT_MIN), defMin["h"].toDouble(INT_MIN));
                QSizeF valueMax = QSizeF(defMax["w"].toDouble(INT_MAX), defMax["h"].toDouble(INT_MAX));
                int decimals = o["decimals"].toInt(2);
                QSizeF value = retrieve_property(setupTree, name, valueDefault).toSizeF();

                QLabel *label = new QLabel(target);
                label->setText(tr("W", "Width, shortly"));
                sizeBoxL->addWidget(label, 0);

                QDoubleSpinBox *w = new QDoubleSpinBox(sizeBox);
                w->setToolTip(tooltip);
                w->setMinimum(valueMin.width());
                w->setMaximum(valueMax.width());
                w->setDecimals(decimals);
                w->setValue(value.width());
                sizeBoxL->addWidget(w, 1000);

                QLabel *labelH = new QLabel(target);
                labelH->setText(tr("H", "Height, shortly"));
                sizeBoxL->addWidget(labelH, 0);

                QDoubleSpinBox *h = new QDoubleSpinBox(sizeBox);
                h->setToolTip(tooltip);
                h->setMinimum(valueMin.height());
                h->setMaximum(valueMax.height());
                h->setDecimals(decimals);
                h->setValue(value.height());
                sizeBoxL->addWidget(h, 1000);

                QObject::connect(w, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, h, this](double val)
                {
                    QSizeF v(val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, w, this](double val)
                {
                    QSizeF v(w->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed height:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }
            else
            {
                QSize valueDefault = QSize(defArr["w"].toInt(), defArr["h"].toInt());
                QSize valueMin = QSize(defMin["w"].toInt(INT_MIN), defMin["h"].toInt(INT_MIN));
                QSize valueMax = QSize(defMax["w"].toInt(INT_MAX), defMax["h"].toInt(INT_MAX));
                QSize value = retrieve_property(setupTree, name, valueDefault).toSize();

                QLabel *label = new QLabel(target);
                label->setText(tr("W", "Width, shortly"));
                sizeBoxL->addWidget(label, 0);

                QSpinBox *w = new QSpinBox(sizeBox);
                w->setToolTip(tooltip);
                w->setMinimum(valueMin.width());
                w->setMaximum(valueMax.width());
                w->setValue(value.width());
                sizeBoxL->addWidget(w, 1000);

                QLabel *labelH = new QLabel(target);
                labelH->setText(tr("H", "Height, shortly"));
                sizeBoxL->addWidget(labelH, 0);

                QSpinBox *h = new QSpinBox(sizeBox);
                h->setToolTip(tooltip);
                h->setMinimum(valueMin.height());
                h->setMaximum(valueMax.height());
                h->setValue(value.height());
                sizeBoxL->addWidget(h, 1000);

                QObject::connect(w, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, h, this](int val)
                {
                    QSize v(val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, w, this](int val)
                {
                    QSize v(w->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed height:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }

            l->addWidget(sizeBox, row, 0, 1, 2);
            row++;
        }


        // Point box
        else if(!control.compare("pointbox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();

            QGroupBox *pointBox = new QGroupBox(target);
            pointBox->setTitle(title);
            pointBox->setToolTip(tooltip);
            QHBoxLayout *pointBoxL = new QHBoxLayout(pointBox);
            pointBox->setLayout(pointBoxL);

            const QString id = setupTree.getPropertyId(name);

            if(!type.compare("double", Qt::CaseInsensitive))
            {
                QPointF valueDefault = QPointF(defArr["x"].toDouble(), defArr["y"].toDouble());
                QPointF valueMin = QPointF(defMin["x"].toDouble(INT_MIN), defMin["y"].toDouble(INT_MIN));
                QPointF valueMax = QPointF(defMax["x"].toDouble(INT_MAX), defMax["y"].toDouble(INT_MAX));
                int decimals = o["decimals"].toInt(2);
                QPointF value = retrieve_property(setupTree, name, valueDefault).toPointF();

                QLabel *label = new QLabel(target);
                label->setText("X");
                pointBoxL->addWidget(label, 0);

                QDoubleSpinBox *x = new QDoubleSpinBox(pointBox);
                x->setToolTip(tooltip);
                x->setMinimum(valueMin.x());
                x->setMaximum(valueMax.x());
                x->setDecimals(decimals);
                x->setValue(value.x());
                pointBoxL->addWidget(x, 1000);

                QLabel *labelH = new QLabel(target);
                labelH->setText("Y");
                pointBoxL->addWidget(labelH, 0);

                QDoubleSpinBox *y = new QDoubleSpinBox(pointBox);
                y->setToolTip(tooltip);
                y->setMinimum(valueMin.y());
                y->setMaximum(valueMax.y());
                y->setDecimals(decimals);
                y->setValue(value.y());
                pointBoxL->addWidget(y, 1000);

                QObject::connect(x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, y, this](double val)
                {
                    QPointF v(val, y->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, x, this](double val)
                {
                    QPointF v(x->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }
            else
            {
                QPoint valueDefault = QPoint(defArr["x"].toInt(), defArr["y"].toInt());
                QPoint valueMin = QPoint(defMin["x"].toInt(INT_MIN), defMin["y"].toInt(INT_MIN));
                QPoint valueMax = QPoint(defMax["x"].toInt(INT_MAX), defMax["y"].toInt(INT_MAX));
                QPoint value = retrieve_property(setupTree, name, valueDefault).toPoint();

                QLabel *label = new QLabel(target);
                label->setText("X");
                pointBoxL->addWidget(label, 0);

                QSpinBox *x = new QSpinBox(pointBox);
                x->setToolTip(tooltip);
                x->setMinimum(valueMin.x());
                x->setMaximum(valueMax.x());
                x->setValue(value.x());
                pointBoxL->addWidget(x, 1000);

                QLabel *labelH = new QLabel(target);
                labelH->setText("Y");
                pointBoxL->addWidget(labelH, 0);

                QSpinBox *y = new QSpinBox(pointBox);
                y->setToolTip(tooltip);
                y->setMinimum(valueMin.y());
                y->setMaximum(valueMax.y());
                y->setValue(value.y());
                pointBoxL->addWidget(y, 1000);

                QObject::connect(x, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, y, this](int val)
                {
                    QPoint v(val, y->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, x, this](int val)
                {
                    QPoint v(x->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }

            l->addWidget(pointBox, row, 0, 1, 2);
            row++;
        }

        // Rectangle
        else if(!control.compare("rectbox", Qt::CaseInsensitive))
        {
            QJsonObject defArr = o["value-default"].toObject();
            QJsonObject defMin = o["value-min"].toObject();
            QJsonObject defMax = o["value-max"].toObject();

            QGroupBox *rectBox = new QGroupBox(target);
            rectBox->setTitle(title);
            rectBox->setToolTip(tooltip);
            QGridLayout *rectBoxL = new QGridLayout(rectBox);
            rectBox->setLayout(rectBoxL);

            const QString id = setupTree.getPropertyId(name);

            if(!type.compare("double", Qt::CaseInsensitive))
            {
                QRectF valueDefault = QRectF(defArr["x"].toDouble(), defArr["y"].toDouble(),
                        defArr["w"].toDouble(), defArr["h"].toDouble());
                QRectF valueMin     = QRectF(defMin["x"].toDouble(INT_MIN), defMin["y"].toDouble(INT_MIN),
                        defMin["w"].toDouble(INT_MIN), defMin["h"].toDouble(INT_MIN));
                QRectF valueMax     = QRectF(defMax["x"].toDouble(INT_MAX), defMax["y"].toDouble(INT_MAX),
                        defMax["w"].toDouble(INT_MAX), defMax["h"].toDouble(INT_MAX));
                int decimals = o["decimals"].toInt(2);
                QRectF value = retrieve_property(setupTree, name, valueDefault).toRectF();

                QLabel *labelX = new QLabel(target);
                labelX->setText("X");
                rectBoxL->addWidget(labelX, 0, 0);

                QDoubleSpinBox *x = new QDoubleSpinBox(rectBox);
                x->setToolTip(tooltip);
                x->setMinimum(valueMin.x());
                x->setMaximum(valueMax.x());
                x->setDecimals(decimals);
                x->setValue(value.x());
                rectBoxL->addWidget(x, 0, 1);

                QLabel *labelY = new QLabel(target);
                labelY->setText("Y");
                rectBoxL->addWidget(labelY, 0, 2);

                QDoubleSpinBox *y = new QDoubleSpinBox(rectBox);
                y->setToolTip(tooltip);
                y->setMinimum(valueMin.y());
                y->setMaximum(valueMax.y());
                y->setDecimals(decimals);
                y->setValue(value.y());
                rectBoxL->addWidget(y, 0, 3);

                QLabel *labelW = new QLabel(target);
                labelW->setText(tr("W", "Width, shortly"));
                rectBoxL->addWidget(labelW, 1, 0);

                QDoubleSpinBox *w = new QDoubleSpinBox(rectBox);
                w->setToolTip(tooltip);
                w->setMinimum(valueMin.width());
                w->setMaximum(valueMax.width());
                w->setDecimals(decimals);
                w->setValue(value.width());
                rectBoxL->addWidget(w, 1, 1);

                QLabel *labelH = new QLabel(target);
                labelH->setText(tr("H", "Height, shortly"));
                rectBoxL->addWidget(labelH, 1, 2);

                QDoubleSpinBox *h = new QDoubleSpinBox(rectBox);
                h->setToolTip(tooltip);
                h->setMinimum(valueMin.height());
                h->setMaximum(valueMax.height());
                h->setDecimals(decimals);
                h->setValue(value.height());
                rectBoxL->addWidget(h, 1, 3);

                QObject::connect(x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, y, w, h, this](double val)
                {
                    QRectF v(val, y->value(), w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, x, w, h, this](double val)
                {
                    QRectF v(x->value(), val, w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(w, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, x, y, h, this](double val)
                {
                    QRectF v(x->value(), y->value(), val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                [id, x, y, w, this](double val)
                {
                    QRectF v(x->value(), y->value(), w->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed height:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }
            else
            {
                QRect valueDefault = QRect(defArr["x"].toInt(), defArr["y"].toInt(),
                        defArr["w"].toInt(), defArr["h"].toInt());
                QRect valueMin     = QRect(defMin["x"].toInt(INT_MIN), defMin["y"].toInt(INT_MIN),
                        defMin["w"].toInt(INT_MIN), defMin["h"].toInt(INT_MIN));
                QRect valueMax     = QRect(defMax["x"].toInt(INT_MAX), defMax["y"].toInt(INT_MAX),
                        defMax["w"].toInt(INT_MAX), defMax["h"].toInt(INT_MAX));
                QRect value = retrieve_property(setupTree, name, valueDefault).toRect();

                QLabel *labelX = new QLabel(target);
                labelX->setText("X");
                rectBoxL->addWidget(labelX, 0, 0);

                QSpinBox *x = new QSpinBox(rectBox);
                x->setToolTip(tooltip);
                x->setMinimum(valueMin.x());
                x->setMaximum(valueMax.x());
                x->setValue(value.x());
                rectBoxL->addWidget(x, 0, 1);

                QLabel *labelY = new QLabel(target);
                labelY->setText("Y");
                rectBoxL->addWidget(labelY, 0, 2);

                QSpinBox *y = new QSpinBox(rectBox);
                y->setToolTip(tooltip);
                y->setMinimum(valueMin.y());
                y->setMaximum(valueMax.y());
                y->setValue(value.y());
                rectBoxL->addWidget(y, 0, 3);

                QLabel *labelW = new QLabel(target);
                labelW->setText(tr("W", "Width, shortly"));
                rectBoxL->addWidget(labelW, 1, 0);

                QSpinBox *w = new QSpinBox(rectBox);
                w->setToolTip(tooltip);
                w->setMinimum(valueMin.width());
                w->setMaximum(valueMax.width());
                w->setValue(value.width());
                rectBoxL->addWidget(w, 1, 1);

                QLabel *labelH = new QLabel(target);
                labelH->setText(tr("H", "Height, shortly"));
                rectBoxL->addWidget(labelH, 1, 2);

                QSpinBox *h = new QSpinBox(rectBox);
                h->setToolTip(tooltip);
                h->setMinimum(valueMin.height());
                h->setMaximum(valueMax.height());
                h->setValue(value.height());
                rectBoxL->addWidget(h, 1, 3);

                QObject::connect(x, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, y, w, h, this](int val)
                {
                    QRect v(val, y->value(), w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, x, w, h, this](int val)
                {
                    QRect v(x->value(), val, w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(w, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, x, y, h, this](int val)
                {
                    QRect v(x->value(), y->value(), val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [id, x, y, w, this](int val)
                {
                    QRect v(x->value(), y->value(), w->value(), val);
#ifdef DEBUG_BUILD
                    qDebug() << "changed height:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });
            }

            rectBoxL->setColumnStretch(0, 0);
            rectBoxL->setColumnStretch(1, 1000);
            rectBoxL->setColumnStretch(2, 0);
            rectBoxL->setColumnStretch(3, 1000);

            l->addWidget(rectBox, row, 0, 1, 2);
            row++;
        }

        // Children group
        else if(!control.compare("group", Qt::CaseInsensitive))
        {
            QJsonArray children = o["children"].toArray();
            if(!children.isEmpty())
            {
                bool noBranch = (name == "..");
                if(noBranch && title == name)
                    title.clear();

                QGroupBox *subGroup = new QGroupBox(target);
                subGroup->setTitle(title);

                if(!noBranch)
                    setupTree.m_setupTree.push(name);

                bool collapsible = o["collapse"].toBool(false);

                if (collapsible) {
                    QPushButton *collapseButton = new QPushButton(target);
                    if (o["default-collapsed"].toBool(false)) {
                        subGroup->setVisible(false);
                        collapseButton->setText("Expand " + title);
                    } else {
                        collapseButton->setText("Collapse " + title);
                    }
                    l->addWidget(collapseButton, row, 0, 1, 2);
                    row++;

                    QObject::connect(collapseButton, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                     [subGroup, collapseButton](bool)
                     {
                        subGroup->setVisible(!subGroup->isVisible());
                        collapseButton->setText((subGroup->isVisible() ? "Collapse " : "Expand ") + subGroup->title());
                     });
                }

                l->addWidget(subGroup, row, 0, 1, 2);

                loadLayoutEntries(setupTree, children, subGroup, err, parent);

                if(!noBranch)
                    setupTree.m_setupTree.pop();
                row++;
            }
        }
        // List group
        else if(!control.compare("listGroup", Qt::CaseInsensitive))
        {
            QJsonArray children = o["children"].toArray();
            if(!children.isEmpty())
            {
                // Pre-fetch the entries that are currently in the list, so they can be initialized at the bottom
                QJsonObject entries = retrieve_property(setupTree, name, QVariant()).toJsonObject();

                // Initialize the item tree
                QGroupBox *subGroup = new QGroupBox(target);
                subGroup->setTitle(title);
                QLayout* subGroupLayout = new QVBoxLayout(subGroup);
                subGroupLayout->setSizeConstraint(QBoxLayout::SizeConstraint::SetMaximumSize);

                QGroupBox* itemGroup = new QGroupBox();
                itemGroup->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
                new QVBoxLayout(itemGroup);

                QPushButton *addButton = new QPushButton(subGroup);
                addButton->setText("+");
                addButton->setToolTip("Add new item");
                addButton->setObjectName("Add item");

                bool collapsible = o["collapse"].toBool(false);

                if (collapsible) {
                    QPushButton *collapseButton = new QPushButton(target);
                    if (o["default-collapsed"].toBool(false)) {
                        itemGroup->setVisible(false);
                        collapseButton->setText("Expand list");
                    } else {
                        collapseButton->setText("Collapse list");
                    }
                    subGroupLayout->addWidget(collapseButton);
                    row++;

                    QObject::connect(collapseButton, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                                     [itemGroup, collapseButton](bool)
                                     {
                                         itemGroup->setVisible(!itemGroup->isVisible());
                                         collapseButton->setText((itemGroup->isVisible() ? "Collapse list" : "Expand list"));
                                     });
                }


                // Go one layer deeper
                const QString id = setupTree.getPropertyId(name);
                setupTree.m_setupTree.push(name);

                // Create the settings group object to organize data
                JsonListSettingsGroup settingsGroup = JsonListSettingsGroup();
                settingsGroup.groupBox = itemGroup;
                settingsGroup.children = children;
                settingsGroup.maxSize = o["max-items"].toInt(0);
                settingsGroup.addButton = addButton;
                m_instantiatedQJsonLists.insert(id, settingsGroup);

                // Keep track of the number of items
                m_setupStack.setValue(setupTree.getPropertyId("count"), 0);

                // Connect the button
                QObject::connect(addButton, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                [setupTree, id, this](bool)
                {
                    AddListElement(setupTree, id);
                    emit settingsChanged();
                });

                // Connect the widgets
                subGroupLayout->addWidget(addButton);
                subGroupLayout->addWidget(itemGroup);
                l->addWidget(subGroup, row, 0, 1, 2);

                // Add pre-existing elements
                int maxNum = retrieve_property(setupTree, "count", 0).toInt();
                for (int i = 0; i < maxNum; i++) {
                    AddListElement(setupTree, id);
                }

                // Emit if pre-existing elements exist
                if (maxNum>0) {
                    emit settingsChanged();
                }

                // Done
                setupTree.m_setupTree.pop();
                row++;
            }
        }
    }
}

void JsonSettingsWidget::AddListElement(SetupStack setupTree, QString id) {
    // Initialize values
    auto settingsGroup = m_instantiatedQJsonLists[id];
    int idx = settingsGroup.size;
    settingsGroup.size++;
    const QString id2 = setupTree.getPropertyId(QString::number(idx));
    m_setupStack.setValue(setupTree.getPropertyId("count"), settingsGroup.size);

    if (settingsGroup.maxSize > 0 && settingsGroup.size == settingsGroup.maxSize) {
        settingsGroup.addButton->setEnabled(false);
    }

    // Build the new layout tree
    QFrame* itemSubGroup = new QFrame();
    QLayout* itemSubGroupLayout = new QHBoxLayout(itemSubGroup);
    settingsGroup.groupBox->layout()->addWidget(itemSubGroup);

    QFrame* childSettings = new QFrame();
    new QGridLayout(childSettings);
    childSettings->setObjectName(id2);

    QPushButton *remButton = new QPushButton(itemSubGroup);
    remButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    remButton->setFixedWidth(24);
    remButton->setText("-");
    remButton->setToolTip("Remove item");

    // Update settingsGroup
    m_instantiatedQJsonLists[id] = settingsGroup;

    // Bind the button event
    QObject::connect(remButton, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
    [setupTree, itemSubGroup, id, this](bool)
    {
        RemoveListElement(setupTree, id, itemSubGroup);
    });

    // Link the tree items
    itemSubGroupLayout->addWidget(remButton);
    itemSubGroupLayout->addWidget(childSettings);

    // Go down one layer, build the values
    setupTree.m_setupTree.push(QString::number(idx));
    loadLayoutEntries(setupTree, settingsGroup.children, childSettings, settingsGroup.err, settingsGroup.parent);
    setupTree.m_setupTree.pop();
}

void JsonSettingsWidget::RemoveListElement(SetupStack setupTree, QString id, QFrame* itemSubGroup) {
    // Initialize values
    auto settingsGroup = m_instantiatedQJsonLists[id];
    auto items = settingsGroup.groupBox->children();
    int num = items.indexOf(itemSubGroup) - 1;
    m_instantiatedQJsonLists[id].size--;
    int count = items.count() - 1;

    if (settingsGroup.maxSize > 0 && m_instantiatedQJsonLists[id].size < settingsGroup.maxSize) {
        settingsGroup.addButton->setEnabled(true);
    }

    // Remove the data
    m_setupStack.removeElement(setupTree.getPropertyId(QString::number(num)));
    setupTree.m_setupCache = m_setupStack.m_setupCache;
    m_setupStack.setValue(setupTree.getPropertyId("count"), m_instantiatedQJsonLists[id].size);

    // Remove the last widget
    QWidget* last = m_instantiatedQJsonLists[id].groupBox->findChild<QWidget*>(setupTree.getPropertyId(QString::number(count-1)), Qt::FindChildrenRecursively);
    m_instantiatedQJsonLists[id].groupBox->layout()->removeWidget(last->parentWidget());
    last->parentWidget()->deleteLater();

    // Shift the data one over
    for (int i = num; i < count - 1; i++) {
        QWidget* wid = m_instantiatedQJsonLists[id].groupBox->findChild<QWidget*>(setupTree.getPropertyId(QString::number(i)), Qt::FindChildrenRecursively);

        setupTree.m_setupTree.push(QString::number(i));
        loadLayoutEntries(setupTree, settingsGroup.children, wid, settingsGroup.err, settingsGroup.parent);
        setupTree.m_setupTree.pop();
    }

    emit settingsChanged();
}

QWidget *JsonSettingsWidget::loadLayoutDetail(JsonSettingsWidget::SetupStack &stack,
                                                                const QByteArray &layoutJson,
                                                                QString &err)
{
    QWidget *widget = nullptr;
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
    {
        QGroupBox *gui_w = new QGroupBox(qobject_cast<QWidget*>(parent()));
        gui_w->setTitle(title);
        widget = gui_w;
        m_spacerNeeded = true;
    }
    else
    {
        widget = new QFrame(qobject_cast<QWidget*>(parent()));
        m_spacerNeeded = true;
    }

    QJsonArray layoutEntries = layoutData["layout"].toArray();
    loadLayoutEntries(stack, layoutEntries, widget, err, qobject_cast<QWidget*>(parent()));

    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    m_browser = widget;

    return widget;
}
