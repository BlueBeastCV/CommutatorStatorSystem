#include "loginform.h"
#include "ui_loginform.h"

loginForm::loginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    moveWidget = ui->widget1;

    moveWidget->installEventFilter(this);

    //加载用户列表
    init();
}

bool loginForm::eventFilter(QObject *watched, QEvent *event)
{
    if (moveWidget != 0 && watched == moveWidget) {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            //如果限定了只能鼠标左键拖动则判断当前是否是鼠标左键
            if (leftButton && mouseEvent->button() != Qt::LeftButton) {
                return false;
            }
            //判断控件的区域是否包含了当前鼠标的坐标
            if (moveWidget->rect().contains(mouseEvent->pos())) {
                lastPoint = mouseEvent->pos();
                pressed = true;
            }
        } else if (mouseEvent->type() == QEvent::MouseMove && pressed) {
            //计算坐标偏移值,调用move函数移动过去
            int offsetX = mouseEvent->pos().x() - lastPoint.x();
            int offsetY = mouseEvent->pos().y() - lastPoint.y();
            int x = this->x() + offsetX;
            int y = this->y() + offsetY;
            this->move(x, y);
        } else if (mouseEvent->type() == QEvent::MouseButtonRelease && pressed) {
            pressed = false;
        }
    }

    return QObject::eventFilter(watched, event);
}


loginForm::~loginForm()
{
    delete ui;
}

void loginForm::init()
{
    QStringList userNameList;
    oper.getUserNameList(userNameList);
    ui->userName->clear();
    ui->userName->addItems(userNameList);
}

void loginForm::on_loginBtn_clicked()
{

    QString name = ui->userName->currentText();
    QString pwd = ui->pwd->text();
    QString role = ui->role->currentText();
    User user;

    if(name.isEmpty()){
        myHelper::ShowMessageBoxError("账号不能为空");
        ui->userName->setFocus();
        return;
    }
    if(pwd.isEmpty()){
        myHelper::ShowMessageBoxError("密码不能为空");
        ui->pwd->setFocus();
        return;
    }
    int ok = 0;
    ok = oper.getUserByName(user, name);
    if(ok == 2){
        myHelper::ShowMessageBoxError("查询错误");
        return;
    }

    if(ok == 1){
        myHelper::ShowMessageBoxError("账号错误");
        ui->userName->setFocus();
        ui->userName->clear();
        return;
    }
    if(user.pwd != pwd){
        myHelper::ShowMessageBoxError("密码错误");
        ui->pwd->setFocus();
        ui->pwd->clear();
        return;
    }
    if(user.role != role){
        myHelper::ShowMessageBoxError("角色选择不对");
        return;
    }
    if(user.is_used == "0"){
        myHelper::ShowMessageBoxError("当前账号已冻结！");
        return;
    }

    mainFm = new Widget(user);
    myHelper::FormInCenter(mainFm);
    mainFm->setAttribute(Qt::WA_DeleteOnClose);

    mainFm->show();
    this->close();

}


void loginForm::on_exitBtn_clicked()
{
    qApp->exit();
}

