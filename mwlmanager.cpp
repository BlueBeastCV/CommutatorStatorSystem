#include "mwlmanager.h"

MWLManager::MWLManager(QObject *parent) : QObject(parent)
{
    // 初始化函数指针
    MwlInitializeLibrary = nullptr;
    MwlFreeLibrary = nullptr;
    MwlGetVersion = nullptr;
    MwlOpen = nullptr;
    MwlClose = nullptr;
    MwlRequestData = nullptr;
    MwlStartContinuousRequestAllData = nullptr;
    MwlStopContinuousRequestAllData = nullptr;
    MwlReset = nullptr;
    MwlRegisterDataCallback = nullptr;
    MwlUnregisterDataCallback = nullptr;
    MwlRegisterMsgCallback = nullptr;
    MwlUnregisterMsgCallback = nullptr;
    MwlGetLastMeasVal = nullptr;
    MwlSetAutoDisconnect = nullptr;
    MwlGetDevice = nullptr;
}

MWLManager::~MWLManager()
{
}

bool MWLManager::initializeLibrary()
{
    HINSTANCE hLib = LoadLibrary(TEXT("MWL.dll"));
    if (!hLib) {
        DWORD errorCode = GetLastError();
        qDebug() << QString("LoadLibrary(\"MWL.dll\") failed <<< %1").arg(errorCode);
        return false;
    }
    m_hLib = hLib;
    MwlInitializeLibrary = (F_MwlInitializeLibrary)GetProcAddress(hLib, "MwlInitializeLibrary");
    if (!MwlInitializeLibrary) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlInitializeLibrary failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlFreeLibrary = (F_MwlFreeLibrary)GetProcAddress(hLib, "MwlFreeLibrary");
    if (!MwlFreeLibrary) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlFreeLibrary failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlGetVersion = (F_MwlGetVersion)GetProcAddress(hLib, "MwlGetVersion");
    if (!MwlGetVersion) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlGetVersion failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlOpen = (F_MwlOpen)GetProcAddress(hLib, "MwlOpen");
    if (!MwlOpen) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlOpen failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlClose = (F_MwlClose)GetProcAddress(hLib, "MwlClose");
    if (!MwlClose) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlClose failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlRequestData = (F_MwlRequestData)GetProcAddress(hLib, "MwlRequestData");
    if (!MwlRequestData) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlRequestData failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlStartContinuousRequestAllData = (F_MwlStartContinuousRequestAllData)GetProcAddress(hLib, "MwlStartContinuousRequestAllData");
    if (!MwlStartContinuousRequestAllData) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlStartContinuousRequestAllData failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlStopContinuousRequestAllData = (F_MwlStopContinuousRequestAllData)GetProcAddress(hLib, "MwlStopContinuousRequestAllData");
    if (!MwlStopContinuousRequestAllData) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlStopContinuousRequestAllData failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlReset = (F_MwlReset)GetProcAddress(hLib, "MwlReset");
    if (!MwlReset) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwLReset failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlRegisterDataCallback = (F_MwlRegisterDataCallback)GetProcAddress(hLib, "MwlRegisterDataCallback");
    if (!MwlRegisterDataCallback) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlRegisterDataCallback failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlUnregisterDataCallback = (F_MwlUnregisterDataCallback)GetProcAddress(hLib, "MwlUnregisterDataCallback");
    if (!MwlUnregisterDataCallback) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlUnregisterDataCallback failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlRegisterMsgCallback = (F_MwlRegisterMsgCallback)GetProcAddress(hLib, "MwlRegisterMsgCallback");
    if (!MwlRegisterMsgCallback) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlRegisterMsgCallback failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlUnregisterMsgCallback = (F_MwlUnregisterMsgCallback)GetProcAddress(hLib, "MwlUnregisterMsgCallback");
    if (!MwlUnregisterMsgCallback) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlUnregisterMsgCallback failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlGetLastMeasVal = (F_MwlGetLastMeasVal)GetProcAddress(hLib, "MwlGetLastMeasVal");
    if (!MwlGetLastMeasVal) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlGetLastMeasVal failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }

    MwlSetAutoDisconnect = (F_MwlSetAutoDisconnect)GetProcAddress(hLib, "MwlSetAutoDisconnect");
    if (!MwlSetAutoDisconnect) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlSetAutoDisconnect failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }
    MwlGetDevice = (F_MwlGetDevice)GetProcAddress(hLib, "MwlGetDevice");
    if (!MwlGetDevice) {
        DWORD errorCode = GetLastError();
        qDebug() << "GetProcAddress for MwlGetDevice failed with error code:" << errorCode;
        freeLibrary();
        return false;
    }
    int result = MwlInitializeLibrary();
    if (result != MWL_SUCCESS) {
        DWORD errorCode = GetLastError();
        qDebug() << "InitializeLibrary DLL failed: " << errorCode;
        freeLibrary();
        return false;
    }

    return true;
}

void MWLManager::freeLibrary()
{
    if (MwlFreeLibrary) {
        MwlFreeLibrary();
    }
}

void MWLManager::getVersion(MWL_VERSION *version)
{
    if (MwlGetVersion)
    {
        int res = MwlGetVersion(version);
        qDebug() << QString("MwlDevice::Open(): MwlGetVersion returned %1 (mwllib v%2.%3-%4)").arg(res).arg(version->mwllib.major).arg(version->mwllib.minor).arg(version->mwllib.micro);
    }

}

bool MWLManager::openDevice(const MWL_DEVICE &device)
{
    MWL_DEVICE *devicePtr = const_cast<MWL_DEVICE*>(&device);

    int res = MwlOpen(devicePtr);
    if(res == MWL_SUCCESS)
    {
        MwlGetDevice(devicePtr);
        qDebug()<< "deviceID: " << devicePtr->DeviceId << "SerialNum: " << devicePtr->SerialNumber;
        return true;
    }
    return false;
}

bool MWLManager::closeDevice(int deviceId)
{
        if (MwlClose)
        {
            MwlClose(deviceId);
            return true;
        }
        return false;
//    if (m_hLib && MwlClose) {
//        qDebug() << "Closing device with ID:" << deviceId;
//        int result = MwlClose(deviceId);
//        if (result == MWL_SUCCESS) {
//            qDebug() << "Device closed successfully.";
//            return true;
//        } else {
//            qDebug() << "Failed to close device. Error code:" << result;
//            return false;
//        }
//    } else {
//        qDebug() << "MwlClose function pointer is not valid.";
//        return false;
//    }
    //    try {
    //        if (m_hLib && MwlClose) {
    //            qDebug() << "Closing device with ID:" << deviceId;
    //            int result = MwlClose(deviceId);
    //            if (result == MWL_SUCCESS) {
    //                qDebug() << "Device closed successfully.";
    //                return true;
    //            } else {
    //                qDebug() << "Failed to close device. Error code:" << result;
    //                return false;
    //            }
    //        } else {
    //            qDebug() << "MwlClose function pointer is not valid.";
    //            return false;
    //        }
    //    } catch (const std::exception& e) {
    //        qDebug() << "Exception in closeDevice: " << e.what();
    //    } catch (...) {
    //        qDebug() << "Unknown exception in closeDevice";
    //    }
    //    return false;
}

int MWLManager::requestData(int deviceId)
{
    if (MwlRequestData)
        return MwlRequestData(deviceId);
    return MWL_FAILURE;
}

void MWLManager::startContinuousRequestAllData(unsigned int interval, int par)
{
    if (MwlStartContinuousRequestAllData)
        MwlStartContinuousRequestAllData(interval, par);
}

void MWLManager::stopContinuousRequestAllData()
{
    if (MwlStopContinuousRequestAllData)
        MwlStopContinuousRequestAllData();
}

int MWLManager::resetDevice(int deviceId)
{

    return MwlReset(deviceId);

}

void MWLManager::registerDataCallback(F_MwlDataCallback callback, int numDevices, int *devNoArray, void *context)
{
    if (MwlRegisterDataCallback)
    {
        MwlRegisterDataCallback(callback, numDevices, devNoArray, context);
    }
}

void MWLManager::unregisterDataCallback(F_MwlDataCallback callback)
{
    if (MwlUnregisterDataCallback)
        MwlUnregisterDataCallback(callback);
}

void MWLManager::registerMsgCallback(F_MwlMsgCallback callback)
{
    if (MwlRegisterMsgCallback)
        MwlRegisterMsgCallback(callback);
}

void MWLManager::unregisterMsgCallback()
{
    if (MwlUnregisterMsgCallback)
        MwlUnregisterMsgCallback();
}

int MWLManager::getLastMeasVal(int deviceId)
{
    char val[20];
    return MwlGetLastMeasVal(deviceId,val);
}

void MWLManager::setAutoDisconnect(int autoDisconnect)
{
    if (MwlSetAutoDisconnect)
        MwlSetAutoDisconnect(autoDisconnect);
}
