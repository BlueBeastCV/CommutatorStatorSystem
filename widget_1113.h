#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "myhelper.h"
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QtCharts>
#include <QLineSeries>
#include <QChartView>
#include "BaseGraphicsview.h"
#include "qtstreambuf.h"
#include "Snap7_sdk/plc_siemens.h"
#include "Snap7_sdk/s7.h"
#include "Snap7_sdk/plcthread.h"
#include <QColor>
#include <qtxlsx/src/xlsx/xlsxdocument.h>
#include <qtxlsx/src/xlsx/xlsxworksheet.h>
#include <mydashboard.h>
#include <QPolarChart>
#include <QSplineSeries>
#include <QToolTip>
#include <limitform.h>
#include <QPdfWriter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPainter>
#include <QDebug>
#include <QDesktopServices>
#include <radialpolarchart.h>
#include <axialpolarchart.h>
#include <mwlmanager.h>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrintDialog>

typedef radialPolarChart myRadialPolarChart;
typedef axialpolarChart myAxialPolarChart;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(User u,QWidget *parent = 0);
    ~Widget();
    void init();
    void initConnect();
    void readControllerObject();
    void initPushbuttonByFrame(QFrame* frame);
    void initPushbuttonByFrame_slot();
    void initPushbuttonByGroup(QGroupBox *box);
    void initPushbuttonByGroup_slot();

    bool areAllSpinboxesZero(QGroupBox *groupBox);
    void changeWidgetButtonStyle(QPushButton* checkButton);
    void showUserTable(QVector<User> users);
    bool isAdmin();
    bool isTechnician();

    void initRadialChart();
    void initAxialChart();
    void getSensorOneMaxValue();
    void getSensorOneMaxAngle();
    void getSensorOneMinValue();
    void getSensorOneMinAngle();
    void getRadialBeatValue();
    void getRadialBeatAngle();
    void getRadialEccentricValue();
    void getRadialEccentricAngle();

    void getSensorTwoMaxValue();
    void getSensorTwoMaxAngle();
    void getSensorTwoMinValue();
    void getSensorTwoMinAngle();
    void getAxialBeatValue();
    void getAxialBeatAngle();
    void getAxialEccentricValue();
    void getAxialEccentricAngle();
    void setTips();
    void addPointToRadialChart(double radialValue, double radialAngle);
    void addPointToAxialChart(double axialValue, double axialAngle);
    //径向评定
    void radialEvaluate(double maxValue, double maxAngle, double minValue, double minAngle);
    //轴向评定
    void axialEvaluate(double maxValue, double maxAngle, double minValue, double minAngle);

    void getMeasureDataByTypeNumOne(const QString measureTypeNum, const QString startTime, const QString endTime);
    void getMeasureDataByTypeNumTwo(const QString measureTypeNum, const QString startTime, const QString endTime);

    void initDoubleSpinBoxInputRange();
    void InitPLCThread();
    void ConnectPLCStatus(PLC_Siemens* plc, int ok);
    void saveMahrDataToTableOne(double radialAngle, double radialDetectValue);
    void saveMahrDataToTableTwo(double axialAngle, double axialDetectValue);
    //pdf导出
    bool radialPDFCreate();
    bool axialPDFCreate();
    void pdfDrawForm(QPainter* paint,int y,int horzBorder,int row,int column,
                     int unitHeight,QFont &font,QStringList& list);

    void initStartDataCallBackDeviceOne();
    void initStartDataCallBackDeviceTwo();
    static int __stdcall data_callback_handler_1(int numDevices, int *pDevNoArray, double *pData, void *pContext);
    static int __stdcall data_callback_handler_2(int numDevices, int *pDevNoArray, double *pData, void *pContext);
    static int __stdcall message_callback_handler(int Msg, int DeviceId, int Param);
    void closeDeviceOne();
    void closeDeviceTwo();
    void initDeviceOne();
    void initDeviceTwo();
    void initMahrDevices();
    void initMode();

    //测试
    QVector<double> generateRandomData(int count);

private slots:
    void on_userTable_itemChanged(QTableWidgetItem *item);
    void updateLineEidtOne_solt(int state);
    void updateLineEidtTwo_solt(int state);
    void updateLineEidtThree_solt(int state);
    void updateLineEidtFour_solt(int state);
    void updateLineEidtSpeed_solt(int state);
    void updateLineEidtAngle_solt(int state);
    void updateLineEidtTrunTableSpeed_solt(int state);
    void modeChange_solt(int state);



protected slots:
    void closeEvent(QCloseEvent* event);

signals:
    void ConnectPLC(QString ip, int Rack, int Slot);
public slots:
    bool speedWriteInSettingFile();

private:
    Ui::Widget *ui;
    QTimer systemTime;
    User currentUser;
    PLC_Siemens *snap7_plc = nullptr;
    int readDbNum = myHelper::readSettings("GPLC/readDB").toInt();
    int writeDbNum = myHelper::readSettings("GPLC/writeDB").toInt();
    QString PLCIP = myHelper::readSettings("GPLC/IP").toString();
    int PLCRack = myHelper::readSettings("GPLC/Rack").toInt();
    int PLCSlot = myHelper::readSettings("GPLC/Slot").toInt();
    int intPLCount = 0;
    QTimer initDeviceTimer;//程序启动定时初始化 硬件设备
    double sensorOneValue; //马尔表1号数据
    double sensorTwoValue; //马尔表2号数据
    bool controllerInitStatus;
    bool loadMeasureProCodeList;
    QTimer timeReadController;
    ControllerObject controllerObject;//实时PLC数据
    ControllerObject oneConObject;//只处理一次的PLC对象
    DataOper oper;
    bool isShowUserTable = true;
    bool isMeasuringStatus = false;
    bool isLocked = false; //参数设置锁定状态
    bool isLockedRadial = false;
    bool isLockedAxial = false;
    bool isLoadTypeNumList = false; //是否加载型号列表
    bool sensorOnePointsIsLoad = false;
    bool isStartingCollectSensorOneData = false;
    bool isStartingCollectSensorTwoData = false;
    bool isCompressed;
    bool mahrOneIsOpened = false;
    bool mahrTwoIsOpened = false;
    bool isChanged = false;
    bool enabled = false;
    bool isUpping = false;
    bool isDowning = false;
    std::shared_ptr<qtStreamBuf> buffer;
    //初始化自定义表盘
    myDashBoard *dashboard1;
    myDashBoard *dashboard2;
    //初始化坐标图
    QChart *radialChart;
    QChart *axialChart;
    QScatterSeries *radialScatterSeries;
    QScatterSeries *axialScatterSeries;
    QValueAxis *radialXAxis;
    QValueAxis *radialYAxis;
    QValueAxis *axialXAxis;
    QValueAxis *axialYAxis;
    QLineSeries *radialSeries;
    QLineSeries *axialSeries;
    qreal radialMinX = 0;
    qreal radialMaxX = 360;
    qreal radialMinY = 0;
    qreal radialMaxY = 1;
    int radialTickCountX = 10;
    int radialTickCountY = 5;

    QTimer *radialReadDataTimer;
    QTimer *axialReadDataTimer;

    QVector<SensorDetectDataOne> sensorOneDataList;
    QVector<SensorDetectDataTwo> sensorTwoDataList;

    //创建密码输出页面
    LimitForm *limitform = nullptr;
    int sampleNumsOne = 1;
    int sampleNumsTwo = 1;
    PLCThread *plcThreadOBJ = nullptr;
    QThread plcThread;
    qreal radialMinValue;
    qreal radialMaxValue;
    qreal radialMinAngle;
    qreal radialMaxAngle;
    double radialbeatValue;
    double radialbeatAngle;
    double radialEccentricValue;
    double radialEccentricAngle;
    qreal axialMinValue;
    qreal axialMaxValue;
    qreal axialMinAngle;
    qreal axialMaxAngle;
    double axialbeatValue;
    double axialbeatAngle;
    double axialEccentricValue;
    double axialEccentricAngle;

    double radialStepAngle;
    double axialStepAngle;

    double mahrDataOne;
    double mahrDataTwo;
    //径向剔除
    QVector<QPointF> removedRadialHigherPoints;
    QVector<QPointF> removedRadialLowerPoints;
    //轴向剔除
    QVector<QPointF> removedAxialHigherPoints;
    QVector<QPointF> removedAxialLowerPoints;
    myRadialPolarChart *radialPolar;
    myAxialPolarChart *axialPolar;
    std::unique_ptr<MWLManager> m_mwlManager;
    MWL_VERSION *version;
    bool deviceOneStatus = false;
    bool deviceTwoStatus = false;
    bool m_bIsCallBackRegisterd = false;
    bool initStatus = false;
    bool isDeviceOneDataGetted;
    bool isDeviceTwoDataGetted;
    MWL_DEVICE deviceOne;
    MWL_DEVICE deviceTwo;
    QTimer initMahrDeviceTimer;
    QTimer *deviceOneGetDataTimer;
    QTimer *deviceTwoGetDataTimer;
    QTimer initTimer;
    //测试
    QTimer *testTimer;
    QVector<double> xData;
    int currentIndex;

};

#endif // WIDGET_H
