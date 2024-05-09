#include "speedsetting.h"
#include "ui_speedsetting.h"

speedSetting::speedSetting(User user,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::speedSetting)
{
    ui->setupUi(this);
//    this->setFixedSize(this->width(),this->height());
    myHelper::FormInCenter(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    ui->titleWidget->set_lab_Title("速度设置");
    ui->titleWidget->setShowMaxBtn(false);
    currentUser = user;
    connect(ui->writeIn,&QPushButton::clicked,this,[=](){
       if(writeInSettingFile() == true)
       {
           myHelper::ShowMessageBoxInfo("速度参数写入配置文件成功！");
       }
       else
       {
           myHelper::ShowMessageBoxError("速度参数写入配置文件失败！");
       }
    });
    connect(ui->reset,&QPushButton::clicked,this,&speedSetting::resetInputSpeed);

}

speedSetting::~speedSetting()
{
    delete ui;
}

bool speedSetting::writeInSettingFile()
{
    myHelper::writeSettings("SpeedSetting/turnTableSpeed",ui->rotationSpeed->value());
    myHelper::writeSettings("SpeedSetting/turnTableCycleSpeed",ui->turnTableSpeed->value());
    myHelper::writeSettings("SpeedSetting/allAxisAbsSpeed",ui->UpAndDownAbsSpeed->value());
    myHelper::writeSettings("SpeedSetting/multiAxisSpeed",ui->allAxisSpeed->value());
    myHelper::writeSettings("SpeedSetting/axisOneSpeed",ui->axisOneSpeed_3->value());
    myHelper::writeSettings("SpeedSetting/axisTwoSpeed",ui->axisTwoSpeed_3->value());
    myHelper::writeSettings("SpeedSetting/axisThreeSpeed",ui->axisThreeSpeed_3->value());
    return true;
}

void speedSetting::resetInputSpeed()
{
    ui->rotationSpeed->setValue(1.0);
    ui->turnTableSpeed->setValue(1.0);
    ui->UpAndDownAbsSpeed->setValue(1.0);
    ui->allAxisSpeed->setValue(1.0);
    ui->axisOneSpeed_3->setValue(1.0);
    ui->axisTwoSpeed_3->setValue(1.0);
    ui->axisThreeSpeed_3->setValue(1.0);
}
