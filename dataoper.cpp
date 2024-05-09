#include "dataoper.h"

DataOper::DataOper(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::database(dataBase_alias);//连接数据库2
}


//释放数据库删除文件占用的空间。清空表数据，并不会改变db文件大小。索要要执行次命令
bool DataOper::releaseDBspace()
{
    QSqlQuery query(db);
    query.prepare("VACUUM");
    if (query.exec()) {
        qDebug() <<  "releaseDBspace success.";
        return true;
    }else{
        return false;
        qDebug() <<  "Can't compress database.";
    }
}
//生成UUID
QString DataOper::getUUID()
{
    QUuid id = QUuid::createUuid();
    QString strID = id.toString();
    strID.remove("{").remove("}").remove("-");
    return strID;
}
//快速删除大量数据  fieldMap 需要删除的数据
int DataOper::deleteTableData(QString oldTable, QMap<QString, QVariant> fieldMap)
{
    QSqlQuery query(db);
    db.transaction();
    //创建临时表存储不需要删除的数据
    QString sql = QString("CREATE TABLE %1temp AS SELECT * FROM %2 where  1=1 ").arg(oldTable).arg(oldTable);
    QMap<QString, QVariant>::const_iterator it = fieldMap.constBegin();
    while (it != fieldMap.constEnd())
    {
        qDebug() << it.key() << ": " << it.value() << endl;
        if(isInteger(it.value())){//QVariant 是整数
            sql.append( QString("and %1 != %2 ").arg(it.key()).arg(it.value().toInt()) );
        }else{
            sql.append( QString("and %1 != '%2' ").arg(it.key()).arg(it.value().toString()) );
        }
        ++it;
    }
    if(query.exec(sql)){
        db.commit();
        //删除旧表
        sql = QString("DROP TABLE %1").arg(oldTable);
        if(query.exec(sql)){
            db.commit();
            //更改临时表到新表
            sql = QString("ALTER TABLE %1temp RENAME TO %2").arg(oldTable).arg(oldTable);
            if(query.exec(sql)){
                db.commit();
                return 0;
            }else{
                qDebug() << "更改临时表到新表 fail:" << query.lastError();
                return 2;
            }
        }else{
            qDebug() << "删除旧表 fail:" << query.lastError();
            return 2;
        }
    }else{
        qDebug() << "创建临时表存储不需要删除的数据 fail:" << query.lastError();
        return 2;
    }
}

//批量删除大量数据 delete 很慢，可以尝试以下方式
//-- 复制表结构并复制需要保留数据
//CREATE TABLE logE AS SELECT * FROM log where name = 'admin';
//-- 删除旧表
//DROP TABLE log;
//-- 重命名
//ALTER TABLE logE RENAME TO log

//判断QVariant 是否是整型
bool DataOper::isInteger(const QVariant &variant)
{
    switch (variant.userType())
    {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return true;
    }
    return false;
}
//判断QVariant 是否是字符串
bool DataOper::isString(const QVariant &variant)
{
    return variant.userType() == QMetaType::QString;
}


//获取用户列表
int DataOper::getUserNameList(QStringList &userNameList)
{
    QSqlQuery query(db);
    QString sql  = QString("select name from user");

    if(query.exec(sql)){
        while(query.next()){
            userNameList.append(query.value("name").toString());
        }
        return 0;
    }else{
        qDebug() << "getUserNameList fail: " + query.lastError().text();
        return 2;
    }
}
//查询所有用户
int DataOper::queryAllUser(QVector<User>& users)
{
    User  user;
    QSqlQuery q(db);
    QString sql = "select name,pwd,role,is_used from user ";

    if(q.exec(sql)){
        while(q.next()){
            user.name = q.value("name").toString();
            user.pwd = q.value("pwd").toString();
            user.role = q.value("role").toString();
            user.is_used = q.value("is_used").toString();
            users.push_back(user);
        }
        return 0;
    }else{
        qDebug() << "queryAllUser fail: " + q.lastError().text();
        return 2;
    }
}
//保存用户
int DataOper::saveUsers(QVector<User> users)
{
    //删除现有的全部用户
    deleteUser();
    QSqlQuery q(db);
    q.prepare("insert into user values (?, ?, ?, ?)");
    QVariantList names;
    QVariantList pwds;
    QVariantList roles;
    QVariantList is_useds;
    for(int i = 0; i < users.size(); i++){
        names << users[i].name;
        pwds << users[i].pwd;
        roles << users[i].role;
        is_useds << users[i].is_used;
    }
    q.addBindValue(names);
    q.addBindValue(pwds);
    q.addBindValue(roles);
    q.addBindValue(is_useds);
    if (q.execBatch()){
        return 0;
    }else{
        qDebug() << "saveUsers fail: " + q.lastError().text();
        return 2;
    }
}
//删除用户
int DataOper::deleteUser(QString name)
{
    QSqlQuery q(db);
    QString sql = nullptr;
    if(name == nullptr){
        sql = QString("delete from user");
    }else{
        sql = QString("delete from user where name = '%1'").arg(name);
    }
    if(q.exec(sql)){
        return 0;
    }else{
        qDebug() << "deleteUserByName fail: " + q.lastError().text();
        return 2;
    }
}
//根据名字获取用户信息
int DataOper::getUserByName(User &user, QString name)
{
    QSqlQuery query(db);
    QString sql  = QString("select name, pwd, role, is_used from user where name = '%1'").arg(name);

    if(query.exec(sql)){
        if(query.next()){
            user.name = query.value("name").toString();
            user.pwd = query.value("pwd").toString();
            user.role = query.value("role").toString();
            user.is_used = query.value("is_used").toString();
            return 0;
        }else{
            return 1;
        }
    }else{
        qDebug() << "getUserByName fail: " + query.lastError().text();
        return 2;
    }
}

int DataOper::saveParametersInfo(QVector<ParametersInfo> &paramtersInfo)
{

    QSqlQuery q(db);
    q.prepare("insert into parametersInfo(fileName, typeNum, unitNum, status, date, detectContentOne, detectObjOne, detectContentTwo, detectObjTwo, samplePoints) values (?,?,?,?,?,?,?,?,?,?)");
    QVariantList fileNames;
    QVariantList typeNums;
    QVariantList unitNums;
    QVariantList statuses;
    QVariantList dates;
    QVariantList detectContentOnes;
    QVariantList detectObjOnes;
    QVariantList detectContentTwos;
    QVariantList detectObjTwos;
    QVariantList samplePoints;
    for(int i=0;i<paramtersInfo.size();i++)
    {
        fileNames << paramtersInfo[i].fileName;
        typeNums << paramtersInfo[i].typeNum;
        unitNums << paramtersInfo[i].unitNum;
        statuses << paramtersInfo[i].status;
        dates << paramtersInfo[i].date;
        detectContentOnes << paramtersInfo[i].detectContentOne;
        detectObjOnes << paramtersInfo[i].detectObjOne;
        detectContentTwos << paramtersInfo[i].detectContentTwo;
        detectObjTwos << paramtersInfo[i].detectObjTwo;
        samplePoints << paramtersInfo[i].samplingPointNum;
    }
    q.addBindValue(fileNames);
    q.addBindValue(typeNums);
    q.addBindValue(unitNums);
    q.addBindValue(statuses);
    q.addBindValue(dates);
    q.addBindValue(detectContentOnes);
    q.addBindValue(detectObjOnes);
    q.addBindValue(detectContentTwos);
    q.addBindValue(detectObjTwos);
    q.addBindValue(samplePoints);
    if (q.execBatch()){
        db.commit();
        return 0;
    }else{
        qDebug() << "saveParameterInfos failed: " + q.lastError().text();
        return -1;
    }

}

int DataOper::saveSensorNumberOneData(QVector<SensorDetectDataOne> &sensorNumOneDataList)
{
    QSqlQuery q(db);
    q.prepare("insert into sensorOneDetectTable(typeNum, samplePoints, angle, detectData, dates) values(?,?,?,?,?)");
    QVariantList typeNums;
    QVariantList samplePts;
    QVariantList angles;
    QVariantList detectDatas;
    QVariantList dates;
    for(int i=0;i<sensorNumOneDataList.size();i++)
    {
        typeNums << sensorNumOneDataList[i].typeNum;
        samplePts << sensorNumOneDataList[i].pointsNum;
        angles << sensorNumOneDataList[i].angle;
        detectDatas << sensorNumOneDataList[i].detectData;
        dates << sensorNumOneDataList[i].date;
    }
    q.addBindValue(typeNums);
    q.addBindValue(samplePts);
    q.addBindValue(angles);
    q.addBindValue(detectDatas);
    q.addBindValue(dates);
    if (q.execBatch()){
        db.commit();
        return 0;
    }else{
        qDebug() << "saveSensorNumberOneData failed: " + q.lastError().text();
        return -1;
    }
}

int DataOper::saveSensorNumberTwoData(QVector<SensorDetectDataTwo> &sensorNumTwoDataList)
{
    QSqlQuery q(db);
    q.prepare("insert into sensorTwoDetectTable(typeNum, samplePoints, angle, detectData, dates) values(?,?,?,?,?)");
    QVariantList typeNums;
    QVariantList samplePts;
    QVariantList angles;
    QVariantList detectDatas;
    QVariantList dates;
    for(int i=0;i<sensorNumTwoDataList.size();i++)
    {
        typeNums << sensorNumTwoDataList[i].typeNum;
        samplePts << sensorNumTwoDataList[i].pointsNum;
        angles << sensorNumTwoDataList[i].angle;
        detectDatas << sensorNumTwoDataList[i].detectData;
        dates << sensorNumTwoDataList[i].date;
    }
    q.addBindValue(typeNums);
    q.addBindValue(samplePts);
    q.addBindValue(angles);
    q.addBindValue(detectDatas);
    q.addBindValue(dates);
    if (q.execBatch()){
        db.commit();
        return 0;
    }else{
        qDebug() << "saveSensorNumberTwoData failed: " + q.lastError().text();
        return -1;
    }
}

int DataOper::getTypeNumList(QStringList &typeNumList)
{
    QSqlQuery q(db);
    QString sql = QString("select typeNum from parametersInfo");
    if(q.exec(sql))
    {
        while(q.next())
        {
            typeNumList.append(q.value("typeNum").toString());
        }
        return 0;
    }
    else
    {
        qDebug() << "getTypeNumList failed: " + q.lastError().text();
        return -1;
    }
}

int DataOper::getTypeNumByTime(QStringList &typeNumList, QString startTime, QString endTime)
{
    QSqlQuery query(db);
    QString sql = QString("SELECT DISTINCT typeNum FROM parametersInfo where date >='%1' and date <= '%2' order by typeNum asc")
          .arg(startTime).arg(endTime);
    if(query.exec(sql)){
        while(query.next()){
            typeNumList.append(query.value("typeNum").toString());
        }
        return 0;
    }else{
        qDebug() << "getTypeNumByTime fail: " + query.lastError().text();
        return 2;
    }
}
//int DataOper::getHardnessListByID(QVector<hardnessList> &hadnessDataList, const QString id)
//{
//    QSqlQuery query(db);
//    QString sql  = QString("select plateType, pointCode, xCoordinate, yCoordinate, hardness, diameter, result, time, operator from hardnessList where plateType='%1' ")
//            .arg(id);
//    if(query.exec(sql)){
//        while(query.next()){
//            hardnessList hardnesses;
//            hardnesses.plateType = query.value("plateType").toString();
//            hardnesses.id = query.value("pointCode").toString();
//            hardnesses.xCoordinate = query.value("xCoordinate").toInt();
//            hardnesses.yCoordinate = query.value("yCoordinate").toInt();
//            hardnesses.hardness = query.value("hardness").toInt();
//            hardnesses.diameter = query.value("diameter").toDouble();
//            hardnesses.time = query.value("time").toString();
//            hardnesses.oper = query.value("operator").toString();
//            hardnesses.result = query.value("result").toString();
//            hadnessDataList.push_back(hardnesses);
//        }
//        return 0;
//    }else{
//        qDebug() << "getHardnessListByID fail:" + query.lastError().text();
//        return 2;
//    }
//}
int DataOper::getSensorNumberOneData(QVector<SensorDetectDataOne> &sensorNumOneDataList, const QString typeNum,
                                     const QString &startTime, const QString &endTime)
{
    QSqlQuery q(db);
    QString sql = QString("select samplePoints, angle, detectData, dates from sensorOneDetectTable where typeNum = '%1' AND dates BETWEEN '%2' AND '%3'")
            .arg(typeNum).arg(startTime + "00:00:00").arg(endTime + "23:59:59");
    if(q.exec(sql))
    {
        while(q.next())
        {
            SensorDetectDataOne datas;
            datas.pointsNum = q.value("samplePoints").toInt();
            datas.angle = q.value("angle").toDouble();
            datas.detectData = q.value("detectData").toDouble();
            datas.date = q.value("dates").toString();
            sensorNumOneDataList.push_back(datas);
        }
        return 0;
    }
    else
    {
        qDebug() << "getSensorNumberOneData fail:" + q.lastError().text();
        return -1;
    }
}


int DataOper::getSensorNumberTwoData(QVector<SensorDetectDataTwo> &sensorNumTwoDataList, const QString typeNum,
                                     const QString &startTime, const QString &endTime)
{
    QSqlQuery q(db);
    QString sql = QString("select samplePoints, angle, detectData, dates from sensorTwoDetectTable where typeNum = '%1' AND dates BETWEEN '%2' AND '%3'")
            .arg(typeNum).arg(startTime + "00:00:00").arg(endTime + "23:59:59");
    if(q.exec(sql))
    {
        while(q.next())
        {
            SensorDetectDataTwo datas;
            datas.pointsNum = q.value("samplePoints").toInt();
            datas.angle = q.value("angle").toDouble();
            datas.detectData = q.value("detectData").toDouble();
            datas.date = q.value("dates").toString();
            sensorNumTwoDataList.push_back(datas);
        }
        return 0;
    }
    else
    {
        qDebug() << "getSensorNumberTwoData fail:" + q.lastError().text();
        return -1;
    }
}

int DataOper::getAxialDataEvaluate(QVector<AxialDataEvaluate> &axialEvaluateDataList, const QString typeNum)
{
    QSqlQuery q(db);
    QString sql = QString("select UnitNum, status, detectContent, dates, detectObj, maxValue, maxAngle, minValue, minAngle,"
                          "beatValue, beatAngle, verticalValue, verticalAngle, concentricity, concentricAngle, eccentricity,"
                          "eccentricAngle, flatness, roundness from axialDataEvaluate where typeNum = '%1'").arg(typeNum);
    if(q.exec(sql))
    {
        while(q.next())
        {
            AxialDataEvaluate axialDatas;
            axialDatas.unitNum = q.value("UnitNum").toInt();
            axialDatas.status = q.value("status").toString();
            axialDatas.detectContent = q.value("detectContent").toString();
            axialDatas.dates = q.value("dates").toString();
            axialDatas.detectObj = q.value("detectObj").toString();
            axialDatas.maxValue = q.value("maxValue").toDouble();
            axialDatas.maxAngle = q.value("maxAngle").toDouble();
            axialDatas.minValue = q.value("minValue").toDouble();
            axialDatas.minAngle = q.value("minAngle").toDouble();
            axialDatas.beatValue = q.value("beatValue").toDouble();
            axialDatas.beatAngle = q.value("beatAngle").toDouble();
            axialDatas.verticalValue = q.value("verticalValue").toDouble();
            axialDatas.verticalAngle = q.value("verticalAngle").toDouble();
            axialDatas.concentricity = q.value("concentricity").toDouble();
            axialDatas.concentriticAngle = q.value("concentricAngle").toDouble();
            axialDatas.eccentricity = q.value("eccentricity").toDouble();
            axialDatas.eccentricAngle = q.value("eccentricAngle").toDouble();
            axialDatas.flatness = q.value("flatness").toDouble();
            axialDatas.roundness = q.value("roundness").toDouble();

            axialEvaluateDataList.push_back(axialDatas);
        }
        return 0;
    }
    else
    {
        qDebug() << "getAxialDataEvaluate fail:" + q.lastError().text();
        return -1;
    }

}

int DataOper::getRadialDataEvaluate(QVector<RadialDataEvaluate> &radialEvaluateDataList, const QString typeNum)
{
    QSqlQuery q(db);
    QString sql = QString("select UnitNum, status, detectContent, dates, detectObj, maxValue, maxAngle, minValue, minAngle,"
                          "beatValue, beatAngle, verticalValue, verticalAngle, concentricity, concentricAngle, eccentricity,"
                          "eccentricAngle, flatness, roundness from radialDataEvaluate where typeNum = '%1'").arg(typeNum);
    if(q.exec(sql))
    {
        while(q.next())
        {
            RadialDataEvaluate radialDatas;
            radialDatas.unitNum = q.value("UnitNum").toInt();
            radialDatas.status = q.value("status").toString();
            radialDatas.detectContent = q.value("detectContent").toString();
            radialDatas.dates = q.value("dates").toString();
            radialDatas.detectObj = q.value("detectObj").toString();
            radialDatas.maxValue = q.value("maxValue").toDouble();
            radialDatas.maxAngle = q.value("maxAngle").toDouble();
            radialDatas.minValue = q.value("minValue").toDouble();
            radialDatas.minAngle = q.value("minAngle").toDouble();
            radialDatas.beatValue = q.value("beatValue").toDouble();
            radialDatas.beatAngle = q.value("beatAngle").toDouble();
            radialDatas.verticalValue = q.value("verticalValue").toDouble();
            radialDatas.verticalAngle = q.value("verticalAngle").toDouble();
            radialDatas.concentricity = q.value("concentricity").toDouble();
            radialDatas.concentriticAngle = q.value("concentricAngle").toDouble();
            radialDatas.eccentricity = q.value("eccentricity").toDouble();
            radialDatas.eccentricAngle = q.value("eccentricAngle").toDouble();
            radialDatas.flatness = q.value("flatness").toDouble();
            radialDatas.roundness = q.value("roundness").toDouble();

            radialEvaluateDataList.push_back(radialDatas);
        }
        return 0;
    }
    else
    {
        qDebug() << "getAxialDataEvaluate fail:" + q.lastError().text();
        return -1;
    }
}






