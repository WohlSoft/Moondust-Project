#include "testbox.h"
#include "ui_testbox.h"

void setupWidget(DirectionSwitchWidget *m_testWidget)
{
    m_testWidget->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/box.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/black_left.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/black_right.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/black_up.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/black_down.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_TOP_LEFT, QPixmap(":/arrows/black_top_left.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_TOP_RIGHT, QPixmap(":/arrows/black_top_right.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_BOTTOM_LEFT, QPixmap(":/arrows/black_bottom_left.png"));
    m_testWidget->setPixmap(DirectionSwitchWidget::S_BOTTOM_RIGHT, QPixmap(":/arrows/black_bottom_right.png"));
    m_testWidget->setPixmapOn(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/red_left.png"));
    m_testWidget->setPixmapOn(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/red_right.png"));
    m_testWidget->setPixmapOn(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/red_up.png"));
    m_testWidget->setPixmapOn(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/red_down.png"));
    m_testWidget->mapValue(DirectionSwitchWidget::S_LEFT, -1);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_LEFT, "Лево");
    m_testWidget->mapValue(DirectionSwitchWidget::S_RIGHT, +1);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_RIGHT, "Право");
    m_testWidget->mapValue(DirectionSwitchWidget::S_TOP, -2);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_TOP, "Верх");
    m_testWidget->mapValue(DirectionSwitchWidget::S_BOTTOM, +2);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_BOTTOM, "Низ");
    m_testWidget->mapValue(DirectionSwitchWidget::S_TOP_LEFT, -3);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_TOP_LEFT, "Верх-Лево");
    m_testWidget->mapValue(DirectionSwitchWidget::S_TOP_RIGHT, +3);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_TOP_RIGHT, "Верх-Право");
    m_testWidget->mapValue(DirectionSwitchWidget::S_BOTTOM_LEFT, -4);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_BOTTOM_LEFT, "Низ-Лево");
    m_testWidget->mapValue(DirectionSwitchWidget::S_BOTTOM_RIGHT, +4);
    m_testWidget->mapToolTip(DirectionSwitchWidget::S_BOTTOM_RIGHT, "Низ-Право");
}


TestBox::TestBox(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TestBox)
{
    ui->setupUi(this);
    setupWidget(ui->test2);

    m_testWidget = new DirectionSwitchWidget(this);
    m_testWidget->setGeometry(QRect(40, 40, 60, 60));
    setupWidget(m_testWidget);

    QObject::connect(m_testWidget, SIGNAL(directionChanged(int)), this, SLOT(directionValue(int)));
    m_testWidget->setDirection(-1);
}

TestBox::~TestBox()
{
    delete m_testWidget;
    delete ui;
}

void TestBox::directionValue(int direct)
{
    ui->value->setText(QString("Clicked value: %1").arg(direct));
}

void TestBox::on_diagonals_clicked(bool checked)
{
    m_testWidget->setUseDiagonals(checked);
}

void TestBox::on_roteCenter_clicked(bool checked)
{
    if(checked)
        m_testWidget->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/compas.png"));
    else
        m_testWidget->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/box.png"));
    m_testWidget->setRotatingCenter(checked);
}

void TestBox::on_disable_clicked(bool checked)
{
    m_testWidget->setDisabled(checked);
}
