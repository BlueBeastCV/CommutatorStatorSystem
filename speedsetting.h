#ifndef SPEEDSETTING_H
#define SPEEDSETTING_H

#include <QWidget>
#include "objectinfo.h"
#include "myhelper.h"
namespace Ui {
class speedSetting;
}

class speedSetting : public QWidget
{
    Q_OBJECT

public:
    explicit speedSetting(User user,QWidget *parent = nullptr);
    ~speedSetting();

private:
    Ui::speedSetting *ui;
    User currentUser;
private slots:
    bool writeInSettingFile();
    void resetInputSpeed();
};

#endif // SPEEDSETTING_H
