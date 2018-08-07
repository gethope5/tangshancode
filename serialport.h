#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
//189382287
#include "serialControlModules/win_qextserialport.h"
#include <qtimer.h>
#include <QSettings>
#include <QDebug>
#include <QString>
#include <qt_windows.h>
#include <QStringList>
#include "math.h"
#include <QMap>
#include <QtWidgets>
#include <qmessagebox.h>
#include "syshead.h"
#include "database.h"
#include <qthread.h>
#include <QtSerialPort>
#define SYS_COM 1
#define DEBUG_COM_0428 0

struct comDataType
{
    int nRailHeight;//超高
    int  nRailWidth;//轨距
    double  walkDistance;//里程
    float angle;
    comDataType()
    {
        nRailHeight=0;//超高
        nRailWidth=0;//轨距
        walkDistance=0;//里程
        angle=0;
    }
};

typedef struct __ConfigCom
{
    QString  com;
    BaudRateType braudrate;
    StopBitsType stopbit;
    DataBitsType databit;
    ParityType  parity;
} ConfigCom;
enum COMTYPE
{
    RAIL_MCU_COM,
    RADAR_COM,
    CLOUD_COM,
    TS_RAIL_COM,
    TS_ANGLE_COM
};

class SerialPort : /*public QObject,*/public QThread
{
    Q_OBJECT
public:
    explicit SerialPort(COMTYPE,QObject *parent = 0);
    ~SerialPort();
    bool connectDevice();
    void SendCommString(QString strSend);
    ConfigCom   commInfo;
    QStringList getComm(void);
    void getComConfigInfo( );
    void clearDistance(void);

    void railMeasure(void);
    void getLaserDis(void);
    void closeLaser(void);
    void openLaser(void);
    bool comClose();
    bool openStatus(void );
    static bool bCom1;
    int nSideDisConst;
    uint curRailWidth;
    void radarOn(void);
    void radarOff(void);
    void startRail(void);
    void stopRail(void);
    void initialDistance(void);
    void cloudControl(CLOUDCONTROL e,int address=1);
    struct Settings
    {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
private:
    void readBasePar(void);
    void CommDeviceClose( );
    void parse_RailSensor(QString strbuffer,bool flag);
    QString MakeSingleCom(int index,QString keyorvalue);
    int nRailWidthC;
    int nRailHeightC;
    float nSideWidthConst;
    comDataType m_comDataType;
    COMTYPE nComType;
    QMap<QString,int> braudMapToInt;
    QMap<int,QString> braudMapToString;
    QMap<QString,int> databitsMapToInt;
    QMap<int,QString> databitsMapToString;
    void enumToString (void);
    int disCount;
    static int preDistanceCount;
    int curDistanceCount;
    qint64 walkStepCounts;//保存里程传感器行驶过程中的脉冲总个数
    double constDiameter;
    int constRound1Pulses;
    double distCoefficients;
    int directFlag;
    void setPreDistanceCount(qint64 tmp);
    bool bFirstWalk;
    bool m_bOpen;
    void  wifiPower(bool f);

    void parse_comData(void);
    QString receiveDataBuffer;
    bool getParData(QString orignal,QString feature,QString &Output);
#if DEBUG_COM_0428
    QFile *file;
    virtual void run(void);
#endif
#if SYS_COM
    QSerialPort *serial;
    Settings comSet;
#else
    Win_QextSerialPort  *RS232;
    QTimer              *readTimerCom;

#endif
    bool bOk;
    QTimer *timerDeviceInfo;
    deviceInfo curDeviceInfo;
    void railComPar(QString & receiveDataBuffer);
signals:
    void getComData(comDataType);// 更新拉出值、导高和里程
    void com_replay(COM_REPLAY_TYPE);
    void updateShowInfo(QString);
    void updateDeviceInfos(deviceInfo curDeviceInfo);
    void baseAngle(float &);
public slots:
    void readComData();
    void slot_baseRailParChanged();
    void setDirectionFlag(int tmp);
    void slot_deviceControl(COM_COMMAND_TYPE controlBuffer);//利用枚举变量COM_COMMAND_TYPE，区分了所有的串口命令
    void slot_updateDevice(void);

};

#endif // SERIALPORT_H
