#pragma execution_character_set("utf-8")
#include "titlewidget.h"
#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>
#include "iconhelper.h"
#include <QSpacerItem>
#include <QDesktopWidget>

//#include <qt_windows.h>
//#pragma comment(lib, "user32.lib")




static QString closeBtnHoverStyle = "QPushButton:hover{background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0 #EE0000, stop:1 #BF2F2F);}";

static QString otherBtnHoverStyle = "QPushButton:hover{background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0 #1189D1, stop:1 #2A89C1);}";

static QString default_title_style = "QWidget{"
                                     "color: #F0F0F0;"
                                     "background-color: #183881;"
                                     "border-radius: 0px;}"
                                     "QPushButton{"
                                     "color: #F0F0F0;"
                                     "border-style: none;}";


TitleWidget::TitleWidget(QWidget *parent) : QWidget(parent)
{
    init();
    lab_Ico = new QLabel(this);
    lab_Title = new QLabel(this);
    btnMenu = new QPushButton(this);
    btnMenu_Min = new QPushButton(this);
    btnMenu_Max = new QPushButton(this);
    btnMenu_Close = new QPushButton(this);

    hLay = new QHBoxLayout(this);
    hLay->addWidget(lab_Ico);
    hLay->addSpacing(10);
    hLay->addWidget(lab_Title);
    lab_Ico->setVisible(showLab_Ico);
    lab_Title->setVisible(showLab_Title);
    hLay->addStretch();


    hLay->addWidget(btnMenu);
    IconHelper::Instance()->SetIcon(btnMenu, QChar(0xf0c9), title_height / 2);
    btnMenu->setFlat(true);
    btnMenu->setMinimumSize(title_height + 10,title_height);
    btnMenu->setStyleSheet(otherBtnHoverStyle);
    btnMenu->setVisible(showMenuBtn);

    hLay->addWidget(btnMenu_Min);
    IconHelper::Instance()->SetIcon(btnMenu_Min, QChar(0xf068), title_height / 2);
    btnMenu_Min->setFlat(true);
    btnMenu_Min->setMinimumSize(title_height + 10,title_height);
    btnMenu_Min->setStyleSheet(otherBtnHoverStyle);
    btnMenu_Min->setVisible(showMinBtn);

    hLay->addWidget(btnMenu_Max);
    IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf096), title_height / 2);
    btnMenu_Max->setFlat(true);
    btnMenu_Max->setMinimumSize(title_height + 10,title_height);
    btnMenu_Max->setStyleSheet(otherBtnHoverStyle);
    btnMenu_Max->setVisible(showMaxBtn);

    hLay->addWidget(btnMenu_Close);
    IconHelper::Instance()->SetIcon(btnMenu_Close, QChar(0xf00d), title_height / 2);
    btnMenu_Close->setFlat(true);
    btnMenu_Close->setMinimumSize(title_height + 10,title_height);
    btnMenu_Close->setStyleSheet(closeBtnHoverStyle);
    btnMenu_Close->setVisible(showCloseBtn);


    lab_Title->setText("窗口标题");
    lab_Title->setFont(QFont("SimSun", 10, QFont::Normal));
    lab_Title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lab_Ico->setPixmap(QPixmap(":/img/images/log.ico"));
    lab_Ico->setMaximumSize(title_height-5,title_height-5);
    lab_Ico->setScaledContents(true);

    this->setMinimumSize(this->window()->width(),title_height);
//    this->setMaximumSize(this->window()->width(),title_height);
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);


    connect(btnMenu_Close, &QPushButton::clicked, this, [=](){
        this->window()->close();
    });
    connect(btnMenu_Min, &QPushButton::clicked, this, [=](){
        this->window()->showMinimized();
    });
    connect(btnMenu_Max, &QPushButton::clicked, this, [=](){
        if(max){
            this->window()->showNormal();
            IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf096), title_height / 2);
        }else{
            this->window()->showMaximized();
            IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf079), title_height / 2);
        }
        max = !max;
    });
    this->layout()->setContentsMargins(0,0,0,0);
    this->layout()->setSpacing(0);
    this->setStyleSheet(default_title_style);
    this->installEventFilter(this);
}

void TitleWidget::set_title_height(int height)
{
    title_height = height;
    this->setMinimumSize(this->window()->width(),title_height);
    IconHelper::Instance()->SetIcon(btnMenu, QChar(0xf0c9), title_height / 2);
    btnMenu->setMinimumSize(title_height + 10,title_height);
    IconHelper::Instance()->SetIcon(btnMenu_Min, QChar(0xf068), title_height / 2);
    btnMenu_Min->setMinimumSize(title_height + 10,title_height);
    IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf096), title_height / 2);
    btnMenu_Max->setMinimumSize(title_height + 10,title_height);
    IconHelper::Instance()->SetIcon(btnMenu_Close, QChar(0xf00d), title_height / 2);
    btnMenu_Close->setMinimumSize(title_height + 10,title_height);
    lab_Ico->setMaximumSize(title_height-5,title_height-5);
}

void TitleWidget::setShowMenuBtn(bool flag)
{
    btnMenu->setVisible(flag);
}

void TitleWidget::setShowMinBtn(bool flag)
{
    btnMenu_Min->setVisible(flag);
}

void TitleWidget::setShowMaxBtn(bool flag)
{
    btnMenu_Max->setVisible(flag);
}

void TitleWidget::setShowCloseBtn(bool flag)
{
    btnMenu_Close->setVisible(flag);
}

void TitleWidget::setShowLabIco(bool flag)
{
    lab_Ico->setVisible(flag);
}

void TitleWidget::setShowLabTitle(bool flag)
{
    lab_Title->setVisible(flag);
}

void TitleWidget::setWindowMove(bool flag)
{
    windowIsMove = flag;
}

void TitleWidget::defaultMax()
{

    if(max){
        this->window()->showNormal();
        IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf096), title_height / 2);
    }else{
        this->window()->showMaximized();
        IconHelper::Instance()->SetIcon(btnMenu_Max, QChar(0xf079), title_height / 2);
    }
    max = !max;
}


void TitleWidget::init()
{
    windowIsMove = true;
    mousePressed = false;
    max = false;
    showCloseBtn = true;
    showMinBtn = true;
    showMaxBtn = false;
    showMenuBtn = false;
    showLab_Ico = true;
    showLab_Title = true;
    title_height = 30;
}

// 重写paintEvent()
void TitleWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt; // 需要头文件#include <QStyleOption>
    opt.init(this);
    QPainter p(this); // 需要头文件#include <QPainter>
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool TitleWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick && btnMenu_Max->isVisible()) {
        emit btnMenu_Max->clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}
void TitleWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->window()->pos();
        e->accept();
    }
}

void TitleWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (windowIsMove && mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->window()->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void TitleWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}
