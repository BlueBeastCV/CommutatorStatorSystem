#ifndef PLC_SIEMENS_H
#define PLC_SIEMENS_H

#include "snap7.h"
#include <QDebug>
#include <QString>

class PLC_Siemens : public QObject
{
    Q_OBJECT

public:
    explicit PLC_Siemens(QObject *parent = nullptr);
    ~PLC_Siemens();

    int connectByIP(QString ip,int Rack = 0, int Slot = 1);
    int disConnect();
    bool connectStatus();

    int DBRead(int DBNumber, int Start, int Size, void *pUsrData);
    int DBWrite(int DBNumber, int Start, int Size, void *pUsrData);


    int ReadArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData);
    int WriteArea(int Area, int DBNumber, int Start, int Amount, int WordLen, void *pUsrData);

    QString ErrorText(int Error);

    //I区
    int ABRead(int Start, int Size, void *pUsrData);
    int ABWrite(int Start, int Size, void *pUsrData);

	int setConnectType(word ConnectionType);



private:
    TS7Client *MyS7Client;

};

#endif // PLC_SIEMENS_H
