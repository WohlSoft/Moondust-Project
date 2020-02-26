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



ImageSelectorMenu::ImageSelectorMenu(QWidget *parent):
    QDialog(parent)
{
    setWindowFlag(Qt::FramelessWindowHint, true);
    setWindowFlag(Qt::Dialog, true);
    setAttribute(Qt::WA_AlwaysStackOnTop, true);
    resize(640, 480);
    setSizeGripEnabled(true);
//    setFocusProxy(parent);
    setFocusPolicy(Qt::ClickFocus);
    m_layout = new QGridLayout(this);

    this->setLayout(m_layout);
    m_filter = new QLineEdit(this);
    m_filter->setPlaceholderText("Filter...");
    m_filter->setClearButtonEnabled(true);

    m_layout->addWidget(m_filter, 0, 0);
    m_list = new QListWidget(this);
    m_list->setViewMode(QListWidget::IconMode);

    m_layout->addWidget(m_list, 1, 0);
    m_layout->setRowStretch(0, 0);
    m_layout->setRowStretch(1, 10000);
}

ImageSelectorMenu::~ImageSelectorMenu()
{}

void ImageSelectorMenu::focusOutEvent(QFocusEvent *event)
{
//    if(event->lostFocus())
//        this->hide();
//    qDebug() << "Focus out reason: " << event->reason();
}



ImageSelector::ImageSelector(QWidget *parent) :
    QWidget(parent)
{
    m_button = new QPushButton(this);
    m_layout = new QVBoxLayout(this);
    m_button->setText("0: <some title>");
    m_button->setStyleSheet("text-align: left; padding: 4pt;");

    QPixmap p = QPixmap(100, 80);
    p.fill(Qt::black);
    m_button->setIconSize(p.size());
    m_button->setIcon(QIcon(p));

    this->setLayout(m_layout);
    m_layout->addWidget(m_button);
    m_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout->setMargin(0);
    m_menuBox = new ImageSelectorMenu(this);
    QObject::connect(m_button,
    static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
    [this](bool)
    {
        QPoint globalCursorPos = m_button->mapToGlobal(m_button->pos());
        globalCursorPos.setY(globalCursorPos.y() + m_button->height());
        m_menuBox->move(globalCursorPos);
        m_menuBox->show();
        m_menuBox->setFocus(Qt::MouseFocusReason);
    });
}

ImageSelector::~ImageSelector()
{}

