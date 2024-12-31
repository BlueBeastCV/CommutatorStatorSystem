#include "widget.h"
#include "ui_widget.h"

Widget::Widget(User u, QWidget *parent):
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    currentUser = u;
    myHelper::FormInCenter(this);//窗体居中显示
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    ui->titleWidget->set_lab_Title("低涡转静子测量系统");
    ui->titleWidget->setShowLabIco(false);
    ui->titleWidget->setShowMaxBtn(true);
    radialPolar = new myRadialPolarChart();
    radialPolar->isCreated = false;
    axialPolar = new myAxialPolarChart();
    axialPolar->isCreated = false;
    init();
    initDoubleSpinBoxInputRange();
    initRadialChart();
    initAxialChart();
    //马尔表连接
    initTimer.singleShot(100,this,[this](){
        initMahrDevices();
    });
    //控制连接
    initDeviceTimer.singleShot(100,this,[this](){
        InitPLCThread();
    });

}

Widget::~Widget()
{
    systemTime.stop();
    //销毁PLC初始化线程
    plcThread.quit();
    plcThread.wait();
    // 注销所有的回调函数
    if (m_mwlManager) {
        qlog("**********Starting Destructor**********");
        m_mwlManager->unregisterMsgCallback();
        qlog("Unregistering message callback...");
        if (m_bIsCallBackRegisterd) {
            m_mwlManager->unregisterDataCallback((F_MwlDataCallback)data_callback_handler_1);
            m_mwlManager->unregisterDataCallback((F_MwlDataCallback)data_callback_handler_2);
            qlog("Unregistering data callbacks...");
        }
        // 设备关闭
        closeDeviceOne();
        closeDeviceTwo();
        if (m_mwlManager->m_hLib) {
            FreeLibrary(m_mwlManager->m_hLib);
            m_mwlManager->m_hLib = nullptr;
            qlog("FreeLibrary m_mwlManager->m_hLib...");
        }
        m_mwlManager.reset();
        qlog("m_mwlManager Reseted...");
    }
    delete version;
    qlog("Deleted version Pointer...");
    delete ui;
    qlog("Deleted ui Pointer...");
}

void Widget::init()
{
    initConnect();
    setTips();
    QIntValidator *intValidator = new QIntValidator();
    intValidator->setRange(0,1000000);
    ui->Edit_smaplingPoints_3->setValidator(intValidator);
    ui->roleName->setText(currentUser.role);
    ui->userName->setText(currentUser.name);
    QDate measureDates = QDate::currentDate();
    ui->startTime->setDate(measureDates.addDays(-1));
    ui->endTime->setDate(measureDates);
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setStyleSheet("QPlainTextEdit { color: red; }");
    initPushbuttonByFrame(ui->frame_4);
    initPushbuttonByFrame(ui->frame3);
    ui->Edit_dateTime_3->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    ui->Edit_dateTime_3->setEnabled(false);
    ui->groupBox_radialEvaluate->setEnabled(false);
    ui->groupBox_axialEvaluate->setEnabled(false);

    changeWidgetButtonStyle(ui->Btn_mainPage);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
    dashboard1 = new myDashBoard(this);
    dashboard2 = new myDashBoard(this);
    dashboard1->m_title = "#1";
    dashboard2->m_title = "#2";
    dashboard1->setParent(ui->dashBoardOne);
    dashboard2->setParent(ui->dashBoardTwo);

    //定时添加数据到统计图
    radialReadDataTimer = new QTimer();
    connect(radialReadDataTimer,&QTimer::timeout,this,[=](){
        addPointToRadialChart(ui->sensorOneData->value(),fabs(ui->angle->value()));
        saveMahrDataToTableOne(fabs(ui->angle->value()),ui->sensorOneData->value());
    });
    axialReadDataTimer = new QTimer();
    connect(axialReadDataTimer,&QTimer::timeout,this,[=](){
        addPointToAxialChart(ui->sensorTwoData->value(),fabs(ui->angle->value()));
        saveMahrDataToTableTwo(fabs(ui->angle->value()),ui->sensorTwoData->value());
    });

}
//初始化PLC线程函数
void Widget::InitPLCThread()
{
    plcThreadOBJ = new PLCThread();
    plcThreadOBJ->moveToThread(&plcThread);
    plcThread.start();

    connect(&plcThread,   &QThread::finished,           plcThreadOBJ, &PLCThread::deleteLater);
    connect(this,         &Widget::ConnectPLC,          plcThreadOBJ, &PLCThread::ConnectPLC);
    connect(plcThreadOBJ, &PLCThread::ConnectPLCStatus, this,         &Widget::ConnectPLCStatus);
    emit ConnectPLC(PLCIP, PLCRack, PLCSlot);
}
//PLC线程初始化返回函数
void Widget::ConnectPLCStatus(PLC_Siemens* plc, int ok)
{
    snap7_plc = plc;
    QString error = snap7_plc->ErrorText(ok);
    qlog(QString("snap7_plc connect : %1").arg(error));
    if(ok == 0){
        intPLCount = 0;
        ui->controllerLabel->setStyleSheet(normal_radius);
        controllerInitStatus = true;
        initMode();
        connect(&timeReadController, &QTimer::timeout, [this](){
            readControllerObject();
        });
        timeReadController.start(100);
    }else{
        intPLCount = intPLCount + 1;
        if(intPLCount == 1){
            ui->controllerLabel->setStyleSheet(fail_radius);
            controllerInitStatus = false;
        }else{
            emit ConnectPLC(PLCIP, PLCRack, PLCSlot);
        }
    }
}
//初始化马尔表设备信息
void Widget::initMahrDevices()
{
    deviceOne.DeviceNo = 1001;
    deviceOne.DeviceTypeId = MWl_dtMarCator1086Ri;
    deviceOne.Eco = FALSE;
    deviceOne.FreqId = 0;
    deviceOne.Pairing = FALSE;
    deviceOne.TimeOut = 60000;
    deviceTwo.DeviceNo = 1002;
    deviceTwo.DeviceTypeId = MWl_dtMarCator1086Ri;
    deviceTwo.Eco = FALSE;
    deviceTwo.FreqId = 0;
    deviceTwo.Pairing = FALSE;
    deviceTwo.TimeOut = 60000;
    m_mwlManager = std::make_unique<MWLManager>(this);
    version = new MWL_VERSION;
    initStatus = m_mwlManager->initializeLibrary();
    if(initStatus == true)
    {
        m_mwlManager->getVersion(version);
        m_mwlManager->registerMsgCallback((F_MwlMsgCallback)message_callback_handler);
        initDeviceOne();
        initDeviceTwo();
    }
}

void Widget::initMode()
{
    if(controllerInitStatus == false) return;
    int modeNum = 1;
    byte modeBuffer[2];
    S7_SetIntAt(modeBuffer,0,modeNum);
    snap7_plc->DBWrite(writeDbNum,66,2,&modeBuffer,"初始化运行模式！");

}
//初始化马尔表1号
void Widget::initDeviceOne()
{
    deviceOneStatus = m_mwlManager->openDevice(deviceOne);
    ui->sensorOneStatus->setStyleSheet(deviceOneStatus == true ? normal_radius : fail_radius);
    qlog(QString("Open Deivce One %1 !!!").arg(deviceOneStatus == true ? "Succeed" : "Failed"));
}
//初始化马尔表2号
void Widget::initDeviceTwo()
{
    deviceTwoStatus = m_mwlManager->openDevice(deviceTwo);
    ui->sensorTwoStatus->setStyleSheet(deviceTwoStatus == true ? normal_radius : fail_radius);
    qlog(QString("Open Deivce Two %1 !!!").arg(deviceTwoStatus == true ? "Succeed" : "Failed"));
}
//关闭马尔表1号
void Widget::closeDeviceOne()
{
    bool res =  m_mwlManager->closeDevice(deviceOne.DeviceId);
    if(res)
    {
        qlog(QString("Close DeviceOne Succeed: %1").arg(res));
    }
    else
    {
        qlog(QString("Close DeviceOne Failed: %1").arg(res));
    }
    deviceOneStatus = false;
}
//关闭马尔表2号
void Widget::closeDeviceTwo()
{
    bool res = m_mwlManager->closeDevice(deviceTwo.DeviceId);
    if(res)
    {
        qlog(QString("Close DeviceTwo Succeed: %1").arg(res));
    }
    else
    {
        qlog(QString("Close DeviceTwo Failed: %1").arg(res));
    }
    deviceTwoStatus = false;
}
//信息打印回调函数
int __stdcall Widget::message_callback_handler(int Msg, int DeviceId, int Param)
{
    if (Msg == WM_MWL_Tick)
    {
        qlog("WM_MWL_Tick");
    }
    else if(Msg == WM_MWL_UsbStickCountChanged)
    {
        qlog("WM_MWL_UsbStickCountChanged");
    }
    else if (Msg == WM_MWL_DeviceOpened)
    {
        qlog( "WM_MWL_DeviceOpened");
    }

    else if (Msg == WM_MWL_DeviceConnected)
    {
        qlog("WM_MWL_DeviceConnected")
    }

    else if (Msg == WM_MWL_NewMeasVal)
    {
        qlog( "WM_MWL_NewMeasVal");
    }

    else if (Msg == WM_MWL_NoRequestAnswer)
    {
        qlog(QString("WM_MWL_NoRequestAnswer device: %1").arg(DeviceId));
    }

    else if (Msg == WM_MWL_Id)
    {
        qlog("WM_MWL_Id");
    }
    qlog(QString("Msg: %1 DeviceId: %2 Param: %3").arg(Msg).arg(DeviceId).arg(Param));
    return 0;
}
//马尔表1号获取数据回调函数
int __stdcall Widget::data_callback_handler_1(int numDevices, int *pDevNoArray, double *pData, void *pContext)
{
    qlog( "Device One Data Callback...");
    Widget *widget = static_cast<Widget*>(pContext);
    for (int i = 0; i < numDevices; i++)
    {
        qlog(QString("Device One Data: %1,%2").arg(pDevNoArray[i]).arg(pData[i]));
        if (pDevNoArray[i] == 0)
        {
            widget->ui->sensorOneData->setValue(pData[i]);
            widget->dashboard1->setValue(pData[i]);
            widget->dashboard1->UpdateAngle();
        }
    }
    return 0;
}
//马尔表2号获取数据回调函数
int __stdcall Widget::data_callback_handler_2(int numDevices, int *pDevNoArray, double *pData, void *pContext)
{
    qlog( "Device Two Data Callback...");
    Widget *widget = static_cast<Widget*>(pContext);
    for (int i = 0; i < numDevices; i++)
    {
        qlog(QString("Device Two Data: %1,%2").arg(pDevNoArray[i]).arg(pData[i]));
        if (pDevNoArray[i] == 1)
        {
            widget->ui->sensorTwoData->setValue(pData[i]);
            widget->dashboard2->setValue(pData[i]);
            widget->dashboard2->UpdateAngle();
        }
    }
    return 0;
}
//马尔表1号获取数据回调相应函数
void Widget::initStartDataCallBackDeviceOne()
{
    int numDevices = 1;
    int pDevNoArray[] = {0};
    void* pContext = static_cast<void*>(this);
    m_mwlManager->registerDataCallback((F_MwlDataCallback)data_callback_handler_1, numDevices, pDevNoArray, pContext);
    m_bIsCallBackRegisterd = true;
}
//马尔表2号获取数据回调相应函数
void Widget::initStartDataCallBackDeviceTwo()
{
    int numDevices = 1;
    int pDevNoArray[] = {1};
    void* pContext = static_cast<void*>(this);
    m_mwlManager->registerDataCallback((F_MwlDataCallback)data_callback_handler_2, numDevices, pDevNoArray, pContext);
    m_bIsCallBackRegisterd = true;
}
//保存径向检测数据到数据库
void Widget::saveMahrDataToTableOne(double radialAngle, double radialDetectValue)
{
    QVector<SensorDetectDataOne> sensorNumOneDataList;
    SensorDetectDataOne dataListOne;
    dataListOne.typeNum = ui->Edit_typeNum_3->text();
    dataListOne.pointsNum = sampleNumsOne;
    dataListOne.angle = radialAngle;
    dataListOne.detectData = radialDetectValue;
    dataListOne.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    sensorNumOneDataList.push_back(dataListOne);
    if(sampleNumsOne != ui->Edit_smaplingPoints_3->text().toInt())
    {
        sampleNumsOne++;
    }
    oper.saveSensorNumberOneData(sensorNumOneDataList);
}
//保存轴向检测数据到数据库
void Widget::saveMahrDataToTableTwo(double axialAngle, double axialDetectValue)
{
    QVector<SensorDetectDataTwo> sensorNumTwoDataList;
    SensorDetectDataTwo dataListTwo;
    dataListTwo.typeNum = ui->Edit_typeNum_3->text();
    dataListTwo.pointsNum = sampleNumsTwo;
    dataListTwo.angle = axialAngle;
    dataListTwo.detectData = axialDetectValue;
    dataListTwo.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    sensorNumTwoDataList.push_back(dataListTwo);
    if(sampleNumsTwo != ui->Edit_smaplingPoints_3->text().toInt())
    {
        sampleNumsTwo++;
    }
    oper.saveSensorNumberTwoData(sensorNumTwoDataList);
}
//connect函数初始化
void Widget::initConnect()
{
    //更新系统时间
    connect(&systemTime, &QTimer::timeout, [this](){
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ui->systemTime->setText(currentDateTime);
    });
    systemTime.start(1000);
    //测量开始日期修改事件
    connect(ui->startTime,&QDateEdit::dateChanged,[this](const QDate &date){
        QString endTime = ui->endTime->date().toString("yyyy-MM-dd");
        QStringList typeList;
        oper.getTypeNumByTime(typeList, date.toString("yyyy-MM-dd"), endTime);
        loadMeasureProCodeList = true;
        ui->TypeList->clear();
        ui->TypeList->addItem("选择测量型号");
        ui->TypeList->addItems(typeList);
        loadMeasureProCodeList = false;
    });
    //马尔表置零
    connect(ui->resetDeviceOne,&QPushButton::clicked,this,[=](){
        if(deviceOneStatus == false)
        {
            myHelper::ShowMessageBoxError("设备1未连接，请连接后再操作！");
            return;
        }
        m_mwlManager->resetDevice(deviceOne.DeviceId);
    });
    connect(ui->resetDeviceTwo,&QPushButton::clicked,this,[=](){
        if(deviceOneStatus == false)
        {
            myHelper::ShowMessageBoxError("设备2未连接，请连接后再操作！");
            return;
        }
        m_mwlManager->resetDevice(deviceTwo.DeviceId);
    });
    deviceOneGetDataTimer = new QTimer;
    connect(deviceOneGetDataTimer,&QTimer::timeout,this,[=](){
        m_mwlManager->requestData(deviceOne.DeviceId);
    });
    deviceTwoGetDataTimer = new QTimer;
    connect(deviceTwoGetDataTimer,&QTimer::timeout,this,[=](){
        m_mwlManager->requestData(deviceTwo.DeviceId);
    });
    // connect(ui->measurePointCodeList,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),[=](const QString& plateType)
    connect(ui->Box_detectObjOne_3,QOverload<const QString &>::of(&QComboBox::currentTextChanged),[=](const QString& detectObjOne){
        if(detectObjOne == "上端面" || detectObjOne == "下端面")
        {
            ui->label_102->setText("倾斜：");
        }
        else if(detectObjOne == "外径" || detectObjOne == "内径")
        {
            ui->label_102->setText("偏心：");
        }

    });
    connect(ui->Box_detectObjTwo_3,QOverload<const QString &>::of(&QComboBox::currentTextChanged),[=](const QString& detectObjTwo){
        if(detectObjTwo == "上端面" || detectObjTwo == "下端面")
        {
            ui->label_105->setText("倾斜：");
        }
        else if(detectObjTwo == "外径" || detectObjTwo == "内径")
        {
            ui->label_105->setText("偏心：");
        }
    });
    //采集马尔表数据------1#
    connect(ui->openSensorOneData,&QPushButton::clicked,this,[=](){
        if(deviceOneStatus == false)
        {
            myHelper::ShowMessageBoxError("设备1未连接，请连接后再操作！");
            return;
        }

        if (!isDeviceOneDataGetted) {
            // 开始获取数据
            isDeviceOneDataGetted = true;
            ui->openSensorOneData->setText("关闭");
            ui->openSensorOneData->setStyleSheet(pushButtonGreenStyle);
            initStartDataCallBackDeviceOne();
            deviceOneGetDataTimer->start(1000);
        } else {
            // 停止获取数据
            isDeviceOneDataGetted = false;
            ui->openSensorOneData->setText("开始");
            ui->openSensorOneData->setStyleSheet(hoverPushButtonStyle);
            deviceOneGetDataTimer->stop();
        }
    });
    //采集马尔表数据------2#
    connect(ui->openSensorTwoData,&QPushButton::clicked,this,[=](){
        if(deviceTwoStatus == false)
        {
            myHelper::ShowMessageBoxError("设备2未连接，请连接后再操作！");
            return;
        }

        if (!isDeviceTwoDataGetted) {
            // 开始获取数据
            isDeviceTwoDataGetted = true;
            ui->openSensorTwoData->setText("关闭");
            ui->openSensorTwoData->setStyleSheet(pushButtonGreenStyle);
            initStartDataCallBackDeviceTwo();
            deviceTwoGetDataTimer->start(1000);
        } else {
            // 停止获取数据
            isDeviceTwoDataGetted = false;
            ui->openSensorTwoData->setText("开始");
            ui->openSensorTwoData->setStyleSheet(hoverPushButtonStyle);
            deviceTwoGetDataTimer->stop();
        }
    });
    //测量结束日期修改事件
    connect(ui->endTime,&QDateEdit::dateChanged,[this](const QDate &date){
        QString startTime = ui->startTime->date().toString("yyyy-MM-dd");
        QStringList typeList;
        oper.getTypeNumByTime(typeList, startTime, date.toString("yyyy-MM-dd"));
        loadMeasureProCodeList = true;
        ui->TypeList->clear();
        ui->TypeList->addItem("选择测量型号");
        ui->TypeList->addItems(typeList);
        loadMeasureProCodeList = false;
    });
    //数据查询
    connect(ui->queryBtn,&QPushButton::clicked,this,[=](){
        QString measureTypeNum = ui->TypeList->currentText();
        if(measureTypeNum== "选择测量型号" || measureTypeNum.isNull())
        {
            myHelper::ShowMessageBoxError("请先选择测量型号");
            return;
        }
        getMeasureDataByTypeNumOne(measureTypeNum,ui->startTime->text(),ui->endTime->text());
        getMeasureDataByTypeNumTwo(measureTypeNum,ui->startTime->text(),ui->endTime->text());

    });
    //移除径向最高点
    connect(ui->removeHighestPoint_1,&QPushButton::clicked,this,[=]{
        if(myHelper::ShowMessageBoxQuesion("是否要剔除径向图最高点？") == QDialog::Accepted)
        {
            QList<QPointF> points = radialSeries->points();
            if(points.isEmpty())
            {
                myHelper::ShowMessageBoxError("点个数为0！");
                return;
            }
            auto maxElement = std::max_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
                return p1.y() < p2.y();
            });
            removedRadialHigherPoints.append(*maxElement);
            radialSeries->remove(*maxElement);
            ui->dataStaticGraphicOne->update();
        }
    });
    //移除径向最低点
    connect(ui->removeLowestPoint_1,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要剔除径向图最低点？") == QDialog::Accepted)
        {
            QList<QPointF> points = radialSeries->points();
            if(points.isEmpty())
            {
                myHelper::ShowMessageBoxError("点个数为0！");
                return;
            }
            auto minElement = std::min_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
                return p1.y() < p2.y();
            });
            removedRadialLowerPoints.append(*minElement);
            radialSeries->remove(*minElement);
            ui->dataStaticGraphicOne->update();
        }

    });
    //径向返回修改
    connect(ui->undoRadialModify,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要返回修改？") == QDialog::Accepted)
        {
            if(removedRadialHigherPoints.isEmpty() && removedRadialLowerPoints.isEmpty())
            {
                myHelper::ShowMessageBoxError("没有可撤回的点！");
                return;
            }
            // 找到最高点（y 值最大的点）
            QPointF highestPoint = removedRadialHigherPoints.first();
            foreach (const QPointF &point, removedRadialHigherPoints) {
                if (point.y() > highestPoint.y()) {
                    highestPoint = point;
                }
            }

            // 找到最低点（y 值最小的点）
            QPointF lowestPoint = removedRadialLowerPoints.first();
            foreach (const QPointF &point, removedRadialLowerPoints) {
                if (point.y() < lowestPoint.y()) {
                    lowestPoint = point;
                }
            }

            // 从 removedPoints 中移除最高点和最低点
            removedRadialHigherPoints.removeAll(highestPoint);
            removedRadialLowerPoints.removeAll(lowestPoint);

            // 恢复最高点和最低点到原来的位置
            QList<QPointF> points = radialSeries->points();
            int insertIndexHighest = 0;
            int insertIndexLowest = 0;
            for (; insertIndexHighest < points.size(); ++insertIndexHighest) {
                if (points[insertIndexHighest].x() > highestPoint.x()) {
                    break;
                }
            }
            for (; insertIndexLowest < points.size(); ++insertIndexLowest) {
                if (points[insertIndexLowest].x() > lowestPoint.x()) {
                    break;
                }
            }
            radialSeries->insert(insertIndexHighest, highestPoint);
            radialSeries->insert(insertIndexLowest, lowestPoint);

            ui->dataStaticGraphicOne->update();
        }
    });
    //移除轴向最高点
    connect(ui->removeHighestPoint_2,&QPushButton::clicked,this,[=]{
        if(myHelper::ShowMessageBoxQuesion("是否要剔除轴向图最高点？") == QDialog::Accepted)
        {
            QList<QPointF> points = axialSeries->points();
            if(points.isEmpty())
            {
                myHelper::ShowMessageBoxError("点个数为0！");
                return;
            }
            auto maxElement = std::max_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
                return p1.y() < p2.y();
            });
            removedAxialHigherPoints.append(*maxElement);
            axialSeries->remove(*maxElement);
            ui->dataStaticGraphicTwo->update();
        }

    });
    //移除轴向最低点
    connect(ui->removeLowestPoint_2,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要剔除轴向图最低点？") == QDialog::Accepted)
        {
            QList<QPointF> points = axialSeries->points();
            if(points.isEmpty())
            {
                myHelper::ShowMessageBoxError("点个数为0！");
                return;
            }
            auto minElement = std::min_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
                return p1.y() < p2.y();
            });
            removedAxialLowerPoints.append(*minElement);
            axialSeries->remove(*minElement);
            ui->dataStaticGraphicTwo->update();
        }

    });
    //轴向返回修改
    connect(ui->undoAxialModify,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否轴向返回修改？") == QDialog::Accepted)
        {
            if(removedAxialHigherPoints.isEmpty() && removedAxialHigherPoints.isEmpty())
            {
                myHelper::ShowMessageBoxError("没有可撤回的点！");
                return;
            }
            // 找到最高点
            QPointF highestPoint = removedAxialHigherPoints.first();
            foreach (const QPointF &point, removedAxialHigherPoints) {
                if (point.y() > highestPoint.y()) {
                    highestPoint = point;
                }
            }

            // 找到最低点
            QPointF lowestPoint = removedAxialLowerPoints.first();
            foreach (const QPointF &point, removedAxialLowerPoints) {
                if (point.y() < lowestPoint.y()) {
                    lowestPoint = point;
                }
            }

            // 从 removedPoints 中移除最高点和最低点
            removedAxialHigherPoints.removeAll(highestPoint);
            removedAxialLowerPoints.removeAll(lowestPoint);

            // 恢复最高点和最低点到原来的位置
            QList<QPointF> points = axialSeries->points();
            int insertIndexHighest = 0;
            int insertIndexLowest = 0;
            for (; insertIndexHighest < points.size(); ++insertIndexHighest) {
                if (points[insertIndexHighest].x() > highestPoint.x()) {
                    break;
                }
            }
            for (; insertIndexLowest < points.size(); ++insertIndexLowest) {
                if (points[insertIndexLowest].x() > lowestPoint.x()) {
                    break;
                }
            }
            axialSeries->insert(insertIndexHighest, highestPoint);
            axialSeries->insert(insertIndexLowest, lowestPoint);

            ui->dataStaticGraphicTwo->update();
        }
    });
    //保存径向数据
    connect(ui->saveRadialCurrentData,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要保存当前径向数据？") == QDialog::Accepted)
        {
            getSensorOneMaxValue();
            getSensorOneMaxAngle();
            getSensorOneMinValue();
            getSensorOneMinAngle();
            getRadialBeatValue();
            getRadialBeatAngle();
            getRadialEccentricAngle();
            getRadialEccentricValue();
            radialEvaluate(ui->sensorOneMaxValue->value(),ui->sensorOneMaxAngle->value(),
                           ui->sensorOneMinValue->value(),ui->sensorOneMinAngle->value());
        }
    });
    connect(ui->saveAxialCurrentData,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要保存当前轴向数据？") == QDialog::Accepted)
        {
            getSensorTwoMaxValue();
            getSensorTwoMaxAngle();
            getSensorTwoMinValue();
            getSensorTwoMinAngle();
            getAxialBeatValue();
            getAxialBeatAngle();
            getAxialEccentricAngle();
            getAxialEccentricValue();
            axialEvaluate(ui->sensorTwoMaxValue->value(),ui->sensorTwoMaxAngle->value(),
                          ui->sensorTwoMinValue->value(),ui->sensorTwoMinAngle->value());
        }
    });
    //1#起点确认
    connect(ui->startMakesure_1,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = radialSeries->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        // 找到x坐标最小的点作为起点
        QPointF startPoint = points.at(0);
        for (const auto& point : points)
        {
            if (point.x() < startPoint.x())
            {
                startPoint = point;
            }
        }
        ui->startAndEndPoint_1->setText(QString("起点：(%1, %2)").arg(QString::number(startPoint.x(),'f',1)).arg(QString::number(startPoint.y(),'f',1)));
    });
    //1#终点确认
    connect(ui->endMakesure_1,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = radialSeries->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        // 找到x坐标最大的点作为终点
        QPointF endPoint = points.last();
        for (const auto& point : points)
        {
            if (point.x() > endPoint.x())
            {
                endPoint = point;
            }
        }
        ui->startAndEndPoint_1->setText(QString("终点：(%1, %2)").arg(QString::number(endPoint.x(),'f',1)).arg(QString::number(endPoint.y(),'f',1)));
    });
    //2#起点确认
    connect(ui->startMakesure_2,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = axialSeries->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        // 找到x坐标最小的点作为起点
        QPointF startPoint = points.at(0);
        for (const auto& point : points)
        {
            if (point.x() < startPoint.x())
            {
                startPoint = point;
            }
        }
        ui->startAndEndPoint_2->setText(QString("起点：(%1, %2)").arg(QString::number(startPoint.x(),'f',1)).arg(QString::number(startPoint.y(),'f',1)));
    });
    //2#终点确认
    connect(ui->endMakesure_2,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = axialSeries->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        // 找到x坐标最大的点作为终点
        QPointF endPoint = points.last();
        for (const auto& point : points)
        {
            if (point.x() > endPoint.x())
            {
                endPoint = point;
            }
        }
        ui->startAndEndPoint_2->setText(QString("终点：(%1, %2)").arg(QString::number(endPoint.x(),'f',1)).arg(QString::number(endPoint.y(),'f',1)));
    });
    //清除起点/终点信息
    connect(ui->clearRadialPoint,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要删除当前起点/终点信息？") == QDialog::Accepted)
        {
            ui->startAndEndPoint_1->clear();
        }
    });
    connect(ui->clearAxialPoint,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要删除当前起点/终点信息？") == QDialog::Accepted)
        {
            ui->startAndEndPoint_2->clear();
        }
    });
    //修改参数和锁定参数设置
    connect(ui->Btn_modifyAndlockParams_3,&QPushButton::clicked,this,[=](){
        if(isLocked)
        {
            // 解锁控件
            ui->Edit_fileName_3->setEnabled(true);
            ui->Edit_typeNum_3->setEnabled(true);
            ui->Edit_unitNumber_3->setEnabled(true);
            ui->Edit_detectContentOne_3->setEnabled(true);
            ui->Box_detectObjOne_3->setEnabled(true);
            ui->Edit_detectContentTwo_3->setEnabled(true);
            ui->Edit_status_3->setEnabled(true);
            ui->Box_detectObjTwo_3->setEnabled(true);
            ui->Edit_smaplingPoints_3->setEnabled(true);
            ui->Btn_modifyAndlockParams_3->setText("锁定\n参数");
            isLocked = false;
        }
        else
        {
            // 锁定控件
            ui->Edit_fileName_3->setEnabled(false);
            ui->Edit_typeNum_3->setEnabled(false);
            ui->Edit_unitNumber_3->setEnabled(false);
            ui->Edit_detectContentOne_3->setEnabled(false);
            ui->Box_detectObjOne_3->setEnabled(false);
            ui->Edit_detectContentTwo_3->setEnabled(false);
            ui->Edit_status_3->setEnabled(false);
            ui->Box_detectObjTwo_3->setEnabled(false);
            ui->Edit_smaplingPoints_3->setEnabled(false);

            // 修改按钮文本
            ui->Btn_modifyAndlockParams_3->setText("修改\n参数");

            // 更新按钮状态
            isLocked = true;
        }
    });
    //提交参数
    connect(ui->Btn_submitParams_3,&QPushButton::clicked,this,[=](){
        if(ui->Edit_fileName_3->text().isEmpty() || ui->Edit_typeNum_3->text().isEmpty() || ui->Edit_unitNumber_3->text().isEmpty()
                || ui->Edit_status_3->text().isEmpty() || ui->Edit_detectContentOne_3->text().isEmpty() || ui->Box_detectObjOne_3->currentText().isEmpty()
                || ui->Edit_detectContentTwo_3->text().isEmpty() || ui->Box_detectObjTwo_3->currentText().isEmpty() || ui->Edit_smaplingPoints_3->text().isEmpty())
        {
            myHelper::ShowMessageBoxError("请填写完整的参数信息！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要提交当前参数？") == QDialog::Accepted)
        {
            QVector<ParametersInfo> infos;
            ParametersInfo info;
            info.fileName = ui->Edit_fileName_3->text();
            info.typeNum = ui->Edit_typeNum_3->text();
            info.unitNum = ui->Edit_unitNumber_3->text().toInt();
            info.status = ui->Edit_status_3->text();
            info.date = ui->Edit_dateTime_3->text();
            info.detectContentOne = ui->Edit_detectContentOne_3->text();
            info.detectObjOne = ui->Box_detectObjOne_3->currentText();
            info.detectContentTwo = ui->Edit_detectContentTwo_3->text();
            info.detectObjTwo = ui->Box_detectObjTwo_3->currentText();
            info.samplingPointNum = ui->Edit_smaplingPoints_3->text().toInt();
            infos.push_back(info);
            int result = oper.saveParametersInfo(infos);
            if(0 == result)
            {
                myHelper::ShowMessageBoxInfo("提交参数成功！");
            }
            else
            {
                myHelper::ShowMessageBoxError("提交参数失败！");
            }
        }
    });

    //相对位置输入框状态
    connect(ui->checkBox,&QCheckBox::stateChanged,this,&Widget::updateLineEidtOne_solt);
    connect(ui->checkBox_2,&QCheckBox::stateChanged,this,&Widget::updateLineEidtTwo_solt);
    connect(ui->checkBox_3,&QCheckBox::stateChanged,this,&Widget::updateLineEidtThree_solt);
    connect(ui->checkBox_4,&QCheckBox::stateChanged,this,&Widget::updateLineEidtFour_solt);
    connect(ui->checkBox_5,&QCheckBox::stateChanged,this,&Widget::updateLineEidtSpeed_solt);
    connect(ui->checkBox_6,&QCheckBox::stateChanged,this,&Widget::updateLineEidtAngle_solt);
    connect(ui->checkBox_7,&QCheckBox::stateChanged,this,&Widget::updateLineEidtTrunTableSpeed_solt);
    connect(ui->modeChange,&QCheckBox::stateChanged,this,&Widget::modeChange_solt);
    //速度参数设置按钮响应
    connect(ui->Btn_writeIn,&QPushButton::clicked,[=](){
        if(speedWriteInSettingFile() == true)
        {
            myHelper::ShowMessageBoxInfo("速度参数成功写入配置文件！");
        }
    });
    connect(ui->Btn_resetSpeed,&QPushButton::clicked,this,[=](){
        ui->speed->setValue(0.0);
        ui->turnTableSpeed->setValue(0.0);
    });
    //使能
    connect(ui->enableBtn,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接");
            return;
        }
        int reply;
        if(enabled == false)
        {
            reply = myHelper::ShowMessageBoxQuesion("是否要进行使能操作？");
            if(reply == QDialog::Accepted)
            {
                int open = snap7_plc->sendBitToRobot(68,5,true,writeDbNum,"使能信号");
                if(0 == open)
                {
                    myHelper::pushButtonStyleChange("去使能",ui->enableBtn,true,true);
                    ui->enableStatus->setStyleSheet("background-color:green;color: rgb(255, 255, 255);");
                    ui->enableStatus->setText("已使能");
                    enabled = true;
                }
            }
        }
        else
        {
            reply = myHelper::ShowMessageBoxQuesion("是否要进行去使能操作？");
            if(reply == QDialog::Accepted)
            {
                int close = snap7_plc->sendBitToRobot(68,5,false,writeDbNum,"去使能信号");
                if(0 == close)
                {
                    myHelper::pushButtonStyleChange("使能",ui->enableBtn,true,true);
                    ui->enableStatus->setStyleSheet("background-color:#F42215;color: rgb(255, 255, 255);");
                    ui->enableStatus->setText("未使能");
                    enabled = false;
                }
            }
        }
    });
    //气缸压紧
    connect(ui->cylinderCompression,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }

        if(!isCompressed) // 如果未压紧
        {
            int reply = myHelper::ShowMessageBoxQuesion("压紧气缸前请确认零件是否找正！");
            if(reply == QDialog::Accepted)
            {
                snap7_plc->sendBitToRobot(3, 6, false, writeDbNum,"气缸压紧信号发送！");
                ui->cylinderCompression->setText("松开气缸");
                ui->cylinderStatus->setStyleSheet("background-color:green;color: rgb(255, 255, 255);");
                ui->cylinderStatus->setText("已压紧");
                isCompressed = true;

            }
        }
        else if(isCompressed) // 如果已经压紧
        {
            int reply = myHelper::ShowMessageBoxQuesion("是否确定要松开气缸？");
            if(reply == QDialog::Accepted)
            {
                snap7_plc->sendBitToRobot(3, 6, true, writeDbNum,"气缸松开信号发送！");
                ui->cylinderCompression->setText("压紧气缸");
                ui->cylinderStatus->setStyleSheet("background-color:red;color: rgb(255, 255, 255);");
                ui->cylinderStatus->setText("已松开");
                isCompressed = false;

            }
        }
    });
    //角度置零
    connect(ui->Btn_angleClear,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要将角度置零？") == QDialog::Accepted)
        {
            snap7_plc->sendBitToRobot(2, 1, true, writeDbNum,"角度清零信号发送！");
            QTimer::singleShot(100,[this](){
                snap7_plc->sendBitToRobot(2, 1, false, writeDbNum,"角度清零信号置零！");
            });
        }
    });
    //气浮转台主轴停止
    connect(ui->Btn_mainAxisStop,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要停止气浮转台主轴？") == QDialog::Accepted)
        {
            snap7_plc->sendBitToRobot(2, 2, true, writeDbNum,"气浮转台主轴停止信号发送！");
            radialReadDataTimer->stop();
            axialReadDataTimer->stop();
            myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,true,true);
            QTimer::singleShot(100,[this](){
                snap7_plc->sendBitToRobot(2, 2, false, writeDbNum,"气浮转台主轴停止信号发送清空！");
            });
        }
    });
    //所有轴暂停
    //    connect(ui->Btn_allAxisStop,&QPushButton::clicked,this,[=](){
    //        if(controllerInitStatus == false)
    //        {
    //            myHelper::ShowMessageBoxError("控制系统未连接！");
    //            return;
    //        }
    //        if(myHelper::ShowMessageBoxQuesion("是否要暂停所有轴？") == QDialog::Accepted)
    //        {
    //            snap7_plc->sendBitToRobot(2, 3, true, writeDbNum,"所有轴暂停信号发送！");
    //            QTimer::singleShot(100,[this](){
    //                snap7_plc->sendBitToRobot(2, 3, false, writeDbNum,"所有轴暂停信号发送清空！");
    //            });
    //        }
    //    });
    //主控制界面上升按钮响应
    connect(ui->Btn_up,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要进行上升操作？上升前请检查垫板尺寸是否合适！") == QDialog::Accepted)
        {
            if(ui->commutatorRangeOne->value() + ui->relativePos->value() > 20.1)
            {
                myHelper::ShowMessageBoxError("输入相对位置值过大！请重新输入");
                return;
            }
            if(isChanged == true)
            {
                myHelper::ShowMessageBoxError("请先关闭控制调试模式！");
                return;
            }
            //发送速度
            float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
            byte speedBuffer[4];
            S7_SetRealAt(speedBuffer,0,axisSpeed);
            snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"主控制界面上升速度写入！");
            //发送相对位置
            double axisPos = ui->relativePos->value();
            byte posBuffer[4];
            S7_SetRealAt(posBuffer,0,static_cast<float>(axisPos));
            snap7_plc->DBWrite(writeDbNum,48,4,&posBuffer,"主控制界面升降轴相对位置写入！");
            //发送上升信号
            snap7_plc->sendBitToRobot(52,0,true,writeDbNum,"发送升降轴启动信号！");
            myHelper::pushButtonStyleChange("上升",ui->Btn_up,false,false);
            myHelper::pushButtonStyleChange("下降",ui->Btn_down,false,true);
            myHelper::pushButtonStyleChange("置零",ui->Btn_clear,false,true);
            myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,false,true);
            isUpping = true;
            //100ms后发送清除信号
            QTimer::singleShot(100,[this](){
                snap7_plc->sendBitToRobot(52,0,false,writeDbNum,"发送升降轴启动信号清空！");
            });
        }

    });
    //主控制界面下降按钮响应
    connect(ui->Btn_down,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要进行下降操作？下降前请检查垫板尺寸是否合适！") == QDialog::Accepted)
        {
            if(ui->commutatorRangeOne->value() - ui->relativePos->value() < -0.5)
            {
                myHelper::ShowMessageBoxError("输入相对位置值过小！请重新输入");
                return;
            }
            if(isChanged == true)
            {
                myHelper::ShowMessageBoxError("请先关闭控制调试模式！");
                return;
            }
            //发送速度
            float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
            byte speedBuffer[4];
            S7_SetRealAt(speedBuffer,0,axisSpeed);
            snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"主控制界面下降速度写入！");
            //发送相对位置
            double axisPos = ui->relativePos->value();
            byte posBuffer[4];
            S7_SetRealAt(posBuffer,0,static_cast<float>(-axisPos));
            snap7_plc->DBWrite(writeDbNum,48,4,&posBuffer,"主控制界面下降相对位置写入！");
            //发送上升信号
            snap7_plc->sendBitToRobot(52,0,true,writeDbNum,"发送下降信号!");
            myHelper::pushButtonStyleChange("上升",ui->Btn_up,false,true);
            myHelper::pushButtonStyleChange("下降",ui->Btn_down,false,false);
            myHelper::pushButtonStyleChange("置零",ui->Btn_clear,false,true);
            myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,false,true);
            isDowning = true;
            //100ms后发送清除信号
            QTimer::singleShot(100,[this](){
                snap7_plc->sendBitToRobot(52,0,false,writeDbNum,"发送下降升信号清空!");
            });
        }

    });
    //清空相对位置
    connect(ui->Btn_clear,&QPushButton::clicked,this,[=](){
        ui->relativePos->setValue(0.0);
    });
    //清空图上的点位
    connect(ui->clearPoints,&QPushButton::clicked,this,[=](){
        if(myHelper::ShowMessageBoxQuesion("是否要清除当前波形图的点？") == QDialog::Accepted)
        {
            radialSeries->clear();
            axialSeries->clear();
        }

    });

    //气浮转台启动 ********************
    connect(ui->turnTableRun,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(ui->Edit_fileName_3->text().isEmpty() || ui->Edit_typeNum_3->text().isEmpty() ||
                ui->Edit_unitNumber_3->text().isEmpty() || ui->Edit_status_3->text().isEmpty()
                || ui->Edit_detectContentOne_3->text().isEmpty() || ui->Edit_detectContentTwo_3->text().isEmpty()
                || ui->Edit_smaplingPoints_3->text().isEmpty())
        {
            myHelper::ShowMessageBoxError("请输入完整的参数！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要进行转动气浮转台操作？") == QDialog::Accepted)
        {
            ui->angle->setValue(0);


            //发送角度
            float tableAngle = ui->setAngle->value();
            byte angleBuffer[4];
            S7_SetRealAt(angleBuffer,0,tableAngle);
            snap7_plc->DBWrite(writeDbNum,34,4,&angleBuffer,"气浮转台转动角度写入！");
            //发送速度
            float tableSpeed = myHelper::readSettings("SpeedSetting/tableSpeed").toFloat();
            byte speedBuffer[4];
            S7_SetRealAt(speedBuffer,0,tableSpeed);
            snap7_plc->DBWrite(writeDbNum,38,4,&speedBuffer,"气浮转台转动速度写入！");
            //发送转台转动信号
            snap7_plc->sendBitToRobot(68,4,true, writeDbNum,"发送转台转动信号！");
            myHelper::pushButtonStyleChange("正在转动",ui->turnTableRun,false,false);
            radialReadDataTimer->start(58);
            axialReadDataTimer->start(58);
            //100ms后发送转台转动置为false
            QTimer::singleShot(100,[this](){
                snap7_plc->sendBitToRobot(68,4,false, writeDbNum,"发送转台转动信号清空！");

            });
        }

    });
    //报警复位
    connect(ui->clearWarning,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        snap7_plc->sendBitToRobot(3,4,true,writeDbNum,"报警复位信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(3,4,false,writeDbNum,"报警复位信号！");
        });
    });
    //===============================调试界面===============================
    //1号电机调试上升
    connect(ui->Btn_up_1,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送1号电机相对位置
        double electricOnePos = ui->ElectricRelativePosOne->value();
        byte posOneBuffer[4];
        S7_SetRealAt(posOneBuffer,0,static_cast<float>(electricOnePos));
        snap7_plc->DBWrite(writeDbNum,54,4,&posOneBuffer,"发送1号电机上升相对位置！");
        //发送1号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送1号电机上升速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,0,true,writeDbNum,"发送1号电机上升启动信号！");

        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,0,false,writeDbNum,"发送1号电机上升启动信号清空！");
        });
    });
    //1号电机调试下降
    connect(ui->Btn_down_1,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送1号电机相对位置
        double electricOnePos = ui->ElectricRelativePosOne->value();
        byte posOneBuffer[4];
        S7_SetRealAt(posOneBuffer,0,static_cast<float>(-electricOnePos));
        snap7_plc->DBWrite(writeDbNum,54,4,&posOneBuffer,"发送1号电机下降相对位置！");
        //发送1号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送1号电机下降速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,0,true,writeDbNum,"发送1号电机下降启动信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,0,false,writeDbNum,"发送1号电机下降启动信号清空！");
        });
    });
    //1号电机调试回原点
    connect(ui->returnOriginOne,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        snap7_plc->sendBitToRobot(68,1,true,writeDbNum,"1号电机调试回原点启动信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(68,1,false,writeDbNum,"1号电机调试回原点启动信号清空！");
        });
    });
    //2号电机调试上升
    connect(ui->Btn_up_2,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送2号电机相对位置
        double electricTwoPos = ui->ElectricRelativePosTwo->value();
        byte posTwoBuffer[4];
        S7_SetRealAt(posTwoBuffer,0,static_cast<float>(electricTwoPos));
        snap7_plc->DBWrite(writeDbNum,58,4,&posTwoBuffer,"发送2号电机上升相对位置！");
        //发送2号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送2号电机上升速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,1,true,writeDbNum,"2号电机调试启动信号！");

        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,1,false,writeDbNum,"2号电机调试启动信号清空！");
        });
    });
    //2号电机调试下降
    connect(ui->Btn_down_2,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送2号电机相对位置
        double electricTwoPos = ui->ElectricRelativePosTwo->value();
        byte posTwoBuffer[4];
        S7_SetRealAt(posTwoBuffer,0,static_cast<float>(-electricTwoPos));
        snap7_plc->DBWrite(writeDbNum,58,4,&posTwoBuffer,"发送2号电机下降相对位置！");
        //发送2号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送2号电机下降速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,1,true,writeDbNum,"2号电机调试启动信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,1,false,writeDbNum,"2号电机调试启动信号清空！");
        });
    });
    //2号电机调试回原点
    connect(ui->returnOriginTwo,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        snap7_plc->sendBitToRobot(68,2,true,writeDbNum,"2号电机调试回原点启动信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(68,2,false,writeDbNum,"2号电机调试回原点启动信号清空！");
        });
    });

    //3号电机调试上升
    connect(ui->Btn_up_3,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送3号电机相对位置
        double electricThreePos = ui->ElectricRelativePosThree->value();
        byte posThreeBuffer[4];
        S7_SetRealAt(posThreeBuffer,0,static_cast<float>(electricThreePos));
        snap7_plc->DBWrite(writeDbNum,62,4,&posThreeBuffer,"发送3号电机上升相对位置！");
        //发送3号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送3号电机上升速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,2,true,writeDbNum,"发送3号电机启动信号！");

        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,2,false,writeDbNum,"发送3号电机启动信号清空！");

        });
    });
    //3号电机调试下降
    connect(ui->Btn_down_3,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送3号电机相对位置
        double electricThreePos = ui->ElectricRelativePosThree->value();
        byte posThreeBuffer[4];
        S7_SetRealAt(posThreeBuffer,0,static_cast<float>(-electricThreePos));
        snap7_plc->DBWrite(writeDbNum,62,4,&posThreeBuffer,"发送3号电机下降相对位置！");
        //发送3号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer,"发送3号电机下降速度！");
        //发送触发信号
        snap7_plc->sendBitToRobot(42,2,true,writeDbNum,"发送3号电机启动信号！");

        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(42,2,false,writeDbNum,"发送3号电机启动信号清空！");
        });
    });
    //3号电机调试回原点
    connect(ui->returnOriginThree,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        snap7_plc->sendBitToRobot(68,3,true,writeDbNum,"3号电机调试回原点启动信号！");
        QTimer::singleShot(100,[this](){
            snap7_plc->sendBitToRobot(68,3,false,writeDbNum,"3号电机调试回原点启动信号清空！");
        });
    });

    //#1数据导出
    connect(ui->exportDataOne,&QPushButton::clicked,this,[=](){
        // 打开 Excel 文件
        QXlsx::Document xlsx;
        //设置居中格式
        QXlsx::Format centerAlignFormat, leftFormat;
        centerAlignFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        centerAlignFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        leftFormat.setHorizontalAlignment(QXlsx::Format::AlignLeft);

        xlsx.write(1,1,"-----测量参数-----",leftFormat);
        xlsx.setColumnWidth(1,1,20);
        xlsx.write(2,1,QString("文件名：%1").arg(ui->Edit_fileName_3->text()),leftFormat);
        xlsx.setColumnWidth(2,1,20);

        xlsx.write(3,1,QString("型号：%1").arg(ui->Edit_typeNum_3->text()),leftFormat);
        xlsx.setColumnWidth(3,1,20);

        xlsx.write(4,1,QString("台份号：%1").arg(ui->Edit_unitNumber_3->text()),leftFormat);
        xlsx.setColumnWidth(4,1,20);

        xlsx.write(5,1,QString("状态：%1").arg(ui->Edit_status_3->text()),leftFormat);
        xlsx.setColumnWidth(5,1,20);

        xlsx.write(7,1,QString("测量内容：%1").arg(ui->Edit_detectContentOne_3->text()),leftFormat);
        xlsx.setColumnWidth(7,1,20);

        xlsx.write(8,1,QString("测量对象：%1").arg(ui->Box_detectObjOne_3->currentText()),leftFormat);
        xlsx.setColumnWidth(8,1,20);

        xlsx.write(9,1,QString("日期：%1").arg(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss")),leftFormat);
        xlsx.setColumnWidth(9,1,40);

        xlsx.write(10,1,"-----测量结果-----",leftFormat);
        xlsx.setColumnWidth(10,1,20);
        xlsx.write(11,1,QString("最大值：%1mm∠%2°").arg(ui->sensorOneMaxValue->value()).arg(ui->sensorOneMaxAngle->value()),leftFormat);
        xlsx.setColumnWidth(11,1,35);

        xlsx.write(12,1,QString("最小值：%1mm∠%2°").arg(ui->sensorOneMinValue->value()).arg(ui->sensorOneMinAngle->value()),leftFormat);
        xlsx.setColumnWidth(12,1,35);

        xlsx.write(13,1,QString("跳动：%1mm").arg(ui->sensorOneBeatValue->value()),leftFormat);
        xlsx.setColumnWidth(13,1,35);

        xlsx.write(14,1,QString("偏心：%1mm∠%2°").arg(ui->sensorOneBiasValue->value()).arg(ui->sensorOneBiasAngle->value()),leftFormat);
        xlsx.setColumnWidth(14,1,35);

        xlsx.write(17,1,"-----原始数据-----",leftFormat);
        xlsx.setColumnWidth(17,1,20);
        int columnCount = ui->result_tableWidget_1->columnCount();
        int rowCount = ui->result_tableWidget_1->rowCount();
        QStringList headerLabels;
        for(int column = 0; column < columnCount; ++column)
        {
            QTableWidgetItem* headerItem = ui->result_tableWidget_1->horizontalHeaderItem(column);
            if (headerItem != nullptr) {
                QString headerText = headerItem->text();
                headerLabels.append(headerText);

                xlsx.write(18, column + 1, headerText, centerAlignFormat);
            }
        }
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QTableWidgetItem* item = ui->result_tableWidget_1->item(row, column);
                if (item != nullptr) {
                    QString text = item->text();
                    xlsx.write(row + 19, column + 1, text.toDouble(), centerAlignFormat);
                }
            }
        }
        // 弹出文件保存对话框并导出Excel文件
        QString filePath = QFileDialog::getSaveFileName(this, "保存Excel文件", "", "Excel 文件 (*.xlsx)");
        if (!filePath.isEmpty()) {
            if (xlsx.saveAs(filePath)) {
                myHelper::ShowMessageBoxInfo("导出成功！");
            } else {
                myHelper::ShowMessageBoxError("导出失败！");
            }
        }
    });
    //#2数据导出
    connect(ui->exportDataTwo,&QPushButton::clicked,this,[=](){
        // 打开 Excel 文件
        QXlsx::Document xlsx;
        //设置居中格式
        QXlsx::Format centerAlignFormat, leftFormat;
        centerAlignFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        centerAlignFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        leftFormat.setHorizontalAlignment(QXlsx::Format::AlignLeft);

        xlsx.write(1,1,"-----测量参数-----",leftFormat);
        xlsx.setColumnWidth(1,1,20);
        xlsx.write(2,1,QString("文件名：%1").arg(ui->Edit_fileName_3->text()),leftFormat);
        xlsx.setColumnWidth(2,1,20);

        xlsx.write(3,1,QString("型号：%1").arg(ui->Edit_typeNum_3->text()),leftFormat);
        xlsx.setColumnWidth(3,1,20);

        xlsx.write(4,1,QString("台份号：%1").arg(ui->Edit_unitNumber_3->text()),leftFormat);
        xlsx.setColumnWidth(4,1,20);

        xlsx.write(5,1,QString("状态：%1").arg(ui->Edit_status_3->text()),leftFormat);
        xlsx.setColumnWidth(5,1,20);

        xlsx.write(7,1,QString("测量内容：%1").arg(ui->Edit_detectContentTwo_3->text()),leftFormat);
        xlsx.setColumnWidth(7,1,20);

        xlsx.write(8,1,QString("测量对象：%1").arg(ui->Box_detectObjTwo_3->currentText()),leftFormat);
        xlsx.setColumnWidth(8,1,20);

        xlsx.write(9,1,QString("日期：%1").arg(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss")),leftFormat);
        xlsx.setColumnWidth(9,1,40);

        xlsx.write(10,1,"-----测量结果-----",leftFormat);
        xlsx.setColumnWidth(10,1,20);
        xlsx.write(11,1,QString("最大值：%1mm∠%2°").arg(ui->sensorTwoMaxValue->value()).arg(ui->sensorTwoMaxAngle->value()),leftFormat);
        xlsx.setColumnWidth(11,1,35);

        xlsx.write(12,1,QString("最小值：%1mm∠%2°").arg(ui->sensorTwoMinValue->value()).arg(ui->sensorTwoMinAngle->value()),leftFormat);
        xlsx.setColumnWidth(12,1,35);

        xlsx.write(13,1,QString("跳动：%1mm").arg(ui->sensorTwoBeatValue->value()),leftFormat);
        xlsx.setColumnWidth(13,1,35);

        xlsx.write(14,1,QString("偏心：%1mm∠%2°").arg(ui->sensorTwoBiasValue->value()).arg(ui->sensorTwoBiasAngle->value()),leftFormat);
        xlsx.setColumnWidth(14,1,35);

        xlsx.write(17,1,"-----原始数据-----",leftFormat);
        xlsx.setColumnWidth(17,1,20);
        int columnCount = ui->result_tableWidget_2->columnCount();
        int rowCount = ui->result_tableWidget_2->rowCount();
        QStringList headerLabels;
        for(int column = 0; column < columnCount; ++column)
        {
            QTableWidgetItem* headerItem = ui->result_tableWidget_2->horizontalHeaderItem(column);
            if (headerItem != nullptr) {
                QString headerText = headerItem->text();
                headerLabels.append(headerText);

                xlsx.write(18, column + 1, headerText, centerAlignFormat);
            }
        }
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QTableWidgetItem* item = ui->result_tableWidget_2->item(row, column);
                if (item != nullptr) {
                    QString text = item->text();
                    xlsx.write(row + 19, column + 1, text.toDouble(), centerAlignFormat);
                }
            }
        }
        // 弹出文件保存对话框并导出Excel文件
        QString filePath = QFileDialog::getSaveFileName(this, "保存Excel文件", "", "Excel 文件 (*.xlsx)");
        if (!filePath.isEmpty()) {
            if (xlsx.saveAs(filePath)) {
                myHelper::ShowMessageBoxInfo("导出成功！");
            } else {
                myHelper::ShowMessageBoxError("导出失败！");
            }
        }

    });
    //生成径向评定极坐标图
    connect(ui->createRadialPolar,&QPushButton::clicked,this,[=](){
        radialPolar->show();
        radialPolar->polarSeries->clear();
        RadialDataEvaluate datas;
        datas.radialPolarPoints = radialSeries->pointsVector();
        // 遍历数组并将点添加到系列中
        for (const QPointF &point : datas.radialPolarPoints) {
            radialPolar->polarSeries->append(point);

        }
        radialPolar->polarChartView->update();
        radialPolar->isCreated = true;

    });
    //生成轴向评定极坐标图
    connect(ui->createAxialPolar,&QPushButton::clicked,this,[=](){
        axialPolar->show();
        axialPolar->polarSeries->clear();
        AxialDataEvaluate datas;
        datas.axialPolarPoints = axialSeries->pointsVector();
        // 遍历数组并将点添加到系列中
        for (const QPointF &point :  datas.axialPolarPoints) {
            axialPolar->polarSeries->append(point);

        }
        axialPolar->polarChartView->update();
        axialPolar->isCreated = true;
    });

    //径向PDF导出
    connect(ui->exportRadialPDF,&QPushButton::clicked,[this](){
        if(radialPolar->isCreated == false)
        {
            myHelper::ShowMessageBoxError("请先生成极坐标图再操作！");
            return;
        }
        if(myHelper::ShowMessageBoxQuesion("是否要导出径向报告？") == QDialog::Accepted)
        {
            if(radialPDFCreate() == true)
            {
                myHelper::ShowMessageBoxInfo("导出径向报告成功！");
            }
            else
            {
                myHelper::ShowMessageBoxError("导出失败！请重新操作！");
            }
        }
    });

    //轴向PDF导出
    connect(ui->exportAxialPDF,&QPushButton::clicked,[this](){
        if(axialPolar->isCreated == false)
        {
            myHelper::ShowMessageBoxError("请先生成极坐标图再操作！");
            return;
        }

        if(myHelper::ShowMessageBoxQuesion("是否要导出轴向报告？") == QDialog::Accepted)
        {

            if(axialPDFCreate() == true)
            {
                myHelper::ShowMessageBoxInfo("导出轴向报告成功！");
            }
            else
            {
                myHelper::ShowMessageBoxError("导出失败！请重新操作！");
            }
        }
    });

}
//设置输入值的范围
void Widget::initDoubleSpinBoxInputRange()
{
    ui->speed->setMaximum(5.0);
    ui->turnTableSpeed->setMaximum(5.0);
    ui->commutatorRangeOne->setMaximum(34.7);
    ui->commutatorRangeTwo->setMaximum(34.7);
    ui->commutatorRangeThree->setMaximum(34.7);

}
//plc到上位机数据读取函数
void Widget::readControllerObject()
{
    byte readByte[25];
    snap7_plc->DBRead(readDbNum, 0, 25, readByte,"从plc中偏移量0地址读取25字节数据");// DBRead(int DBNumber, int Start, int Size 字节数, void *pUsrData)

    //AKD升降轴1,2,3当前位置
    controllerObject.axisOnePosition = S7_GetRealAt(readByte, 0);
    if(S7_GetRealAt(readByte, 0) != oneConObject.axisOnePosition)
    {
        oneConObject.axisOnePosition = S7_GetRealAt(readByte, 0);
        ui->commutatorRangeOne->setValue(static_cast<double>(oneConObject.axisOnePosition - 14.7));

    }
    controllerObject.axisTwoPosition = S7_GetRealAt(readByte, 4);
    if(S7_GetRealAt(readByte, 4) != oneConObject.axisTwoPosition)
    {
        oneConObject.axisTwoPosition = S7_GetRealAt(readByte, 4);
        ui->commutatorRangeTwo->setValue(static_cast<double>(oneConObject.axisTwoPosition - 14.7));
    }
    controllerObject.axisThreePosition = S7_GetRealAt(readByte, 8);
    if(S7_GetRealAt(readByte, 8) != oneConObject.axisThreePosition)
    {
        oneConObject.axisThreePosition = S7_GetRealAt(readByte, 8);
        ui->commutatorRangeThree->setValue(static_cast<double>(oneConObject.axisThreePosition - 14.7));

    }
    //气缸压紧状态反馈
    controllerObject.compressionStatus = S7_GetBitAt(readByte, 12, 0);
    if(S7_GetBitAt(readByte, 12, 0) != oneConObject.compressionStatus)
    {
        oneConObject.compressionStatus = S7_GetBitAt(readByte, 12, 0);

    }
    //气缸松开状态反馈
    controllerObject.loosingStatus = S7_GetBitAt(readByte, 12, 1);
    if(S7_GetBitAt(readByte, 12, 1) != oneConObject.loosingStatus)
    {
        oneConObject.loosingStatus = S7_GetBitAt(readByte, 12, 1);
    }
    //气浮转台轴定位完成
    controllerObject.turnTableLocationFinish = S7_GetBitAt(readByte,24,1);
    if(S7_GetBitAt(readByte,24,1) != oneConObject.turnTableLocationFinish)
    {
        oneConObject.turnTableLocationFinish = S7_GetBitAt(readByte,24,1);
        if(oneConObject.turnTableLocationFinish == true)
        {
            myHelper::ShowMessageBoxInfo("气浮转台轴运动完成！",true,1000);
            myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,true,true);
            QTimer::singleShot(1000,this,[this](){
                radialReadDataTimer->stop();
                axialReadDataTimer->stop();
                getSensorOneMaxAngle();
                getSensorOneMaxValue();
                getSensorOneMinAngle();
                getSensorOneMinValue();

                getRadialBeatValue();
                getRadialBeatAngle();
                getRadialEccentricValue();
                getRadialEccentricAngle();

                getSensorTwoMaxAngle();
                getSensorTwoMaxValue();
                getSensorTwoMinAngle();
                getSensorTwoMinValue();
                getAxialEccentricValue();
                getAxialEccentricAngle();
                getAxialBeatValue();
                getAxialBeatAngle();
                QTimer::singleShot(1000,this,[this](){
                    radialEvaluate(ui->sensorOneMaxValue->value(),ui->sensorOneMaxAngle->value(),ui->sensorOneMinValue->value(),ui->sensorOneMinAngle->value());
                    axialEvaluate(ui->sensorTwoMaxValue->value(),ui->sensorTwoMaxAngle->value(),ui->sensorTwoMinValue->value(),ui->sensorTwoMinAngle->value());
                });
            });

        }
    }
    //轴1定位完成信号
    controllerObject.axisOneLocationFinish = S7_GetBitAt(readByte,12,3);
    if(S7_GetBitAt(readByte,12,3) != oneConObject.axisOneLocationFinish)
    {
        oneConObject.axisOneLocationFinish = S7_GetBitAt(readByte,12,3);
    }
    //轴2定位完成信号
    controllerObject.axisTwoLocationFinish = S7_GetBitAt(readByte,12,4);
    if(S7_GetBitAt(readByte,12,4) != oneConObject.axisTwoLocationFinish)
    {
        oneConObject.axisTwoLocationFinish = S7_GetBitAt(readByte,12,4);
    }
    //轴3定位完成信号
    controllerObject.axisThreeLocationFinish = S7_GetBitAt(readByte,12,5);
    if(S7_GetBitAt(readByte,12,5) != oneConObject.axisThreeLocationFinish)
    {
        oneConObject.axisThreeLocationFinish = S7_GetBitAt(readByte,12,5);
    }
    //上升到位
    if(oneConObject.axisOneLocationFinish == true && oneConObject.axisTwoLocationFinish == true
            && controllerObject.axisThreeLocationFinish == true && isUpping == true)
    {
        myHelper::ShowMessageBoxInfo("上升运动到位！",true,1000);
        myHelper::pushButtonStyleChange("上升",ui->Btn_up,true,true);
        myHelper::pushButtonStyleChange("下降",ui->Btn_down,true,true);
        myHelper::pushButtonStyleChange("置零",ui->Btn_clear,true,true);
        myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,true,true);
        isUpping = false;

    }
    //下降到位
    if(oneConObject.axisOneLocationFinish == true && oneConObject.axisTwoLocationFinish == true
            && controllerObject.axisThreeLocationFinish == true && isDowning == true)
    {
        myHelper::ShowMessageBoxInfo("下降运动到位！",true,1000);
        myHelper::pushButtonStyleChange("上升",ui->Btn_up,true,true);
        myHelper::pushButtonStyleChange("下降",ui->Btn_down,true,true);
        myHelper::pushButtonStyleChange("置零",ui->Btn_clear,true,true);
        myHelper::pushButtonStyleChange("转台启动",ui->turnTableRun,true,true);
        isDowning = false;
    }

    //急停信号
    controllerObject.jerkStatus = S7_GetBitAt(readByte,12,6);
    if(S7_GetBitAt(readByte,12,6) != oneConObject.jerkStatus)
    {
        oneConObject.jerkStatus = S7_GetBitAt(readByte,12,6);
        if(oneConObject.jerkStatus == true)
        {
            ui->jerkStatus->setStyleSheet("background-color: #F42215;border-radius: 15px;");
        }
        if(oneConObject.jerkStatus == false)
        {
            ui->jerkStatus->setStyleSheet("background-color: rgb(26,250,41);border-radius: 15px;");
        }
    }
    //气浮转台实时角度
    controllerObject.turnTableAngel = S7_GetRealAt(readByte,14);
    if(S7_GetRealAt(readByte,14) != oneConObject.turnTableAngel)
    {
        oneConObject.turnTableAngel = S7_GetRealAt(readByte,14);
        ui->angle->setValue(fmod(oneConObject.turnTableAngel,360));
    }
    //轴存在未上使能报警
    controllerObject.warningStatus = S7_GetBitAt(readByte,18,0);
    if(S7_GetBitAt(readByte,18,0) != oneConObject.warningStatus)
    {
        oneConObject.warningStatus = S7_GetBitAt(readByte,18,0);
        if(oneConObject.warningStatus == true)
        {
            myHelper::ShowMessageBoxError("旋转轴未上使能！",true);
            ui->warningLabel->setPixmap(QPixmap(":/img/images/redAlarm.png"));
        }
        else
        {
            ui->warningLabel->setPixmap(QPixmap(":/img/images/normalAlarm.png"));
        }
    }
    //AKD升降轴高度范围显示
    controllerObject.axisHeightRange = S7_GetRealAt(readByte,20);
    if(S7_GetRealAt(readByte,20) != oneConObject.axisHeightRange)
    {
        oneConObject.axisHeightRange = S7_GetRealAt(readByte,20);
        ui->heightRange->setValue(static_cast<double>(34.7 - oneConObject.axisHeightRange));
        ui->ElectricHeightRangeOne->setValue(static_cast<double>(34.7 - oneConObject.axisHeightRange));
        ui->ElectricHeightRangeTwo->setValue(static_cast<double>(34.7 - oneConObject.axisHeightRange));
        ui->ElectricHeightRangeThree->setValue(static_cast<double>(34.7 - oneConObject.axisHeightRange));
    }
    //zhuantai gaojing  24.4   zhuantai baojing 24.3  zhuantai kongzhizhou baojing 24.5
    //转台动作反馈
    controllerObject.turnTableActionFeedBack = S7_GetBitAt(readByte,24,0);
    if(S7_GetBitAt(readByte,24,0) != oneConObject.turnTableActionFeedBack)
    {
        oneConObject.turnTableActionFeedBack = S7_GetBitAt(readByte,24,0);
    }

    controllerObject.turnTableWarning = S7_GetBitAt(readByte,24,3);
    if(S7_GetBitAt(readByte,24,3) != oneConObject.turnTableWarning)
    {
        oneConObject.turnTableWarning = S7_GetBitAt(readByte,24,3);
        if(oneConObject.turnTableWarning == true)
        {
            myHelper::ShowMessageBoxError("气浮转台报警！",true);
            ui->warningLabel->setPixmap(QPixmap(":/img/images/redAlarm.png"));
        }
        else
        {
            ui->warningLabel->setPixmap(QPixmap(":/img/images/normalAlarm.png"));
        }
    }
    controllerObject.turnTableEmergency = S7_GetBitAt(readByte,24,4);
    if(S7_GetBitAt(readByte,24,4) != oneConObject.turnTableEmergency)
    {
        oneConObject.turnTableEmergency = S7_GetBitAt(readByte,24,4);
        if(oneConObject.turnTableEmergency == true)
        {
            myHelper::ShowMessageBoxError("气浮转台告警！",true);
            ui->warningLabel->setPixmap(QPixmap(":/img/images/redAlarm.png"));
        }
        else
        {
            ui->warningLabel->setPixmap(QPixmap(":/img/images/normalAlarm.png"));
        }
    }
    controllerObject.turnTableControlAxisWarning = S7_GetBitAt(readByte,24,5);
    if(S7_GetBitAt(readByte,24,5) != oneConObject.turnTableControlAxisWarning)
    {
        oneConObject.turnTableControlAxisWarning = S7_GetBitAt(readByte,24,5);
        if(oneConObject.turnTableControlAxisWarning == true)
        {
            myHelper::ShowMessageBoxError("气浮转台控制轴报警！",true);
            ui->warningLabel->setPixmap(QPixmap(":/img/images/redAlarm.png"));
        }
        else
        {
            ui->warningLabel->setPixmap(QPixmap(":/img/images/normalAlarm.png"));
        }
    }
}

void Widget::initPushbuttonByFrame(QFrame *frame)
{
    QList<QPushButton *> btns = frame->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns)
    {
        connect(btn, &QPushButton::clicked, this, &Widget::initPushbuttonByFrame_slot);
    }
}

void Widget::initPushbuttonByFrame_slot()
{
    QPushButton *btn = (QPushButton *)sender();
    QString objName = btn->objectName();
    //切换主页
    if(objName == "Btn_mainPage")
    {
        ui->stackedWidget->setCurrentWidget(ui->mainPage);
        changeWidgetButtonStyle(ui->Btn_mainPage);
    }
    //切换评估页面
    if(objName == "Btn_Evaluate")
    {
        ui->stackedWidget->setCurrentWidget(ui->evaluatePage);
        changeWidgetButtonStyle(ui->Btn_Evaluate);
    }
    //切换控制页面
    if(objName == "Btn_controller_3")
    {
        ui->stackedWidget->setCurrentWidget(ui->controlPage);
        changeWidgetButtonStyle(ui->Btn_controller_3);
    }
    //切换到控制调试页面
    if(objName == "Btn_controlDebug")
    {
        if(isChanged == false)
        {
            myHelper::ShowMessageBoxError("请先切换到控制调试模式！");
            return;
        }
        limitform = new LimitForm();
        limitform->setAttribute(Qt::WA_DeleteOnClose);
        limitform->setWindowModality(Qt::ApplicationModal);
        connect(limitform,&LimitForm::hideCurrentWindow,[=](){
            ui->stackedWidget->setCurrentWidget(ui->controlDebugPage);
            changeWidgetButtonStyle(ui->Btn_controlDebug);
        });
        limitform->show();

    }
    //切换用户管理页面
    if(objName == "Btn_userManage")
    {
        if(isAdmin() == false)
        {
            myHelper::ShowMessageBoxError("只有管理员账号才可以操作！");
            return;
        }
        ui->stackedWidget->setCurrentWidget(ui->userPage);
        changeWidgetButtonStyle(ui->Btn_userManage);
        QVector<User> users;
        oper.queryAllUser(users);
        showUserTable(users);
    }
    //切换到数据管理页面
    if(objName == "Btn_dataQuery")
    {
        ui->stackedWidget->setCurrentWidget(ui->dataQuery);
        changeWidgetButtonStyle(ui->Btn_dataQuery);
    }
    //用户管理-- 新增按钮
    if(objName == "addUserBtn")
    {
        isShowUserTable = true;
        int rowCount = ui->userTable->rowCount();
        ui->userTable->insertRow(rowCount);

        QTableWidgetItem *item0 = new QTableWidgetItem();
        item0->setFlags(item0->flags() & 33);
        item0->setTextAlignment(Qt::AlignCenter);
        item0->setText(QString::number(rowCount + 1));

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setTextAlignment(Qt::AlignCenter);
        item1->setText("");

        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setTextAlignment(Qt::AlignCenter);
        item2->setText("1");

        ui->userTable->setItem(rowCount, 0, item0);
        ui->userTable->setItem(rowCount, 1, item1);
        ui->userTable->setItem(rowCount, 2, item2);

        QStringList roleList;
        roleList.append("管理员");
        roleList.append("操作员");
        //        roleList.append("技术员");
        QComboBox *box = new QComboBox();
        box->addItems(roleList);
        ui->userTable->setCellWidget(rowCount, 3, box);

        QCheckBox *check = new QCheckBox();
        check->setText("是否可用");
        check->setChecked(true);
        ui->userTable->setCellWidget(rowCount, 4, check);

        ui->userTable->clearSelection();
        ui->userTable->setCurrentItem(nullptr);

        isShowUserTable = false;
    }
    //用户管理-- 删除按钮
    if(objName == "deleteUserBtn")
    {
        int currentRow = ui->userTable->currentRow();
        if(currentRow == -1)
        {
            myHelper::ShowMessageBoxInfo("选择需要删除的行");
        }
        else
        {
            //删除数据库
            if((ui->userTable->item(currentRow, 1) != nullptr) && (!ui->userTable->item(currentRow, 1)->text().remove(QRegExp("\\s")).isEmpty()))
            {
                oper.deleteUser(ui->userTable->item(currentRow, 1)->text());
            }
            ui->userTable->removeRow(currentRow);
            ui->userTable->clearSelection();
            ui->userTable->setCurrentItem(nullptr);
        }

    }
    //用户管理-- 保存按钮
    if(objName == "saveUserBtn")
    {
        QVector<User> users;
        for(int i = 0; i < ui->userTable->rowCount(); i++)
        {
            User user;
            for(int j = 1; j < ui->userTable->columnCount(); j++)
            {
                if(j == 1)
                {
                    if(ui->userTable->item(i, j) == nullptr ||  ui->userTable->item(i, j)->text().remove(QRegExp("\\s")).isEmpty())
                    {
                        myHelper::ShowMessageBoxError("账号不能为空");
                        return;
                    }
                    else
                    {
                        user.name = ui->userTable->item(i, j)->text();
                    }
                }
                if(j == 2)
                {
                    if(ui->userTable->item(i, j) == nullptr ||  ui->userTable->item(i, j)->text().remove(QRegExp("\\s")).isEmpty())
                    {
                        user.pwd = "0";
                    }else
                    {
                        user.pwd = ui->userTable->item(i, j)->text();
                    }
                }
                if(j == 3)
                {
                    QComboBox *box = dynamic_cast<QComboBox*>(ui->userTable->cellWidget(i, j));
                    user.role = box->currentText();
                }
                if(j == 4)
                {
                    QCheckBox *box = (QCheckBox*)ui->userTable->cellWidget(i, j);
                    user.is_used = box->isChecked() ? "1" : "0";
                }

            }
            users.push_back(user);
        }
        int ok = oper.saveUsers(users);
        if(ok == 0)
        {
            myHelper::ShowMessageBoxInfo("保存成功");
            QVector<User> users;
            oper.queryAllUser(users);
            showUserTable(users);
            ui->userTable->clearSelection();
            ui->userTable->setCurrentItem(nullptr);
        }else
        {
            myHelper::ShowMessageBoxError("保存失败");
        }

    }

}

void Widget::changeWidgetButtonStyle(QPushButton *checkButton)
{
    QList<QPushButton *> btns = ui->frame_4->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        if(btn == checkButton){
            btn->setStyleSheet(pushButtonGreenStyle);
        }else{
            btn->setStyleSheet(defaultPushButtonStyle);
            btn->setStyleSheet(hoverPushButtonStyle);
            btn->setStyleSheet(disablePushButtonStyle);
        }
    }
}

void Widget::showUserTable(QVector<User> users)
{
    isShowUserTable = true;
    int row = users.size();
    int col = ui->userTable->columnCount();
    ui->userTable->setRowCount(row);
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(j == 0 || j == 1 || j == 2){
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                if(j == 0){
                    item->setFlags(item->flags() & 33);
                    item->setText(QString::number(i + 1));
                }
                if(j == 1){
                    item->setFlags(item->flags() & 33);
                    item->setText(users[i].name);
                }
                if(j == 2){
                    item->setText(users[i].pwd);
                }
                ui->userTable->setItem(i, j, item);
            }else{
                if(j == 3){
                    QStringList roleList;
                    roleList.append("管理员");
                    roleList.append("操作员");
                    QComboBox *box = new QComboBox();
                    box->addItems(roleList);
                    box->setCurrentText(users[i].role);
                    ui->userTable->setCellWidget(i, j, box);
                }
                if(j == 4){
                    QCheckBox *check = new QCheckBox();
                    check->setText("是否可用");
                    if(users[i].is_used.compare("0") == 0){
                        check->setChecked(false);
                    }else{
                        check->setChecked(true);
                    }
                    ui->userTable->setCellWidget(i, j, check);
                }
            }
        }
    }
    isShowUserTable = false;
}

bool Widget::isAdmin()
{
    if(currentUser.role == "管理员"){//管理员
        return true;
    }else{//操作员
        return false;
    }
}

bool Widget::isTechnician()
{
    if(currentUser.role == "技术员"){//管理员
        return true;
    }else{//操作员
        return false;
    }
}
//初始化1号马尔表实时曲线
void Widget::initRadialChart()
{
    radialChart = ui->dataStaticGraphicOne->chart();
    radialScatterSeries = new QScatterSeries();
    radialXAxis = new QValueAxis();
    radialYAxis = new QValueAxis();
    radialSeries = new QLineSeries();
    radialSeries->setPointsVisible(true);
    radialScatterSeries->setName("马尔表1#实时曲线");
    radialScatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    radialScatterSeries->setBorderColor(QColor(255, 0, 0)); //离散点边框颜色
    radialScatterSeries->setBrush(QBrush(QColor(255, 0, 0)));//离散点背景色
    radialScatterSeries->setMarkerSize(2); //离散点大小
    radialScatterSeries->setUseOpenGL(true);

    QPen radialSeriesPen = radialSeries->pen();
    radialSeriesPen.setWidth(10);
    radialSeriesPen.setBrush(QBrush(QColor(20,230,20)));
    radialSeriesPen.setColor(QColor(20,230,20));

    radialXAxis->setRange(radialMinX,radialMaxX);//125:111
    radialXAxis->setTickCount(radialTickCountX);
    radialXAxis->setLabelFormat("%d"); //设置刻度的格式

    radialYAxis->setRange(radialMinY,radialMaxY);//88.8 90
    radialYAxis->setTickCount(radialTickCountY);
    radialYAxis->setLabelFormat("%d"); //设置刻度的格式
    radialChart->addSeries(radialScatterSeries);
    radialChart->setAxisX(radialXAxis, radialScatterSeries);
    radialChart->setAxisY(radialYAxis, radialScatterSeries);
    radialChart->addSeries(radialSeries);
    radialChart->setAxisX(radialXAxis, radialSeries);
    radialChart->setAxisY(radialYAxis, radialSeries);

    foreach (QAbstractSeries *series, radialChart->series()){
        if(series->type() == QAbstractSeries::SeriesTypeLine){
            foreach (QLegendMarker* marker, radialChart->legend()->markers(series)){
                marker->setVisible(false);
            }
        }
    }
}
//初始化2号马尔表实时曲线
void Widget::initAxialChart()
{
    axialChart = ui->dataStaticGraphicTwo->chart();
    axialScatterSeries = new QScatterSeries();
    axialXAxis = new QValueAxis();
    axialYAxis = new QValueAxis();
    axialSeries = new QLineSeries();
    axialSeries->setPointsVisible(true);
    axialScatterSeries->setName("马尔表#2实时曲线");
    axialScatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    axialScatterSeries->setBorderColor(QColor(255, 0, 0)); //离散点边框颜色
    axialScatterSeries->setBrush(QBrush(QColor(255, 0, 0)));//离散点背景色
    axialScatterSeries->setMarkerSize(2); //离散点大小
    axialScatterSeries->setUseOpenGL(true);

    QPen axialSeriesPen = axialSeries->pen();
    axialSeriesPen.setWidth(10);
    axialSeriesPen.setBrush(QBrush(QColor(20,230,20)));
    axialSeriesPen.setColor(QColor(20,230,20));

    axialXAxis->setRange(radialMinX,radialMaxX);//125:111
    axialXAxis->setTickCount(radialTickCountX);
    axialXAxis->setLabelFormat("%d"); //设置刻度的格式

    axialYAxis->setRange(radialMinY,radialMaxY);//88.8 90
    axialYAxis->setTickCount(radialTickCountY);
    axialYAxis->setLabelFormat("%d"); //设置刻度的格式
    axialChart->addSeries(axialScatterSeries);
    axialChart->setAxisX(axialXAxis, axialScatterSeries);
    axialChart->setAxisY(axialYAxis, axialScatterSeries);
    axialChart->addSeries(axialSeries);
    axialChart->setAxisX(axialXAxis, axialSeries);
    axialChart->setAxisY(axialYAxis, axialSeries);

    foreach (QAbstractSeries *series, axialChart->series()){
        if(series->type() == QAbstractSeries::SeriesTypeLine){
            foreach (QLegendMarker* marker, axialChart->legend()->markers(series)){
                marker->setVisible(false);
            }
        }
    }
}
//获取1#马尔表采集数据最大Y值
void Widget::getSensorOneMaxValue()
{
    QList<QPointF> points = radialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    radialMaxValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() > radialMaxValue)
        {
            radialMaxValue = point.y();
        }
    }
    ui->sensorOneMaxValue->setValue(static_cast<double>(radialMaxValue));
}
//获取1#马尔表最大角度
void Widget::getSensorOneMaxAngle()
{
    QList<QPointF> points = radialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    radialMaxAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() > radialMaxAngle)
        {
            radialMaxAngle = point.x();
        }
    }
    ui->sensorOneMaxAngle->setValue(static_cast<double>(radialMaxAngle));
}
//获取1#马尔表采集数据最小Y值
void Widget::getSensorOneMinValue()
{
    QList<QPointF> points = radialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal radialMinValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() < radialMinValue)
        {
            radialMinValue = point.y();
        }
    }
    ui->sensorOneMinValue->setValue(static_cast<double>(radialMinValue));

}
//获取1#马尔表最小角度
void Widget::getSensorOneMinAngle()
{
    QList<QPointF> points = radialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    radialMinAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() < radialMinAngle)
        {
            radialMinAngle = point.x();
        }
    }
    ui->sensorOneMinAngle->setValue(static_cast<double>(radialMinAngle));
}
//获取径向跳动值
void Widget::getRadialBeatValue()
{

    if(ui->sensorOneMaxValue->value() < 0 || ui->sensorOneMinValue->value() < 0)
    {
        radialbeatValue = fabs(ui->sensorOneMaxValue->value()) + fabs(ui->sensorOneMinValue->value());
    }
    if(ui->sensorOneMaxValue->value() < 0 && ui->sensorOneMinValue->value() < 0)
    {
        radialbeatValue = fabs(fabs(ui->sensorOneMaxValue->value())-fabs(ui->sensorOneMinValue->value()));
    }
    if(ui->sensorOneMaxValue->value() > 0 && ui->sensorOneMinValue->value() > 0)
    {
        radialbeatValue = fabs(fabs(ui->sensorOneMaxValue->value())-fabs(ui->sensorOneMinValue->value()));
    }
    ui->sensorOneBeatValue->setValue(radialbeatValue);
}
//获取径向跳动角
void Widget::getRadialBeatAngle()
{
    radialbeatAngle = ui->sensorOneMaxAngle->value() - ui->sensorOneMinAngle->value();
    ui->sensorOneBeatAngle->setValue(radialbeatAngle);
}
//获取径向偏心值
void Widget::getRadialEccentricValue()
{
    if(ui->sensorOneMaxValue->value() < 0 || ui->sensorOneMinValue->value() < 0)
    {
        radialEccentricValue = fabs(ui->sensorOneMaxValue->value()) + fabs(ui->sensorOneMinValue->value());
    }
    if(ui->sensorOneMaxValue->value() < 0 && ui->sensorOneMinValue->value() < 0)
    {
        radialEccentricValue = fabs(fabs(ui->sensorOneMaxValue->value())-fabs(ui->sensorOneMinValue->value()));
    }
    if(ui->sensorOneMaxValue->value() > 0 && ui->sensorOneMinValue->value() > 0)
    {
        radialEccentricValue = fabs(fabs(ui->sensorOneMaxValue->value())-fabs(ui->sensorOneMinValue->value()));
    }
    ui->sensorOneBiasValue->setValue(radialEccentricValue);
}
//获取径向偏心角
void Widget::getRadialEccentricAngle()
{
    radialEccentricAngle = ui->sensorOneMaxAngle->value() - ui->sensorOneMinAngle->value();
    ui->sensorOneBiasAngle->setValue(radialEccentricAngle);
}
//获取2#马尔表采集数据最大Y值
void Widget::getSensorTwoMaxValue()
{
    QList<QPointF> points = axialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    axialMaxValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() > axialMaxValue)
        {
            axialMaxValue = point.y();
        }
    }
    ui->sensorTwoMaxValue->setValue(static_cast<double>(axialMaxValue));
}
//获取2#马尔表最大角度
void Widget::getSensorTwoMaxAngle()
{
    QList<QPointF> points = axialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    axialMaxAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() > axialMaxAngle)
        {
            axialMaxAngle = point.x();
        }
    }
    ui->sensorTwoMaxAngle->setValue(static_cast<double>(axialMaxAngle));
}
//获取2#马尔表采集数据最小Y值
void Widget::getSensorTwoMinValue()
{
    QList<QPointF> points = axialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    axialMinValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() < axialMinValue)
        {
            axialMinValue = point.y();
        }
    }
    ui->sensorTwoMinValue->setValue(static_cast<double>(axialMinValue));
}
//获取2#马尔表最小角度
void Widget::getSensorTwoMinAngle()
{
    QList<QPointF> points = axialSeries->points();
    if(points.isEmpty())
    {
        return;
    }
    axialMinAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() < axialMinAngle)
        {
            axialMinAngle = point.x();
        }
    }
    ui->sensorTwoMinAngle->setValue(static_cast<double>(axialMinAngle));
}
//获取轴向跳动值
void Widget::getAxialBeatValue()
{
    if(ui->sensorTwoMaxValue->value() < 0 || ui->sensorTwoMinValue->value() < 0)
    {
        axialbeatValue = fabs(ui->sensorTwoMaxValue->value()) + fabs(ui->sensorTwoMinValue->value());
    }
    if(ui->sensorTwoMaxValue->value() < 0 && ui->sensorTwoMinValue->value() < 0)
    {
        axialbeatValue = fabs(fabs(ui->sensorTwoMaxValue->value()) - fabs(ui->sensorTwoMinValue->value()));
    }
    if(ui->sensorTwoMaxValue->value() > 0 && ui->sensorTwoMinValue->value() > 0)
    {
        axialbeatValue = fabs(fabs(ui->sensorTwoMaxValue->value()) - fabs(ui->sensorTwoMinValue->value()));
    }
    ui->sensorTwoBeatValue->setValue(axialbeatValue);
}
//获取轴向跳动角
void Widget::getAxialBeatAngle()
{
    axialbeatAngle = ui->sensorTwoMaxAngle->value() - ui->sensorTwoMinAngle->value();
    ui->sensorTwoBeatAngle->setValue(axialbeatAngle);
}
//获取轴向偏心值
void Widget::getAxialEccentricValue()
{
    if(ui->sensorTwoMaxValue->value() < 0 || ui->sensorTwoMinValue->value() < 0)
    {
        axialEccentricValue = fabs(ui->sensorTwoMaxValue->value()) + fabs(ui->sensorTwoMinValue->value());
    }
    if(ui->sensorTwoMaxValue->value() < 0 && ui->sensorTwoMinValue->value() < 0)
    {
        axialEccentricValue = fabs(fabs(ui->sensorTwoMaxValue->value()) - fabs(ui->sensorTwoMinValue->value()));
    }
    if(ui->sensorTwoMaxValue->value() > 0 && ui->sensorTwoMinValue->value() > 0)
    {
        axialEccentricValue = fabs(fabs(ui->sensorTwoMaxValue->value()) - fabs(ui->sensorTwoMinValue->value()));
    }
    ui->sensorTwoBiasValue->setValue(axialEccentricValue);
}
//获取轴向偏心角
void Widget::getAxialEccentricAngle()
{
    axialEccentricAngle = ui->sensorTwoMaxAngle->value() - ui->sensorTwoMinAngle->value();
    ui->sensorTwoBiasAngle->setValue(axialEccentricAngle);
}

void Widget::setTips()
{
    ui->Btn_mainPage->setToolTip("点击跳转到主页");
    ui->Btn_controller_3->setToolTip("点击跳转到控制页面");
    ui->Btn_Evaluate->setToolTip("点击跳转到结果评定页面");
    ui->Btn_userManage->setToolTip("点击跳转到用户管理页面");
    ui->Btn_controlDebug->setToolTip("点击跳转到控制调试页面");
    ui->Btn_dataQuery->setToolTip("点击跳转数据查询页面");
}
//添加点到径向测量统计图
void Widget::addPointToRadialChart(double radialValue, double radialAngle)
{
    QPointF point(radialAngle,radialValue);
    radialSeries->append(point);

    if(radialSeries->points().size() == ui->Edit_smaplingPoints_3->text().toInt())
    {
        radialReadDataTimer->stop();
    }
    ui->dataStaticGraphicOne->update();

}
//添加点到轴向测量统计图
void Widget::addPointToAxialChart(double axialValue, double axialAngle)
{
    QPointF point(axialAngle,axialValue);
    axialSeries->append(point);
    //        splineSeries2->append(points);
    if(axialSeries->points().size() == ui->Edit_smaplingPoints_3->text().toInt())
    {
        axialReadDataTimer->stop();
    }
    ui->dataStaticGraphicTwo->update();
    //    splineView2->update();
}
//径向评定
void Widget::radialEvaluate(double maxValue,double maxAngle,double minValue,double minAngle)
{
    RadialDataEvaluate radialData;
    radialData.typeNum = ui->Edit_typeNum_3->text();
    radialData.status = ui->Edit_status_3->text();
    radialData.detectContent = ui->Edit_detectContentOne_3->text();
    radialData.dates = ui->Edit_dateTime_3->text();
    radialData.detectObj = ui->Box_detectObjOne_3->currentText();
    radialData.unitNum = ui->Edit_unitNumber_3->text().toInt();
    radialData.maxValue = maxValue;
    radialData.maxAngle = maxAngle;
    radialData.minValue = minValue;
    radialData.minAngle = minAngle;
    radialData.beatValue = radialbeatValue;
    radialData.beatAngle = radialbeatAngle;
    radialData.eccentricity = radialEccentricValue;
    radialData.eccentricAngle = radialEccentricAngle;
    radialData.verticalValue = radialbeatValue;
    radialData.verticalAngle = maxAngle + minAngle;
    radialData.concentricity = radialbeatValue;
    radialData.concentriticAngle = maxAngle + minAngle;
    radialData.flatness = radialbeatValue;
    radialData.roundness = radialbeatValue;
    ui->radialTypeNum->setText(radialData.typeNum);
    ui->radialStatus->setText(radialData.status);
    ui->radialDetectContent->setText(radialData.detectContent);
    ui->radialDate->setText(radialData.dates);
    ui->radialDetectObj->setText(radialData.detectObj);
    ui->radialUnitNum->setText(QString::number(radialData.unitNum));
    ui->radialEvaluateMaxValue->setValue(radialData.maxValue);
    ui->radialEvaluateMaxAngle->setValue(radialData.maxAngle);
    ui->radialEvaluateMinValue->setValue(radialData.minValue);
    ui->radialEvaluateMinAngle->setValue(radialData.minAngle);
    ui->radialEvaluateBeatValue->setValue(radialData.beatValue);
    ui->radialEvaluateBeatAngle->setValue(radialData.beatAngle);
    ui->radialEccentricity->setValue(radialData.eccentricity);
    ui->radialEccentricAngle->setValue(radialData.eccentricAngle);
    ui->radialEvaluateVerticalValue->setValue(radialData.verticalValue);
    ui->radialEvaluateVerticalAngle->setValue(radialData.verticalAngle);
    ui->radialConcentricity->setValue(radialData.concentricity);
    ui->radialConcentricAngle->setValue(radialData.concentriticAngle);
    ui->radialFlatness->setValue(radialData.flatness);
    ui->radialRoundness->setValue(radialData.roundness);

}
//轴向评定
void Widget::axialEvaluate(double maxValue, double maxAngle, double minValue, double minAngle)
{
    AxialDataEvaluate axialData;
    axialData.typeNum = ui->Edit_typeNum_3->text();
    axialData.status = ui->Edit_status_3->text();
    axialData.detectContent = ui->Edit_detectContentTwo_3->text();
    axialData.dates = ui->Edit_dateTime_3->text();
    axialData.detectObj = ui->Box_detectObjTwo_3->currentText();
    axialData.unitNum = ui->Edit_unitNumber_3->text().toInt();
    axialData.maxValue = maxValue;
    axialData.maxAngle = maxAngle;
    axialData.minValue = minValue;
    axialData.minAngle = minAngle;
    axialData.beatValue = axialbeatValue;
    axialData.beatAngle = axialbeatAngle;
    axialData.eccentricity = axialEccentricValue;
    axialData.eccentricAngle = axialEccentricAngle;
    axialData.verticalValue = axialbeatValue;
    axialData.verticalAngle = maxAngle + minAngle;
    axialData.concentricity = axialbeatValue;
    axialData.concentriticAngle = maxAngle + minAngle;
    axialData.flatness = axialbeatValue;
    axialData.roundness = axialbeatValue;
    ui->axialTypeNum->setText(axialData.typeNum);
    ui->axialStatus->setText(axialData.status);
    ui->axialDetectContent->setText(axialData.detectContent);
    ui->axialDate->setText(axialData.dates);
    ui->axialDetectObj->setText(axialData.detectObj);
    ui->axialUnitNum->setText(QString::number(axialData.unitNum));
    ui->axialEvaluateMaxValue->setValue(axialData.maxValue);
    ui->axialEvaluateMaxAngle->setValue(axialData.maxAngle);
    ui->axialEvaluateMinValue->setValue(axialData.minValue);
    ui->axialEvaluateMinAngle->setValue(axialData.minAngle);
    ui->axialEvaluateBeatValue->setValue(axialData.beatValue);
    ui->axialEvaluateBeatAngle->setValue(axialData.beatAngle);
    ui->axialEccentricity->setValue(axialData.eccentricity);
    ui->axialEccentricAngle->setValue(axialData.eccentricAngle);
    ui->axialEvaluateVerticalValue->setValue(axialData.verticalValue);
    ui->axialEvaluateVerticalAngle->setValue(axialData.verticalAngle);
    ui->axialConcentricity->setValue(axialData.concentricity);
    ui->axialConcentricAngle->setValue(axialData.concentriticAngle);
    ui->axialFlatness->setValue(axialData.flatness);
    ui->axialRoundness->setValue(axialData.roundness);

}

void Widget::on_userTable_itemChanged(QTableWidgetItem *item)
{
    if(isShowUserTable == true){
        return;
    }
    QString name = nullptr;
    if(item == nullptr ||  item->text().remove(QRegExp("\\s")).isEmpty()){
        return;
    }else{
        name = item->text();
        for(int i = 0; i < ui->userTable->rowCount(); i++){
            if(ui->userTable->item(i, 1) == nullptr ||  ui->userTable->item(i, 1)->text().remove(QRegExp("\\s")).isEmpty() || item->row() == i){
                continue;
            }else{
                if(name.compare(ui->userTable->item(i, 1)->text()) == 0){
                    myHelper::ShowMessageBoxInfo("当前账号已经存在");
                    item->setText(" ");
                    return;
                }
            }
        }
    }
}

void Widget::updateLineEidtOne_solt(int state)
{
    ui->relativePos->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtTwo_solt(int state)
{
    ui->ElectricRelativePosOne->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtThree_solt(int state)
{
    ui->ElectricRelativePosTwo->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtFour_solt(int state)
{
    ui->ElectricRelativePosThree->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtSpeed_solt(int state)
{
    ui->speed->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtAngle_solt(int state)
{
    ui->setAngle->setEnabled(state == Qt::Checked);
}

void Widget::updateLineEidtTrunTableSpeed_solt(int state)
{
    ui->turnTableSpeed->setEnabled(state == Qt::Checked);
}
//模式切换
void Widget::modeChange_solt(int state)
{
    if(controllerInitStatus == false)
    {
        ui->modeChange->setCheckable(false);
        return;
    }
    int modeNum;
    if(state == Qt::Checked)
    {
        modeNum = 2;
        byte modeBuffer[2];
        S7_SetIntAt(modeBuffer,0,modeNum);
        int res = snap7_plc->DBWrite(writeDbNum,66,2,&modeBuffer,"启动控制调试模式！");
        if(0 == res)
        {
            isChanged = true;
        }
    }
    else
    {
        modeNum = 1;
        byte modeBuffer[2];
        S7_SetIntAt(modeBuffer,0,modeNum);
        int res = snap7_plc->DBWrite(writeDbNum,66,2,&modeBuffer,"关闭控制调试模式！");
        if(0 == res)
        {
            isChanged = false;
        }
    }
}
//径向PDF报告打印
bool Widget::radialPDFCreate()
{

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString file_path = QFileDialog::getSaveFileName(this, "Save PDF File", desktopPath, "PDF Files (*.pdf)");
    if(file_path.isEmpty())
    {
        return false;
    }
    QFile pdfFile(file_path);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter* pWriter = new QPdfWriter(&pdfFile);

    //Init Page
    pWriter->setPageSize(QPagedPaintDevice::A4);
    pWriter->setResolution(300);    //设置dpi 每个平方英寸像素为300
    pWriter->setPageMargins(QMarginsF(30, 30, 30, 30));

    QPainter* pPainter = new QPainter(pWriter);
    //Init Font
    QFont font[5]={QFont("宋体",26,60),QFont("宋体",26,61),QFont("宋体",26,QFont::Normal),QFont("宋体",26,QFont::Normal),QFont("宋体",26,QFont::Normal)};
    font[0].setPixelSize(120);
    font[1].setPixelSize(75);
    font[2].setPixelSize(50);
    font[3].setPixelSize(50);
    font[4].setPixelSize(54);

    //Painter PDF
    int nPDFWidth = pPainter->viewport().width();

    //在50%的头部居中显示
    int y=50;
    pPainter->setFont(font[0]);
    pPainter->drawText(QRect(0,y, nPDFWidth, 100), Qt::AlignCenter, "低压涡轮转静子装配座检测报告");
    y+=140;
    pPainter->setPen(QPen(QBrush(QColor(0,0,0)),5));
    pPainter->drawLine(0,y,nPDFWidth,y);
    pPainter->drawLine(0,y+18,nPDFWidth,y+18);

    y+=60;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("型号: %1").arg(ui->Edit_typeNum_3->text()));

    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(nPDFWidth/2+100, y, nPDFWidth/2-100, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("台份号: %1").arg(ui->Edit_unitNumber_3->text()));

    y+=80;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("状态: %1").arg(ui->Edit_status_3->text()));

    y+=100;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("测量内容: %1").arg(ui->Edit_detectContentOne_3->text()));
    QString bias;
    if(ui->Box_detectObjOne_3->currentText() == "上端面" || ui->Box_detectObjOne_3->currentText() == "下端面")
    {
        bias = QString("%1mm∠%2°").arg(ui->sensorOneBiasValue->value()).arg(ui->sensorOneBiasAngle->value());
    }
    if(ui->Box_detectObjOne_3->currentText() == "内径" || ui->Box_detectObjOne_3->currentText() == "外径")
    {
        bias = "偏心无倾斜参数";
    }
    if(ui->Box_detectObjOne_3->currentText() == "无" )
    {
        bias = "";
    }
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(nPDFWidth/2+100,y, nPDFWidth/2-100, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("倾斜: %1").arg(bias));

    y+=120;
    QString beat;
    if(ui->Box_detectObjOne_3->currentText() == "上端面" || ui->Box_detectObjOne_3->currentText() == "下端面")
    {
        beat = "端面无偏心参数";
    }
    if(ui->Box_detectObjOne_3->currentText() == "内径" || ui->Box_detectObjOne_3->currentText() == "外径")
    {
        beat = QString("%1mm∠%2°").arg(ui->sensorOneBiasValue->value()).arg(ui->sensorOneBiasAngle->value());
    }
    if(ui->Box_detectObjOne_3->currentText() == "无" )
    {
        beat = "";
    }
    QStringList list;
    list << "测量时间"<<"采样点数"<<"高点"<<"低点"<<"跳动"<<"偏心";
    list << QString("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd\nhh:mm:ss"))
         << QString("%1个").arg(ui->Edit_smaplingPoints_3->text().toInt())
         << QString("%1mm∠%2°").arg(ui->sensorOneMaxValue->value()).arg(ui->sensorOneMaxAngle->value())
         << QString("%1mm∠%2°").arg(ui->sensorOneMinValue->value()).arg(ui->sensorOneMinAngle->value())
         << QString("%1mm").arg(ui->sensorOneBeatValue->value()) << QString("%1").arg(beat);
    pdfDrawForm(pPainter,y,0,2,6,120,font[2],list);

    y+=300;
    pPainter->setFont(font[1]);
    pPainter->drawText(QRect(0,y, nPDFWidth, 80), Qt::AlignVCenter | Qt::AlignHCenter, "径 向 极 坐 标 图");

    y+=200;
    //获取界面图片
    int imageBorder=80;        //设置图片水平边距为150
    QPixmap pixmap = QPixmap::grabWidget(radialPolar->widget, radialPolar->widget->rect());
    float x = (float)(nPDFWidth-imageBorder*2)/(float)pixmap.width();
    pixmap= pixmap.scaled(nPDFWidth-imageBorder*2, x*pixmap.height(),Qt::IgnoreAspectRatio);    //根据大小比例,来放大缩小图片
    pPainter->drawPixmap(imageBorder, y, pixmap);

    y+=pixmap.height() + 10;
    QStringList memberList;
    memberList << "测量人员：" << "" << "检验人员：" << "";
    pdfDrawForm(pPainter,y,0,1,4,150,font[1],memberList);
    // pWriter->newPage(); //写下一页
    //绘制完毕
    delete pPainter;
    delete pWriter;
    pdfFile.close();
    //通过PDF阅读器来打开PDF
    bool res =  QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
    if(res == true)
    {
        return true;
    }
    else
    {
        return false;
    }

}
//轴向PDF报告打印
bool Widget::axialPDFCreate()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString file_path = QFileDialog::getSaveFileName(this, "Save PDF File", desktopPath, "PDF Files (*.pdf)");
    if(file_path.isEmpty())
    {
        return false;
    }
    QFile pdfFile(file_path);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter* pWriter = new QPdfWriter(&pdfFile);

    //Init Page
    pWriter->setPageSize(QPagedPaintDevice::A4);
    pWriter->setResolution(300);    //设置dpi 每个平方英寸像素为300
    pWriter->setPageMargins(QMarginsF(30, 30, 30, 30));

    QPainter* pPainter = new QPainter(pWriter);
    //Init Font
    QFont font[5]={QFont("宋体",26,60),QFont("宋体",26,61),QFont("宋体",26,QFont::Normal),QFont("宋体",26,QFont::Normal),QFont("宋体",26,QFont::Normal)};
    font[0].setPixelSize(120);
    font[1].setPixelSize(75);
    font[2].setPixelSize(50);
    font[3].setPixelSize(50);
    font[4].setPixelSize(54);
    //Painter PDF
    int nPDFWidth = pPainter->viewport().width();
    //    int nPDFHeight = pPainter->viewport().height();

    //在50%的头部居中显示
    int y=50;
    pPainter->setFont(font[0]);
    pPainter->drawText(QRect(0,y, nPDFWidth, 100), Qt::AlignCenter, "低压涡轮转静子装配座检测报告");
    y+=140;
    pPainter->setPen(QPen(QBrush(QColor(0,0,0)),5));
    pPainter->drawLine(0,y,nPDFWidth,y);
    pPainter->drawLine(0,y+18,nPDFWidth,y+18);

    y+=60;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("型号: %1").arg(ui->Edit_typeNum_3->text()));

    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(nPDFWidth/2+100, y, nPDFWidth/2-100, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("台份号: %1").arg(ui->Edit_unitNumber_3->text()));

    y+=80;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("状态: %1").arg(ui->Edit_status_3->text()));

    y+=100;
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(100, y, nPDFWidth/2, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("测量内容: %1").arg(ui->Edit_detectContentTwo_3->text()));
    QString bias;
    if(ui->Box_detectObjTwo_3->currentText() == "上端面" || ui->Box_detectObjTwo_3->currentText() == "下端面")
    {
        bias = QString("%1mm∠%2°").arg(ui->sensorTwoBiasValue->value()).arg(ui->sensorTwoBiasAngle->value());
    }
    if(ui->Box_detectObjTwo_3->currentText() == "内径" || ui->Box_detectObjTwo_3->currentText() == "外径")
    {
        bias = "偏心无倾斜参数";
    }
    if(ui->Box_detectObjTwo_3->currentText() == "无" )
    {
        bias = "";
    }
    pPainter->setFont(font[3]);
    pPainter->drawText(QRect(nPDFWidth/2+100,y, nPDFWidth/2-100, 70), Qt::AlignVCenter | Qt::AlignLeft, QString("倾斜: %1").arg(bias));

    y+=120;
    QString beat;
    if(ui->Box_detectObjTwo_3->currentText() == "上端面" || ui->Box_detectObjTwo_3->currentText() == "下端面")
    {
        beat = "端面无偏心参数";
    }
    if(ui->Box_detectObjTwo_3->currentText() == "内径" || ui->Box_detectObjTwo_3->currentText() == "外径")
    {
        beat = QString("%1mm∠%2°").arg(ui->sensorTwoBiasValue->value()).arg(ui->sensorTwoBiasAngle->value());
    }
    if(ui->Box_detectObjTwo_3->currentText() == "无" )
    {
        beat = "";
    }
    QStringList list;
    list << "测量时间"<<"采样点数"<<"高点"<<"低点"<<"跳动"<<"偏心";
    list << QString("%1").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd\nhh:mm:ss"))
         << QString("%1个").arg(ui->Edit_smaplingPoints_3->text().toInt())
         << QString("%1mm∠%2°").arg(ui->sensorTwoMaxValue->value()).arg(ui->sensorTwoMaxAngle->value())
         << QString("%1mm∠%2°").arg(ui->sensorTwoMinValue->value()).arg(ui->sensorTwoMinAngle->value())
         << QString("%1mm").arg(ui->sensorTwoBeatValue->value()) << QString("%1").arg(beat);
    pdfDrawForm(pPainter,y,0,2,6,120,font[2],list);

    y+=300;
    pPainter->setFont(font[1]);
    pPainter->drawText(QRect(0,y, nPDFWidth, 80), Qt::AlignVCenter | Qt::AlignHCenter, "轴 向 极 坐 标 图");
    y+=200;
    //获取界面图片
    int imageBorder=100;        //设置图片水平边距为150
    QPixmap pixmap = QPixmap::grabWidget(axialPolar->widget, axialPolar->widget->rect());
    float x = (float)(nPDFWidth-imageBorder*2)/(float)pixmap.width();
    pixmap= pixmap.scaled(nPDFWidth-imageBorder*2, x*pixmap.height(),Qt::IgnoreAspectRatio);    //根据大小比例,来放大缩小图片
    pPainter->drawPixmap(imageBorder, y, pixmap);
    y+=pixmap.height()+10;

    QStringList memberList;
    memberList << "测量人员：" << "" << "检验人员：" << "";
    pdfDrawForm(pPainter,y,0,1,4,150,font[1],memberList);

    //绘制完毕
    delete pPainter;
    delete pWriter;
    pdfFile.close();

    bool res =  QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
    if(res == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//设置速度
bool Widget::speedWriteInSettingFile()
{
    myHelper::writeSettings("SpeedSetting/Speed",ui->speed->value());
    myHelper::writeSettings("SpeedSetting/tableSpeed",ui->turnTableSpeed->value());
    return true;
}

void Widget::initPushbuttonByGroup(QGroupBox *box)
{
    QList<QPushButton *> btns = box->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns)
    {
        connect(btn, &QPushButton::clicked, this, &Widget::initPushbuttonByGroup_slot);
    }
}

void Widget::initPushbuttonByGroup_slot()
{
    QPushButton *btn = (QPushButton *)sender();
    QString objName = btn->objectName();
}

bool Widget::areAllSpinboxesZero(QGroupBox *groupBox)
{
    QList<QLineEdit *> lineEdits = groupBox->findChildren<QLineEdit *>();
    // 遍历所有QLineEdit，检查文本是否为空
    for(QLineEdit *lineEdit : lineEdits)
    {
        if(lineEdit->text().isEmpty())
        {
            return false;
        }
    }
    return true;
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(isMeasuringStatus == true)
    {
        myHelper::ShowMessageBoxInfo("正在检测中,不能关闭程序");
        event->ignore();
    }
    else
    {
        if(myHelper::ShowMessageBoxQuesion("是否要关闭程序？") == QDialog::Accepted)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

void Widget::getMeasureDataByTypeNumTwo(const QString measureTypeNum, const QString startTime, const QString endTime)
{
    sensorTwoDataList.clear();
    oper.getSensorNumberTwoData(sensorTwoDataList,measureTypeNum,startTime,endTime,ui->queryNum->value());
    int row = sensorTwoDataList.size();
    ui->result_tableWidget_2->setRowCount(row);
    int col = ui->result_tableWidget_2->columnCount();
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & 33);
            if(j==0)
            {
                item->setText(QString::number(sensorTwoDataList[i].pointsNum));
            }
            if(j==1)
            {
                item->setText(QString::number(sensorTwoDataList[i].angle));
            }
            if(j==2)
            {
                item->setText(QString::number(sensorTwoDataList[i].detectData));
            }
            ui->result_tableWidget_2->setItem(i,j,item);
        }
    }
}

void Widget::getMeasureDataByTypeNumOne(const QString measureTypeNum, const QString startTime, const QString endTime)
{
    sensorOneDataList.clear();
    oper.getSensorNumberOneData(sensorOneDataList,measureTypeNum,startTime,endTime,ui->queryNum->value());
    int row = sensorOneDataList.size();
    ui->result_tableWidget_1->setRowCount(row);
    int col = ui->result_tableWidget_1->columnCount();
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & 33);
            if(j==0)
            {
                item->setText(QString::number(sensorOneDataList[i].pointsNum));
            }
            if(j==1)
            {
                item->setText(QString::number(sensorOneDataList[i].angle));
            }
            if(j==2)
            {
                item->setText(QString::number(sensorOneDataList[i].detectData));
            }
            ui->result_tableWidget_1->setItem(i,j,item);
        }
    }
}
//绘制PDF报告模板
void Widget::pdfDrawForm(QPainter *paint, int y, int horzBorder, int row, int column, int unitHeight, QFont &font, QStringList &list)
{
    paint->setFont(font);
    paint->setPen(QPen(QBrush(QColor(0,0,0)),2));
    int Width = paint->viewport().width()-horzBorder*2;
    int unitWidth = Width/column;
    for(int i=0;i<row;i++)
    {
        int x = horzBorder;
        for(int j=0;j<column;j++)
        {
            paint->drawText(QRect(x,y, unitWidth, unitHeight), Qt::AlignCenter,list[i*column+j]);
            paint->drawRect(QRect(x,y, unitWidth, unitHeight));
            x+=unitWidth;
        }
        y += unitHeight;
    }
}
