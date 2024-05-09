#ifndef WIDGET_H
#define WIDGET_H

#define MY_VID 0x0fcf
#define MY_PID 0x1009
// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint 2
#define EP_IN 0x81
#define EP_OUT 0x01

#define MY_VID_TWO 0x0fcf
#define MY_PID_TWO 0x1009
// Device configuration and interface id.
#define MY_CONFIG_TWO 1
#define MY_INTF_TWO 0

// Device endpoint 2
#define EP_IN_TWO 0x81
#define EP_OUT_TWO 0x01
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
#include <QColor>
#include <QtXlsx/xlsxdocument.h>
#include <QtXlsx/xlsxworksheet.h>
#include <mydashboard.h>
#include <QPolarChart>
// #include <speedsetting.h>
#include <QSplineSeries>
#include <QToolTip>
#include <QSerialPort>
#include <lusb0_usb.h>
#include <QProcess>
#include <limitform.h>

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

    void initPLC();
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
    bool isProcessRunning(const QString &processName);
    void openMahrExe();
//    void loadTypeNumList(QComboBox *box);

    void initLineSeriesOne(QWidget *widget, QChart *chart, QChartView *view, QLineSeries *series, QVector<QPointF> dataPoints, QString xAxisTitle, QString yAxisTitle, int maxXRange, int maxYRange, int xTickCount, int yTickCount, const QColor &color);
//    void initSplineSeries(QWidget *widget, QChart *chart, QChartView *view, QSplineSeries *series, QString xAxisTitle, QString yAxisTitle, int axisXmin, int axisXmax, int axisYmin, int axisYmax, int xTickCount, int yTickCount);
//    void initSplineSeries(QWidget *widget, QChart *chart, QChartView *view, QSplineSeries *series, QString xAxisTitle, QString yAxisTitle, int axisYmin, int axisYmax, int xTickCount, int yTickCount);
    void initSplineSeries(QWidget *widget, QChart *chart, QChartView *view, QLineSeries *series, QString xAxisTitle, QString yAxisTitle, int axisYmin, int axisYmax, int xTickCount, int yTickCount, const QColor &color);
    void initPolarChart(QWidget *widget, QPolarChart *polarChart, QChartView *view, QSplineSeries *series, qreal angularMin, qreal angularMax, qreal radialMin, qreal radialMax);

    void getSensorOneMaxValue();
    void getSensorOneMaxAngle();
    void getSensorOneMinValue();
    void getSensorOneMinAngle();

    void getSensorTwoMaxValue();
    void getSensorTwoMaxAngle();
    void getSensorTwoMinValue();
    void getSensorTwoMinAngle();
    void setTips();
    //径向评定
    void radialEvaluate(double maxValue,double maxAngle,double minValue,double minAngle,double jumpValue,
                        double jumpAngle,double verticality,double verticalAngle,double flatness,double roundness);
    //轴向评定
    void axialEvaluate(double maxValue,double maxAngle,double minValue,double minAngle,double jumpValue,
                       double jumpAngle,double verticality,double verticalAngle,double flatness,double roundness);
    //连接USB
    usb_dev_handle *openDeviceOne();
    void initUSBDeviceOne();
    bool conncetSensorOneUSB();

    usb_dev_handle *openDeviceTwo();
    void initUSBDeviceTwo();
    bool conncetSensorTwoUSB();
    void getMeasureDataByTypeNumOne(const QString measureTypeNum, const QString startTime, const QString endTime);
    void getMeasureDataByTypeNumTwo(const QString measureTypeNum, const QString startTime, const QString endTime);
    void sendBitToRobot(int offset, int bit, bool on_off);

    void initDoubleSpinBoxInputRange();

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

protected:

public slots:
    void drawSplineOneLine();
    void drawSplineTwoLine();
    bool speedWriteInSettingFile();
private:
    Ui::Widget *ui;
    QTimer systemTime;
    User currentUser;
    PLC_Siemens *snap7_plc = nullptr;
    int writeDbNum;//写入DB块号
    int readDbNum;//读取DB块号
    double sensorOneValue; //马尔表1号数据
    double sensorTwoValue; //马尔表2号数据
    bool controllerInitStatus;
    bool loadMeasureProCodeList;
    QTimer timeReadController;
    QTimer *sensorOneTimer;
    QTimer *sensorTwoTimer;
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
    std::shared_ptr<qtStreamBuf> buffer;
    myDashBoard *dashboard1;
    myDashBoard *dashboard2;
    QChart *chart1;
    QChart *chart2;
    QChart *splineChart1;
    QChart *splineChart2;
    QPolarChart *radialPolarChart; //径向
    QPolarChart *axialPolarChart; //轴向
    QLineSeries *series1;
    QLineSeries *series2;
    QSplineSeries *splineSeries1;
    QSplineSeries *splineSeries2;
    QSplineSeries *polarSeries1;
    QSplineSeries *polarSeries2;
    QChartView *chartView1;
    QChartView *chartView2;
    QChartView *splineView1;
    QChartView *splineView2;
    QChartView *polarView1;
    QChartView *polarView2;
//    QList<QPointF> lineSeriesPointsOne;
//    QList<QPointF> lineSeriesPointsTwo;
    QTimer *splineSeriesOneTimer;
    QTimer *splineSeriesTwoTimer;
//    speedSetting *speedSettingForm;

    struct usb_device *devOne;
    usb_dev_handle *udevOne;
    struct usb_device *devTwo;
    usb_dev_handle *udevTwo;
    QTimer *listenUSBOneStatusTimer;
    QTimer *listenUSBTwoStatusTimer;
    QSerialPort *serialOne;
    QSerialPort *serialTwo;

    QVector<SensorDetectDataOne> sensorOneDataList;
    QVector<SensorDetectDataTwo> sensorTwoDataList;

    //马尔千分表客户端进程创建
    QProcess *mharProcess;

    //创建密码输出页面
    LimitForm *limitform = nullptr;

};

#endif // WIDGET_H
