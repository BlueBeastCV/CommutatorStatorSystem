﻿#ifndef SAVELOG_H
#define SAVELOG_H

#include <QObject>

class QFile;

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT SaveLog : public QObject
#else
class SaveLog : public QObject
#endif

{
    Q_OBJECT
public:
    static SaveLog *Instance();
    explicit SaveLog(QObject *parent = 0);
    ~SaveLog();

private:
    static QScopedPointer<SaveLog> self;

    //文件对象
	QFile *file = nullptr;
    //日志文件路径
	QString path = nullptr;
    //日志文件名称
	QString name = nullptr;
    //日志文件完整名称
	QString fileName = nullptr;
	//日志文件是否存储本地
	bool saveLocal = false;
	signals:
	void sendLogInfoToUI(const QString &content);

public slots:
    //启动日志服务
    void start();
    //暂停日志服务
    void stop();
    //保存日志
    void save(const QString &content);

    //设置日志文件存放路径
    void setPath(const QString &path);
    //设置日志文件名称
    void setName(const QString &name);
	//设置日志文件是否存储本地
	void setIsSave(const bool &saveLocal);

};

#endif // SAVELOG_H
