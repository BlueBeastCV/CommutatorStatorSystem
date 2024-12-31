#ifndef MWLMANAGER_H
#define MWLMANAGER_H

#include <QObject>
#include <Windows.h>
#include <minwindef.h>
#include <libloaderapi.h>
#include <synchapi.h>
#include <errhandlingapi.h>
#include <Mwl_SDK/includes/MWL.h>
#include <Mwl_SDK/includes/stdafx.h>
#include <Mwl_SDK/includes/targetver.h>
#include <QDebug>
class MWLManager : public QObject
{
    Q_OBJECT
public:
    explicit MWLManager(QObject *parent = nullptr);
    ~MWLManager();
    HINSTANCE m_hLib;
    // 初始化库
    bool initializeLibrary();
    // 释放库
    void freeLibrary();
    // 获取版本信息
    void getVersion(MWL_VERSION *version);
    // 打开设备
    bool openDevice(const MWL_DEVICE &device);
    // 关闭设备
    bool closeDevice(int deviceId);
    // 请求数据
    int requestData(int deviceId);
    // 开始连续请求所有数据
    void startContinuousRequestAllData(unsigned int interval, int par);
    // 停止连续请求所有数据
    void stopContinuousRequestAllData();
    // 重置设备
    int resetDevice(int deviceId);
    // 注册数据回调
    void registerDataCallback(F_MwlDataCallback callback, int numDevices, int *devNoArray, void *context);
    // 取消注册数据回调
    void unregisterDataCallback(F_MwlDataCallback callback);
    // 注册消息回调
    void registerMsgCallback(F_MwlMsgCallback callback);
    // 取消注册消息回调
    void unregisterMsgCallback();
    // 获取最后一个测量值
    int getLastMeasVal(int deviceId);
    // 设置自动断开连接
    void setAutoDisconnect(int autoDisconnect);
signals:
    // 当设备状态发生变化时发送信号
    void deviceStateChanged(int deviceId, int state);
    // 当获取到新数据时发送信号
    void newDataReceived(int deviceId, double data);
private:
    F_MwlInitializeLibrary MwlInitializeLibrary;
    F_MwlFreeLibrary MwlFreeLibrary;
    F_MwlGetVersion MwlGetVersion;
    F_MwlOpen MwlOpen;
    F_MwlClose MwlClose;
    F_MwlRequestData MwlRequestData;
    F_MwlStartContinuousRequestAllData MwlStartContinuousRequestAllData;
    F_MwlStopContinuousRequestAllData MwlStopContinuousRequestAllData;
    F_MwlReset MwlReset;
    F_MwlRegisterDataCallback MwlRegisterDataCallback;
    F_MwlUnregisterDataCallback MwlUnregisterDataCallback;
    F_MwlRegisterMsgCallback MwlRegisterMsgCallback;
    F_MwlUnregisterMsgCallback MwlUnregisterMsgCallback;
    F_MwlGetLastMeasVal MwlGetLastMeasVal;
    F_MwlSetAutoDisconnect MwlSetAutoDisconnect;
    F_MwlGetDevice MwlGetDevice;
};

#endif // MWLMANAGER_H
