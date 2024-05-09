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
    ui->titleWidget->set_lab_Title("成都迪迈沃克光电科技有限公司");
    ui->titleWidget->setShowMaxBtn(true);
    init();
    initDoubleSpinBoxInputRange();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    initConnect();
    //    openMahrExe();
    setTips();
    ui->roleName->setText(currentUser.role);
    ui->userName->setText(currentUser.name);
    QDate measureDates = QDate::currentDate();
    ui->startTime->setDate(measureDates);
    ui->endTime->setDate(measureDates);
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setStyleSheet("QPlainTextEdit { color: red; }");
    initPushbuttonByFrame(ui->frame_4);
    initPushbuttonByFrame(ui->frame3);
    ui->Edit_dateTime_3->setText(QDate::currentDate().toString("yyyy-MM-dd"));
    ui->Edit_dateTime_3->setEnabled(false);
    ui->groupBox_radialEvaluate->setEnabled(false);
    ui->groupBox_axialEvaluate->setEnabled(false);

    initPLC();
    serialOne = new QSerialPort(this);
    serialOne->setPortName("COM1");
    serialTwo = new QSerialPort(this);
    serialTwo->setPortName("COM2");
    listenUSBOneStatusTimer = new QTimer(this);
    connect(listenUSBOneStatusTimer,&QTimer::timeout,this,[=](){
        bool connectRes = conncetSensorOneUSB();
        if(connectRes == false)
        {
            sensorOneTimer->stop();
        }
    });
    listenUSBOneStatusTimer->start(10);

    listenUSBTwoStatusTimer = new QTimer(this);
    connect(listenUSBTwoStatusTimer,&QTimer::timeout,this,[=](){
        bool connectRes = conncetSensorTwoUSB();
        if(connectRes == false)
        {
            sensorTwoTimer->stop();
        }
    });
    listenUSBTwoStatusTimer->start(10);

    changeWidgetButtonStyle(ui->Btn_mainPage);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
    dashboard1 = new myDashBoard(this);
    dashboard2 = new myDashBoard(this);
    dashboard1->m_title = "#1";
    dashboard2->m_title = "#2";
    dashboard1->setParent(ui->dashBoardOne);
    dashboard2->setParent(ui->dashBoardTwo);
    //通过定时器的方式实时显示仪表盘数据
    sensorOneTimer = new QTimer();
    connect(sensorOneTimer,&QTimer::timeout,this,[=](){
        if(serialOne->bytesAvailable() > 0)
        {
            QByteArray data = serialOne->readAll(); // 读取串口数据
            QString dataString = QString::fromUtf8(data); // 将QByteArray转换为QString
            QRegularExpression re("(\\+|\\-)([-+]?[0-9]*\\.?[0-9]+)"); // 正则表达式匹配数字部分
            QRegularExpressionMatch match = re.match(dataString);

            if (match.hasMatch())
            {
                QString sign = match.captured(1); // 提取匹配到的正负号部分
                QString extractedData = match.captured(2); // 提取匹配到的数字部分
                bool ok;
                sensorOneValue = extractedData.toDouble(&ok); // 将提取的数据转换为double类型
                if (ok)
                {
                    if(sign == "-")
                    {
                        sensorOneValue = -sensorOneValue;
                    }
                    dashboard1->setValue(sensorOneValue);
                    dashboard1->UpdateAngle();
                    ui->sensorOneData->setValue(dashboard1->m_value);
                }
                else
                {
                    qDebug() << "1#马尔表数据转换为浮点数类型失败！";
                }
            }
            else
            {
                qDebug() << "未匹配到1#马尔表的数据！";
            }
        }

    });
    sensorTwoTimer = new QTimer();
    connect(sensorTwoTimer,&QTimer::timeout,this,[=](){
        if(serialTwo->bytesAvailable() > 0)
        {
            QByteArray data = serialTwo->readAll(); // 读取串口数据
            QString dataString = QString::fromUtf8(data); // 将QByteArray转换为QString
            QRegularExpression re("(\\+|\\-)([-+]?[0-9]*\\.?[0-9]+)"); // 正则表达式匹配数字部分
            QRegularExpressionMatch match = re.match(dataString);
            if (match.hasMatch())
            {
                QString sign = match.captured(1); // 提取匹配到的正负号部分
                QString extractedData = match.captured(2); // 提取匹配到的数字部分
                bool ok;
                sensorTwoValue = extractedData.toDouble(&ok); // 将提取的数据转换为double类型
                if (ok)
                {
                    if(sign == "-")
                    {
                        sensorTwoValue = -sensorTwoValue;
                    }
                    dashboard2->setValue(sensorTwoValue);
                    dashboard2->UpdateAngle();
                    ui->sensorTwoData->setValue(dashboard2->m_value);
                }
                else
                {
                    qDebug() << "2#马尔表数据转换为浮点数类型失败！";
                }
            }
            else
            {
                qDebug() << "未匹配到2#马尔表的数据！";
            }
        }

    });
    series1 = new QLineSeries();
    series2 = new QLineSeries();
    chart1 = new QChart();
    chart2 = new QChart();
    chartView1 = new QChartView(chart1);
    chartView2 = new QChartView(chart2);
    //    initLineSeriesOne(ui->dataStaticGraphicOne,chart1,chartView1,series1,lineSeriesPointsOne,"角度","距离",360,90,10,5,Qt::blue);
    //    initLineSeriesOne(ui->dataStaticGraphicTwo,chart2,chartView2,series2,lineSeriesPointsTwo,"角度","距离",360,90,10,5,Qt::red);

    splineSeriesOneTimer = new QTimer();
    //    splineChart1 = new QChart();
    //    splineSeries1 = new QSplineSeries();
    //    splineView1 = new QChartView(splineChart1);
    splineSeriesTwoTimer = new QTimer();
    //    splineChart2 = new QChart();
    //    splineSeries2 = new QSplineSeries();
    //    splineView2 = new QChartView(splineChart2);

    initSplineSeries(ui->dataStaticGraphicOne,chart1,chartView1,series1,"角度(°)","测量数据(mm)",0,20,8,5,Qt::blue);
    initSplineSeries(ui->dataStaticGraphicTwo,chart2,chartView2,series2,"角度(°)","测量数据(mm)",0,20,8,5,Qt::red);
    splineSeriesOneTimer->setInterval(1000);
    connect(splineSeriesOneTimer,&QTimer::timeout,this,&Widget::drawSplineOneLine);

    splineSeriesTwoTimer->setInterval(1000);
    connect(splineSeriesTwoTimer,&QTimer::timeout,this,&Widget::drawSplineTwoLine);

    radialPolarChart = new QPolarChart();
    polarView1 = new QChartView(radialPolarChart);
    polarSeries1 = new QSplineSeries();
    initPolarChart(ui->radialPolarChart_2,radialPolarChart,polarView1,polarSeries1,0,360,-90,90);

    axialPolarChart = new QPolarChart();
    polarView2 = new QChartView(axialPolarChart);
    polarSeries2 = new QSplineSeries();
    initPolarChart(ui->axialPolarChart_2,axialPolarChart,polarView2,polarSeries2,0,360,-90,90);

    connect(series1,&QLineSeries::hovered,this,[=](QPointF point, bool state){
        if(state)
        {
            QToolTip::showText(QCursor::pos(),QString("(%1, %2)").arg(QString::number(point.x(),'f',1)).arg(QString::number(point.y(),'f',4)));
        }
    });
    connect(series2,&QLineSeries::hovered,this,[=](QPointF point, bool state){
        if(state)
        {
            QToolTip::showText(QCursor::pos(),QString("(%1, %2)").arg(QString::number(point.x(),'f',1)).arg(QString::number(point.y(),'f',4)));
        }

    });

    connect(polarSeries1,&QSplineSeries::hovered,this,[=](QPointF point, bool state){
        if(state)
        {
            QToolTip::showText(QCursor::pos(),QString("(%1mm,∠%2)").arg(QString::number(point.x(),'f',1)).arg(QString::number(point.y(),'f',1)),nullptr,QRect(),6000);
        }

    });
    connect(polarSeries2,&QSplineSeries::hovered,this,[=](QPointF point, bool state){
        if(state)
        {
            QToolTip::showText(QCursor::pos(),QString("(%1mm,∠%2)").arg(QString::number(point.x(),'f',1)).arg(QString::number(point.y(),'f',1)),nullptr,QRect(),6000);
        }

    });
}

void Widget::initConnect()
{
    //更新系统时间
    connect(&systemTime, &QTimer::timeout, [this](){
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ui->systemTime->setText(currentDateTime);
    });
    systemTime.start(1000);
    connect(ui->openSensorOneData,&QPushButton::clicked,this,[=](){
        if(conncetSensorOneUSB() == false)
        {
            myHelper::ShowMessageBoxError("马尔数显表#1未连接！");
            return;
        }
        if(serialOne->open(QIODevice::ReadOnly) || isStartingCollectSensorOneData)
        {
            sensorOneTimer->start(1000);
            splineSeriesOneTimer->start();
            ui->openSensorOneData->setStyleSheet(pushButtonGreenStyle);
            ui->openSensorOneData->setText("停止");
            isStartingCollectSensorOneData = false;
            mahrOneIsOpened = true;
        }
        else
        {
            sensorOneTimer->stop();
            splineSeriesOneTimer->stop();
            ui->openSensorOneData->setText("开始");
            ui->openSensorOneData->setStyleSheet(hoverPushButtonStyle);
            isStartingCollectSensorOneData = true;
            mahrOneIsOpened = false;
        }
    });

    connect(ui->openSensorTwoData,&QPushButton::clicked,this,[=](){
        if(conncetSensorTwoUSB() == false)
        {
            myHelper::ShowMessageBoxError("马尔数显表#2未连接！");
            return;
        }
        if(serialTwo->open(QIODevice::ReadOnly) || isStartingCollectSensorTwoData)
        {
            sensorTwoTimer->start(1000);
            splineSeriesTwoTimer->start();
            ui->openSensorTwoData->setStyleSheet(pushButtonGreenStyle);
            ui->openSensorTwoData->setText("停止");
            isStartingCollectSensorTwoData = false;
            mahrTwoIsOpened = true;
        }
        else
        {
            sensorTwoTimer->stop();
            splineSeriesTwoTimer->stop();
            ui->openSensorTwoData->setText("开始");
            ui->openSensorTwoData->setStyleSheet(hoverPushButtonStyle);
            isStartingCollectSensorTwoData = true;
            mahrTwoIsOpened = false;
        }

    });
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
    //移除最高点
    connect(ui->removeHighestPoint_1,&QPushButton::clicked,this,[=]{
        QList<QPointF> points = series1->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        auto maxElement = std::max_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
            return p1.y() < p2.y();
        });
        series1->remove(*maxElement);
        chartView1->update();

    });
    connect(ui->removeHighestPoint_2,&QPushButton::clicked,this,[=]{
        QList<QPointF> points = series2->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        auto maxElement = std::max_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
            return p1.y() < p2.y();
        });
        series2->remove(*maxElement);
        chartView2->update();
    });
    //移除最低点
    connect(ui->removeLowestPoint_1,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = series1->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        auto minElement = std::min_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
            return p1.y() < p2.y();
        });
        series1->remove(*minElement);
        chartView1->update();
    });
    connect(ui->removeLowestPoint_2,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = series2->points();
        if(points.isEmpty())
        {
            myHelper::ShowMessageBoxError("点个数为0！");
            return;
        }
        auto minElement = std::min_element(points.begin(),points.end(),[=](const QPointF& p1,const QPointF& p2){
            return p1.y() < p2.y();
        });
        series2->remove(*minElement);
        chartView2->update();
    });
    //1#起点确认
    connect(ui->startMakesure_1,&QPushButton::clicked,this,[=](){
        QList<QPointF> points = series1->points();
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
        QList<QPointF> points = series1->points();
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
        QList<QPointF> points = series2->points();
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
        QList<QPointF> points = series2->points();
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
    //保存数据
    connect(ui->saveData_1,&QPushButton::clicked,this,[this](){
        getSensorOneMaxAngle();
        getSensorOneMaxValue();
        getSensorOneMinAngle();
        getSensorOneMinValue();
    });
    connect(ui->saveData_2,&QPushButton::clicked,this,[this](){
        getSensorTwoMaxAngle();
        getSensorTwoMaxValue();
        getSensorTwoMinAngle();
        getSensorTwoMinValue();
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

    });
    //测试写入数据
    //    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
    //        QVector<SensorDetectDataOne> sensorNumOneDataList;
    //        SensorDetectDataOne dataList;
    //        dataList.typeNum = "测试类型3";
    //        dataList.pointsNum = 3;
    //        dataList.angle = 57.49;
    //        dataList.detectData = 24.51;
    //        dataList.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //        sensorNumOneDataList.push_back(dataList);
    //        if(0 == oper.saveSensorNumberOneData(sensorNumOneDataList))
    //        {
    //            myHelper::ShowMessageBoxInfo("测试写入数据1成功！");
    //        }
    //        else
    //        {
    //            myHelper::ShowMessageBoxError("测试写入数据1失败！");
    //        }
    //    });
    //    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
    //        QVector<SensorDetectDataTwo> sensorNumTwoDataList;
    //        SensorDetectDataTwo dataList;
    //        dataList.typeNum = "测试类型3";
    //        dataList.pointsNum = 4;
    //        dataList.angle = 64.21;
    //        dataList.detectData = 52.35;
    //        dataList.date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //        sensorNumTwoDataList.push_back(dataList);

    //        if(0 == oper.saveSensorNumberTwoData(sensorNumTwoDataList))
    //        {
    //            myHelper::ShowMessageBoxInfo("测试写入数据2成功！");
    //        }
    //        else
    //        {
    //            myHelper::ShowMessageBoxError("测试写入数据2失败！");
    //        }
    //    });
    //测试评定
    //    connect(ui->testRadial,&QPushButton::clicked,this,[=](){
    //        radialEvaluate(20.0, 50.0, 10.0, 25.0, 10.0, 25.0, 40.0, 90.0, 4.0, 35.0);
    //    });
    //    connect(ui->testAxial,&QPushButton::clicked,this,[=](){
    //        axialEvaluate(20.0, 50.0, 10.0, 25.0, 10.0, 25.0, 40.0, 90.0, 4.0, 35.0);
    //    });
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
    //气缸压紧
    connect(ui->cylinderCompression,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }

        if(!isCompressed) // 如果未压紧
        {
            int reply = myHelper::ShowMessageBoxQuesion("零件是否找正？");
            if(reply == QDialog::Accepted)
            {
                sendBitToRobot(3, 6, true);
                ui->cylinderCompression->setStyleSheet(pushButtonGreenStyle);
                ui->label_cylindercompress->setText("气缸松开");
                isCompressed = true;
                if(oneConObject.compressionStatus == true)
                {
                    myHelper::ShowMessageBoxInfo("气缸已压紧！");
                }
            }
        }
        else if(isCompressed) // 如果已经压紧
        {
            int reply = myHelper::ShowMessageBoxQuesion("是否确定要松开气缸？");
            if(reply == QDialog::Accepted)
            {
                sendBitToRobot(3, 6, false);
                ui->cylinderCompression->setStyleSheet(hoverPushButtonStyle);
                ui->label_cylindercompress->setText("气缸压紧");
                isCompressed = false;
                if(oneConObject.loosingStatus == true)
                {
                    myHelper::ShowMessageBoxInfo("气缸已松开！");
                }
            }
        }
    });

    //开始检测整体运动
    connect(ui->Btn_startDetect_3,&QPushButton::clicked,this,[=](){

        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        if(mahrOneIsOpened == false)
        {
            myHelper::ShowMessageBoxError("请提前打开马尔表#1！");
            return;
        }
        if(mahrTwoIsOpened == false)
        {
            myHelper::ShowMessageBoxError("请提前打开马尔表#2！");
            return;
        }
        if(mahrOneIsOpened == false||mahrTwoIsOpened == false)
        {
            myHelper::ShowMessageBoxError("请提前打开马尔表#1和马尔表#2！");
            return;
        }
    });
    //角度清零
    connect(ui->Btn_angleClear,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(2, 1, true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(2, 1, false);
        });

    });
    //气浮转台主轴停止
    connect(ui->Btn_mainAxisStop,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(2, 2, true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(2, 2, false);
        });
    });
    //所有轴暂停
    connect(ui->Btn_allAxisStop,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(2, 3, true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(2, 3, false);
        });
    });
    //主控制界面上升按钮响应
    connect(ui->Btn_up,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送相对位置
        double axisPos = ui->relativePos->value();
        byte posBuffer[4];
        S7_SetRealAt(posBuffer,0,static_cast<float>(axisPos));
        snap7_plc->DBWrite(writeDbNum,48,4,&posBuffer);
        //100ms后发送上升信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(52,0,true);
        });
        //100ms后发送清除信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(52,0,false);
        });
    });
    //主控制界面下降按钮响应
    connect(ui->Btn_down,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送相对位置
        double axisPos = ui->relativePos->value();
        byte posBuffer[4];
        S7_SetRealAt(posBuffer,0,static_cast<float>(-axisPos));
        snap7_plc->DBWrite(writeDbNum,48,4,&posBuffer);
        //100ms后发送上升信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(52,0,true);
        });
        //100ms后发送清除信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(52,0,false);
        });
    });
    //清空相对位置
    connect(ui->Btn_clear,&QPushButton::clicked,this,[=](){
        ui->relativePos->setValue(0.0);
    });
    //气浮转台启动
    connect(ui->turnTableRun,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        //发送角度
        double tableAngle = ui->setAngle->value();
        byte angleBuffer[4];
        S7_SetRealAt(angleBuffer,0,static_cast<float>(tableAngle));
        snap7_plc->DBWrite(writeDbNum,34,0,&angleBuffer);
        //发送速度
        float tableSpeed = myHelper::readSettings("SpeedSetting/tableSpeed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,tableSpeed);
        snap7_plc->DBWrite(writeDbNum,38,4,&speedBuffer);
        //100ms后发送转台转动信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(68,4,true);
        });
        //100ms后发送转台转动置为false
        QTimer::singleShot(100,[this](){
            sendBitToRobot(68,4,false);
        });
        if(oneConObject.turnTableLocationFinish == true)
        {
            myHelper::ShowMessageBoxInfo("气浮转台轴定位完成！");
        }
    });
    //调试界面
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
        snap7_plc->DBWrite(writeDbNum,54,4,&posOneBuffer);
        //发送1号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,0,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,0,false);
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
        snap7_plc->DBWrite(writeDbNum,54,4,&posOneBuffer);
        //发送1号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,0,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,0,false);
        });
    });
    //1号电机调试回原点
    connect(ui->returnOriginOne,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(68,1,true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(68,1,false);
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
        snap7_plc->DBWrite(writeDbNum,58,4,&posTwoBuffer);
        //发送2号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,1,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,1,false);
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
        snap7_plc->DBWrite(writeDbNum,58,4,&posTwoBuffer);
        //发送2号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,1,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,1,false);
        });
    });
    //2号电机调试回原点
    connect(ui->returnOriginTwo,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(68,2,true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(68,2,false);
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
        snap7_plc->DBWrite(writeDbNum,62,4,&posThreeBuffer);
        //发送3号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,2,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,2,false);
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
        snap7_plc->DBWrite(writeDbNum,62,4,&posThreeBuffer);
        //发送3号电机速度
        float axisSpeed = myHelper::readSettings("SpeedSetting/Speed").toFloat();
        byte speedBuffer[4];
        S7_SetRealAt(speedBuffer,0,axisSpeed);
        snap7_plc->DBWrite(writeDbNum,44,4,&speedBuffer);
        //发送触发信号
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,2,true);
        });
        QTimer::singleShot(100,[this](){
            sendBitToRobot(42,2,false);
        });
    });
    //3号电机调试回原点
    connect(ui->returnOriginThree,&QPushButton::clicked,this,[=](){
        if(controllerInitStatus == false)
        {
            myHelper::ShowMessageBoxError("控制系统未连接！");
            return;
        }
        sendBitToRobot(68,3,true);
        QTimer::singleShot(100,[this](){
            sendBitToRobot(68,3,false);
        });
    });
    //#1数据导出
    connect(ui->exportDataOne,&QPushButton::clicked,this,[=](){
        if(ui->result_tableWidget_1->rowCount() == 0)
        {
            myHelper::ShowMessageBoxError("表1没有数据可供导出！");
            return;
        }
        QXlsx::Document xlsx;
        int columnCount = ui->result_tableWidget_1->columnCount();
        int rowCount = ui->result_tableWidget_1->rowCount();

        QStringList headerLabels;
        for(int column = 0; column < columnCount; ++column)
        {
            QTableWidgetItem* headerItem = ui->result_tableWidget_1->horizontalHeaderItem(column);
            if (headerItem != nullptr) {
                QString headerText = headerItem->text();
                headerLabels.append(headerText);

                xlsx.write(1, column + 1, headerText);
            }
        }
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QTableWidgetItem* item = ui->result_tableWidget_1->item(row, column);
                if (item != nullptr) {
                    QString text = item->text();
                    xlsx.write(row + 2, column + 1, text);
                }
            }
        }
        // 设置列宽和自动筛选
        for (int column = 0; column < columnCount; ++column) {
            xlsx.setColumnWidth(column + 1, 15);
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
        if(ui->result_tableWidget_2->rowCount() == 0)
        {
            myHelper::ShowMessageBoxError("表2没有数据可供导出！");
            return;
        }
        QXlsx::Document xlsx;
        int columnCount = ui->result_tableWidget_2->columnCount();
        int rowCount = ui->result_tableWidget_2->rowCount();

        QStringList headerLabels;
        for(int column = 0; column < columnCount; ++column)
        {
            QTableWidgetItem* headerItem = ui->result_tableWidget_2->horizontalHeaderItem(column);
            if (headerItem != nullptr) {
                QString headerText = headerItem->text();
                headerLabels.append(headerText);

                xlsx.write(1, column + 1, headerText);
            }
        }
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QTableWidgetItem* item = ui->result_tableWidget_2->item(row, column);
                if (item != nullptr) {
                    QString text = item->text();
                    xlsx.write(row + 2, column + 1, text);
                }
            }
        }
        // 设置列宽和自动筛选
        for (int column = 0; column < columnCount; ++column) {
            xlsx.setColumnWidth(column + 1, 15);
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
        QPointF points[] = {
            QPointF(ui->radialEvaluateMaxValue->value(), ui->radialEvaluateMaxAngle->value()),
            QPointF(ui->radialEvaluateMinValue->value(), ui->radialEvaluateMinAngle->value()),
            QPointF(ui->radialEvaluateBeatValue->value(), ui->radialEvaluateBeatAngle->value()),
            QPointF(ui->radialEvaluateVerticalValue->value(), ui->radialEvaluateVerticalAngle->value()),
            QPointF(ui->radialConcentricity->value(), ui->radialConcentricity->value()),
            QPointF(ui->radialEccentricity->value(), ui->radialEccentricAngle->value()),
            QPointF(ui->radialFlatness->value(), ui->radialRoundness->value())
        };

        // 遍历数组并将点添加到系列中
        for (const QPointF &point : points) {
            polarSeries1->append(point);
        }

        // 更新图表以显示新添加的点
        radialPolarChart->update();
    });
    //生成轴向评定极坐标图
    connect(ui->createAxialPolar,&QPushButton::clicked,this,[=](){
        QPointF points[] = {
            QPointF(ui->axialEvaluateMaxValue->value(), ui->axialEvaluateMaxAngle->value()),
            QPointF(ui->axialEvaluateMinValue->value(), ui->axialEvaluateMinAngle->value()),
            QPointF(ui->axialEvaluateBeatValue->value(), ui->axialEvaluateBeatAngle->value()),
            QPointF(ui->axialEvaluateVerticalValue->value(), ui->axialEvaluateVerticalAngle->value()),
            QPointF(ui->axialConcentricity->value(), ui->axialConcentricity->value()),
            QPointF(ui->axialEccentricity->value(), ui->axialEccentricAngle->value()),
            QPointF(ui->axialFlatness->value(), ui->axialRoundness->value())
        };

        // 遍历数组并将点添加到系列中
        for (const QPointF &point : points) {
            polarSeries2->append(point);
        }

        // 更新图表以显示新添加的点
        radialPolarChart->update();
    });
    //径向评定数据导出
    connect(ui->exportRadialData,&QPushButton::clicked,this,[=](){
        if(areAllSpinboxesZero(ui->groupBox_radialEvaluate) == false)
        {
            myHelper::ShowMessageBoxError("径向评定数据不完整，无法导出！");
            return;
        }
        // 打开 Excel 文件
        QXlsx::Document xlsx("./评定数据/径向评定数据.xlsx");

        // 在第一行添加表头
        if (xlsx.dimension().rowCount() == 0) {
            QStringList headers = {"型号", "台份号", "状态", "测量内容", "日期",
                                   "测量对象", "最大值", "最大角", "最小值", "最小角",
                                   "跳动值", "跳动角", "垂直度", "垂直角",
                                   "同心度", "同心角", "偏心量", "偏心角",
                                   "平面度", "圆度"};
            for (int col = 1; col <= headers.size(); ++col) {
                xlsx.write(1, col, headers.at(col - 1));
            }
        }
        // 添加新数据
        int row = xlsx.dimension().rowCount() + 1;
        xlsx.write(row, 1, ui->radialTypeNum->text());
        xlsx.write(row, 2, ui->radialUnitNum->text().toInt());
        xlsx.write(row, 3, ui->radialStatus->text());
        xlsx.write(row, 4, ui->radialDetectContent->text());
        xlsx.write(row, 5, ui->radialDate->text());
        xlsx.write(row, 6, ui->radialDetectObj->text());
        xlsx.write(row, 7, ui->radialEvaluateMaxValue->value());
        xlsx.write(row, 8, ui->radialEvaluateMaxAngle->value());
        xlsx.write(row, 9, ui->radialEvaluateMinValue->value());
        xlsx.write(row, 10, ui->radialEvaluateMinAngle->value());
        xlsx.write(row, 11, ui->radialEvaluateBeatValue->value());
        xlsx.write(row, 12, ui->radialEvaluateBeatAngle->value());
        xlsx.write(row, 13, ui->radialEvaluateVerticalValue->value());
        xlsx.write(row, 14, ui->radialEvaluateVerticalAngle->value());
        xlsx.write(row, 15, ui->radialConcentricity->value());
        xlsx.write(row, 16, ui->radialConcentricAngle->value());
        xlsx.write(row, 17, ui->radialEccentricity->value());
        xlsx.write(row, 18, ui->radialEccentricAngle->value());
        xlsx.write(row, 19, ui->radialFlatness->value());
        xlsx.write(row, 20, ui->radialRoundness->value());
        if(xlsx.save())
        {
            myHelper::ShowMessageBoxInfo("导出径向评价结果成功！");
        }
        else
        {
            myHelper::ShowMessageBoxError("导出径向评价结果失败！");
        }
    });
    //轴向评定数据导出
    connect(ui->exportAxialData,&QPushButton::clicked,this,[=](){
        if(areAllSpinboxesZero(ui->groupBox_axialEvaluate) == false)
        {
            myHelper::ShowMessageBoxError("轴向评定数据不完整，无法导出！");
            return;
        }
        // 打开 Excel 文件
        QXlsx::Document xlsx("./评定数据/轴向评定数据.xlsx");

        // 在第一行添加表头
        if (xlsx.dimension().rowCount() == 0) {
            QStringList headers = {"型号", "台份号", "状态", "测量内容", "日期",
                                   "测量对象", "最大值", "最大角", "最小值", "最小角",
                                   "跳动值", "跳动角", "垂直度", "垂直角",
                                   "同心度", "同心角", "偏心量", "偏心角",
                                   "平面度", "圆度"};
            for (int col = 1; col <= headers.size(); ++col) {
                xlsx.write(1, col, headers.at(col - 1));
            }
        }
        // 添加新数据
        int row = xlsx.dimension().rowCount() + 1;
        xlsx.write(row, 1, ui->axialTypeNum->text());
        xlsx.write(row, 2, ui->axialUnitNum->text().toInt());
        xlsx.write(row, 3, ui->axialStatus->text());
        xlsx.write(row, 4, ui->axialDetectContent->text());
        xlsx.write(row, 5, ui->axialDate->text());
        xlsx.write(row, 6, ui->axialDetectObj->text());
        xlsx.write(row, 7, ui->axialEvaluateMaxValue->value());
        xlsx.write(row, 8, ui->axialEvaluateMaxAngle->value());
        xlsx.write(row, 9, ui->axialEvaluateMinValue->value());
        xlsx.write(row, 10, ui->axialEvaluateMinAngle->value());
        xlsx.write(row, 11, ui->axialEvaluateBeatValue->value());
        xlsx.write(row, 12, ui->axialEvaluateBeatAngle->value());
        xlsx.write(row, 13, ui->axialEvaluateVerticalValue->value());
        xlsx.write(row, 14, ui->axialEvaluateVerticalAngle->value());
        xlsx.write(row, 15, ui->axialConcentricity->value());
        xlsx.write(row, 16, ui->axialConcentricAngle->value());
        xlsx.write(row, 17, ui->axialEccentricity->value());
        xlsx.write(row, 18, ui->axialEccentricAngle->value());
        xlsx.write(row, 19, ui->axialFlatness->value());
        xlsx.write(row, 20, ui->axialRoundness->value());
        if(xlsx.save())
        {
            myHelper::ShowMessageBoxInfo("导出轴向评价结果成功！");
        }
        else
        {
            myHelper::ShowMessageBoxError("导出轴向评价结果失败！");
        }
    });

}

void Widget::initPLC()
{
    snap7_plc = new PLC_Siemens(this);
    snap7_plc->setConnectType(CONNTYPE_OP);//设置连接方式，避免变成软件连不上PLC了，必须在连接之前设置
    int ok = snap7_plc->connectByIP(myHelper::readSettings("GPLC/IP").toString(),
                                    myHelper::readSettings("GPLC/Rack").toInt(),
                                    myHelper::readSettings("GPLC/Slot").toInt());
    readDbNum = myHelper::readSettings("GPLC/readDB").toInt();
    writeDbNum = myHelper::readSettings("GPLC/writeDB").toInt();

    if(ok == 0){
        ui->controllerLabel->setStyleSheet(normal_radius);
        controllerInitStatus = true;

        connect(&timeReadController, &QTimer::timeout, [this](){
            readControllerObject();
        });
        timeReadController.start(10);
    }else{
        QString error = snap7_plc->ErrorText(ok);
        qDebug() << "snap7_plc connect :" << error;
        ui->controllerLabel->setStyleSheet(fail_radius);
        controllerInitStatus = false;

        ui->clearWarning->setEnabled(false);
        ui->Btn_up->setEnabled(false);
        ui->Btn_up_1->setEnabled(false);
        ui->Btn_up_2->setEnabled(false);
        ui->Btn_up_3->setEnabled(false);
        ui->Btn_down->setEnabled(false);
        ui->Btn_down_1->setEnabled(false);
        ui->Btn_down_2->setEnabled(false);
        ui->Btn_down_3->setEnabled(false);
        ui->Btn_clear->setEnabled(false);
        ui->returnOriginOne->setEnabled(false);
        ui->returnOriginTwo->setEnabled(false);
        ui->returnOriginThree->setEnabled(false);
        ui->turnTableRun->setEnabled(false);

    }

}

void Widget::sendBitToRobot(int offset, int bit, bool on_off)
{
    byte bitBool[1];
    snap7_plc->DBRead(writeDbNum, offset, 1, &bitBool);
    for(int i = 0; i < 8; i++){
        if(i == bit){
            S7_SetBitAt(bitBool, 0, i,  on_off);
        }else{
            S7_SetBitAt(bitBool, 0, i,  S7_GetBitAt(bitBool, 0, i));
        }
    }
    snap7_plc->DBWrite(writeDbNum, offset, 1, &bitBool);
}
//设置输入值的范围
void Widget::initDoubleSpinBoxInputRange()
{
    ui->heightRange->setMaximum(20.0);
    ui->speed->setMaximum(5.0);
    ui->turnTableSpeed->setMaximum(5.0);
    ui->ElectricHeightRangeOne->setMaximum(20.0);
    ui->ElectricHeightRangeTwo->setMaximum(20.0);
    ui->ElectricHeightRangeThree->setMaximum(20.0);
    ui->commutatorRangeOne->setMaximum(34.7);
    ui->commutatorRangeTwo->setMaximum(34.7);
    ui->commutatorRangeThree->setMaximum(34.7);

}

void Widget::readControllerObject()
{
    byte readByte[25];
    snap7_plc->DBRead(readDbNum, 0, 25, readByte);// DBRead(int DBNumber, int Start, int Size 字节数, void *pUsrData)

    //AKD升降轴1,2,3当前位置
    controllerObject.axisOnePosition = S7_GetRealAt(readByte, 0);
    if(S7_GetRealAt(readByte, 0) != oneConObject.axisOnePosition)
    {
        oneConObject.axisOnePosition = S7_GetRealAt(readByte, 0);
        ui->commutatorRangeOne->setValue(static_cast<double>(oneConObject.axisOnePosition));

    }
    controllerObject.axisTwoPosition = S7_GetRealAt(readByte, 4);
    if(S7_GetRealAt(readByte, 4) != oneConObject.axisTwoPosition)
    {
        oneConObject.axisTwoPosition = S7_GetRealAt(readByte, 4);
        ui->commutatorRangeTwo->setValue(static_cast<double>(oneConObject.axisTwoPosition));
    }
    controllerObject.axisThreePosition = S7_GetRealAt(readByte, 8);
    if(S7_GetRealAt(readByte, 8) != oneConObject.axisThreePosition)
    {
        oneConObject.axisThreePosition = S7_GetRealAt(readByte, 8);
        ui->commutatorRangeThree->setValue(static_cast<double>(oneConObject.axisTwoPosition));

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
    controllerObject.turnTableLocationFinish = S7_GetBitAt(readByte,12,2);
    if(S7_GetBitAt(readByte,12,2) != oneConObject.turnTableLocationFinish)
    {
        oneConObject.turnTableLocationFinish = S7_GetBitAt(readByte,12,2);
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
    //急停信号
    controllerObject.jerkStatus = S7_GetBitAt(readByte,12,6);
    if(S7_GetBitAt(readByte,12,6) != oneConObject.jerkStatus)
    {
        oneConObject.jerkStatus = S7_GetBitAt(readByte,12,6);
        if(oneConObject.jerkStatus == true)
        {
            ui->jerkStatus->setStyleSheet("background-color: #F42215;");
        }
        if(oneConObject.jerkStatus == false)
        {
            ui->jerkStatus->setStyleSheet("background-color: green;");
        }
    }
    //气浮转台实时角度
    controllerObject.turnTableAngel = S7_GetRealAt(readByte,14);
    if(S7_GetRealAt(readByte,14) != oneConObject.turnTableAngel)
    {
        oneConObject.turnTableAngel = S7_GetRealAt(readByte,14);
        ui->angle->setValue(static_cast<double>(oneConObject.turnTableAngel));
    }
    //轴存在未上使能报警
    controllerObject.warningStatus = S7_GetBitAt(readByte,18,0);
    if(S7_GetBitAt(readByte,18,0) != oneConObject.warningStatus)
    {
        oneConObject.warningStatus = S7_GetBitAt(readByte,18,0);
        if(oneConObject.warningStatus == true)
        {
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
        ui->heightRange->setValue(static_cast<double>(oneConObject.axisHeightRange));
        ui->ElectricHeightRangeOne->setValue(static_cast<double>(oneConObject.axisHeightRange));
        ui->ElectricHeightRangeTwo->setValue(static_cast<double>(oneConObject.axisHeightRange));
        ui->ElectricHeightRangeThree->setValue(static_cast<double>(oneConObject.axisHeightRange));
    }
    //转台动作反馈
    controllerObject.turnTableActionFeedBack = S7_GetBitAt(readByte,24,0);
    if(S7_GetBitAt(readByte,24,0) != oneConObject.turnTableActionFeedBack)
    {
        oneConObject.turnTableActionFeedBack = S7_GetBitAt(readByte,24,0);
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

bool Widget::isProcessRunning(const QString &processName)
{
    // 使用tasklist命令获取当前运行的进程列表，并通过grep查找特定进程
    QProcess tasklist;
    tasklist.start("tasklist", QStringList());
    tasklist.waitForFinished();

    QString output = tasklist.readAllStandardOutput();
    QRegExp regex(processName + ".*\\s+\\d+"); // 匹配进程名称及之后的空格和数字（PID）
    return regex.indexIn(output) != -1;
}

//void Widget::openMahrExe()
//{
//    // 判断MarComProf.exe是否已经在运行
//    if (!isProcessRunning("MarComProf.exe")) {
//        mharProcess = new QProcess(this);
//        mharProcess->start("E:/MarCom/MarComProf.exe");
//    } else {
//        myHelper::ShowMessageBoxError("马尔表客户端已经打开，无需再打开！");
//    }
//}

//void Widget::loadTypeNumList(QComboBox *box)
//{
//    QStringList typeNums;
//    oper.getTypeNumList(typeNums);
//    box->clear();
//    box->addItem("选择型号");
//    box->addItems(typeNums);

//}

void Widget::initLineSeriesOne(QWidget *widget, QChart *chart, QChartView *view, QLineSeries *series, QVector<QPointF> dataPoints, QString xAxisTitle, QString yAxisTitle,
                               int maxXRange, int maxYRange,int xTickCount, int yTickCount,const QColor& color)
{
    series->setPointsVisible(true);
    series->setColor(color);
    QPen pen = series->pen();
    pen.setWidth(3);
    series->setPen(pen);
    for(const QPointF& points : dataPoints)
    {
        series->append(points);
    }
    chart->legend()->hide();
    chart->addSeries(series);

    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0xd2d0d1));
    backgroundGradient.setColorAt(1.0, QRgb(0x4c4547));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(backgroundGradient);

    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 1));
    plotAreaGradient.setFinalStop(QPointF(1, 0));
    plotAreaGradient.setColorAt(0.0, QRgb(0x555555));
    plotAreaGradient.setColorAt(1.0, QRgb(0x80FF80));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart->setPlotAreaBackgroundVisible(true);

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    axisX->setRange(0, maxXRange);
    axisX->setTickCount(xTickCount);
    axisY->setRange(-90, maxYRange);
    axisY->setTickCount(yTickCount);
    axisX->setTitleText(xAxisTitle);
    axisY->setTitleText(yAxisTitle);
    // 将QValueAxis对象添加到QChart中
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    view = new QChartView(chart);
    // 创建QChartView对象，并设置QChart为其图表
    view->setRenderHint(QPainter::Antialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(view, 0, Qt::AlignBottom | Qt::AlignHCenter);
}

void Widget::initSplineSeries(QWidget *widget, QChart *chart, QChartView *view, QLineSeries *series, QString xAxisTitle,
                              QString yAxisTitle, int axisYmin, int axisYmax, int xTickCount, int yTickCount, const QColor &color)
{
    QDateTimeAxis *axisX = new QDateTimeAxis();
    //    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    chart->legend()->hide();
    chart->addSeries(series);
    //    series->setPointsVisible(true);
    series->setColor(color);
    //    for(const QPointF& point : points)
    //    {
    //        series->append(point);
    //    }
    axisX->setMin(QDateTime::currentDateTime().addSecs(-60*1));
    axisX->setMax(QDateTime::currentDateTime().addSecs(0));
    //    axisX->setMin(axisXmin);
    //    axisX->setMax(axisXmax);
    axisX->setTitleText(xAxisTitle);
    //    axisX->setRange(0, 360);
    axisX->setFormat("hh:mm:ss");
    axisY->setMin(axisYmin);
    axisY->setMax(axisYmax);
    axisY->setTitleText(yAxisTitle);
    axisX->setTickCount(xTickCount);
    axisY->setTickCount(yTickCount);
    axisY->setLinePenColor(QColor(Qt::darkBlue));
    axisY->setGridLineColor(QColor(Qt::darkBlue));
    axisY->setGridLineVisible(false);
    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0xd2d0d1));
    backgroundGradient.setColorAt(1.0, QRgb(0x4c4547));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(backgroundGradient);

    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 1));
    plotAreaGradient.setFinalStop(QPointF(1, 0));
    plotAreaGradient.setColorAt(0.0, QRgb(0x555555));
    plotAreaGradient.setColorAt(1.0, QRgb(0x80FF80));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart->setPlotAreaBackgroundVisible(true);
    QPen penY1(Qt::gray,3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    axisY->setLinePen(penY1);

    view->setRenderHint(QPainter::Antialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(view, 0, Qt::AlignBottom | Qt::AlignHCenter);
}

void Widget::initPolarChart(QWidget *widget, QPolarChart *polarChart, QChartView *view, QSplineSeries *series,qreal angularMin,qreal angularMax,qreal radialMin,qreal radialMax)
{

    polarChart->addSeries(series);
    polarChart->legend()->hide();
    QPen pen = series->pen();
    pen.setWidth(2);
    series->setPen(pen);
    series->setPointsVisible(true);
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9);
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    polarChart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(9);
    radialAxis->setLabelFormat("%d");
    radialAxis->setRange(radialMin, radialMax);
    angularAxis->setRange(angularMin, angularMax);
    polarChart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    series->attachAxis(radialAxis);
    series->attachAxis(angularAxis);
    view->setRenderHint(QPainter::HighQualityAntialiasing);
    polarChart->setGeometry(view->rect());
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(view, 0, Qt::AlignBottom | Qt::AlignHCenter);

}

void Widget::getSensorOneMaxValue()
{
    QList<QPointF> points = series1->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal maxValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() > maxValue)
        {
            maxValue = point.y();
        }
    }
    ui->sensorOneMaxValue->setValue(static_cast<double>(maxValue));
}

void Widget::getSensorOneMaxAngle()
{
    QList<QPointF> points = series1->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal maxAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() > maxAngle)
        {
            maxAngle = point.x();
        }
    }
    ui->sensorOneMaxAngle->setValue(static_cast<double>(maxAngle));
}

void Widget::getSensorOneMinValue()
{
    QList<QPointF> points = series1->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal minValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() < minValue)
        {
            minValue = point.y();
        }
    }
    ui->sensorOneMinValue->setValue(static_cast<double>(minValue));
}

void Widget::getSensorOneMinAngle()
{
    QList<QPointF> points = series1->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal minAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() < minAngle)
        {
            minAngle = point.x();
        }
    }
    ui->sensorOneMinAngle->setValue(static_cast<double>(minAngle));
}

void Widget::getSensorTwoMaxValue()
{
    QList<QPointF> points = series2->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal maxValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() > maxValue)
        {
            maxValue = point.y();
        }
    }
    ui->sensorTwoMaxValue->setValue(static_cast<double>(maxValue));
}

void Widget::getSensorTwoMaxAngle()
{
    QList<QPointF> points = series2->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal maxAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() > maxAngle)
        {
            maxAngle = point.x();
        }
    }
    ui->sensorTwoMaxAngle->setValue(static_cast<double>(maxAngle));
}

void Widget::getSensorTwoMinValue()
{
    QList<QPointF> points = series2->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal minValue = points.first().y();
    foreach (const QPointF& point, points)
    {
        if(point.y() < minValue)
        {
            minValue = point.y();
        }
    }
    ui->sensorTwoMinValue->setValue(static_cast<double>(minValue));
}

void Widget::getSensorTwoMinAngle()
{
    QList<QPointF> points = series2->points();
    if(points.isEmpty())
    {
        return;
    }
    qreal minAngle = points.first().x();
    foreach (const QPointF& point, points)
    {
        if(point.x() < minAngle)
        {
            minAngle = point.x();
        }
    }
    ui->sensorTwoMinAngle->setValue(static_cast<double>(minAngle));
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

void Widget::radialEvaluate(double maxValue,double maxAngle,double minValue,double minAngle,double jumpValue,
                            double jumpAngle,double verticality,double verticalAngle,double flatness,double roundness)
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
    radialData.beatValue = jumpValue;
    radialData.beatAngle = jumpAngle;
    radialData.verticalValue = verticality;
    radialData.verticalAngle = verticalAngle;
    radialData.concentricity = (maxValue+minValue) / 2;
    radialData.concentriticAngle = (maxAngle + minAngle) / 2;
    radialData.eccentricity = (maxValue - minValue) / 2;
    radialData.eccentricAngle = (maxAngle - minAngle) / 2;
    radialData.flatness = flatness;
    radialData.roundness = roundness;

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
    ui->radialEvaluateVerticalValue->setValue(radialData.verticalValue);
    ui->radialEvaluateVerticalAngle->setValue(radialData.verticalAngle);
    ui->radialConcentricity->setValue(radialData.concentricity);
    ui->radialConcentricAngle->setValue(radialData.concentriticAngle);
    ui->radialEccentricity->setValue(radialData.eccentricity);
    ui->radialEccentricAngle->setValue(radialData.eccentricAngle);
    ui->radialFlatness->setValue(radialData.flatness);
    ui->radialRoundness->setValue(radialData.roundness);

}

void Widget::axialEvaluate(double maxValue, double maxAngle, double minValue, double minAngle, double jumpValue,
                           double jumpAngle, double verticality, double verticalAngle, double flatness, double roundness)
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
    axialData.beatValue = jumpValue;
    axialData.beatAngle = jumpAngle;
    axialData.verticalValue = verticality;
    axialData.verticalAngle = verticalAngle;
    axialData.concentricity = (maxValue+minValue) / 2;
    axialData.concentriticAngle = (maxAngle + minAngle) / 2;
    axialData.eccentricity = (maxValue - minValue) / 2;
    axialData.eccentricAngle = (maxAngle - minAngle) / 2;
    axialData.flatness = flatness;
    axialData.roundness = roundness;

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
    ui->axialEvaluateVerticalValue->setValue(axialData.verticalValue);
    ui->axialEvaluateVerticalAngle->setValue(axialData.verticalAngle);
    ui->axialConcentricity->setValue(axialData.concentricity);
    ui->axialConcentricAngle->setValue(axialData.concentriticAngle);
    ui->axialEccentricity->setValue(axialData.eccentricity);
    ui->axialEccentricAngle->setValue(axialData.eccentricAngle);
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
        modeNum = 1;
        byte modeBuffer[2];
        S7_SetIntAt(modeBuffer,0,modeNum);
        int res = snap7_plc->DBWrite(writeDbNum,66,2,&modeBuffer);
        if(0 == res)
        {
            isChanged = true;
        }
    }
    else
    {
        modeNum = 2;
        byte modeBuffer[2];
        S7_SetIntAt(modeBuffer,0,modeNum);
        int res = snap7_plc->DBWrite(writeDbNum,66,2,&modeBuffer);
        if(0 == res)
        {
            isChanged = false;
        }
    }
}

void Widget::drawSplineOneLine()
{
    QDateTime time = QDateTime::currentDateTime();
    chart1->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60*1));
    chart1->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    series1->append(time.toMSecsSinceEpoch(),sensorOneValue);
//    series1->append(sensorOneAngle, sensorOneValue);

    //下面代码可要可不要
//    // 限制显示的最大数据点数量，例如只保留最新的360个数据点
//    if (series1->count() > 360) {
//        series1->remove(0);
//    }
//    // X轴的范围会自动根据数据点调整，但你可以设定最小间隔等属性
//    chart1->axisX()->setMin(0); // 确保X轴起始点与数据同步
//    chart1->axisX()->setMax(360); // X轴最大值随最新数据更新

//    // 更新图表视图
//    chartView1->update();


}

void Widget::drawSplineTwoLine()
{

    QDateTime time = QDateTime::currentDateTime();
    chart2->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60*1));
    chart2->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    series2->append(time.toMSecsSinceEpoch(),sensorTwoValue);

}

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
    Q_UNUSED(event)
    if(isMeasuringStatus == true)
    {
        myHelper::ShowMessageBoxInfo("正在检测中,不能关闭程序");
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void Widget::initUSBDeviceOne()
{
    //libusb规定下面这两个函数必须要被调用
    if (usb_set_configuration(udevOne, MY_CONFIG) < 0) {
        qDebug("error setting config #%d: %s", MY_CONFIG, usb_strerror());

    }
    if (usb_claim_interface(udevOne, MY_INTF) < 0) {
        qDebug("error claiming interface #%d:\n%s", MY_INTF, usb_strerror());

    }
}

void Widget::initUSBDeviceTwo()
{
    //libusb规定下面这两个函数必须要被调用
    if (usb_set_configuration(udevTwo, MY_CONFIG_TWO) < 0) {
        qDebug("error setting config #%d: %s", MY_CONFIG_TWO, usb_strerror());

    }
    if (usb_claim_interface(udevTwo, MY_INTF_TWO) < 0) {
        qDebug("error claiming interface #%d:\n%s", MY_INTF_TWO, usb_strerror());

    }
}

bool Widget::conncetSensorOneUSB()
{
    usb_init(); /* initialize the library */
    //usb_set_debug(255);
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */

    if (udevOne = openDeviceOne())
    {
        ui->sensorOneStatus->setStyleSheet(normal_radius);
        return true;
    }
    else
    {
        ui->sensorOneStatus->setStyleSheet(fail_radius);
        return false;
    }

}

bool Widget::conncetSensorTwoUSB()
{
    usb_init(); /* initialize the library */
    //usb_set_debug(255);
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */

    if (udevTwo = openDeviceTwo())
    {
        ui->sensorTwoStatus->setStyleSheet(normal_radius);

        return true;
    }
    else
    {
        ui->sensorTwoStatus->setStyleSheet(fail_radius);
        return false;
    }
}

void Widget::getMeasureDataByTypeNumTwo(const QString measureTypeNum, const QString startTime, const QString endTime)
{
    sensorTwoDataList.clear();
    oper.getSensorNumberTwoData(sensorTwoDataList,measureTypeNum,startTime,endTime);
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
            if(j==3)
            {
                item->setText(sensorTwoDataList[i].date);
            }
            ui->result_tableWidget_2->setItem(i,j,item);
        }
    }
}

void Widget::getMeasureDataByTypeNumOne(const QString measureTypeNum, const QString startTime, const QString endTime)
{
    sensorOneDataList.clear();
    oper.getSensorNumberOneData(sensorOneDataList,measureTypeNum,startTime,endTime);
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
            if(j==3)
            {
                item->setText(sensorOneDataList[i].date);
            }
            ui->result_tableWidget_1->setItem(i,j,item);
        }
    }
}

usb_dev_handle *Widget::openDeviceOne()
{
    struct usb_bus *bus;
    for(bus = usb_get_busses(); bus; bus = bus->next) {
        for(devOne = bus->devices; devOne; devOne = devOne->next) {
            if((devOne->descriptor.idVendor == MY_VID) && (devOne->descriptor.idProduct == MY_PID)) {
                return usb_open(devOne);
            }
        }
    }
    return 0;
}

usb_dev_handle *Widget::openDeviceTwo()
{
    struct usb_bus *bus;
    for(bus = usb_get_busses(); bus; bus = bus->next) {
        for(devTwo = bus->devices; devTwo; devTwo = devTwo->next) {
            if((devTwo->descriptor.idVendor == MY_VID_TWO) && (devTwo->descriptor.idProduct == MY_PID_TWO)) {
                return usb_open(devTwo);
            }
        }
    }
    return 0;
}
