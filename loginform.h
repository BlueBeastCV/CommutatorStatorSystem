#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "widget.h"
#include "dataoper.h"

namespace Ui {
class loginForm;
}

class loginForm : public QWidget
{
        Q_OBJECT

    public:
        explicit loginForm(QWidget *parent = nullptr);
        ~loginForm();
        void init();
    protected:
        bool eventFilter(QObject *watched, QEvent *event);

    private slots:
        void on_loginBtn_clicked();

        void on_exitBtn_clicked();

    private:
        Ui::loginForm *ui;
        Widget *mainFm;

        QWidget *moveWidget;    //移动的控件
        QPoint lastPoint;   //最后按下的坐标
        bool pressed;       //鼠标是否按下
        bool leftButton;    //限定鼠标左键

        DataOper oper;
};

#endif // LOGINFORM_H
