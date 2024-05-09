#pragma execution_character_set("utf-8")
#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql>
#include "myhelper.h"



static bool db_sqllitecreateConnection()
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE",dataBase_alias);
	database.setDatabaseName(myHelper::readSettings("sqllite/name").toString());
	if (!database.open()){
		myHelper::ShowMessageBoxInfo(QString("打开sqllite数据库失败:%1").arg(database.lastError().text()));
		return false;
	} else {
		return true;
	}
}

static bool db_mysqlcreateConnection()
{
	QSqlDatabase db_mysql = QSqlDatabase::addDatabase("QMYSQL",dataBase_alias);
	db_mysql.setHostName(myHelper::readSettings("SQL/HostName").toString());
	db_mysql.setDatabaseName(myHelper::readSettings("SQL/DataBaseName").toString());
	db_mysql.setUserName(myHelper::readSettings("SQL/UserName").toString());
	db_mysql.setPassword(myHelper::readSettings("SQL/PWD").toString());
	if (!db_mysql.open()) {
		myHelper::ShowMessageBoxInfo(QString("打开mysql数据库失败:%1").arg(db_mysql.lastError().text()));
		return false;
	}
	return true;
}


#endif // CONNECTION_H
