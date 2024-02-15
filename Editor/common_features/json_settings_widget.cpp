#include "json_settings_widget.h"

#include <QFile>

#include <QJsonParseError>
#include <QFileIconProvider>
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
#include <QToolButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>
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
#include <audio/music_player.h>
#include <audio/sdl_music_player.h>
#include <common_features/main_window_ptr.h>
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
    // QBrush bl = QBrush(Qt::black);
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


QJsonObject JsonSettingsWidget::SetupStack::rectToArray(const QVariant &r)
{
    QRect rekt = r.toRect();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::rectfToArray(const QVariant& r)
{
    QRectF rekt = r.toRectF();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::sizeToArray(const QVariant& r)
{
    QSize rekt = r.toSize();
    QJsonObject a;
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::sizefToArray(const QVariant& r)
{
    QSizeF rekt = r.toSizeF();
    QJsonObject a;
    a["w"] = QJsonValue::fromVariant(rekt.width());
    a["h"] = QJsonValue::fromVariant(rekt.height());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::pointToArray(const QVariant& r)
{
    QPoint rekt = r.toPoint();
    QJsonObject a;
    a["x"] = QJsonValue::fromVariant(rekt.x());
    a["y"] = QJsonValue::fromVariant(rekt.y());
    return a;
}

QJsonObject JsonSettingsWidget::SetupStack::pointfToArray(const QVariant& r)
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
    for(const QString& t : m_setupTree)
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

void JsonSettingsWidget::SetupStack::setValue(const QString &propertyId, const QVariant& value)
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

void JsonSettingsWidget::SetupStack::setValue(const QString& propertyId, const QJsonArray& value)
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

    o[top] = value;

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
    valTrInitLang();
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

QJsonArray JsonSettingsWidget::retrieve_property(const SetupStack& setupTree, QString prop, const QJsonArray& defaultValue)
{
    const QJsonDocument d = setupTree.m_setupCache;
    QJsonObject o = d.object();
    QJsonArray out;
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
        out = o[t].toArray();
    }

    if(!o.contains(prop))
    {
#ifdef DEBUG_BUILD
        qDebug() << outPr << prop << defaultValue << "DEFAULT-PROP";
#endif
        return defaultValue;
    }

    out = o[prop].toArray();

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
        newRgba.setRed(color.midRef(0, 2).toInt(nullptr, 16));
        newRgba.setGreen(color.midRef(2, 2).toInt(nullptr, 16));
        newRgba.setBlue(color.midRef(4, 2).toInt(nullptr, 16));
    }

    if(color.size() == 8)
        newRgba.setAlpha(color.midRef(6, 2).toInt(nullptr, 16));
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

static void setListWidgetValue(QListWidget* widget,
                               const QJsonArray &value,
                               Qt::ItemFlag extraFlags,
                               bool hasIcon, const QString &root)
{
    widget->clear();
    if(value.isEmpty())
        return;

    QFileIconProvider prov;

    for(const QJsonValue& item : value)
    {
        if(!item.isString())
            return;

        QListWidgetItem *it = new QListWidgetItem(widget);
        QString line = item.toString();
        it->setText(line);
        if(hasIcon)
            it->setIcon(prov.icon(QFileInfo(root + line)));
        it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | extraFlags);
        widget->addItem(it);
    }
}

static QJsonArray getListWidgetValue(QListWidget* widget)
{
    QJsonArray ret;
    int count = widget->count();

    for(int i = 0; i < count; ++i)
    {
        auto *it = widget->item(i);
        ret.append(it->text());
    }

    return ret;
}


QString JsonSettingsWidget::browseForFileValue(QWidget* target, LineEditType type,
                                               const QString& root, const QString& filePath,
                                               const QStringList& filters,
                                               const QString& dialogueTitle, const QString& dialogueDescription,
                                               bool *ok)
{
    QString ret;

    if(ok)
        *ok = false;

    if(type == JSS_LineEdit_File)
    {
        FileListBrowser file(root, filePath, target);
        if(!dialogueTitle.isEmpty())
            file.setWindowTitle(dialogueTitle);

        if(!dialogueDescription.isEmpty())
            file.setDescription(dialogueDescription);

        if(!filters.isEmpty())
            file.setFilters(filters);

        file.startListBuilder();

        if(file.exec() == QDialog::Accepted)
        {
            ret = file.currentFile();
            if(ok)
                *ok = true;
        }
    }
    else if(type == JSS_LineEdit_Level)
    {
        LevelFileList levels(root, filePath, target);
        if(!dialogueTitle.isEmpty())
            levels.setWindowTitle(dialogueTitle);

        if(!dialogueDescription.isEmpty())
            levels.setDescription(dialogueDescription);

        if(levels.exec() == QDialog::Accepted)
        {
            ret = levels.currentFile();
            if(ok)
                *ok = true;
        }
    }
    else
    {
        QString audioPath = filePath;
        MusicFileList muz(root, audioPath, target, (type == JSS_LineEdit_SFX));

        if(!dialogueTitle.isEmpty())
            muz.setWindowTitle(dialogueTitle);

        if(!dialogueDescription.isEmpty())
            muz.setDescription(dialogueDescription);

#ifndef UNIT_TEST
        if(type != JSS_LineEdit_SFX)
            muz.setMusicPlayState(MainWinConnect::pMainWin->getPlayMusicAction()->isChecked());

        QObject::connect(&muz, &MusicFileList::musicFileChanged, [&audioPath](const QString &music)->void
        {
            audioPath = music;
        });

        QObject::connect(&muz, &MusicFileList::updateSongPlay, [this, &root, &audioPath]()->void
        {
            if(MainWinConnect::pMainWin->getPlayMusicAction()->isChecked())
                LvlMusPlay::previewCustomMusicAbs(root + audioPath);
        });

        QObject::connect(&muz, &MusicFileList::musicTempoChanged, [](double tempo)->void
        {
            LvlMusPlay::setTempo(tempo);
        });

        QObject::connect(&muz, &MusicFileList::playSoundFile, [this, &audioPath](const QString &file)->void
        {
            PGE_SfxPlayer::playFile(file);
        });

        QObject::connect(&muz, &MusicFileList::musicButtonClicked, [this, &root, &audioPath](bool st)->void
        {
            MainWinConnect::pMainWin->getPlayMusicAction()->setChecked(st);
            if(st)
                LvlMusPlay::previewCustomMusicAbs(root + audioPath);
            else
                LvlMusPlay::previewSilence();
        });
#endif

        if(muz.exec() == QDialog::Accepted)
        {
            ret = muz.currentFile();
            if(ok)
                *ok = true;
        }

#ifndef UNIT_TEST
        if(type == JSS_LineEdit_SFX)
            PGE_SfxPlayer::freeBuffer();
        else
            LvlMusPlay::previewReset(MainWinConnect::pMainWin);
#endif
    }

    return ret;
}


void JsonSettingsWidget::loadLayoutEntries(JsonSettingsWidget::SetupStack setupTree,
                                           const QJsonArray &elements,
                                           QWidget *target,
                                           QString &err,
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
        QString title = valTr(o, "title").toString(name);
        QString control = o["control"].toString();
        QString tooltip = valTr(o, "tooltip").toString();

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
                QObject::connect(it, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [id, this](int val)
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
                QObject::connect(it, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
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
            QObject::connect(it, static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked), this,
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

            QObject::connect(colorChoose, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked), this,
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

            QObject::connect(colorString, &QLineEdit::editingFinished, this,
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
            QString placeholder = valTr(o, "placeholder").toString();
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
            QObject::connect(it, static_cast<void(QLineEdit::*)(const QString &)>(&QLineEdit::textChanged), this,
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
            QObject::connect(it, &QPlainTextEdit::textChanged, this,
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
            QString placeholder = valTr(o, "placeholder").toString();
            QString dialogTitle = valTr(o, "dialog-title").toString();
            QString dialogDescription = valTr(o, "dialog-description").toString();

            QStringList filters;
            QVariantList filtersList = o["filters"].toArray().toVariantList();
            for(QVariant &j : filtersList)
                filters.push_back(j.toString());

            LineEditType lineType = JSS_LineEdit_Text;

            if(!control.compare("musicFile", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_Music;
            else if(!control.compare("soundFile", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_SFX;
            else if(!control.compare("levelFile", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_Level;
            else if(!control.compare("file", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_File;

            bool lookAtEpisode = true;

            if(o.keys().contains("directory"))
                lookAtEpisode = !o["directory"].toString("episode").compare("episode", Qt::CaseInsensitive);

            QString root = lookAtEpisode ? m_directoryEpisode : m_directoryData;
            if(!root.endsWith('/'))
                root.append('/');

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

            if(lineType == JSS_LineEdit_Music || lineType == JSS_LineEdit_SFX)
            {
                QPushButton *preview = new QPushButton(fileBox);
                preview->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
                preview->setFixedWidth(24);
                preview->setText("▶️");
                preview->setToolTip(tr("Play"));
                fileBoxL->addWidget(preview, 0);

                QObject::connect(preview, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked), this,
                [id, it, lineType, root, lookAtEpisode, this](bool)
                {
                    QString file = it->text();
                    QString fileNA = file.split('|').first();

                    if(!file.isEmpty() && QFile::exists(root + fileNA))
                    {
                        qDebug() << "Trying to play music" << root + file;
#ifndef UNIT_TEST
                        if(lineType == JSS_LineEdit_SFX)
                            PGE_SfxPlayer::playFile(root + file);
                        else
                            LvlMusPlay::previewCustomMusicAbs(root + file);
#endif
                    }
                });
            }

            l->addWidget(fileBox, row, 1);

            QObject::connect(browse, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked), this,
            [id, it, lineType, target, root, dialogTitle, dialogDescription, filters, this](bool)
            {
                bool ok;
                QString newFile = browseForFileValue(target, lineType, root, it->text(),
                                                     filters,
                                                     dialogTitle, dialogDescription, &ok);

                if(ok)
                {
                    it->setText(newFile);
                    m_setupStack.setValue(id, newFile);
                    emit settingsChanged();
                }
            });

            QObject::connect(it, static_cast<void(QLineEdit::*)(const QString &)>(&QLineEdit::textChanged), this,
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

        // Array of strings
        else if(
            !control.compare("lineArray", Qt::CaseInsensitive) ||
            !control.compare("musicFileArray", Qt::CaseInsensitive) ||
            !control.compare("soundFileArray", Qt::CaseInsensitive) ||
            !control.compare("levelFileArray", Qt::CaseInsensitive) ||
            !control.compare("fileArray", Qt::CaseInsensitive)
            )
        {
            const QJsonArray arrayValue = retrieve_property(setupTree, name, o["value-default"].toArray());
            QString dialogTitle = valTr(o, "dialog-title").toString();
            QString dialogDescription = valTr(o, "dialog-description").toString();

            QStringList filters;
            QVariantList filtersList = o["filters"].toArray().toVariantList();
            for(QVariant &j : filtersList)
                filters.push_back(j.toString());

            LineEditType lineType = JSS_LineEdit_Text;
            Qt::ItemFlag extraFlags = Qt::NoItemFlags;

            if(!control.compare("musicFileArray", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_Music;
            else if(!control.compare("soundFileArray", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_SFX;
            else if(!control.compare("levelFileArray", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_Level;
            else if(!control.compare("fileArray", Qt::CaseInsensitive))
                lineType = JSS_LineEdit_File;

            bool lookAtEpisode = true;

            if(o.keys().contains("directory"))
                lookAtEpisode = !o["directory"].toString("episode").compare("episode", Qt::CaseInsensitive);

            if(lineType == JSS_LineEdit_Text)
                extraFlags = Qt::ItemIsEditable;

            QString root = lookAtEpisode ? m_directoryEpisode : m_directoryData;
            if(!root.endsWith('/'))
                root.append('/');


            QGroupBox *sizeBox = new QGroupBox(target);
            sizeBox->setTitle(title);
            sizeBox->setToolTip(tooltip);
            QGridLayout *sizeBoxL = new QGridLayout(sizeBox);
            sizeBox->setLayout(sizeBoxL);
            sizeBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

            const QString id = setupTree.getPropertyId(name);
            int col = 0;

            QToolButton *button_add = new QToolButton(sizeBox);
            button_add->setText("+");
            button_add->setToolTip(tr("Add new item into the list"));
            sizeBoxL->addWidget(button_add, 0, col++);

            QToolButton *button_remove = new QToolButton(sizeBox);
            button_remove->setText("-");
            button_remove->setToolTip(tr("Remove selected item from the list"));
            sizeBoxL->addWidget(button_remove, 0, col++);

            sizeBoxL->addItem(new QSpacerItem(20, 20), 0, col++);

            QToolButton *button_preview = nullptr;

            if(lineType == JSS_LineEdit_Music || lineType == JSS_LineEdit_SFX)
            {
                button_preview = new QToolButton(sizeBox);
                button_preview->setText("▶️");
                button_preview->setToolTip(tr("Play selected file"));
                sizeBoxL->addWidget(button_preview, 0, col++);
            }

            QToolButton *button_open = nullptr;

            if(lineType == JSS_LineEdit_Level)
            {
                button_open = new QToolButton(sizeBox);
                button_open->setText(tr("Open..."));
                button_open->setToolTip(tr("Open selected file"));
                sizeBoxL->addWidget(button_open, 0, col++);
            }

            QListWidget *list = new QListWidget(sizeBox);
            sizeBoxL->addWidget(list, 1, 0, 1, col);
            list->setDragEnabled(true);
            list->setDragDropMode(QAbstractItemView::InternalMove);
            list->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

            if(button_preview)
            {
                QObject::connect(button_preview, static_cast<void(QToolButton::*)(bool)>(&QToolButton::clicked), this,
                [id, list, lineType, root, lookAtEpisode, this](bool)
                {
                    auto selected = list->selectedItems();
                    if(selected.isEmpty())
                        return;

                    QString file = selected.first()->text();
                    QString fileNA = file.split('|').first();

                    if(!file.isEmpty() && QFile::exists(root + fileNA))
                    {
                        qDebug() << "Trying to play music" << root + file;
#ifndef UNIT_TEST
                        if(lineType == JSS_LineEdit_SFX)
                            PGE_SfxPlayer::playFile(root + file);
                        else
                            LvlMusPlay::previewCustomMusicAbs(root + file);
#endif
                    }
                });
            }

            if(button_open)
            {
                QObject::connect(button_open, static_cast<void(QToolButton::*)(bool)>(&QToolButton::clicked), this,
                [id, list, root, this](bool)
                {
                    auto selected = list->selectedItems();
                    if(selected.isEmpty())
                        return;

                    QString file = selected.first()->text();
                    emit fileOpenRequested(root + file);
                });
            }

            QObject::connect(button_add, static_cast<void(QToolButton::*)(bool)>(&QToolButton::clicked), this,
            [this, list, id, lineType, root, filters, target, dialogTitle, dialogDescription](bool)->void
            {
                bool ok;
                QString newLine;
                bool hasIcon = false;
                Qt::ItemFlag extraFlags = Qt::NoItemFlags;

                switch(lineType)
                {
                case JSS_LineEdit_Text:
                    newLine = QInputDialog::getText(m_browser,
                                                    tr("Add new line"),
                                                    tr("Please enter a text line to add:"),
                                                    QLineEdit::Normal,
                                                    QString(), &ok);
                    extraFlags = Qt::ItemIsEditable;
                    break;

                case JSS_LineEdit_File:
                case JSS_LineEdit_Music:
                case JSS_LineEdit_SFX:
                case JSS_LineEdit_Level:
                    newLine = browseForFileValue(target, lineType, root, QString(),
                                                 filters,
                                                 dialogTitle, dialogDescription, &ok);
                    hasIcon = true;
                    break;

                default:
                    return;
                }

                if(!ok || newLine.isEmpty())
                    return;

                QListWidgetItem *it = new QListWidgetItem(list);
                it->setText(newLine);
                it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | extraFlags);
                if(hasIcon)
                {
                    QFileIconProvider prov;
                    QFileInfo info(root + newLine);
                    it->setIcon(prov.icon(info));
                }
                list->addItem(it);
                it->setSelected(true);
                list->scrollToItem(it);

                m_setupStack.setValue(id, getListWidgetValue(list));
                emit settingsChanged();
            });

            QObject::connect(button_remove, static_cast<void(QToolButton::*)(bool)>(&QToolButton::clicked), this,
            [this, list, id](bool)->void
            {
                auto selected = list->selectedItems();
                if(selected.isEmpty())
                    return;

                delete selected.first();

                m_setupStack.setValue(id, getListWidgetValue(list));
                emit settingsChanged();
            });

            // Item edited as a string
            QObject::connect(list, &QListWidget::itemChanged, this,
            [this, list, id](QListWidgetItem*)->void
            {
                m_setupStack.setValue(id, getListWidgetValue(list));
                emit settingsChanged();
            });

            // Opening an editor of item
            QObject::connect(list, &QListWidget::itemDoubleClicked, this,
            [this, list, id, lineType, root, filters, target, dialogTitle, dialogDescription](QListWidgetItem* item)->void
            {
                if(lineType == JSS_LineEdit_Text)
                    return;

                bool ok;
                QString newLine = browseForFileValue(target, lineType, root, item->text(),
                                                     filters,
                                                     dialogTitle, dialogDescription, &ok);
                if(!ok)
                    return;

                item->setText(newLine);
            });

            QObject::connect(list->model(), &QAbstractItemModel::rowsMoved, this,
            [this, list, id](QModelIndex, int, int, QModelIndex, int)->void
            {
                m_setupStack.setValue(id, getListWidgetValue(list));
                emit settingsChanged();
            });

            setListWidgetValue(list, arrayValue, extraFlags, lineType != JSS_LineEdit_Text, root);

            l->addWidget(sizeBox, row, 0, 1, 2);
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
            QString text = valTr(o, "text").toString();
            QLabel *label = new QLabel(target);
            label->setText(text);
            label->setToolTip(tooltip);
            label->setWordWrap(true);
            label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
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
            QObject::connect(it, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
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
                QObject::connect(cb, static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked), this,
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

                QObject::connect(w, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                [id, h, this](double val)
                {
                    QSizeF v(val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
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

                QObject::connect(w, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                [id, h, this](int val)
                {
                    QSize v(val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
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

                QObject::connect(x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                [id, y, this](double val)
                {
                    QPointF v(val, y->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
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

                QObject::connect(x, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                [id, y, this](int val)
                {
                    QPoint v(val, y->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
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

                QObject::connect(x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                [id, y, w, h, this](double val)
                {
                    QRectF v(val, y->value(), w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                [id, x, w, h, this](double val)
                {
                    QRectF v(x->value(), val, w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(w, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                [id, x, y, h, this](double val)
                {
                    QRectF v(x->value(), y->value(), val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
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

                QObject::connect(x, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                [id, y, w, h, this](int val)
                {
                    QRect v(val, y->value(), w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed x:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(y, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                [id, x, w, h, this](int val)
                {
                    QRect v(x->value(), val, w->value(), h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed y:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(w, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
                [id, x, y, h, this](int val)
                {
                    QRect v(x->value(), y->value(), val, h->value());
#ifdef DEBUG_BUILD
                    qDebug() << "changed width:" << id << v;
#endif
                    m_setupStack.setValue(id, v);
                    emit settingsChanged();
                });

                QObject::connect(h, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
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

                l->addWidget(subGroup, row, 0, 1, 2);

                loadLayoutEntries(setupTree, children, subGroup, err, parent);

                if(!noBranch)
                    setupTree.m_setupTree.pop();
                row++;
            }
        }
    }
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
    title = valTr(layoutData, "title").toString();
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

void JsonSettingsWidget::valTrInitLang()
{
    QLocale loc = m_browser ? m_browser->locale() : QLocale::system();
    m_langFull = loc.name().toLower().replace('_', '-');
    m_langShort.clear();

    if(m_langFull.contains('-'))
        m_langShort = m_langFull.mid(0, m_langFull.indexOf('-'));
}

QJsonValue JsonSettingsWidget::valTr(const QJsonObject& v, const QString& key)
{
    QString tKey = QString("%1-%2").arg(key).arg(m_langFull);

    if(v.contains(tKey))
        return v.value(tKey);

    else if(!m_langShort.isEmpty())
    {
        tKey = QString("%1-%2").arg(key).arg(m_langShort);
        if(v.contains(tKey))
            return v.value(tKey);
    }

    return v.value(key);
}
