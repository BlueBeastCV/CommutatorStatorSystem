#include "limitform.h"
#include "ui_limitform.h"

LimitForm::LimitForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LimitForm)
{
    ui->setupUi(this);

    myHelper::FormInCenter(this);//窗体居中显示
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    ui->titleWidget->set_lab_Title("密码输入");
    ui->titleWidget->setShowMaxBtn(false);
    ui->titleWidget->setShowMinBtn(false);

}

LimitForm::~LimitForm()
{
    delete ui;
}

void LimitForm::on_pushButton_clicked()
{
    QString pwd = ui->lineEdit->text();
    QString p = myHelper::readSettings("parameter/pwd").toString();
    if(pwd == p){
        emit hideCurrentWindow();
        this->close();
    }else{
        myHelper::ShowMessageBoxError("密码错误");
    }
}

