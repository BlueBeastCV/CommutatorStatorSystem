#include "widget.h"
#include <QApplication>
#include <QMessageBox>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include "connection.h"
#include "myhelper.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMetaType>
#include "savelog.h"
#include "loginform.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font = a.font();
    font.setPointSize(12);//字体大小
    font.setFamily("Microsoft YaHei");//微软雅黑字体
    a.setFont(font);
    //只运行一次
    QSystemSemaphore ss("CommutatorStatorSystem",1,QSystemSemaphore::Open);
    ss.acquire();// 在临界区操作共享内存   SharedMemory
    QSharedMemory mem("MyCommutatorStatorSystem");// 全局对象名
    if (!mem.create(1)){// 如果全局对象以存在则退出
        myHelper::ShowMessageBoxInfo("程序已经运行，无需再启动！");
        ss.release();// 如果是 Unix 系统，会自动释放。
        return 0;
    }
    ss.release();// 临界区

    //初始化配置文件
    myHelper::initSettings();

    //初始化日志记录
    SaveLog::Instance()->setIsSave(false);//存储本地
    SaveLog::Instance()->setPath(qApp->applicationDirPath());
//    SaveLog::Instance()->start();

    //初始化数据库
    //db_mysqlcreateConnection
    //db_sqllitecreateConnection
    if(false == db_sqllitecreateConnection()){
        return 0;
    }

    loginForm w;
//	myHelper::FormInCenter(&lg);
    w.show();
    return a.exec();
}
