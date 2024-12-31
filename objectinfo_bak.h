
#pragma execution_character_set("utf-8")
#ifndef OBJECTINFO_H
#define OBJECTINFO_H
#include <QString>
#include <QDebug>

class User
{
    public:
        User(){
            name = nullptr;
            pwd = nullptr;
            role = nullptr;
            is_used = nullptr;
        }
        friend QDebug& operator<<(QDebug out, const User& user)
        {
            out << "账号：" << user.name << "密码：" << user.pwd << "角色：" << user.role << "是否可用：" << user.is_used;
            return out;
        }
    public:
        QString name;
        QString pwd;
        QString role;
        QString is_used;
};

//控制系统读取对象
class ControllerObject
{
    public:
        ControllerObject(){
        }
    public:



};


#endif // OBJECTINFO_H
