#ifndef SETFORM_H
#define SETFORM_H

#include <QWidget>
#include <QColorDialog>
#include <QMessageBox>
#include "dlgdrawstly.h"
#include <QSettings>
#include <QMessageBox>
#include <QCheckBox>
#include <qtextcodec.h>
#include "syshead.h"
//#include "verifysetdlg.h"
#include "database.h"
#include "SpeechLib\SpeechLib.h"
//系统参数设计部分设备操控
namespace Ui {
    class setForm;
}


class setForm : public QWidget
{
    Q_OBJECT
public:
    explicit setForm(QWidget *parent = 0);
    ~setForm();
    QColorDialog *setColor;
    void setDefaultIP(QString strIP);
    bool curAngleTcpStatus;
    bool pointMeasureStatus;
    void getPar(void);
    void checkoutPar(void);
    bool bLedStatus;
    bool bWIFI;
private:
    Ui::setForm *ui;
    void uiInitial(void);
    bool m_bMeasureType;
    bool m_bMeasureStatus;
    bool m_bpower;
    int  m_nExpand;
    void saveInitialPar(sensorInitialValues &sensorIntialValue);
    bool bFirstPower;//作为每次监测到雷达上电后，只发送一次设备连接的标志位
    bool bRadarPower;
    bool bUIExpand;
    void setPbtnUI(QPushButton *pbtn,bool flag);
    char m_verifyType;
    void updateRadorStatus(bool);
    void updateWIFIStatus(bool b);
    void updateLEDStatus(bool);
    bool bOpenRadar;//判断是否发送了雷达开启命令
    QVector<int> radorPowerStatus;
    void radarPowerStack(int );
public slots:
    void slot_comReplay(COM_REPLAY_TYPE);
    void slot_radorConnect(DEVICE_CONNECT_STATUS flag);
    void slot_updateDeviceInfo(deviceInfo );
private slots:
    void slot_measureType();
    void slot_deviceControl(void);
    void slot_updateIPDeviceStatus(QString);
    void slot_meassureStauts(bool);
    void on_pbtnDeviceConnect_clicked();
    void on_pbtnUIExpand_clicked();
    void on_pbtnLED_clicked();
    void on_pbtnCurveSyn_clicked();
    void on_pbtnOverAlarm_clicked();

    void on_pbtnOverUpdate_clicked();
    void radorFirstPower(void);
signals:
    void baseParChanged(void);
    void measureType(bool);                     //测量状态的选择，false，模拟测量;true,实时测量
    void radorAndWIFIControl(COM_COMMAND_TYPE);
    void railComControl(COM_COMMAND_TYPE);

    void expandFlag(bool);
    void conDevice(void);                           //连接雷达设备
    void radorPowerOff();
    void signal_parDimarcate(wireStandardValues);
 };

#endif // SETFORM_H
