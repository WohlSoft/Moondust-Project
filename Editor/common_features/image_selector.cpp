#include "image_selector.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFocusEvent>
#include <QtDebug>
#include <QGridLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPixmap>
#include <QImage>
#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>


#define ISM_ENTRY_WIDTH 100
#define ISM_ENTRY_HEIGHT 100

ImageSelectorMenu::ImageSelectorMenu(QWidget *parent):
    QDialog(parent)
{
    setWindowFlag(Qt::FramelessWindowHint, true);
    setWindowFlag(Qt::Dialog, true);
//    setAttribute(Qt::WA_AlwaysStackOnTop, true);
    resize(640, 480);
    setSizeGripEnabled(true);
//    setFocusProxy(parent);
    setFocusPolicy(Qt::StrongFocus);
    m_layout = new QGridLayout(this);

    this->setLayout(m_layout);

    // TODO: Make it later, not important
//    m_filter = new QLineEdit(this);
//    m_filter->setPlaceholderText("Filter...");
//    m_filter->setClearButtonEnabled(true);

//    m_layout->addWidget(m_filter, 0, 0);

    m_list = new QListWidget(this);
    m_list->setViewMode(QListWidget::IconMode);
    m_list->setIconSize(QSize(ISM_ENTRY_WIDTH, ISM_ENTRY_HEIGHT));
    m_list->setSpacing(5);
    m_list->setUniformItemSizes(true);
    m_list->setTextElideMode(Qt::ElideRight);
    m_list->setMovement(QListView::Snap);
    m_list->setResizeMode(QListView::Adjust);

    QObject::connect(m_list, &QListWidget::itemClicked,
                     this,
                     &ImageSelectorMenu::itemClicked);

    QObject::connect(this, &QDialog::accept,
                     this,
                     &ImageSelectorMenu::doAccept);

    QObject::connect(this, &QDialog::reject,
                     this,
                     &ImageSelectorMenu::doReject);

//    m_layout->addWidget(m_list, 1, 0);
    m_layout->addWidget(m_list, 0, 0);
//    m_layout->setRowStretch(0, 0);
//    m_layout->setRowStretch(1, 10000);
}

ImageSelectorMenu::~ImageSelectorMenu()
{}

void ImageSelectorMenu::clear()
{
    m_items.clear();
    m_list->clear();
    m_currentItem = -1;
}

void ImageSelectorMenu::addItem(const QPixmap &picture, const QString &title, int key)
{
    QListWidgetItem *q = new QListWidgetItem(m_list);
    q->setIcon(picture);
    q->setText(title);
    q->setToolTip(QString("%1: %2").arg(key).arg(title));
    q->setData(Qt::UserRole, key);
    q->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    m_list->addItem(q);
    m_items.insert(key, q);
}

void ImageSelectorMenu::updateItem(int key, const QPixmap &picture, const QString &title)
{
    if(!m_items.contains(key))
        return;
    QListWidgetItem *q = m_items[key];
    q->setIcon(picture);
    q->setText(title);
}

void ImageSelectorMenu::getItem(int key, QPixmap &picture, QString &title)
{
    if(!m_items.contains(key))
    {
        picture = QPixmap(ISM_ENTRY_WIDTH, ISM_ENTRY_HEIGHT);
        picture.fill(Qt::gray);
        title = "<INVALID>";
        return;
    }

    QListWidgetItem *q = m_items[key];
    picture = q->icon().pixmap(ISM_ENTRY_WIDTH, ISM_ENTRY_HEIGHT);
    title = q->text();
}

void ImageSelectorMenu::setItem(int key)
{
    if(m_items.contains(key))
        m_currentItem = key;
    else
        m_currentItem = -1;
}

int ImageSelectorMenu::currentItem()
{
    return m_currentItem;
}

void ImageSelectorMenu::autoSelect()
{
    if(!m_items.contains(m_currentItem))
        return;
    auto *q = m_items[m_currentItem];
    m_list->clearSelection();
    q->setSelected(true);
    m_list->scrollToItem(q);
}

void ImageSelectorMenu::itemClicked(QListWidgetItem *item)
{
    if(!item)
        return;
    m_currentItem = item->data(Qt::UserRole).toInt();
    emit currentItemChanged(m_currentItem);
    hide();
}

void ImageSelectorMenu::doAccept()
{
    auto q = m_list->selectedItems();
    if(q.isEmpty())
        return;
    auto *i = q.first();
    m_currentItem = i->data(Qt::UserRole).toInt();
    emit currentItemChanged(m_currentItem);
    hide();
}

void ImageSelectorMenu::doReject()
{
    hide();
}

void ImageSelectorMenu::focusOutEvent(QFocusEvent * /*event*/)
{
//    if(event->reason() == Qt::ActiveWindowFocusReason)
//        this->hide();
//    qDebug() << "Focus out reason: " << event->reason();
}

static void alignWindow(ImageSelectorMenu *box)
{
    auto wg = box->geometry();
    auto screens = QApplication::screens();
    auto globalPos = QCursor::pos();
    for(auto s : screens)
    {
        auto sg = s->geometry();
        if(sg.contains(globalPos) || screens.size() == 1)
        {
//            qDebug() << s->name() << "contains window";
            if(!sg.contains(wg))
            {
//                qDebug() << "not intersects, align it!";
                auto si = sg.intersected(wg);
                int moveX = 0, moveY = 0;
                if(wg.right() > sg.right())
                    moveX = -(wg.width() - si.width());
                if(wg.bottom() > sg.bottom())
                    moveY = -(wg.height() - si.height());

                if(wg.left() < sg.left())
                    moveX = (wg.width() - si.width());
                if(wg.top() < sg.top())
                    moveY = (wg.height() - si.height());

                box->move(wg.x() + moveX, wg.y() + moveY);
            }
//            else
//                qDebug() << "intersects!";
        }
    }
}



void ImageSelector::updateView()
{
    QPixmap x; QString t, ft;
    int i = m_menuBox->currentItem();
    m_menuBox->getItem(i, x, ft);
    m_button->setIconSize(x.size());
    m_button->setIcon(QIcon(x));

    t = ft;

    if(t.size() > 15)
    {
        t.remove(13, t.size() - 13);
        t.append("...");
    }

    m_button->setText(t);
    m_button->setToolTip(QString("%1: %2").arg(i).arg(ft));
}

void ImageSelector::itemChangeProcess(int key)
{
    updateView();
    emit currentItemChanged(key);
}

ImageSelector::ImageSelector(QWidget *parent) :
    QWidget(parent)
{
    m_button = new QPushButton(this);
    m_layout = new QVBoxLayout(this);
    m_button->setStyleSheet("text-align: left; padding: 4pt;");

    setMinimumHeight(ISM_ENTRY_HEIGHT + 4);

    QPixmap p = QPixmap(ISM_ENTRY_WIDTH, ISM_ENTRY_HEIGHT);
    p.fill(Qt::black);
    m_button->setIconSize(p.size());
    m_button->setIcon(QIcon(p));

    this->setLayout(m_layout);
    m_layout->addWidget(m_button);
    m_button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    m_layout->setMargin(0);
    m_menuBox = new ImageSelectorMenu(this);
    QObject::connect(m_button,
    static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
    [this](bool)
    {
        QPoint globalCursorPos = m_button->mapToGlobal(m_button->pos());
        globalCursorPos.setY(globalCursorPos.y() + m_button->height());
        m_menuBox->move(globalCursorPos);
        m_menuBox->autoSelect();
        m_menuBox->show();
        alignWindow(m_menuBox);
        m_menuBox->raise();
    });

    QObject::connect(m_menuBox, &ImageSelectorMenu::currentItemChanged,
                     this, &ImageSelector::itemChangeProcess);

    updateView();
}

ImageSelector::~ImageSelector()
{}

void ImageSelector::clear()
{
    m_menuBox->clear();
    updateView();
}

void ImageSelector::addItem(const QPixmap &picture, const QString &title, int key)
{
    m_menuBox->addItem(picture, title, key);
    updateView();
}

void ImageSelector::updateItem(int key, const QPixmap &picture, const QString &title)
{
    m_menuBox->updateItem(key, picture, title);
}

void ImageSelector::setItem(int key)
{
    m_menuBox->setItem(key);
    updateView();
}

int ImageSelector::currentItem()
{
    return m_menuBox->currentItem();
}

