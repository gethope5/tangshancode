#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets>
#include <QList>
#include <qtextcodec.h>
#include <qDebug>
#include <QHostInfo>
#include <windows.h>
#include "global.h"
#include "setform.h"
#include "serialport.h"
#include "twoplotform.h"
#include "raillineedit.h"
#include "rador/tcpangle.h"
#include "recordmeasureform.h"
#include "rador/radorscannermodule.h"
#include "onepoleoneform.h"
#include "PreRunSettingDlg.h"
#include "tcpangle.h"
#include "exituidlg.h"
#include "devicestatus.h"
#include "firstform.h"
#include "hoverbitmapbtn.h"
#include <qevent.h>
#include <QStringList>
#include <qstring.h>
#include "simform.h"
#include "speak.h"
#include <qmenu.h>
#include "Video/videoplayfrm.h"
#define DistanceDebug 0
#define Debug_Size 0
#define debug_data    1
#define TREND_SIZE 5
//
#define MEASURE_TIME_INTERVAL 1000 
#define stackVerify_size 20
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool m_bMeasureType;        //true 实时测量,false,模拟测量
    bool bDetectPoleOldRail;    //检测到新的杆号
    bool bDetectPoleNewRail;
    int preSS;                  //存储一跨的起始里程，应用于侧面界限或支柱的识别
    bool fristLimitFlag;        //第一次获取传感器标志位，判断是否调用初始设置对话框               
    QString pathPrefix;         //数据文件的存储路径前缀，默认为d:/measure data
    unsigned int nPicDistance;
    bool bCapture;
    QString m_strPrevPoleId;
    QString strLMSIPAddress;
    QString strDeviceStatusInfo;
    MeasureDebugData curMeaData;
    QVector<MeasureDebugData> prePoleData;
    QVector<MeasureDebugData> curPoleData;

    MeasureIdEx_t curMeaExData;
    //ui界面对象
    twoPlotForm *otherPlot;             //1 动态绘制拉出值和导高
    recordMeasureForm *recordMeasure;   //2 表格形式显示测量数据
    RailLineEdit *baseRailData;         //3 线路基础数据
    setForm *setform;                   //4 设置相关参数
    //状态栏对象
    QLabel *m_lblCurMeasure;            //1、状态栏指示当前测量状态，包括“未测量”，“测量中”几种选择
    QLabel *m_lblMeasureType;           //2、指示实时测量或模拟测量
       //
    QLabel *m_lbTourInfo;              //4、显示巡视线路名、行车方向、行车站区
    //其他对象
    radorScannerModule *m_pRadorScanner;        //1、扫描雷达对象
    QTimer *readTimerLMS;                       //4、控制动态测量的披露

    saveUI saveSubThread;

    bool fristmeasure;

    QTimer *tmRadarPower;

    PreRunSettingDlg *m_measureIntial;  //动态测量初始设置对话框
    //
    int m_fPrevMaxDaogao;
    int m_fPrevMinDaogao;
    int m_fDaogaoMax;
    int m_nPremaxLachuzhi;
    int m_nmaxLachuzhi;

    void createDIR(void);
    void initialUI(void);

    void detectSpacer(void);
    void setMeasureDB(RunProfile &profile);
    void radorInitial(void);
    void parse_biaoding(singleWires &,QString);
    void updateDeviceStatus();
    void measureParInial(void);
    void connectToDevices(void);
    void readConfigerFile(void);
    void getDevicesStatus(void);
    void updateUIRailInfo(void);
    void setMeasureStatusTip(bool);
    void getLimitValue(MeasureDebugData);

    void setProfile(RunProfile& pr, bool beReset=false);

    void setDisplayTourInfo(bool);
    void updateWireValue(singleWires &tmp);
    double m_virtualWalk;
    //超限参数
    int overWidthMax;
    int overWidthMin;
    int overHeightMax;
    int overHeightMin;
    int nFirstDropperStep;
    int minDropperStep;
    int maxDropperStep;
    void getDropperStep(double dist,QVector<double> &dropperSteps,float curWalk);
    float prePoleWalk;
    void speak1(QString );
    Speak speak;
    void  parse_dropperGrade(QVector<double> isDropper,double startDist,QVector<MeasureDebugData> &prePoleData);
    float  updatePoleDist(void );
    void verifyWatchdog(bool verifyType);
    void parseOnePoleData(bool flag);
    void onePolePar(MeasureDebugData limiteValue,int limitIndex);
    long simConut;
public slots:
    void slot_ConnectDevice();
    void slot_IPSensorStatus(bool);
    void slot_MeasureControl();
    void tcpDisconnect_slot(void);
    void slot_timerReadRador(void );
    void parameter_setting_success_slot(bool);
    void connectLMS_status_slot(bool bret);
    void slot_updateComData(comDataType);
    void slot_updateShowMes(QString);
    void slot_newRailPoleNumer(QString);
    void slot_changePoleId(QString toPole);
    void slot_meassureType(bool flag);
    void slot_shutupSys(void);
    void setVerifyInitial(wireStandardValues);

    void slot_clicked(void);//用于设置主界面pbtn的界面样式
    void stopMeasure(void);
    void slot_radarPowerOn(void);
    void slot_dataReplay(QStringList cc,double virtualWalk);
    void verifyFailure(void);
    void slot_dataUpdateTimer(bool,int);
    void slot_overParChange(void);
    void updateOverPar(void);
    void slot_updateErrorPole(bool f);
    void slot_radorPowerOff(void);
    void slot_minUI(void);
    void actionProcessing(void);
    void slot_cloudControl(CLOUDCONTROL type,bool flag);
protected:
    RunProfile profile;
    int    m_currIdx;
    int    m_prevIdx;           //边界条件，据此不进入计算支柱函数
    double  m_tinyspacer;       //据最近杆号的距离
    double  m_disapperSpacer;   //消失的整数个杆的距离和
    double m_startDist;         //火车开动时，距离线路表上+第一个支柱的距离
    MeasureDB  *m_pmDb;
    QTimer sysShut;
    void saveUIFrm(void);
    void stationChanged();
    void initParams(void);
    void closeEvent(QCloseEvent *event);
    void startMeasure(void);
    void resizeEvent(QResizeEvent *);
    void statusBarInitial(void);

    QVector<wireStandardValues> biaodingArrays;
    wireStandardValues verifyDatas;

    firstForm *firstUI;                         //启动主界面
    QStackedLayout *stackWdg;                   //界面堆栈，包含了启动主界面、检测曲线、检测数据表格、设置界面、基础数据等，每次仅显示选中的界面
    QVector<HoverBitmapBtn*>mainPbtnUI;         //用于主界面UI的选中格式
    int nBiaodingCount;

    void paintEvent(QPaintEvent *);
    void initialPbtnUI(HoverBitmapBtn *pbtn,QString path,QString hoverPath);

    simForm *m_simwidget;
    void alarmPar(int width,int height);
    int constMinPoleDist;    //最小跨距
    int m_alarmSlope;   //坡度预警参数
    bool bErrorPole;
    QPair<bool,int> preWireWidth;
    QPair<bool,QVector<MeasureDebugData>> midWireJudge;
    QPair<bool,QVector< MeasureDebugData > > onePoleDatas;
    QPair<bool,QPair<int,int> > bFirstTrend;
    QVector<int> trendCounts;
    void getExData(MeasureDebugData dval);
    bool trendChangedJudge(QVector<int> &trendChanged,int tmp);
    QString midPoleNum;
    void getBeforeHalfData(int index);
    bool judgeRetreat(void);
    QVector<MeasureDebugData> stackVerify;
    QPair<bool,QVector<int> > verifyEndJudge;
    void upDateVerifyJudageStack(int );
    void updateVerifyValue(int wireWidth,int wireHeight,int railWidth,int railHeight,int side);
    void parseVerifyData(void);
    QString preFlag;
    bool verifyWireType;//标定方式的选择，true,设备固定在某一点进行标定；false，设备推行过程中，获取之字值进行标定
    bool bSimForm;
    void verify_newType(singleWires &tmp );
    void newPoleArithmetic(void);
    int preSumVerify;
    void oldPoleModify(void);
    bool bOldPole;//由配置文件更新该值，确定杆号更新方式.true,利用对支柱的识别进行杆号定位;false,利用拉出值的‘之’字值进行杆号定位
    QPair<bool,double> preWalk;
    QPair<int ,int >getDataTrend(QVector<MeasureDebugData> &dataVector);
    bool parseTrend(QVector<MeasureDebugData> &datas,QPair<int,int> &rightTrend);
    void getMidWire(QVector< MeasureDebugData > &curPoleDatas);
    bool closeExe(void);
    DeviceStatus *m_pbtnRadorStatus;    //1、雷达连接状态提示
    DeviceStatus *m_pbtnDirectionStatus;     //6 行车方向
    DeviceStatus *m_pbtnVideoLeftStatus;    //3、雷达连接状态提示
    DeviceStatus *m_pbtnVideoRightStatus;    //4、雷达连接状态提示
    DeviceStatus *m_pbtnVideoMidStatus;    //5、雷达连接状态提示
    QString m_videoLeftName;
    QString m_videoRightName;
    QString m_videoMidName;
    VideoPlayFrm *ipVideo;                 //6 视频巡视界面
    //工具栏按钮
    QAction *exitACT;           //退出
    QAction *measureControl;       //启动
    QAction *measureStop;      //暂停
    QAction *connectToDevice;   //连接设备
    QLabel *lblRailwayInfo;
    //云台操控
    QAction *rightCloud;    //右转
    QAction *leftCloud;     //左转
    QAction *upCloud;    //右转
    QAction *downCloud;     //左转
    QTabWidget *centralTab;
    SerialPort *angleComSensor;                  //2、底盘参数对象，获取轨道几何参数、磁轨枕参数
    SerialPort *disComSensor;                  //2、底盘参数对象，获取轨道几何参数、磁轨枕参数

    SerialPort *radarPowerCom;                  //3、对雷达供电进行控制
    SerialPort *railComSensor;                  //2、底盘参数对象，获取轨道几何参数、磁轨枕参数
    DeviceStatus *m_pbtnSideStatus;    //2、雷达连接状态提示

    QString labelName;
    //添加临时线路数据
    QAction *actSetDisplayPar;     //显示界面设置
    QToolButton *pbtnClearPoleDis;
    void setToolsUI(void);
    double prePoleDist;
    void updatePoleInfo(void);
signals:
    void deviceStatusChange(bool);
    void rador_connectStatus(DEVICE_CONNECT_STATUS);
    void debuginfo(QString);
    void startinitialed(void );
    void dropperData(QPair<int,QVector<double> >);
    void newRailPole(void);
    void simStop(bool);
    void simStart(bool);
    void baseParChanged(void);
};

#endif // MAINWINDOW_H
