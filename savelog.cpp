#include "savelog.h"
#include "qmutex.h"
#include "qfile.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qstringlist.h"
#include <QTextStream>

#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))

//日志重定向
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
void Log(QtMsgType type, const char *msg)
#else
void Log(QtMsgType type, const QMessageLogContext &, const QString &msg)
#endif
{
    //加锁,防止多线程中qdebug太频繁导致崩溃
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString content;

    //这里可以根据不同的类型加上不同的头部用于区分
    switch (type) {
    case QtDebugMsg:
        content = QString("%1").arg(msg);
        break;

    case QtWarningMsg:
        content = QString("%1").arg(msg);
        break;

    case QtCriticalMsg:
        content = QString("%1").arg(msg);
        break;

    case QtFatalMsg:
        content = QString("%1").arg(msg);
        break;
    }

    SaveLog::Instance()->save(content);
}

QScopedPointer<SaveLog> SaveLog::self;
SaveLog *SaveLog::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SaveLog);
        }
    }

    return self.data();
}

SaveLog::SaveLog(QObject *parent) : QObject(parent)
{
    file = new QFile(this);
    //默认取应用程序根目录
    path = qApp->applicationDirPath();
	saveLocal = false;
    //默认取应用程序可执行文件名称
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
    fileName = "";
}

SaveLog::~SaveLog()
{
    file->close();    
}

//安装日志钩子,输出调试信息到文件,便于调试
void SaveLog::start()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    qInstallMsgHandler(Log);
#else
    qInstallMessageHandler(Log);
#endif
}

//卸载日志钩子
void SaveLog::stop()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    qInstallMsgHandler(0);
#else
    qInstallMessageHandler(0);
#endif
}

void SaveLog::save(const QString &content)
{
	//方法改进:之前每次输出日志都打开文件,改成只有当日期改变时才新建和打开文件
	if(saveLocal == true){//写入本地
		QString fileName = QString("%1/%2_log_%3.txt").arg(path).arg(name).arg(QDATE);
		if (this->fileName != fileName) {
			this->fileName = fileName;
			if (file->isOpen()) {
				file->close();
			}

			file->setFileName(fileName);
			file->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
		}

		QTextStream logStream(file);
		logStream << content << "\n";
	}
	emit sendLogInfoToUI(content);
}


void SaveLog::setPath(const QString &path)
{
    this->path = path;
}
void SaveLog::setIsSave(const bool &saveLocal)
{
	this->saveLocal = saveLocal;
}
void SaveLog::setName(const QString &name)
{
    this->name = name;
}
