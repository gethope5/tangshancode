#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
  , m_lblCurMeasure(NULL),
    recordMeasure(NULL),
    m_pbtnRadorStatus(NULL),
    firstUI(NULL),
    setform(NULL),
    otherPlot(NULL),
    baseRailData(NULL),

    m_pRadorScanner(NULL),
    ipVideo(NULL)
{
    ui->setupUi(this);
    initParams();
    readConfigerFile();

    initialUI();
    radorInitial();
    createDIR();
    
//    this->menuBar()->setVisible(false);

#if DistanceDebug
    railComSensor->connectDevice();
    readTimerLMS->start(MEASURE_TIME_INTERVAL);
#endif
}
//函数功能：初始化ui界面
void MainWindow::initialUI(void)
{
    //创建传感器对象
    m_pRadorScanner=new radorScannerModule(this);       //1雷达设备
    //
    otherPlot=new twoPlotForm (ui->wdgMain);
    recordMeasure=new recordMeasureForm (pathPrefix,ui->wdgMain);
    baseRailData=new RailLineEdit (ui->wdgMain);
    setform=new setForm (ui->wdgMain);
    m_measureIntial= new PreRunSettingDlg(QString::fromWCharArray(L"系统初始设置"),this);

    radarPowerCom=new SerialPort(RADAR_COM,this);       //3轨距串口设备
    railComSensor=new SerialPort(RAIL_MCU_COM,this);    //2轨距串口设备
    m_simwidget=new simForm (this);

#ifdef GUIYANG_TUICHE
    disComSensor=new SerialPort(TS_RAIL_COM,this);
    ipVideo=new VideoPlayFrm("labelName",this);           //6 视频巡视界面
    centralTab=new QTabWidget(this);

    this->resize(QApplication::desktop()->availableGeometry().size());
    //    this->setWindowIcon(QIcon(QPixmap::fromImage(QImage("D:/guiyangTuiche/qt58-code0507/images/sysicon.ico"))));
    //    this->setWindowIcon(QIcon(QPixmap::fromImage(QImage(":/images/sysicon.ico"))));
    this->setWindowIcon(QIcon("./images/ApertureAdd.png"));
#else
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);//去掉标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon("./images/display.png"));
    stackWdg=new QStackedLayout;
    tmRadarPower=new QTimer(this);
    firstUI=new firstForm(ui->wdgMain);

    connect(firstUI,SIGNAL( signal_minUi()),this,SLOT(slot_minUI()));
#endif

    //创建UI对象

    //    connect(otherPlot->initialPosSet,SIGNAL(initialPos(RunProfile &)),this,SLOT(slot_getInitialPos(RunProfile&)));
    mainPbtnUI.clear();
    initialPbtnUI(ui->pbtnMainUI,("./images/firstUI.png"),("./images/firstUI-focus.png"));
    initialPbtnUI(ui->pbtnDetectedCurve,("./images/detectCurve.png"),("./images/detectCurve-focus.png"));
    initialPbtnUI(ui->pbtnDectedTable,"./images/detectTable.png","./images/detectTable-focus.png");
    //    initialPbtnUI(ui->pbtnRailData,"./images/database.png","./images/database-focus.png");
    ui->pbtnRailData->setHidden(true);
    initialPbtnUI(ui->pbtnSet,"./images/set.png","./images/set-focus.png");
    initialPbtnUI(ui->pbtnExit,"./images/exit.png","./images/exit-focus.png");
#if 1
    initialPbtnUI(ui->pbtnSave,"./images/save.png","./images/save-focus.png");
#else
    connect(ui->pbtnSave,SIGNAL(clicked()),this,SLOT(slot_clicked()));
    //    initialPbtnUI(ui->pbtnSave,"./images/save.png","./images/save-focus.png");
    ui->pbtnSave->setIcon(QIcon(QPixmap::fromImage(QImage("./images/save.png"))));
    ui->pbtnSave->setIconSize(QSize(50,50));
#endif
    //关联相关信号及槽函数
    connect(m_simwidget,SIGNAL( oneStrPackage(QStringList,double)),this,SLOT(slot_dataReplay(QStringList,double)));
    //    connect(m_simwidget,SIGNAL( simulationVerify()),setform,SLOT(slot_simulationVerify()));
    //调整超高产生对测量数据影响
    connect(m_simwidget,SIGNAL( updateBaseAngle(int,int ,bool)),m_pRadorScanner,SLOT(slot_simBaseAngle(int ,int ,bool)));
    connect(m_simwidget,SIGNAL(updateRailAngle(QPair<bool,bool>)),m_pRadorScanner,SLOT(slot_updateRailAngle(QPair<bool,bool>)));
    connect(m_simwidget,SIGNAL(startMeausre()),this,SLOT(slot_MeasureControl()));
    connect(m_simwidget,SIGNAL( updateWireRange(int,int)),m_pRadorScanner,SLOT(slot_updateWireRange(int,int)));
    //调试窗口的启动和停止,同步控制定时器readTimerLMS
    connect(m_simwidget,SIGNAL( controlTimer(bool,int)),this,SLOT(slot_dataUpdateTimer(bool,int)));
    //根据配置文件的状态，确定是否显示模拟数据窗口
    if(bSimForm)
    {
        m_simwidget->show();
//        if(QApplication::desktop()->numScreens()>1)
//        {
//            m_simwidget->setGeometry(QApplication::desktop()->screenGeometry(1));
//        }
        //将包含了支柱信息的一帧完整数据中的支柱部分在调试窗体simform中显示
        connect(m_pRadorScanner,SIGNAL( sideOrignalData(QVector<QPair<int,QPair<int,int> > > )),m_simwidget,SLOT(slot_updateSideCuve(QVector<QPair<int,QPair<int,int> > > )));
        //将导线在一帧数据中的位置利用+进行标识
        connect(m_pRadorScanner,SIGNAL( allwire(QVector<singleWireInfo>,bool)),m_simwidget,SLOT(slot_allwire(QVector<singleWireInfo>,bool)));
        //将一帧完整的数据在调试窗体simform中显示
        connect(m_pRadorScanner,SIGNAL( onePackage(QString)),m_simwidget,SLOT(radorData(QString)));
    }
    else
    {
        m_simwidget->hide();
    }
    MeasureDB::bSimUI=true;

    connect(this,SIGNAL( startinitialed()),m_simwidget,SLOT(slot_startinitialed()));

    connect(this,SIGNAL(dropperData(QPair<int,QVector<double> > )),otherPlot,SLOT(slot_dropperData(QPair<int,QVector<double> >)));
    connect(this,SIGNAL(newRailPole()),otherPlot->newRailPole,SLOT(slot_newPole()));
    connect(this,SIGNAL(simStop(bool)),m_simwidget,SLOT(simStop(bool)));
    connect(this,SIGNAL(simStart(bool)),m_simwidget,SLOT(simStart(bool)));
    connect(this,SIGNAL(baseParChanged()),m_pRadorScanner,SLOT(slot_baseWireParChanged()));
    connect(this,SIGNAL(baseParChanged()),otherPlot,SLOT(slot_overParChange()));
    connect(this,SIGNAL(baseParChanged()),this,SLOT(slot_overParChange()));

    connect(this,SIGNAL(baseParChanged()),railComSensor,SLOT(slot_baseRailParChanged()));

    connect(m_measureIntial,SIGNAL(clearDistanc(COM_COMMAND_TYPE)),railComSensor,SLOT(slot_deviceControl(COM_COMMAND_TYPE)));
    connect(m_measureIntial,SIGNAL(directFlag(int)),railComSensor,SLOT(setDirectionFlag(int)));
    connect(m_measureIntial,SIGNAL(setMeasureType(bool,QString)),otherPlot,SLOT(slot_initialPole(bool,QString)));
    //临时线路添加杆号
    //    connect(recordMeasure,SIGNAL(dataReplay(QVector<double> dDistance,QVector<QPoint> wire1,QVector<QPoint> wire2,QVector<double> dCarrierHeight,QVector<QPair<double,int> > &poles)),otherPlot,SLOT(slot_dataReplay(QVector<double> dDistance,QVector<QPoint> wire1,QVector<QPoint> wire2,QVector<double> dCarrierHeight,QVector<QPair<double,int> > &poles)));

    connect(recordMeasure,SIGNAL(dataReplay(QVector<double>,QVector<QPoint>,QVector<QPoint>,QVector<double>,QVector<QPair<double,int> > &)),otherPlot,SLOT(slot_dataReplay(QVector<double>,QVector<QPoint>,QVector<QPoint>,QVector<double>,QVector<QPair<double,int> > &)));
    //    connect(recordMeasure,SIGNAL(poleReplay(QVector<QPair<double,int> > &)),otherPlot,SLOT(slot_poleReplay(QVector<QPair<double,int> > &)));
    connect(recordMeasure,SIGNAL(showMessage(QString)),this,SLOT(slot_updateShowMes(QString)));

    //1连接设备信号
    //3界面开始测量信号
    connect(otherPlot,SIGNAL(errorPole(bool)),this,SLOT(slot_updateErrorPole(bool)));
    connect(otherPlot,  SIGNAL(MeasuresSatus()),this,SLOT(slot_MeasureControl()));
    connect(otherPlot->oldRailPole,  SIGNAL(changingPole(QString)),this,SLOT(slot_changePoleId(QString)));
    connect(otherPlot,SIGNAL(stopMeasure()),this,SLOT(stopMeasure()));
    connect(otherPlot->newRailPole,SIGNAL(newRailPole(QString)),this,SLOT(slot_newRailPoleNumer(QString)));


    //基础参数被修改后,及时更新对应的参数
    connect(setform,SIGNAL(conDevice( )),this,SLOT(slot_ConnectDevice( )));
    connect(setform,SIGNAL(signal_parDimarcate(wireStandardValues)),m_simwidget,SLOT(slot_getVerify(wireStandardValues)));
    connect(setform,SIGNAL(baseParChanged()),m_pRadorScanner,SLOT(slot_baseWireParChanged()));
    connect(setform,SIGNAL(baseParChanged()),railComSensor,SLOT(slot_baseRailParChanged()));
    connect(setform,SIGNAL(baseParChanged()),otherPlot,SLOT(slot_overParChange()));

    connect(setform,SIGNAL(baseParChanged()),this,SLOT(slot_overParChange()));

    connect(setform,SIGNAL(measureType(bool)),this,SLOT(slot_meassureType(bool)));
    //    connect(setform,SIGNAL(signal_parDimarcate(wireStandardValues)),this,SLOT(setVerifyInitial(wireStandardValues)));

    connect(setform,SIGNAL(expandFlag(bool)),recordMeasure,SLOT(slot_expandUI(bool)));
    connect(setform,SIGNAL(expandFlag(bool)),baseRailData,SLOT(slot_expandUI(bool)));
    connect(setform,SIGNAL(expandFlag(bool)),otherPlot,SLOT(slot_expandUI(bool)));

    statusBarInitial();
    //    qDebug()<<"set title";
    //    qDebug()<<"status title ";
    //检测车软件
#ifdef GUIYANG_TUICHE    
    ui->wdgMain->setVisible(false);
    //    ui->verticalLayoutSetNoConstraint
    ui->pbtnDectedTable->setVisible(false);
    ui->pbtnDetectedCurve->setVisible(false);
    ui->pbtnExit->setVisible(false);
    ui->pbtnMainUI->setVisible(false);
    ui->pbtnSet->setVisible(false);
    ui->pbtnSave->setVisible(false);
    ui->pbtnRailData->setVisible(false);

    setToolsUI();


    this->resize(QApplication::desktop()->availableGeometry().size().width(),QApplication::desktop()->availableGeometry().size().height()-30);

    //    this->resize(ui->centralWidget->size());
    //    centralTab->resize(ui->centralWidget->size().width(),ui->centralWidget->size().height()-50);
    this->centralTab->addTab(otherPlot,QString::fromWCharArray(L"实时测量图像(&S)"));
    this->centralTab->addTab(recordMeasure,QString::fromWCharArray(L"实时测量数据(&C)"));
    this->centralTab->addTab(setform,QString::fromWCharArray(L"参数设置(&P)"));
    this->centralTab->addTab(baseRailData,QString::fromWCharArray(L"线路基础信息(&R)"));
    this->centralTab->addTab(ipVideo,QString::fromWCharArray(L"巡视视频"));
    //    centralTab->setTabIcon(1,QIcon("./images/open.png"));
    centralTab->setIconSize(QSize(50,30));//->setMinimumHeight(100)
    this->setCentralWidget(centralTab);
    connect(ipVideo,SIGNAL(showMessage(QString)),this,SLOT(slot_updateShowMes(QString)));
#else
    stackWdg->addWidget(recordMeasure);
    stackWdg->addWidget(setform);
    stackWdg->addWidget(baseRailData);
    stackWdg->addWidget(otherPlot);
    stackWdg->addWidget(firstUI);
    ui->wdgMain->setLayout(stackWdg);
    stackWdg->setCurrentWidget(firstUI);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(radarPowerCom,SIGNAL(com_replay(COM_REPLAY_TYPE)),setform,SLOT(slot_comReplay(COM_REPLAY_TYPE)));
    //更新拉出值、导高
    connect(radarPowerCom,SIGNAL(updateShowInfo(QString)),this,SLOT(slot_updateShowMes(QString)));
    //更新串口数据，包括轨距、超高和里程
    connect(railComSensor,SIGNAL(baseAngle(float &)),m_pRadorScanner,SLOT(slot_updateBaseAngle(float &)));
    connect(railComSensor,SIGNAL(baseAngle(float &)),m_simwidget,SLOT(slot_updateRailAngle(float &)));
    connect(railComSensor,SIGNAL(updateDeviceInfos(deviceInfo)),setform,SLOT(slot_updateDeviceInfo(deviceInfo)));
    connect(railComSensor,SIGNAL(getComData(comDataType)),this,SLOT(slot_updateComData(comDataType )));
    connect(railComSensor,SIGNAL(com_replay(COM_REPLAY_TYPE)),setform,SLOT(slot_comReplay(COM_REPLAY_TYPE)));

    connect(setform,SIGNAL(railComControl(COM_COMMAND_TYPE)),railComSensor,SLOT(slot_deviceControl(COM_COMMAND_TYPE)));
    connect(setform,SIGNAL(radorAndWIFIControl(COM_COMMAND_TYPE)),radarPowerCom,SLOT(slot_deviceControl(COM_COMMAND_TYPE)));
    connect(setform,SIGNAL(radorPowerOff()),this,SLOT(slot_radorPowerOff()));
    connect(tmRadarPower,SIGNAL(timeout()),this,SLOT(slot_radarPowerOn()));
    tmRadarPower->start(200);
#endif
}
//函数功能：用于检测车部分工具栏的初始化
void MainWindow::setToolsUI(void)
{
    connectToDevice = new QAction(QIcon("./images/connectDevice.png"), QString::fromWCharArray(L"连接设备"), this);
    connectToDevice->setStatusTip(QString::fromWCharArray(L"连接设备"));
    connect(connectToDevice, SIGNAL(triggered()), this, SLOT(actionProcessing()));


    measureControl= new QAction(QIcon("./images/play.png"),QString::fromWCharArray(L"开始测量"), this);
    measureControl->setStatusTip(QString::fromWCharArray(L"开始测量"));
    connect(measureControl, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    measureStop= new QAction(QIcon("./images/stop.png"),QString::fromWCharArray(L"停止测量"), this);
    measureStop->setStatusTip(QString::fromWCharArray(L"停止测量..."));
    connect(measureStop, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    rightCloud = new QAction(QIcon("./images/right.png"),QString::fromWCharArray(L"云台右转"), this);
    rightCloud->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    rightCloud->setStatusTip(QString::fromWCharArray(L"云台右转..."));
    connect(rightCloud, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    leftCloud = new QAction(QIcon("./images/left.png"),QString::fromWCharArray(L"云台左转"), this);
    leftCloud->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    leftCloud->setStatusTip(QString::fromWCharArray(L"云台左转..."));
    connect(leftCloud, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    upCloud = new QAction(QIcon("./images/up.png"),QString::fromWCharArray(L"云台上转"), this);
    upCloud->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    upCloud->setStatusTip(QString::fromWCharArray(L"云台上转..."));
    connect(upCloud, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    downCloud = new QAction(QIcon("./images/down.png"),QString::fromWCharArray(L"云台下转"), this);
    downCloud->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    downCloud->setStatusTip(QString::fromWCharArray(L"云台下转..."));
    connect(downCloud, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    actSetDisplayPar = new QAction(QIcon("./images/screen-settings.png"),QString::fromWCharArray(L"界面设置"), this);
    actSetDisplayPar->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    actSetDisplayPar->setStatusTip(QString::fromWCharArray(L"测量曲线界面设置..."));
    connect(actSetDisplayPar, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    exitACT = new QAction(QIcon("./images/tool-exit.png"),QString::fromWCharArray(L"退出"), this);
    exitACT ->setShortcuts(QKeySequence::Close);
    exitACT->setStatusTip(QString::fromWCharArray(L"关闭程序..."));
    connect(exitACT, SIGNAL(triggered()),this, SLOT(actionProcessing()));

    ui->mainToolBar->setIconSize(QSize(50,50));
    ui->mainToolBar->addAction(connectToDevice);
    ui->mainToolBar->addAction(measureControl);
    ui->mainToolBar->addAction(measureStop);
    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(rightCloud);
    ui->mainToolBar->addAction(leftCloud);
    ui->mainToolBar->addAction(upCloud);
    ui->mainToolBar->addAction(downCloud);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actSetDisplayPar);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(exitACT);
    ui->mainToolBar->addSeparator();

    lblRailwayInfo=new QLabel(this);
    lblRailwayInfo->setStyleSheet("font: 900 12pt \"微软雅黑\";");
    ui->mainToolBar->addWidget(lblRailwayInfo);
    lblRailwayInfo->setText(QString::fromWCharArray(L"线路信息："));
    ui->mainToolBar->setMovable(false);
    //    ui->menu->addAction(connectToDevice);
    //    ui->menu->addAction(measureControl);
    //    ui->menu->addAction(measureStop);
    //    ui->menu->addSeparator();
    //    ui->menu->addAction(rightCloud);
    //    ui->menu->addAction(leftCloud);
    //    ui->menu->addAction(upCloud);
    //    ui->menu->addAction(downCloud);
    //    ui->menu->addSeparator();
    //    ui->menu->addAction(actSetDisplayPar);
    //    ui->menu->addSeparator();
    //    ui->menu->addAction(exitACT);
    qDebug()<<"end tool";
}
//函数功能：处理工具栏各按钮的单击信号
void MainWindow::actionProcessing()
{
    //    qDebug()<<"tool run here";
    QAction *getActSender=(QAction*)this->sender();
    if(getActSender==this->exitACT)
    {
        closeExe();
    }
    else if(getActSender==connectToDevice)
    {
        connectToDevices();    //连接设备
    }
    else if(getActSender==measureControl)
    {

        //        emit MeasuresSatus();            //开始测量


    }
    else if(getActSender==measureStop)
    {
        stopMeasure();
    }
    //云台控制
    else if(getActSender==rightCloud)
    {
        slot_cloudControl(RIGHT,true);
    }
    else if(getActSender==leftCloud)
    {
        slot_cloudControl(LEFT,true);
    }
    else if(getActSender==upCloud)
    {
        slot_cloudControl(UP,true);
    }
    else if(getActSender==downCloud)
    {
        slot_cloudControl(DOWN,true);
    }
    else if(getActSender==actSetDisplayPar)
    {
        otherPlot->setDisplayPar();
    }
}
//函数功能：云台控制指令，包括左转、右转、复位、停止
void MainWindow:: slot_cloudControl(CLOUDCONTROL type,bool flag)
{
#ifdef GUIYANG_TUICHE
    ipVideo->cloudControl(type);
#endif
}
void MainWindow::slot_dataReplay(QStringList cc,double virtualWalk)
{
    //    QString cc;
    //    m_pRadorScanner->(cc);
    //    radorScannerModule
    int failCount=0;
    //    for(int i=0;i<cc.count();i++)
    //    {
    if(!readTimerLMS->isActive())
        readTimerLMS->start(MEASURE_TIME_INTERVAL);

    //    if(virtualWalk)
    curMeaData.nWalkDistance=QString::number(virtualWalk,'f',2).toFloat();
    //        qDebug()<<cc.at(i);
    //        QString tmp=cc.at(i);
    //        cc.join()
    //        m_pRadorScanner->parser_measure_data(tmp.toLocal8Bit());
    //    QTime time;
    //    time.start();
    m_pRadorScanner->parser_measure_data(cc.join(0x20).toLocal8Bit());
    //    qDebug()<<"parse one package tm="<<time.elapsed();
    //    }
    //    qDebug()<<"fail data count="<<failCount;
}

void MainWindow::slot_radarPowerOn(void)
{
#ifndef GUIYANG_TUICHE
    radarPowerCom->radarOn();
    tmRadarPower->stop();
#else
    connectToDevices();
#endif
}
void  MainWindow::statusBarInitial(void)
{
    //状态栏界面设置
    m_lblMeasureType=new QLabel(this->statusBar());
    //    m_lblMeasureType->setText(QTextCodec::codecForName("gbk")->toUnicode("实时测量"));
    m_lblMeasureType->setFixedWidth(60);
    m_lblMeasureType->setToolTip(QTextCodec::codecForName("gbk")->toUnicode("当前测量模式"));

    m_lbTourInfo=new QLabel(this->statusBar());
    m_lbTourInfo->setText(QTextCodec::codecForName("gbk")->toUnicode("成渝线"));
    m_lbTourInfo->setFixedWidth(200);
    m_lbTourInfo->setToolTip(QTextCodec::codecForName("gbk")->toUnicode("显示当前巡视信息"));


    m_lblCurMeasure=new QLabel(this->statusBar());
    m_lblCurMeasure->setFixedWidth(60);
    m_lblCurMeasure->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_lblCurMeasure->setText(QTextCodec::codecForName("gbk")->toUnicode("未测量"));
    m_lblCurMeasure->setToolTip(QTextCodec::codecForName("gbk")->toUnicode("当前测量状态"));
    m_pbtnRadorStatus=new DeviceStatus(QString::fromWCharArray(L"激光雷达"),DISCONNECTED,this);
    m_pbtnDirectionStatus=new DeviceStatus(QString::fromWCharArray(L"前进"),GO,this);

    ui->statusBar->setStyleSheet("border:50px;border-color:red");
    ui->statusBar->setFixedHeight(40);
    connect(m_pRadorScanner,SIGNAL(rador_connectStatus(DEVICE_CONNECT_STATUS)),m_pbtnRadorStatus,SLOT(slot_deviceStatusChange(DEVICE_CONNECT_STATUS)));
    connect(m_pRadorScanner,SIGNAL(rador_connectStatus(DEVICE_CONNECT_STATUS)),setform,SLOT(slot_radorConnect(DEVICE_CONNECT_STATUS)));
    connect(this,SIGNAL(rador_connectStatus(DEVICE_CONNECT_STATUS)),setform,SLOT(slot_radorConnect(DEVICE_CONNECT_STATUS)));

    ui->statusBar->addPermanentWidget(m_lbTourInfo);
    ui->statusBar->addPermanentWidget(m_lblCurMeasure);
    ui->statusBar->addPermanentWidget(m_lblMeasureType);
    ui->statusBar->addPermanentWidget(m_pbtnDirectionStatus);
#ifndef GUIYANG_TUICHE
    m_pbtnSideStatus=new DeviceStatus(QString::fromWCharArray(L"支柱"),DISCONNECTED,this);
    ui->statusBar->addPermanentWidget(m_pbtnSideStatus);
#else
    m_pbtnVideoLeftStatus=new DeviceStatus(m_videoLeftName,DISCONNECTED,this);
    m_pbtnVideoRightStatus=new DeviceStatus(m_videoRightName,DISCONNECTED,this);
    m_pbtnVideoMidStatus=new DeviceStatus(m_videoMidName,DISCONNECTED,this);
    if(!ipVideo)
    {
        ipVideo=new VideoPlayFrm("labelName",this);           //6 视频巡视界面
    }
    connect(ipVideo->videoLeftDevice,SIGNAL(videoStatus(DEVICE_CONNECT_STATUS)),m_pbtnVideoLeftStatus,SLOT(slot_deviceStatusChange(DEVICE_CONNECT_STATUS)));
    connect(ipVideo->videoMidDevice,SIGNAL(videoStatus(DEVICE_CONNECT_STATUS)),m_pbtnVideoRightStatus,SLOT(slot_deviceStatusChange(DEVICE_CONNECT_STATUS)));
    connect(ipVideo->videoRightDevice,SIGNAL(videoStatus(DEVICE_CONNECT_STATUS)),m_pbtnVideoMidStatus,SLOT(slot_deviceStatusChange(DEVICE_CONNECT_STATUS)));

    ui->statusBar->addPermanentWidget(m_pbtnVideoLeftStatus);
    ui->statusBar->addPermanentWidget(m_pbtnVideoRightStatus);
    ui->statusBar->addPermanentWidget(m_pbtnVideoMidStatus);
#endif
    ui->statusBar->addPermanentWidget(m_pbtnRadorStatus);
    setDisplayTourInfo(false);
    if(m_bMeasureType)
    {
#if DEBUG_DATA
        connectToDevices();
#endif
    }
    slot_meassureType(m_bMeasureType);
}
void MainWindow::setDisplayTourInfo(bool flag)
{
    m_lbTourInfo->setVisible(flag);
}
//函数功能：初始化雷达设备参数
void MainWindow::radorInitial(void)
{
#if 0
    connect(m_pRadorScanner, SIGNAL(connectLMS_status_signal(bool)),this, SLOT(parameter_setting_success_slot(bool)));
#else
    connect(m_pRadorScanner, SIGNAL(connectLMS_status_signal(bool)),this, SLOT(connectLMS_status_slot(bool)));
    connect(m_pRadorScanner, SIGNAL(parameter_setting_success_signal(bool)),this, SLOT(parameter_setting_success_slot(bool)));
#endif
    //connect(m_pRadorScanner, SIGNAL(bufReady()), this, SLOT(readRadorData()));
    connect(m_pRadorScanner, SIGNAL(tcpDisconnect()), this, SLOT(tcpDisconnect_slot()));

    connect(otherPlot, SIGNAL(endWire2()), m_pRadorScanner, SLOT(slot_updateWire2()));
    connect(m_pRadorScanner, SIGNAL(wire2End()), otherPlot, SLOT(slot_endWire2()));
    connect(m_pRadorScanner, SIGNAL(wire1Create()), otherPlot, SLOT(slot_createWire1Curve()));
    connect(m_pRadorScanner, SIGNAL(carrierCreate()), otherPlot, SLOT(slot_createCarrier()));

    //    connect(m_pRadorScanner, SIGNAL(getLMSAngleInfor(radorAngleParametrize )),this,SLOT(getLMSAngleInfo(radorAngleParametrize)));
    readTimerLMS=new QTimer(this);
    connect(readTimerLMS,SIGNAL(timeout()),this,SLOT(slot_timerReadRador()));
}
//函数功能：雷达参数设置成功槽函数
void MainWindow::parameter_setting_success_slot(bool bBuffer)
{
    qDebug() <<" ++++++++++ LMS device set parameter successfully! +++++++++";
    //    strDeviceStfatusInfo+=QString::fromWCharArray(L" 雷达初始化完成!");
    ui->statusBar->showMessage(strDeviceStatusInfo);

}
//函数功能：雷达连接状态判断.bret=true，连接成功；bret=false，连接失败
void MainWindow::connectLMS_status_slot(bool bret)
{
    m_pRadorScanner->setVirtual(false);
    qDebug()<<"the called thread"<<QThread::currentThreadId()<<bret;
    if (bret)
    {
        m_pRadorScanner->setLMSDeviceParameter();
        statusBar()->showMessage(QTextCodec::codecForName("gbk")->toUnicode("雷达连接成功!!!"));
        getDevicesStatus();
    }
    else
    {
        qDebug()<<"can not connect......."<<reconnectCount;
        QString cc=statusBar()->currentMessage().replace(QString("^[0-9]+$"),QString::number(reconnectCount));
        statusBar()->showMessage(cc);

        if(reconnectCount++<10)
        {
            m_pRadorScanner->connectLMSDevice(strLMSIPAddress, 2111);
            //                 QMessageBox::critical(this, "ERROR", tr("请确认雷达设备是否正常！"));
        }
        else
        {
            qDebug()<<"reconnectCount is over 10.......";
            statusBar()->showMessage(QString::fromWCharArray(L"雷达未连接成功!!!"));
            return;
        }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
    if(railComSensor)
        delete railComSensor;
    if(m_pmDb)
        delete m_pmDb;
    if(otherPlot)
        delete otherPlot;
    if(recordMeasure)
        delete recordMeasure;
    if(setform)
        delete setform;
    if(baseRailData)
        delete baseRailData;
    if(m_simwidget)
    {
        m_simwidget->close();
        m_simwidget->hide();
        delete m_simwidget;
    }
}
void MainWindow::slot_ConnectDevice( )
{
    connectToDevices();
}
void MainWindow::tcpDisconnect_slot(void)
{
    if(m_bMeasureType)
        statusBar()->showMessage(QString::fromWCharArray(L"激光雷达未连接成功..."));
    readTimerLMS->stop();
}
void MainWindow::readConfigerFile(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);

    strLMSIPAddress=settings.value("Rador/LMSIPAddress", "192.168.1.8").toString();
    nPicDistance=settings.value("initialPar/PictureDistance", 4).toInt();
    pathPrefix=settings.value("system/DataPaths","d:/measure data").toString();
    qDebug()<<"measure data save paths="<<pathPrefix;

    m_bMeasureType=settings.value("system/measureType", "true").toBool();
    bOldPole=settings.value("system/poleType", "true").toBool();
    verifyWireType=settings.value("system/verifyType", "true").toBool();
    bSimForm=settings.value("system/bSimUI", "true").toBool();
    if(verifyWireType)
        qDebug()<<"+++++++++++++++++++veriy old type,get veriy data when device is not moving";
    else
        qDebug()<<"+++++++++++++++++++veriy new type,get veriy data by limit value";
    if(m_bMeasureType)
    {
        qDebug()<<"m_bMeasureType="<<m_bMeasureType<<QString::fromWCharArray(L"实时测量");
    }
    else
    {
        qDebug()<<"m_bMeasureType="<<m_bMeasureType<<QString::fromWCharArray(L"模拟测量");
    }
    nFirstDropperStep=settings.value("initialPar/firstDropper", 4).toInt();
    minDropperStep=settings.value("initialPar/minRange", 4).toInt();
    maxDropperStep=settings.value("initialPar/maxRang", 4).toInt();

    m_videoLeftName=QString::fromWCharArray(L"左视频");
    m_videoRightName=QString::fromWCharArray(L"右视频");
    m_videoMidName=QString::fromWCharArray(L"受电弓视频");
    updateOverPar();
}
void MainWindow::slot_shutupSys(void)
{
    sysShut.stop();
    close();
}
//函数功能：连接所有设备
void MainWindow::connectToDevices()
{
    qDebug()<<"measure type staus"<<m_bMeasureType<<m_pRadorScanner->getRadorStatus();
    if(m_bMeasureType)
    {
        //2.连接雷达
        if(m_pRadorScanner->getRadorStatus())
        {
            qDebug()<<"rador is connected!";
            emit rador_connectStatus(CONNECTED);
        }
        else
        {
            m_pRadorScanner->connectLMSDevice(strLMSIPAddress, 2111);
        }
        //3.连接角度传感器IP
        //    if(!setform->currentAngleTcpStatus)
        //5.连接地盘串口设备
        if(!railComSensor->openStatus())
        {
            if(railComSensor->connectDevice())
            {
                railComSensor->startRail();
            }
            qDebug()<<"comm is opened";//<<railComSensor->connectDevice();
        }
        else
        {
            railComSensor->startRail();
            //            qDebug()<<"com is not open";
        }

        //    if(cloudIPDevice->curTcpStatus|drawMeasure->currentRadorStatus)
        //        QMessageBox::about(this,tr("提示"),showData.append(tr("已经连接!!!")));
    }
    //    else
    //    {
    //        isSensorsOK=true;
    //    }
}

void MainWindow::slot_meassureType(bool flag)
{
    if(flag)
    {
        m_lblMeasureType->setText(QString::fromWCharArray(L"实时测量"));
    }
    else
    {
        m_lblMeasureType->setText(QString::fromWCharArray(L"模拟测量"));
    }
    m_bMeasureType=flag;
}
//函数功能：由模拟调试UI或测量曲线界面的开始测量信号触发该槽函数。
void MainWindow::slot_MeasureControl()
{
    //true false false true
    //    qDebug()<<"measure status"<<MeasureDB::bVerify<<MeasureDB::bRadarStatus<<MeasureDB::bSimDraw<<m_bMeasureType;
    if(!MeasureDB::bVerify)
    {
        QMessageBox::warning(this, QString::fromWCharArray(L"提示"), QString::fromWCharArray(L"标定中..."));
        return ;
    }
    //实际测量情况下进行设备连接状态检测
    if(!MeasureDB::bSimDraw)
    {
        if(!MeasureDB::bRadarStatus)
        {
            if(m_bMeasureType)
            {
                qDebug()<<"connect 1";
                connectToDevices();
                QMessageBox::warning(this, QTextCodec::codecForName("gbk")->toUnicode("提示"), QTextCodec::codecForName("gbk")->toUnicode("设备未连接..."));
                return;
            }
        }
    }
    //        qDebug()<<"bmeasure="<<MeasureDB::bMeasure;
    if(MeasureDB::bMeasure)
    {
        if(m_bMeasureType)
        {
            if(MeasureDB::bSimDraw)
            {
                emit simStop(false);
            }
            else
            {
                if(!m_pRadorScanner->getRadorStatus())
                {
                    qDebug()<<"m_bRadorInitialSuccess  false";
                    return;
                }
            }
        }
        if(!MeasureDB::bSimDraw)
        {
            readTimerLMS->stop();
        }
        //        qDebug()<<"bmeasure="<<MeasureDB::bMeasure;
        setMeasureStatusTip(MeasureDB::bMeasure);
    }
    else
    {
        qDebug()<<"fristmeasure value="<<fristmeasure;
        if(fristmeasure)
        {
            startMeasure();
            setMeasureStatusTip(MeasureDB::bMeasure);
        }
        else
        {
            //获取线路基础数据库中已有线路的名称
            //            QStringList cc=Global::getInstance()->gbase.getRailList();
            //m_measureIntial->setOldRailList(cc);
            m_measureIntial->fileName();
            bool createDB=m_measureIntial->exec();
            if(createDB)
            {
                //railType=true,临时线路;false,老线路
                measureParInial();
                m_measureIntial->getProfile(profile);
                MeasureDB::strFileData=profile.dbpath.left(profile.dbpath.lastIndexOf(".db"));
                setMeasureDB(profile);//创建数据库文件
                MeasureDB::bVerify=m_measureIntial->isVerify();
                //测量
                if(MeasureDB::bVerify)
                {
                    setMeasureStatusTip(MeasureDB::bMeasure);
                }
                //标定
                else
                {
                    verifyDatas=m_measureIntial->getStandardValue();

                    qDebug()<<"std wire par,height="
                           <<verifyDatas.nVerifyWireHeight
                          <<"width"<< verifyDatas.nVerifyWireWidth
                         <<"side limit ="<<verifyDatas.nVerifySide;
                    setVerifyInitial(verifyDatas);//进行标定相关的初始设置，包括启动雷达(实时测量状态)或者模拟数据进行标定(仿真状态)
                    return ;
                }
                otherPlot->setMeasureStatus(MeasureDB::bMeasure);
                //里程清零
                if(m_measureIntial->bClearDist)
                {
                    curMeaData.nWalkDistance=0;
                }

                otherPlot->setRailType(true);
                curMeaData.poleID=QString::number(MeasureDB::nPoleNum);
                qDebug()<<"initial pole id"<<curMeaData.poleID;
                //                bDetectPoleNewRail=true;
                preSS=curMeaData.nWalkDistance;

                qDebug()<<"profile dbpat="<<profile.dbpath<<profile.startKm;
                startMeasure();
                //                recordMeasure->selectShowTable=false;
                //                QMessageBox::about(this,tr("this"),profile.dbpath);
                //                setDisplayTourInfo(true);
                if(MeasureDB::bSimDraw)
                    emit startinitialed();
            }
            else
            {
                return;
            }
        }
    }
    MeasureDB::bMeasure=!MeasureDB::bMeasure;
}
void MainWindow::setMeasureStatusTip(bool flag)
{
    if(flag)
    {
        m_lblCurMeasure->setText(QString::fromWCharArray(L"暂停测量"));
    }
    else
    {
        m_lblCurMeasure->setText(QString::fromWCharArray(L"测量中"));
    }
    otherPlot->setMeasureStatus(flag);
}
void MainWindow::slot_newRailPoleNumer(QString newPoleNumber)
{
    MeasureDB::nPoleNum=newPoleNumber.toInt();
    //    otherPlot->updateMeasureData(curMeaData,curMeaExData,false);
    bDetectPoleNewRail=true;//应用于利用里程定位杆号时
}
void MainWindow::measureParInial(void)
{
    fristmeasure=true;
}
void MainWindow::startMeasure(void)
{
    if(m_bMeasureType)
    {
        readTimerLMS->start(MEASURE_TIME_INTERVAL);
        //  pciDistance->startPCI();
        qDebug()<<"start measure";
        //onePole1->ra
        if(MeasureDB::bSimDraw)
        {
            emit simStart(true);
        }
        else
        {
            m_pRadorScanner->startLMSDevice();
            railComSensor->railMeasure();
        }
        //    railComSensor->clearDistance();
        //    bClearDistance=true;
    }
    else
        readTimerLMS->start(MEASURE_TIME_INTERVAL);
}
void MainWindow::stopMeasure(void)
{
    //    if(MeasureDB::bMeasure)
    {
        if(readTimerLMS->isActive())
            readTimerLMS->stop();
        //pciDistance->stopPCI();
        setMeasureStatusTip(true);
        //        fristmeasure=false;
        //if(m_bMeasureType)
        //{
        //  m_pRadorScanner->closeLMS();
        //}
        //isSensorsOK=false;
        //        if(!m_pRadorScanner->getRadorStatus())
        //        {
        //            qDebug()<<"m_bRadorInitialSuccess  false";
        //        }
        initParams();
        if(MeasureDB::bSimDraw)
        {

            emit simStop(true);
        }
        else
        {
            m_pRadorScanner->stopLMSDevice();
            railComSensor->clearDistance();
        }
        otherPlot->clearCurves();
        recordMeasure->clearTable();
        m_pRadorScanner->clearStackData();

        MeasureDB::bMeasure=false;
        m_lblCurMeasure->setText(QString::fromWCharArray(L"未测量"));
    }
}
void MainWindow::slot_timerReadRador(void )
{
    if(m_bMeasureType)
    {
        railComSensor->railMeasure();
        //        m_pRadorScanner->startLMSDevice();
        //                QMessageBox::about(this,tr("this"),"shishi");
    }
    else
        //模拟测量处理
    {
        comDataType tmpCom;
        static unsigned long count=0;
        unsigned int c1=rand()%5;
        tmpCom.walkDistance=3*(count++)+(rand()%10)*0.6;
        tmpCom.nRailWidth=1435;
        tmpCom.nRailHeight=10;
        if(MeasureDB::bVerify)
        {
            slot_updateComData(tmpCom);
            singleWires tmp;
            tmp.wire1=QPoint(200+c1,5500+c1);
            tmp.wire2=QPoint(122+c1,5600+c1);
            updateWireValue(tmp);
        }
        else
        {
            curMeaData.nRailHeight=tmpCom.nRailHeight;
            curMeaData.nRailWidth=tmpCom.nRailWidth;
            curMeaData.wire1=QPoint(200+c1,5500+c1);
        }
    }
    //    singleWires wireValue=m_pRadorScanner->wireValue;
    updateWireValue(m_pRadorScanner->wireValue);
    //    QMessageBox::about(this,tr("this"),"what's wrong");
    //    m_pmDb->insertRecord_debugMeasure(curMeaData);
}
//函数功能：后退判别，返回true，后退；false，前进
bool MainWindow::judgeRetreat(void)
{
    if(preWalk.second<-1000)
    {
        if(curMeaData.nWalkDistance!=0)
        {
            preWalk.second=curMeaData.nWalkDistance;
            qDebug()<<"value"<<curMeaData.nWalkDistance;
            if(curMeaData.nWalkDistance>0)
            {
                preWalk.first=true;
            }
            else if(curMeaData.nWalkDistance<0)
            {
                preWalk.first=false;
            }
        }
    }
    else
    {
        //        qDebug()<<"first "<<preWalk.second<<curMeaData.nWalkDistance;
        if(preWalk.first)
        {
            if(preWalk.second<=curMeaData.nWalkDistance)
            {
                preWalk.second=curMeaData.nWalkDistance;
                m_pbtnDirectionStatus->setCurStatus(GO);
            }
            else
            {
                m_pbtnDirectionStatus->setCurStatus(BACK);
                return true;

            }
        }
        else
        {
            if(preWalk.second>=curMeaData.nWalkDistance)
            {
                preWalk.second=curMeaData.nWalkDistance;
                m_pbtnDirectionStatus->setCurStatus(GO);
            }
            else
            {
                m_pbtnDirectionStatus->setCurStatus(BACK);
                return true;
            }
        }
    }
    return false;
}
//函数功能：由雷达数据（拉出值和导高）更新触发该槽函数
void MainWindow::updateWireValue(singleWires &tmp)// QPoint wireInfo1,QPoint wireInfo2,QString strOrignalData)
{
    //qDebug()<<"value"<<tmp.wire1<<tmp.wire2<<MeasureDB::bMeasure<<tmp.valid<<railType;
    if(MeasureDB::bVerify)//bVerify=false,开始标定
    {
        if( judgeRetreat())
        {
            MeasureDB::moving=false;
            return ;
        }
        else
        {
            MeasureDB::moving=true;
        }
        if(MeasureDB::bMeasure&&tmp.valid)
        {
            //            qDebug()<<"pre walk "<<preWalkDistance<<curMeaData.nWalkDistance;
            //slot_newRailPoleNumer
            if(bOldPole)
            {
                curMeaData.poleID=MeasureDB::nPoleNum;
                curMeaData.station=profile.station;
            }
            curMeaData.wire2Valid=tmp.wires2Judge;
            curMeaData.tm=QDateTime::currentDateTime().toString("hh-mm-ss");
            curMeaData.wire1=tmp.wire1;
            curMeaData.wire2=tmp.wire2;
            //            curMeaData.
            curMeaData.dKm=profile.startKm+(curMeaData.nWalkDistance/1000.000);

            curMeaData.carrier=tmp.carrier;

            if(tmp.wire500.first)
            {
                tmp.wire500.first=false;
                curMeaData.wire500=tmp.wire500.second;
                if(!MeasureDB::oneTableData)
                    curMeaExData.onePolePar.wire500Height=tmp.wire500.second;
            }
            else
            {
                curMeaData.wire500=0;
            }
            //          qDebug()<<"original data "<<strOrignalData.split(" ").count();
            if(!MeasureDB::oneTableData)
            {
                curMeaExData.tm=QDateTime::currentDateTime().toString("hh-mm-ss");
                curMeaExData.nWalkDistance=curMeaData.nWalkDistance;
                curMeaExData.dKm=curMeaData.dKm;
                curMeaExData.station =curMeaData.station;
                curMeaExData.poleid= curMeaData.poleID;
            }
            //  dItem.structFlag = polesets[m_currIdx].structFlag;
            //  dItem.v = m_velocity * 3.6;
            //  dItem.tm = m_elaspeTm;
            //  qDebug()<<"walkDistance="<<curMeaData.nWalkDistance;
            //向数据文件中插入当前测量数据及原始数据
            //  m_measureIntial->m_pmDb->insertRecord_debugMeasure(curMeaData,strOrignalData);
            //  qDebug()<<"new rail number="<<curMeaData.station;
            //  qDebug()<<"insert table"<<
            alarmPar(curMeaData.wire1.x(),curMeaData.wire1.y());//参数播报
            //curMeaData.dataType=QString::fromWCharArray(L"普通");

            recordMeasure->updateShowTable(true);

            //qDebug()<<"get value"<<tmp.wire1.x()<<tmp.wire1.y()<<overWidthMin<<overWidthMax<<overHeightMin<<overHeightMax;
            //qDebug()<<"over par is ok "<<m_pmDb->insertRecord_overPars(curMeaData)<<MeasureDB::overAlarm;
            getLimitValue(curMeaData);
            //            curMeaData.wire1;
            updateUIRailInfo();
#ifdef GUIYANG_TUICHE
            otherPlot->updateMeasureData(curMeaData,0);
            m_pmDb->insertRecord_detailAndVerify(curMeaData,0,true);
#else
            //更新侧面界限值,包括向曲线界面显示侧面界限、数据表格中添加侧面界限数据、主界面的支柱指示器
            if(tmp.side)
            {
                if(!MeasureDB::oneTableData)
                    curMeaExData.sideLimit=tmp.side;
                otherPlot->updateMeasureData(curMeaData,tmp.side);
                m_pmDb->insertRecord_detailAndVerify(curMeaData,tmp.side,true);
                m_pbtnSideStatus->setCurStatus(CONNECTED);
            }
            else
            {
                m_pbtnSideStatus->setCurStatus(DISCONNECTED);
                otherPlot->updateMeasureData(curMeaData,0);
                m_pmDb->insertRecord_detailAndVerify(curMeaData,0,true);
            }
#endif
            //分析杆号信息
            if(bOldPole)
            {
                detectSpacer();
                oldPoleModify();
            }
            else
            {
#if 0
                newPoleArithmetic();
#else
                //                qDebug()<<simConut++<<QString("d=%1,poleEX=%2,all Ex=%3,h=%4,qx=%5,pole str=%6").arg(curMeaData.nWalkDistance,5).arg(tmp.exBuffer1,6).arg(tmp.exValue,6).arg(tmp.wire1.y()-1811,4).arg(tmp.qxPole).arg(tmp.poleBuffer,45)<<
                //                          prePoleDist<<bDetectPoleNewRail;

                bool bUpdateData=false;
                //新杆号=((侧面界限)|(qxPole<800))&&(里程)
                if((/*(tmp.exValue>2500&&tmp.exBuffer1)||*/(tmp.side)||(tmp.qxPole<800))&&(!prePoleDist||((abs(prePoleDist-curMeaData.nWalkDistance)>constMinPoleDist)&&prePoleDist)))
                {
                    prePoleData<<curMeaData;
                    bUpdateData=true;
                    qDebug()<<"new pole"<<tmp.side<<tmp.qxPole<<prePoleDist<<curMeaData.nWalkDistance;
                    prePoleDist=curMeaData.nWalkDistance;
                }
                else
                {
                    prePoleData<<curMeaData;
                    if(bDetectPoleNewRail)
                    {
                        bDetectPoleNewRail=false;
                        bUpdateData=true;
                    }
                }
                if(tmp.side)
                {
                    //                    qDebug()<<"side pole";
                    tmp.side=0;
                }
                if(bUpdateData)
                {
                    bUpdateData=false;
                    updatePoleInfo();
                    otherPlot->setPole(curMeaData.poleID,curMeaData.nWalkDistance);
                    qDebug()<<"                         i get the pole,"<<curMeaData.nWalkDistance<<curMeaData.poleID<<tmp.exValue;

                    getMidWire(prePoleData);
                    //        qDebug()<<"pole info"<<prePoleData.at(0).wire1<<prePoleData.at(0).nWalkDistance<<prePoleData.at(maxWireWidthIndex).wire1<<prePoleData.at(maxWireWidthIndex).nWalkDistance<<curMeaData.poleID;
                    if(MeasureDB::oneTableData)
                    {
                        qDebug()<<"update detail tb,error side"<<m_pmDb->updateDetailTb(curMeaData,0,QString::fromWCharArray(L"定位点"));
                    }
                    else
                    {
                        getExData(curMeaData);
                        curMeaExData.locationRemark=QString::fromWCharArray(L"定位点");
                        m_pmDb->insertRecord_MeasureEx(curMeaExData);
                    }
                    //tmp.exValue=0;
                    prePoleData.clear();
                }
#endif
            }
        }
    }
    else
    {
        //标定拉出值及导高
        if(verifyDatas.strVerifyType.contains("wire"))
        {
            if(tmp.wire1.x()&&tmp.wire1.y())
            {
                curMeaData.wire1=tmp.wire1;
                //curMeaData.nWireWidth1=tmp.wire2.x();
                //curMeaData.nWireHeight1=tmp.wire2.y();

                //            qDebug()<<"biaoding wire"<<tmp.wire1.x()<<tmp.wire1.y()<<tmp.side.first<<tmp.side.second;//<<wireInfo2.x()<<wireInfo2.y();
                //设备不动时，进行拉出值及导高标定
                if (verifyWireType)
                {
                    parse_biaoding(tmp,verifyDatas.strVerifyType);
                }
                //利用“之”字值进行标定
                else
                {
                    verify_newType(tmp);//通过之字值获取拉出值及导高的极限值并进行标定
                }
            }
        }
        //标定侧面界限
        else if(verifyDatas.strVerifyType.contains("side"))
        {
            if(tmp.side)
            {
                parse_biaoding(tmp,verifyDatas.strVerifyType);
            }
            else
            {
                return ;
            }
        }
        else if(verifyDatas.strVerifyType.contains("rail"))
        {
            parse_biaoding(tmp,verifyDatas.strVerifyType);
        }
    }
}
//函数功能：动态推行过程中，获取定位点的值，并作为标准值，进行拉出值和导高的校验。
void MainWindow::verify_newType(singleWires &tmp )
{
    curMeaData.wire2Valid=tmp.wires2Judge;
    curMeaData.tm=QDateTime::currentDateTime().toString("hh-mm-ss");

    curMeaData.wire1=tmp.wire1;
    curMeaData.wire2=QPoint(0,0);
    curMeaData.dKm=curMeaData.nWalkDistance/1000.000;
    curMeaData.carrier=0;
    if(stackVerify.count())
    {
        if(stackVerify.count()<stackVerify_size)
        {
            stackVerify<<curMeaData;
        }
        else
        {
            stackVerify.push_back(curMeaData);
            stackVerify.pop_front();
            int beforeWidthAverage=0;
            int afterWidthAverage=0;
            int tmpSumWidth=0;
            for(int i=0;i<3;i++)
            {
                tmpSumWidth+=(stackVerify.at(i).wire1.x());
            }
            beforeWidthAverage=tmpSumWidth/3;
            tmpSumWidth=0;
            for(int i=stackVerify_size-3;i<stackVerify_size;i++)
            {
                tmpSumWidth+=(stackVerify.at(i).wire1.x());
                //qDebug()<<i<<stackVerify.at(i).wire1.x();
            }
            afterWidthAverage=tmpSumWidth/3;
            //                    qDebug()<<beforeWidthAverage<<afterWidthAverage;
            if(beforeWidthAverage>afterWidthAverage)
            {
                upDateVerifyJudageStack(1);
            }
            else if (beforeWidthAverage<afterWidthAverage)
            {
                upDateVerifyJudageStack(-1);
            }
            else
            {
                upDateVerifyJudageStack(0);
            }
            if(verifyEndJudge.second.count()==stackVerify_size)
            {
                int tmpSumVerify=0;
                for(int i=0;i<verifyEndJudge.second.count();i++)
                {
                    tmpSumVerify+=verifyEndJudge.second.at(i);
                }
                //                        qDebug()<<"verify sum"<<tmpSumVerify<<verifyEndJudge.second;
                if(verifyEndJudge.first)
                {
                    //                            qDebug()<<"love you"<<preSumVerify<<tmpSumVerify;
                    if((tmpSumVerify)==preSumVerify)
                    {
                        parseVerifyData();
                    }
                }
                else
                {
                    if(abs(tmpSumVerify)==stackVerify_size)
                    {
                        //                                qDebug()<<"get first verify flag1";
                        verifyEndJudge.first=true;
                        preSumVerify=-tmpSumVerify;
                    }
                }
                verifyEndJudge.second.clear();
            }
        }
    }
    else
    {
        stackVerify<<curMeaData;
    }
    m_pmDb->insertRecord_detailAndVerify(curMeaData,0,false);
    otherPlot->updateMeasureData(curMeaData,0);
    recordMeasure->updateShowTable(false);
}
//函数功能：利用拉出值的"之"字值进行杆号的确定。
void MainWindow::newPoleArithmetic(void)
{
    midWireJudge.second<<curMeaData;
    int countTmp=midWireJudge.second.count();
    //        qDebug()<<"wire value"<<midWireJudge.first<<curMeaData.wire1<<bFirstTrend<<countTmp;
    prePoleData<<curMeaData;
    if(countTmp>10)
    {
        midWireJudge.second.pop_front();
        if(bFirstTrend.first)
        {
            if(parseTrend(midWireJudge.second,bFirstTrend.second))
            {
                //if(abs(bFirstTrend.second.first-bFirstTrend.second.second)>5)
                {
                    qDebug()<<"i get new pole trend "<<bFirstTrend.second<<curMeaData.nWalkDistance;
                    trendCounts.clear();
                    bFirstTrend.first=false;
                }
            }
        }
        else
        {
            QPair<int ,int > curTrend;
            if(parseTrend(midWireJudge.second,curTrend))
            {
                if(bFirstTrend.second.first>bFirstTrend.second.second)
                {
                    if(curTrend.first<curTrend.second)
                    {
                        qDebug()<<"i get the limit position+"<<bFirstTrend.second<<curMeaData.nWalkDistance;
                        //                        otherPlot->setPole(curMeaData.poleID,curMeaData.nWalkDistance);
                        parseOnePoleData(true);//确定杆号
                        bFirstTrend.first=true;
                    }
                }
                else if(bFirstTrend.second.first<bFirstTrend.second.second)
                {
                    if(curTrend.first>curTrend.second)
                    {
                        qDebug()<<"i get the limit position-"<<bFirstTrend.second<<curMeaData.nWalkDistance;
                        //                        otherPlot->setPole(curMeaData.poleID,curMeaData.nWalkDistance);
                        parseOnePoleData(false);//确定杆号
                        bFirstTrend.first=true;
                    }
                }
            }
        }
        //        preWireWidth.first=true;
        //        preWireWidth.second=curMeaData.wire1.x();
    }
}
bool MainWindow::trendChangedJudge( QVector<int> &trendChanged,int tmp)
{
    int countTmp=trendChanged.count();
    //    qDebug()<<"trend vector "<<trendChanged;
    if(countTmp<TREND_SIZE)
    {
        trendChanged.append(tmp);
    }
    else
    {
        trendChanged.append(tmp);
        trendChanged.pop_front();
        int sumTmp=0;
        for(int i=0;i<countTmp;i++)
        {
            sumTmp+=trendChanged.at(i);
        }
        if((countTmp==sumTmp)||(!sumTmp))
        {
            return true;
        }
    }
    return false;
}
//函数功能：
bool MainWindow::parseTrend(QVector<MeasureDebugData> &datas,QPair<int,int> &rightTrend)
{
    QPair<int ,int> tmpTrend=getDataTrend(datas);
    //qDebug()<<"                             "<<tmpTrend<<curMeaData.nWalkDistance<<trendCounts;
    if(tmpTrend.first>tmpTrend.second)
    {
        if( trendChangedJudge(trendCounts,1))
        {
            //            qDebug()<<"start trend first +"<<tmpTrend<<curMeaData.nWalkDistance;
            rightTrend=tmpTrend;
            return true;
        }
    }
    else if(tmpTrend.first<tmpTrend.second)
    {
        if( trendChangedJudge(trendCounts,0))
        {
            //            qDebug()<<"start trend first -"<<tmpTrend<<curMeaData.nWalkDistance;
            rightTrend=tmpTrend;
            return true;
        }
    }
    return false;
}
//函数功能：用于分析存放数据
QPair<int ,int > MainWindow::getDataTrend(QVector<MeasureDebugData> &dataVector)
{
    QPair<int ,int > curTrend;
    //    qDebug()<<"data counts"<<dataVector.count();
    int countTmp=dataVector.count();
    for(int i=0;i<countTmp;i++)
    {
        if(i<5)
        {
            //            qDebug()<<"i+"<<i<<dataVector.at(i).wire1.x();
            curTrend.first+=dataVector.at(i).wire1.x();
        }
        else /*if(i>(countTmp-4))*/
        {
            curTrend.second+=dataVector.at(i).wire1.x();
            //            qDebug()<<"i-"<<i<<dataVector.at(i).wire1.x();
        }

    }
    curTrend.first=curTrend.first/5.0;
    curTrend.second=curTrend.second/5.0;
    dataVector.clear();
    //    qDebug()<<"data counts"<<curTrend;

    return curTrend;
}
//函数功能：
void MainWindow::onePolePar(MeasureDebugData limiteValue,int limitIndex)
{
    getMidWire(curPoleData);
    //        qDebug()<<"pole info"<<prePoleData.at(0).wire1<<prePoleData.at(0).nWalkDistance<<prePoleData.at(maxWireWidthIndex).wire1<<prePoleData.at(maxWireWidthIndex).nWalkDistance<<curMeaData.poleID;
    if(MeasureDB::oneTableData)
    {
        //        if(sideLimit.first)
        //        {
        //            sideLimit.first=false;
        //            qDebug()<<"update detail tb,side ok"<<m_pmDb->updateDetailTb(limiteValue,sideLimit.second,QString::fromWCharArray(L"定位点"));
        //        }
        //        else
        {
            qDebug()<<"update detail tb,error side"<<m_pmDb->updateDetailTb(limiteValue,0,QString::fromWCharArray(L"定位点"));
        }
    }
    else
    {
        getExData(limiteValue);
        curMeaExData.locationRemark=QString::fromWCharArray(L"定位点");
        m_pmDb->insertRecord_MeasureEx(curMeaExData);
    }
    //    qDebug()<<QString("pre poles,count=%1,first dist=%2,end dist=%3").arg(prePoleData.count()).arg(prePoleData.at(0).nWalkDistance).arg(prePoleData.at(prePoleData.count()-1).nWalkDistance)<<str;
    //    qDebug()<<QString("cur poles,count=%1,first dist=%2,end dist=%3").arg(curPoleData.count()).arg(curPoleData.at(0).nWalkDistance).arg(curPoleData.at(curPoleData.count()-1).nWalkDistance);
    curPoleData.clear();
    otherPlot->setPole(curMeaExData.poleid,limiteValue.nWalkDistance);
    updatePoleInfo();
    for(int i=limitIndex;i<prePoleData.count();i++)
    {
        curPoleData.append(prePoleData.at(i));
    }
}
//函数功能：更新杆号信息
void MainWindow::updatePoleInfo(void)
{
    if(MeasureDB::bPoleDir)
    {
        curMeaData.poleID=QString::number(MeasureDB::nPoleNum+2);
    }
    else
    {
        curMeaData.poleID=QString::number(MeasureDB::nPoleNum-2);
    }
    otherPlot->updatePoleNumber(curMeaData.poleID);
    //        emit updatePoleDisplay(curMeaData.poleID);
    //    }

}
//函数功能：根据跨中信息，分析一跨内的数据，获得定位点、跨中等数据，并存入数据库及更新曲线信息
void MainWindow::parseOnePoleData(bool flag)
{
    prePoleData<<curMeaData;
#if 1
    int maxWireWidthIndex=0;
    int minWireWidthIndex=0;
    int tmpMaxWidth=prePoleData.at(0).wire1.x();
    int tmMinWidth=prePoleData.at(0).wire1.x();
    //    qDebug()<<"cur pole datas"<<prePoleData.count()<<prePoleData.at(0).nWalkDistance<<prePoleData.at(prePoleData.count()-1).nWalkDistance;
    for(int i=1;i<prePoleData.count();i++)
    {
        if((tmpMaxWidth)<(prePoleData.at(i).wire1.x()))
        {
            maxWireWidthIndex=i;
            tmpMaxWidth=prePoleData.at(i).wire1.x();
        }
        if((tmMinWidth)>(prePoleData.at(i).wire1.x()))
        {
            minWireWidthIndex=i;
            tmMinWidth=prePoleData.at(i).wire1.x();
        }
    }
    QString str;
    if(flag)
    {
        str=QString("min=%1,%2 ").arg(prePoleData.at(minWireWidthIndex).wire1.x()).arg(prePoleData.at(minWireWidthIndex).nWalkDistance);
        for(int i=0;i<minWireWidthIndex+1;i++)
        {
            curPoleData.append(prePoleData.at(i));
        }
        onePolePar(prePoleData.at(minWireWidthIndex),minWireWidthIndex);//确定杆号，更新对应表格中的相关信息，如定位点及跨中的位置
    }
    else
    {
        str=QString("max=%1,%2 ").arg(prePoleData.at(maxWireWidthIndex).wire1.x()).arg(prePoleData.at(maxWireWidthIndex).nWalkDistance);
        for(int i=0;i<maxWireWidthIndex+1;i++)
        {
            curPoleData.append(prePoleData.at(i));
        }
        onePolePar(prePoleData.at(maxWireWidthIndex),maxWireWidthIndex);//确定杆号，更新对应表格中的相关信息，如定位点及跨中的位置
    }

    prePoleData.clear();
#else
    QPair<bool ,int> pole =otherPlot->newRailPole->poleInfo;
    int maxWireWidthIndex=0;
    int tmpWireWidth=prePoleData.at(0).wire1.x();
    qDebug()<<"cur pole datas"<<prePoleData.count()<<prePoleData.at(0).nWalkDistance<<prePoleData.at(prePoleData.count()-1).nWalkDistance;
    for(int i=1;i<prePoleData.count();i++)
    {
        if(abs(tmpWireWidth)<abs(prePoleData.at(i).wire1.x()))
        {
            maxWireWidthIndex=i;
            tmpWireWidth=prePoleData.at(i).wire1.x();
        }
    }
    //
    if(abs(prePoleData.at(0).nWalkDistance-prePoleData.at(maxWireWidthIndex).nWalkDistance)<constMinPoleDist)
    {
        qDebug()<<"error pole value"<<prePoleData.at(0).nWalkDistance<<prePoleData.at(maxWireWidthIndex).nWalkDistance<<prePoleData.at(0).poleID;
        return ;
    }
    if(onePoleDatas.first)
    {
        if(maxWireWidthIndex)
        {
            getBeforeHalfData(maxWireWidthIndex);
        }
        onePoleDatas.first=false;
    }
    else
    {
        for(int i=0;i<maxWireWidthIndex;i++)
        {
            onePoleDatas.second<<prePoleData.at(i);
        }
#if 0
        double curPoleDist=abs(onePoleDatas.second.at(0).nWalkDistance-onePoleDatas.second.at(onePoleDatas.second.count()-1).nWalkDistance);
        QVector<double>  droppers;
        getDropperStep(curPoleDist,droppers,prePoleWalk);
        qDebug()<<"pre distance="<<prePoleWalk<<curMeaExData.nWalkDistance;
        QPair<int,QVector<double> > isDropper;//int表示吊弦绘制Y轴坐标;qvector<double>表示吊弦对应位置的里程
        isDropper.first=curMeaData.wire1.y();
        isDropper.second=droppers;
        //prePoleData.insert();
        qDebug()<<"before dropper count="<<prePoleData.count()<<isDropper.second;

        //  for(int i=0;i<prePoleData.count();i++)
        //  {
        //      qDebug()<<i<<prePoleData.at(i).nWalkDistance;//<<prePoleData.at(i).nWireWidth<<prePoleData.at(i).nWireHeight;
        //      if(i<isDropper.second.count())
        //      {
        //          qDebug()<<"dropper"<<i<<isDropper.second.at(i);
        //      }
        //  }
        parse_dropperGrade(isDropper.second,prePoleWalk,prePoleData);
        emit dropperData(isDropper);
#endif
        getMidWire(onePoleDatas.second);
        onePoleDatas.second.clear();
        getBeforeHalfData(maxWireWidthIndex);
    }

    if(MeasureDB::bPoleDir)
    {
        curMeaData.poleID=QString::number(otherPlot->newRailPole->poleInfo.second+2);
    }
    else
    {
        curMeaData.poleID=QString::number(otherPlot->newRailPole->poleInfo.second-2);
    }
    otherPlot->updatePoleNumber(curMeaData.poleID);
    getExData(prePoleData.at(maxWireWidthIndex));
    qDebug()<<"pole info"<<prePoleData.at(0).wire1<<prePoleData.at(0).nWalkDistance<<prePoleData.at(maxWireWidthIndex).wire1<<prePoleData.at(maxWireWidthIndex).nWalkDistance<<curMeaData.poleID;
    curMeaExData.locationRemark=QString::fromWCharArray(L"定位点");
    m_pmDb->insertRecord_MeasureEx(curMeaExData);
    //memset(&curMeaExData.onePolePar,0,sizeof(OnePolePar));
    //speak1(QString::fromWCharArray(L"杆号%1").arg(curMeaData.poleID));
    //显示杆号信息
    otherPlot->setPole(curMeaExData.poleid,curMeaExData.nWalkDistance);
    otherPlot->oldRailPole->fillPoleSelList(m_currIdx);

    qDebug()<<"add pole,cur pole info,distance yy"<<prePoleData.count()<<curMeaExData.nWalkDistance<<maxWireWidthIndex;
    prePoleData.clear();
#endif
}
//函数功能：根据当前跨内的所有数据，获取其跨中的值
void MainWindow::getMidWire(QVector< MeasureDebugData > &curPoleDatas)
{
    double curPoleDist=abs(curPoleDatas.at(0).nWalkDistance-curPoleDatas.at(curPoleDatas.count()-1).nWalkDistance);
    double midPoleDist=curPoleDist/2.0+curPoleDatas.at(0).nWalkDistance;
    int midIndex=0;
    for(int i=0;i<curPoleDatas.count();i++)
    {
        if((midPoleDist>curPoleDatas.at(i).nWalkDistance)&&((midPoleDist<curPoleDatas.at(i+1).nWalkDistance)))
        {
            //            qDebug()<<"mid distance "<<curPoleDatas.at(i).nWalkDistance<<midPoleDist<<curPoleDatas.at(i+1).nWalkDistance<<"pole id"<<curPoleDatas.at(i).poleID;
            if(abs(midPoleDist-curPoleDatas.at(i).nWalkDistance)>abs(midPoleDist-curPoleDatas.at(i).nWalkDistance))
            {
                midIndex=i+1;
            }
            else
            {
                midIndex=i;;
            }
            break;
        }
        //qDebug()<<"one pole all dist"<<i<<curPoleDatas.at(i).nWalkDistance;
    }
    //qDebug()<<"one pole all ddd"<<midPoleDist<<curPoleDatas.count()<<midIndex;
    if(midIndex)
    {
        //                    qDebug()<<"insert mid wire wirdth";
        if(!midPoleNum.isEmpty())
            curMeaExData.poleid=midPoleNum;
        if(MeasureDB::oneTableData)
        {
            m_pmDb->updateDetailTb(curPoleDatas.at(midIndex),0,QString::fromWCharArray(L"跨中"));
        }
        else
        {
            getExData(curPoleDatas.at(midIndex));
            curMeaExData.locationRemark=QString::fromWCharArray(L"跨中");
            m_pmDb->insertRecord_MeasureEx(curMeaExData);
        }
    }
}
//函数功能：根据校验数据表格的数据，分析获取定位点的实测值
void MainWindow::parseVerifyData(void)
{
    QSqlQuery query(m_pmDb->currDatabase());
    QString strSql;
    //获取最后一次里程数据
    double lastDist=0;
    strSql=QString("select id,dist from %1 order by id desc limit 1").arg(MeasureDB::verifyTb);
    if(query.exec(strSql))
    {
        while(query.next())
        {
            lastDist=query.value(1).toDouble();
            qDebug()<<"verify...,last dist info.  id="<<query.value(0).toInt()<<"lastDistance= "<<query.value(1).toDouble();
        }
    }
    if(lastDist)
    {
        double curDist=0;
        int wireWidth=0;
        int wireHeight=0;
        int railWidth=0;
        int railHeight=0;
        //                      0                  1   2        3           4   5         6
        strSql=QString("select max(abs(wireWidth)),id,wireWidth,wireHeight,dist,railWidth,railHeight from %1").arg(MeasureDB::verifyTb);
        //                                    qDebug()<<"verify sql"<<strSql;
        if(query.exec(strSql))
        {
            while(query.next())
            {
                wireWidth=query.value(2).toInt();
                wireHeight=query.value(3).toInt();
                curDist=query.value(4).toDouble();
                railWidth=query.value(5).toInt();
                railHeight=query.value(6).toInt();
                qDebug()<<"verify...,wire width limit value info,w="<<wireWidth<<"h= "<<wireHeight<<"d="<<curDist<<"id,"<<query.value(1).toInt();
            }
        }
        if(curDist)
        {
            if(abs(curDist-lastDist)>0.5)
            {
                updateVerifyValue(wireWidth,wireHeight,railWidth,railHeight,0);
            }
        }
    }
}
void MainWindow::upDateVerifyJudageStack(int n )
{
    if(verifyEndJudge.second.count()<stackVerify_size)
    {
        verifyEndJudge.second<<n;
    }
    else
    {
        verifyEndJudge.second.push_back(n);
        verifyEndJudge.second.pop_front();
    }
}
void MainWindow::getBeforeHalfData(int index)
{
    for(int i=index;i<prePoleData.count();i++)
    {
        onePoleDatas.second<<prePoleData.at(i);
    }
    midPoleNum=prePoleData.at(0).poleID;
}
void MainWindow::getExData(MeasureDebugData dval)
{
    //时间
    curMeaExData.tm=QDateTime::currentDateTime().toString("hh-mm-ss");
    //里程
    curMeaExData.nWalkDistance=dval.nWalkDistance;
    //公里标
    curMeaExData.dKm=dval.dKm;
    //站区
    curMeaExData.station =dval.station;
    //杆号
    curMeaExData.poleid= dval.poleID;
    //拉出值
    curMeaExData.onePolePar.dWireWidth=dval.wire1.x();
    //导高
    curMeaExData.onePolePar.dWireHeight=dval.wire1.y();
}
void MainWindow::oldPoleModify(void)
{
    //检测了新的杆号
    if((bDetectPoleOldRail|bDetectPoleNewRail))
    {
        //                    qDebug()<<"rail flag="<<bDetectPoleOldRail<<bDetectPoleNewRail;
        qDebug()<<"get a new pole,pole name="<<curMeaExData.poleid;
        //curMeaExData.maxLachuzhi=m_nmaxLachuzhi;

        //curMeaExData.onePolePar.gaocha=abs(m_fDaogaoMax-m_fDaogaoMin);
        float curPoleDist=updatePoleDist();
        if(prePoleWalk>-1000)
        {
            QVector<double>  droppers;
            getDropperStep(curPoleDist,droppers,prePoleWalk);
            qDebug()<<"pre distance="<<prePoleWalk<<curMeaExData.nWalkDistance;
            QPair<int,QVector<double> > isDropper;//int表示吊弦绘制Y轴坐标;qvector<double>表示吊弦对应位置的里程
            isDropper.first=curMeaData.wire1.y();
            isDropper.second=droppers;
            //prePoleData.insert();
            qDebug()<<"before dropper count="<<prePoleData.count()<<isDropper.second;

            //  for(int i=0;i<prePoleData.count();i++)
            //  {
            //      qDebug()<<i<<prePoleData.at(i).nWalkDistance;//<<prePoleData.at(i).nWireWidth<<prePoleData.at(i).nWireHeight;
            //      if(i<isDropper.second.count())
            //      {
            //          qDebug()<<"dropper"<<i<<isDropper.second.at(i);
            //      }
            //  }
            parse_dropperGrade(isDropper.second,prePoleWalk,prePoleData);
            emit dropperData(isDropper);
        }
        //该语句需位于parse_dropperGrade之后,使得在数据库中,先添加吊弦信息,在添加支柱信息
        curMeaExData.locationRemark=QString::fromWCharArray(L"定位点");
        curMeaExData.onePolePar.dWireHeight=m_fDaogaoMax;
        curMeaExData.onePolePar.dWireWidth=m_nmaxLachuzhi;
        qDebug()<<"after data"<<curMeaExData.onePolePar.dWireWidth<<curMeaExData.onePolePar.dWireHeight;
        m_pmDb->insertRecord_MeasureEx(curMeaExData);
        //                memset(&curMeaExData.onePolePar,0,sizeof(OnePolePar));
        //        speak1(QString::fromWCharArray(L"杆号%1").arg(curMeaData.poleID));
        otherPlot->setPole(curMeaData.poleID,curMeaData.nWalkDistance);
        otherPlot->oldRailPole->fillPoleSelList(m_currIdx);

        qDebug()<<"distance yy"<<prePoleWalk<<curMeaExData.nWalkDistance;
        prePoleData.clear();
        prePoleWalk=curMeaExData.nWalkDistance;
        fristLimitFlag=true;
    }
    else
    {
        prePoleData<<curMeaData;
    }
}
//函数功能：更新跨距信息,应用于里程定位杆号的时候
float  MainWindow::updatePoleDist(void )
{
    float curPoleDist=0;
    if(bDetectPoleOldRail)
    {
        QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
        if(m_currIdx<polesets.count())
        {
            qDebug()<<"+++++++++++++++++++++++++++++++++++++++++cur step value="<<polesets[m_currIdx].dist<<polesets[m_currIdx].poleID;
        }
        else
        {
            qDebug()<<"mm"<<m_currIdx<<polesets.count();
        }
        bDetectPoleOldRail=false;
        curPoleDist=polesets[m_currIdx].dist;
    }
    if(bDetectPoleNewRail)
    {
        bDetectPoleNewRail=false;
        curPoleDist=abs(prePoleWalk-curMeaExData.nWalkDistance);
    }
    return curPoleDist;
}
//函数功能：标定数据分析计算函数，true，标定拉出值及导高;false,标定侧面界限
void MainWindow::parse_biaoding(singleWires &tmp,QString verifyType)
{
    wireStandardValues tmp1;
    tmp1.nVerifyRailHeight=curMeaData.nRailHeight;
    tmp1.nVerifyRailWidth=curMeaData.nRailWidth;
    if(verifyType.contains("wire"))
    {
        tmp1.nVerifyWireHeight=tmp.wire1.y();
        tmp1.nVerifyWireWidth=tmp.wire1.x();
    }
    else if(verifyType.contains("side"))
    {
        tmp1.nVerifySide=tmp.side;
    }
    else if(verifyType.contains("rail"))
    {
        tmp1.nVerifyRailHeight=curMeaData.nRailHeight;
        tmp1.nVerifyRailWidth=curMeaData.nRailWidth;
    }
    //    qDebug()<<"biaoding";
    if(10==nBiaodingCount++)
    {

        readTimerLMS->stop();
        //                    qDebug()<<"i<<biaodingArrays.at(i).nInitialRailHeight";
        int constWireWidth=0;
        int constWireHeight=0;
        int constRailWidth=0;
        int constRailHeight=0;
        int constRailSide=0;

        for(int i=0;i<biaodingArrays.count();i++)
        {
            if(verifyType.contains("wire"))
            {
                constWireHeight +=biaodingArrays.at(i).nVerifyWireHeight;
                constWireWidth+= biaodingArrays.at(i).nVerifyWireWidth;
                qDebug()<<"verify array,wire "<<i<<biaodingArrays.at(i).nVerifyWireHeight<<biaodingArrays.at(i).nVerifyWireWidth;
            }
            else if(verifyType.contains("side"))
            {
                constRailSide+=biaodingArrays.at(i).nVerifySide;
                qDebug()<<"verify array,side "<<i<<biaodingArrays.at(i).nVerifySide;//<<biaodingArrays.at(i).nVerifySide;//.nStandardRailHeight<<biaodingArrays.at(i).nStandardRailWidth;
            }
            else if(verifyType.contains("rail"))
            {
                constRailWidth+=biaodingArrays.at(i).nVerifyRailWidth;
                constRailHeight+=biaodingArrays.at(i).nVerifyRailHeight;
            }
            //            qDebug()<<"i"<<i<<biaodingArrays.at(i).nVerifyWireWidth;
        }
        if(verifyType.contains("wire"))
        {
            constWireWidth/=biaodingArrays.count();
            constWireHeight/=biaodingArrays.count();
            updateVerifyValue( constWireWidth,constWireHeight,0,0,0);
        }
        else if(verifyType.contains("side"))
        {
            constRailSide/=biaodingArrays.count();
            qDebug()<<"average side"<<constRailSide;
            updateVerifyValue( 0,0,0,0,constRailSide);
        }
        else if(verifyType.contains("rail"))
        {
            constRailWidth/=biaodingArrays.count();
            constRailHeight/=biaodingArrays.count();
            updateVerifyValue( 0,0,constRailWidth,constRailHeight,0);
        }
        MeasureDB::bVerify=true;
        biaodingArrays.clear();
        m_lblCurMeasure->setText(QString::fromWCharArray(L"标定成功"));
        if(!MeasureDB::bMeasure)
        {
            qDebug()<<"stop timer"<<readTimerLMS->isActive();
            if(readTimerLMS->isActive())
            {
                readTimerLMS->stop();
            }
            m_pRadorScanner->stopLMSDevice();
        }
        if(MeasureDB::bSimDraw)
        {
            emit simStop(true);
        }
        m_pRadorScanner->clearStackData();
        speak1(QString::fromWCharArray(L"标定成功"));
        stopMeasure();
        emit baseParChanged();
        QMessageBox::about(0,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"标定成功"));
    }
    else
    {
        if(verifyType.contains("wire"))
        {
            if(tmp1.nVerifyWireHeight)
                biaodingArrays.append(tmp1);
            qDebug()<<"verify data,wire="<<biaodingArrays.count()<<nBiaodingCount<<tmp1.nVerifyWireHeight<<tmp1.nVerifyWireWidth;
        }
        else if(verifyType.contains("rail"))
        {
            biaodingArrays.append(tmp1);
        }
        else if(verifyType.contains("side"))
        {
            if(tmp1.nVerifySide)
            {
                biaodingArrays.append(tmp1);
                emit simStart(false);
            }
            qDebug()<<"verify data,side="<<biaodingArrays.count()<<nBiaodingCount<<tmp1.nVerifySide;
        }
    }
}
//函数功能: 用于设备固定不动时对参数进行标定，
//      wireWidth,wireHeight,railWidth,railHeight,side为均值计算过后的对应参数。wireheight=0，不标定导高；side=0，不标定侧面界限
void MainWindow::updateVerifyValue(int wireWidth,int wireHeight,int railWidth,int railHeight,int side )
{
    sensorInitialValues tmp2;
    QSettings  settings("Config.ini", QSettings::IniFormat);
    int m_nLachuzhiC = settings.value("initialPar/m_nLachuzhiC", 0).toInt();
    int m_nDaogaoC = settings.value("initialPar/m_nDaogaoC", 0).toInt();
    int m_nSideC = settings.value("initialPar/sideC", 0).toInt();

    int m_nRailWidthC = settings.value("initialPar/RailWidthC", 0).toInt();
    int m_nRailHeightC = settings.value("initialPar/RailHeightC", 0).toInt();

    //tmp2.nInitialRailHeight=verifyDatas.nStandardRailHeight-railHeight;
    //tmp2.nInitialRailWidth=verifyDatas.nStandardRailWidth-railWidth;
    //qDebug()<<"std"<<verifyDatas.nVerifyWireHeight<<verifyDatas.nVerifyWireWidth;
    //拉出值导高标定
    if(wireHeight)
    {
        tmp2.nInitialWireHeight=verifyDatas.nVerifyWireHeight-wireHeight+m_nDaogaoC;
        tmp2.nInitialWireWidth=verifyDatas.nVerifyWireWidth-wireWidth+m_nLachuzhiC;
        qDebug()<<QString("------------------------------,before const width=%1,before const height=%2;now const width=%3,now const heigth=%4").arg(m_nLachuzhiC).arg(m_nDaogaoC).arg(tmp2.nInitialWireWidth).arg(tmp2.nInitialWireHeight);
    }
    else
    {
        tmp2.nInitialWireHeight=0;
        tmp2.nInitialWireWidth=0;
    }
    //侧面界限标定
    if(side)
    {
        tmp2.nSide=verifyDatas.nVerifySide-side/*+m_nSideC*/;
        qDebug()<<"standard side"<<verifyDatas.nVerifySide<<side;
        qDebug()<<QString("------------------------------,before const side=%1,now const side=%2").arg(m_nSideC).arg(tmp2.nSide);
    }
    else
    {
        tmp2.nSide=0;
    }
    //轨距及超高标定
    if(railWidth)
    {
        tmp2.nInitialRailHeight=verifyDatas.nVerifyRailHeight-railHeight;
        tmp2.nInitialRailWidth=verifyDatas.nVerifyRailWidth-railWidth;
        qDebug()<<QString("------------------------------rail verify,before rail w=%1,h=%2,now w=%3,h=%4").arg(m_nRailWidthC).arg(m_nRailHeightC).arg(tmp2.nInitialRailWidth).arg(tmp2.nInitialRailHeight)<<railHeight<<railWidth;
    }
    else
    {
        tmp2.nInitialRailHeight=0;
        tmp2.nInitialRailWidth=0;
    }
    m_measureIntial->saveInitialPar(tmp2);
}
//函数功能：作为标定开始的槽函数。开始标定，进行拉出值、导高和侧面限界的标定，并提供标准值（包含在变量tmp中）。
void MainWindow::setVerifyInitial(wireStandardValues tmp)
{
    qDebug()<<"dd"<<m_pRadorScanner->getRadorStatus()<<"bMeasure="<<MeasureDB::bMeasure;
    //两种情况下，可以进行数据标定：1，雷达模拟数据状态；2，实际测量状态下，雷达链接成功的情况下。
    if(MeasureDB::bSimDraw|((!MeasureDB::bSimDraw)&&m_pRadorScanner->getRadorStatus()))
    {
        //        qDebug()<<"bmeasure="<<MeasureDB::bMeasure;
        if(MeasureDB::bMeasure)
        {
            QMessageBox::about(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("系統巡視中,无法进行标定..."));
        }
        else
        {
            nBiaodingCount=0;
            verifyDatas=tmp;
            biaodingArrays.clear();
            //if(tmp.strVerifyType)
            m_lblCurMeasure->setText(QTextCodec::codecForName("gbk")->toUnicode("标定中..."));
            //else
            //m_lblCurMeasure->setText(QTextCodec::codecForName("gbk")->toUnicode("准备标定..."));
            m_pRadorScanner->clearStackData();
            if(!readTimerLMS->isActive())
                readTimerLMS->start(MEASURE_TIME_INTERVAL);

            if(tmp.strVerifyType.contains("side"))
            {
                verifyWatchdog(false);//进行标定超时计时
            }
            else
            {
                verifyWatchdog(true);//进行标定超时计时
            }
            if(MeasureDB::bSimDraw)
            {
                if(tmp.strVerifyType.contains("side"))
                {
                    emit simStart(true);//对于侧面界限的标定，必须是获得支柱的情况下才能进行
                }
                else
                {
                    emit simStart(false);
                }
            }
            else
            {
                m_pRadorScanner->startLMSDevice();
            }
        }
    }
    else
        ui->statusBar->showMessage(QTextCodec::codecForName("gbk")->toUnicode("设备未连接,无法进行标定..."));
}
//函数功能：标定开始后，设定一定时时间，如在该段时间内未标定成功，默认为标定失败
void MainWindow::verifyWatchdog(bool verifyType)
{
    int timeTmp=0;
    if(verifyType)
    {
        qDebug()<<"---------------------------wire par verify";
        timeTmp=30000;
    }
    else
    {
        qDebug()<<"---------------------------side verify";
        timeTmp=60000;
    }
    QTimer::singleShot(timeTmp,this,SLOT(verifyFailure()));
}
//函数功能：标定超时处理
void MainWindow::verifyFailure(void)
{
    if(!MeasureDB::bVerify)
    {
        MeasureDB::bVerify=true;
        m_lblCurMeasure->setText(QTextCodec::codecForName("gbk")->toUnicode("标定失败..."));
        qDebug()<<"verify failure";
    }
}
void MainWindow::updateUIRailInfo(void)
{
    //    QString temp=QString(tr("线路:%1,方向:%2,站区:%3,杆号：%4")).arg(curMeaExData.railwayName).arg(curMeaExData.direction).arg(curMeaExData.station).arg(curMeaExData.poleid);
    //    QString temp=QString(tr("%1 %2 %3 杆号：%4")).arg(curMeaExData.railwayName).arg(curMeaExData.direction).arg(curMeaExData.station).arg(curMeaExData.poleid);
    QString temp=QString(QTextCodec::codecForName("gbk")->toUnicode("%1,%2,%3,%4")).arg(QTextCodec::codecForName("gbk")->toUnicode(curMeaExData.railwayName.toLatin1()))
            .arg(curMeaExData.direction)
            .arg(curMeaExData.station )
            .arg(curMeaExData.poleid);

    m_lbTourInfo->setText(temp);
}
//函数功能：更新轨距、里程和超高信息
void MainWindow::slot_updateComData(comDataType comData)
{
    //        qDebug()<<"com distance "<<comData.walkDistance;
    //    if(bClearDistance)
    {
        //comData.walkDistance=comData.walkDistance;
        //qDebug()<<"com distance "<<comData.walkDistance;

        if(!MeasureDB::bSimDraw)
        {
            curMeaData.nWalkDistance=(QString::number(comData.walkDistance,'f',2).toFloat());
            curMeaData.baseAngle=comData.angle;
            if(comData.nRailWidth<2000)
            {
                curMeaData.nRailWidth=comData.nRailWidth;
                curMeaData.nRailHeight=comData.nRailHeight;
            }
            else
            {
                curMeaData.nRailWidth=0;
                curMeaData.nRailHeight=0;
            }
            //            qDebug()<<"angle lvoe"<<curMeaData.baseAngle;
        }
    }
}
void MainWindow::slot_updateShowMes(QString b)
{
    ui->statusBar->showMessage(b);
}

void MainWindow::setProfile(RunProfile& pr, bool beReset)
{
#ifdef NO_SIMULATION
    beReset=false;
#else
    beReset=true;
#endif
    profile = pr;
    //  get the dynamic test's initial message
    Global::getInstance()->setPostionAsProfile(profile);
    m_currIdx = profile.m_currIdx;

    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();

    //1.定位杆号所处的index
    //2011-10-12-20.17修改
    //    int i;
    //    for (i = 0; i < polesets.size(); i++)
    //    {
    //        if (polesets[i].poleID == profile.poleStr)
    //            break;
    //    }
    //    m_currIdx = i;
    /*******************************************************************/
    int i=0;int j=0;
    for(i=0;i < polesets.size(); i++)
    {
        if (polesets[i].station == profile.station)
        {
            for(j=i;j<polesets.size(); j++)
            {
                if (polesets[j].poleID == profile.poleStr)
                {
                    break;
                }
            }
            break;
        }
    }

    m_currIdx = j;

    /***********************************************************************/
    //2.确定距离
    m_tinyspacer = profile.position;
    while (m_currIdx < polesets.size()-1 && m_tinyspacer > polesets[m_currIdx].dist)
    {
        m_tinyspacer -= polesets[m_currIdx].dist;
        m_currIdx++;
    }

    while (m_tinyspacer < 0 && m_currIdx > 0)
    {
        m_tinyspacer += polesets[m_currIdx-1].dist;
        m_currIdx--;
    }

    //labelPosition->setText(QString::number(m_tinyspacer)+QString::fromWCharArray(L" 米"));
    m_disapperSpacer = 0;

    m_startDist = polesets[m_currIdx].sum_dist;
    m_prevIdx = m_currIdx;
    //    m_switchToAI = false;

    CalcPole spole;
    spole.nID = 0;
    spole.poleID = polesets[m_currIdx].poleID;
    //    2012-06-04:zbw
    //    spole.sum_dist = 0;
    spole.sum_dist = polesets[m_currIdx].sum_dist;
    //    qDebug()<<"spole sum_dist"<<spole.sum_dist<<spole.poleID<<spole.nID;
    Global::getInstance()->calcPolesets.push_back(spole);

    qDebug()<<"the m_currIdx is " << m_currIdx;
    qDebug()<<"the poleID is " << polesets[m_currIdx].poleID;
    qDebug()<<"the station is "<< polesets[m_currIdx].station;
    qDebug()<<"the distance is " << polesets[m_currIdx+1].dist;
    qDebug()<<"sum distance is : " <<m_startDist;
    qDebug()<<"the tiny dis: " <<m_tinyspacer;

    stationChanged();
    qDebug()<<"m_currIdx ="<<m_currIdx << "m_startDist="<<m_startDist;;
}
void MainWindow::setMeasureDB(RunProfile &profile)
{
    qDebug()<<"measures database name:"<<profile.dbpath;
    if(m_pmDb)
    {
        //m_pmDb->closeDb();
        m_pmDb ->createDb(profile.dbpath);

        qDebug()<<"create database";
        //        delete m_pmDb;
    }
    else
    {
        m_pmDb = new MeasureDB;
        m_pmDb ->createDb(profile.dbpath);
        qDebug()<<"frist create database";
    }
    curMeaExData.railwayName=profile.lineName;

    if(profile.direction)
        curMeaExData.direction=QString::fromWCharArray(L"下行");
    else
        curMeaExData.direction=QString::fromWCharArray(L"上行");
    bool createDb=m_pmDb->createTables();
    if(createDb)
    {
        qDebug()<<"data base is created,tables="<<m_pmDb->tables();
        qDebug()<<"";
        if(recordMeasure)
        {
            recordMeasure->setCurDatabase(&(m_pmDb->currDatabase()),profile.dbpath);
        }
        else
            qDebug()<<"recordMeasure is null";
    }
}
void MainWindow::initParams(void)
{
    bErrorPole=false;
    m_pmDb=NULL;

    strDeviceStatusInfo.clear();
    MeasureDB::bMeasure=false;
    strLMSIPAddress="";
    bDetectPoleOldRail=false;
    bDetectPoleNewRail=false;
    bFirstTrend.first=true;
    fristLimitFlag=true;
    m_nmaxLachuzhi=0;
    m_fDaogaoMax=0;
    fristmeasure=false;
    prePoleWalk=-1000;
    prePoleData.clear();

    bOldPole=false;
    memset(&curMeaExData.onePolePar,0,sizeof(OnePolePar));
    preWireWidth.first=false;
    midWireJudge.first=false;
    onePoleDatas.first=true;
    midPoleNum="";
    nBiaodingCount=0;
    preWalk.second=-2000;
    verifyEndJudge.first=false;
    prePoleDist=0;
    simConut=0;
    reconnectCount=0;
    //    m_fDistOffset = 0.0;
    //    m_fdistPrev = 0;
    //    m_ids = 0;
    //    m_velocity = 0;
    //    m_prevTm = 0;
    //    m_prevSpace = 0;
    //    m_fDaogaoMax = 0;
    //    m_fDaogaoMin = 0;
    //    m_fOffsetSpace = 0;
    //    m_abortFirst = true;
}
void MainWindow::stationChanged()
{
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_pmDb)
    {
        m_pmDb->Commit();
        m_pmDb->m_db.close();
    }
    event->accept();
}
//函数功能：根据网络连接状态，及时更新传感器状态标志位
void MainWindow::slot_IPSensorStatus(bool statusBuffer)
{
    tcpAngle *tcpSensor=(tcpAngle*)this->sender();
    getDevicesStatus();
}
void MainWindow::getDevicesStatus(void)
{
    if(m_bMeasureType)
    {
        if(m_pRadorScanner->getRadorStatus())
            //    if(setform->curCloudTcpStatus&&setform->curAngleTcpStatus&&m_pRadorScanner->getRadorStatus())
        {
            MeasureDB::bRadarStatus=true;
            MeasureDB::bSimDraw=false;
            //            QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"所有设备已连接!!!"));
        }
        else
        {
            MeasureDB::bRadarStatus=false;
            QMessageBox::about(this,QString::fromWCharArray(L"提示"),QString::fromWCharArray(L"设备连接失败,请确认设备连接状态!!!"));
        }
    }
}
void MainWindow::detectSpacer(void)
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    //qDebug()<<"the tm.elapsed() is " << tm.elapsed();
    double offsetspace = curMeaData.nWalkDistance + m_tinyspacer - m_disapperSpacer;
    double currPoleDistance=Global::getInstance()->getPoleBase(m_currIdx).dist-1;
    double lastPoleDistance;
    static bool cp=false;
    if(m_currIdx)
        lastPoleDistance=Global::getInstance()->getPoleBase(m_currIdx-1).dist-1;
    else
        lastPoleDistance=0;

    if ((m_currIdx < polesets.size()-1) && offsetspace >= currPoleDistance)
    {
        //        qDebug()<<"distantce = "<<m_currSpace<<" id: "<<Global::getInstance()->getPoleBase(m_currIdx).poleID;
        m_disapperSpacer = curMeaData.nWalkDistance + m_tinyspacer + 1;
        m_prevIdx=m_currIdx;
        cp=true;
        m_currIdx++;
        //        this->labelJindu->setText(QString::number(-1)+QTextCodec::codecForName("gbk")->toUnicode(" 米mm"));
        stationChanged();
        bDetectPoleOldRail=true;
    }
    else if(offsetspace<0&& abs(offsetspace)>= lastPoleDistance&&m_currIdx)
    {
        {
            m_disapperSpacer = curMeaData.nWalkDistance + m_tinyspacer + 1;
            m_prevIdx=m_currIdx;
            cp=true;
            m_currIdx--;
            //        this->labelJindu->setText(QString::number(-1)+QTextCodec::codecForName("gbk")->toUnicode(" 米mm"));
            stationChanged();
            bDetectPoleOldRail=true;
        }
    }
    //进行定点拍照距离计算
    float cc;
    if(offsetspace>0)
        cc=currPoleDistance-offsetspace;
    else
        cc=currPoleDistance+offsetspace;

    //    qDebug()<<"--------------------------offset="<<currPoleDistance<<offsetspace<<cc<<nPicDistance<<cp;
    if((nPicDistance>cc)&&cp)
    {
        cp=false;
    }

    //    qDebug()<<"offsetspce value="<<offsetspace
    //           <<"currPoleDistance="<<currPoleDistance
    //          <<lastPoleDistance;
}
void MainWindow::getLimitValue(MeasureDebugData dItem)
{
    //获取极值
    /* 2012-06-07 */
    //        qDebug()<<"limit data value"<<fristLimitFlag<<dItem.nWireHeight;
#if 0
    if(fristLimitFlag)
    {
        fristLimitFlag=false;
        m_fPrevMaxDaogao=dItem.wire1.y();
        m_fPrevMinDaogao=dItem.wire1.y();
        m_fDaogaoMin=dItem.wire1.y();
        m_fDaogaoMax=dItem.wire1.y();
        m_nPremaxLachuzhi=abs(dItem.wire1.x());
        m_nmaxLachuzhi=abs(dItem.wire1.x());
    }
    else
    {
        //获得导高极值
        m_fPrevMaxDaogao=qMax(dItem.wire1.y(),m_fPrevMaxDaogao);
        m_fDaogaoMax= m_fPrevMaxDaogao;
        m_fPrevMinDaogao=qMin(dItem.wire1.y(),m_fPrevMinDaogao);
        m_fDaogaoMin=m_fPrevMinDaogao;
        //获得拉出值极值
        m_nPremaxLachuzhi=qMax(abs(dItem.wire1.x()),m_nPremaxLachuzhi);
        m_nmaxLachuzhi=m_nPremaxLachuzhi;
    }
#else
    if(fristLimitFlag)
    {
        fristLimitFlag=false;
        m_nmaxLachuzhi=abs(dItem.wire1.x());
        m_fDaogaoMax=dItem.wire1.y();
    }
    else
    {
        if(abs(m_nmaxLachuzhi)<abs(dItem.wire1.x()))
        {
            m_nmaxLachuzhi=dItem.wire1.x();
            m_fDaogaoMax=dItem.wire1.y();
        }
    }
#endif

    //    qDebug()<<"limit value"<<m_nmaxLachuzhi<<m_fDaogaoMax<<curMeaData.wire1.x()<<curMeaData.wire1.y();
}
//函数功能：若PC机中没有数据文件夹，则创建文件存储路径
void MainWindow::createDIR(void)
{
    QDir *temp=new QDir;
    if(!temp->exists(pathPrefix))
        temp->mkdir(pathPrefix);

    if(!temp->exists(QString(pathPrefix).append("//Video")))

        temp->mkdir(QString(pathPrefix).append("//Video"));

    if(!temp->exists(QString(pathPrefix).append("//Picture")))
        temp->mkdir(QString(pathPrefix).append("//Picture"));

    if(!temp->exists(QString(pathPrefix).append("//Data")))
        temp->mkdir(QString(pathPrefix).append("//Data"));
}
void MainWindow::slot_changePoleId(QString toPole)
{
    QVector<PoleBase>& polesets = Global::getInstance()->getPoleSets();
    //    qDebug()<<"
    m_currIdx=toPole.toInt();
    otherPlot->oldRailPole->walkDistance=curMeaData.nWalkDistance;
    qDebug()<<"after ==="<<m_currIdx;
    curMeaData.station=polesets[m_currIdx].station;
    curMeaData.poleID=polesets[m_currIdx].poleID;
    bDetectPoleOldRail=true;
}
void MainWindow::saveUIFrm(void)
{
    QString  fileBasePath=pathPrefix;
    fileBasePath.append("/Picture");
    QString tmpFileName;//=ipVideo->videoControl->captureFilePrefix();

    if(tmpFileName.isEmpty())
    {
        QString fileName=QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss");
        fileBasePath+="/";
        fileBasePath+=fileName;
    }
    else
    {
        QString fileName=QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss");
        fileBasePath+="/";
        fileBasePath+=tmpFileName+" "+fileName;
    }
    fileBasePath+=".jpg";
    fileBasePath.replace("//","\\");
    //利用多线程进行界面的图片保存
#if  1
    saveSubThread.UIPixmap= QPixmap::grabWindow(this->centralWidget()->winId()); //抓取当前屏幕的图片
#else
    saveSubThread.UIPixmap= QPixmap::grabWindow(this->stackWdg->currentWidget()->winId()); //抓取当前屏幕的图片
#endif
    saveSubThread.saveFilePath=fileBasePath;
    qDebug()<<"save ui paths="<<fileBasePath;
    saveSubThread.start();
    //    saveSubThread.run();
}
void MainWindow::paintEvent(QPaintEvent *)
{
#ifndef GUIYANG_TUICHE

    //主界面阴影
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));
    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }

    //    QImage dd=imageBackground ->scaled( ui->wdgMain->size());
    //    qDebug()<<"image size"<<dd.size();
#endif
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
#if Debug_Size
    QSize sizeUI=QSize(800,560);
#else
    QSize sizeUI=QApplication::desktop()->availableGeometry().size();
#endif
    qDebug()<<"ui size"<<sizeUI;
    this->setFixedSize(sizeUI.width(),sizeUI.height()-30);//->resize(sizeUI);
    this->move(0,0);
    setform->resize(ui->wdgMain->size());
    recordMeasure->resize(ui->wdgMain->size());
    baseRailData->resize(ui->wdgMain->size());
    update();
//    qDebug()<<"this size="<<this->size()<<"ui size"<<ui->wdgMain->size()<<firstUI->size();
}
void MainWindow::slot_clicked(void)
{
    HoverBitmapBtn *pbtn=(HoverBitmapBtn *)this->sender();
    for(int i=0;i<mainPbtnUI.count();i++)
    {
        mainPbtnUI.at(i)->bHover=true;
        if(mainPbtnUI.at(i)!=pbtn)
        {
            mainPbtnUI.at(i)->noHovedDisplay();
        }
    }
    pbtn->bHover=false;
    if(pbtn==ui->pbtnDectedTable)
    {
        stackWdg->setCurrentWidget(recordMeasure);
    }
    else if(pbtn==ui->pbtnDetectedCurve)
    {
        stackWdg->setCurrentWidget(otherPlot);
    }
    else if(pbtn==ui->pbtnMainUI)
    {
        stackWdg->setCurrentWidget(firstUI);
    }
    else if(pbtn==ui->pbtnRailData)
    {
        stackWdg->setCurrentWidget(baseRailData);
    }
    else if(pbtn==ui->pbtnSet)
    {
        stackWdg->setCurrentWidget(setform);
        updateDeviceStatus();
    }
    else if(pbtn==ui->pbtnExit)
    {
        if(closeExe())
            return;
    }
    else if(pbtn==ui->pbtnSave)
    {
        saveUIFrm();
#if 0
        static bool c=false;
        setDisplayTourInfo(c);
        c=!c;
#endif
    }
}
bool MainWindow::closeExe(void)
{
    if(QMessageBox::information(this,QTextCodec::codecForName("gbk")->toUnicode("提示"),QTextCodec::codecForName("gbk")->toUnicode("确定退出系统?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)==QMessageBox::Ok)
    {
        if(QApplication::desktop()->numScreens()>1)
        {
            ipVideo->display1Screen();
        }
        close();
        return false;
    }
    else
    {

        qDebug()<<"dlg CANCEL";
        return true;
    }
}
void MainWindow::updateDeviceStatus(void)
{
    //    slot_deviceControl("updateStatus");
}
void MainWindow::initialPbtnUI(HoverBitmapBtn *pbtn,QString path,QString hoverPath)
{
    pbtn->setText("");
    QPixmap dd(path);
    //    qDebug()<<"pixmap is valid"<<dd.isNull();
    pbtn->setBitmap(dd,QPixmap(hoverPath));
    //    pbtn->setBitmap(QPixmap(path),QPixmap(hoverPath));
    pbtn->setStyleSheet("border:0px;");
    connect(pbtn,SIGNAL(clicked()),this,SLOT(slot_clicked()));
    mainPbtnUI<<pbtn;
}
void MainWindow::slot_dataUpdateTimer(bool b,int n)
{
    if(b)
    {
        readTimerLMS->start(n);
    }
    else
    {
        readTimerLMS ->stop();
    }
}
void MainWindow::slot_overParChange(void)
{
    updateOverPar();
}
void MainWindow::updateOverPar(void)
{
    QSettings  settings("Config.ini", QSettings::IniFormat);
    //    qDebug()<<"over chanage";
    //定点拍摄时，距离支柱的距离
    //载入超限默认参数
    overHeightMax=settings.value("OverRunPar/MaxWireHeight").toInt();
    overHeightMin=settings.value("OverRunPar/MinWireHeight").toInt();
    overWidthMax=settings.value("OverRunPar/MaxWireWidth").toInt();
    overWidthMin=settings.value("OverRunPar/MinWireWidth").toInt();
    constMinPoleDist=settings.value("initialPar/minPoleDis", 4).toInt();
    m_alarmSlope=settings.value("initialPar/alarmSlope", 3).toInt();
    qDebug()<<"min pole dist"<<constMinPoleDist;
}
void MainWindow::speak1(QString dd)
{
    speak.strSpeak=dd;
    if(speak.isFinished())
    {
        speak.start();
    }
    else
    {
        qDebug()<<"speak is not finished";
    }
    //    wchar_t* speek_str = new wchar_t[dd.length()+1];
    //    if(dd.toWCharArray(speek_str))
    //    {
    //        Speek((wchar_t*)dd.utf16());
    //    }
}
//函数功能:根据吊弦的理论位置、开始位置和跨内所有检测数据信息，获得吊弦的拉出值和导高信息,计算吊弦坡度,并将获得的吊弦数据插入数据库
void  MainWindow::parse_dropperGrade(QVector<double> isDropper,double startDist,QVector<MeasureDebugData> &onePoleDatas)
{
    //吊弦对应的里程信息插入当前跨内,吊弦点所对应的拉出值和导高由最近点对应拉出值和导高代替
    //    qDebug()<<"dropper="<<isDropper;
    if((isDropper.count()==0)||(onePoleDatas.count()==0))
    {
        qDebug()<<"dropper count =0";
        return ;
    }
    for(int j=0;j<isDropper.count();j++)
    {
        double curDistance=isDropper.at(j);
        QPair<bool ,int> tmpIndex;
        tmpIndex.first=false;
        QPoint dropperWire=QPoint(0,0);
        for(int i=0;i<onePoleDatas.count()-1;i++)
        {
            int upAbs=abs(curDistance-onePoleDatas.at(i).nWalkDistance);
            int downAbs=abs(curDistance-onePoleDatas.at(i+1).nWalkDistance);

            if((curDistance>=onePoleDatas.at(i).nWalkDistance)&&(curDistance<=onePoleDatas.at(i+1).nWalkDistance))
            {
                tmpIndex.second=i+1;

                tmpIndex.first=true;
                qDebug()<<i<<onePoleDatas.at(i).nWalkDistance<<onePoleDatas.at(i+1).nWalkDistance <<curDistance;
                if(upAbs<downAbs)
                {
                    dropperWire=QPoint(onePoleDatas.at(i).wire1.x(),onePoleDatas.at(i).wire1.y());
                }
                else
                {
                    dropperWire=QPoint(onePoleDatas.at(i+1).wire1.x(),onePoleDatas.at(i).wire1.y());
                }
                break;
            }
        }
        if(tmpIndex.first)
        {
            MeasureDebugData tmp;
            tmp.nWalkDistance=curDistance;
            tmp.wire1=dropperWire;
            tmp.bDropper=true;
            onePoleDatas.insert(tmpIndex.second,tmp);
        }
    }
    float midPoleDis=abs(onePoleDatas.at(0).nWalkDistance-onePoleDatas.at(onePoleDatas.count()-1).nWalkDistance)/2+onePoleDatas.at(0).nWalkDistance;
    for(int i=0;i<onePoleDatas.count()-1;i++)
    {
        if((midPoleDis>=onePoleDatas.at(i).nWalkDistance)&&(midPoleDis<=onePoleDatas.at(i+1).nWalkDistance))
        {
            if(abs(midPoleDis-onePoleDatas.at(i).nWalkDistance)<abs(midPoleDis-onePoleDatas.at(i+1).nWalkDistance))
            {
                MeasureDebugData tmp=onePoleDatas.at(i);
                tmp.bMidPole=true;
                onePoleDatas.replace(i,tmp);
                //                tmp.locationRemark=QString::fromWCharArray(L"跨中");
            }
            else
            {
                MeasureDebugData tmp=onePoleDatas.at(i+1);
                tmp.bMidPole=true;
                onePoleDatas.replace(i+1,tmp);
            }
            break;
        }
    }
    qDebug()<<"fire"<<midPoleDis<<onePoleDatas.count()<<onePoleDatas.at(0).nWalkDistance<<onePoleDatas.at(onePoleDatas.count()-1).nWalkDistance;
    //获取相邻吊弦之间的导高极值信息
    //qDebug()<<"after dropper count="<<onePoleDatas.count();
    int maxHeight=onePoleDatas.at(0).wire1.y();
    int minHeight=onePoleDatas.at(0).wire1.y();
    double preDropperDis=startDist;
    for(int i=0;i<onePoleDatas.count();i++)
    {
        //        qDebug()<<i<<onePoleDatas.at(i).nWalkDistance<<onePoleDatas.at(i).bDropper<<onePoleDatas.at(i).nWireHeight;//<<onePoleDatas.at(i).nWireWidth<<onePoleDatas.at(i).nWireHeight;
        if(onePoleDatas.at(i).bMidPole)
        {
            MeasureIdEx_t tmp=curMeaExData;
            memset(&curMeaExData.onePolePar,0,sizeof(OnePolePar));
            tmp.onePolePar.gaocha=0;
            tmp.locationRemark=QString::fromWCharArray(L"跨中");
            tmp.onePolePar.dWireHeight=onePoleDatas.at(i).wire1.y();
            tmp.onePolePar.dWireWidth=onePoleDatas.at(i).wire1.x();
            tmp.nWalkDistance=onePoleDatas.at(i).nWalkDistance;
            m_pmDb->insertRecord_MeasureEx(tmp);
        }
        if(onePoleDatas.at(i).bDropper)
        {
            int absDis=abs(onePoleDatas.at(i).nWalkDistance-preDropperDis);
            int curSlope=0;
            if(absDis==0)
                curSlope=0;
            else
                curSlope=abs(maxHeight-minHeight)/absDis;

            //            qDebug()<<QString("max=%1,min=%2,abs height=%3,abs dis=%4,slope=%5,preDis=%6,curDis=%7").arg(maxHeight).arg(minHeight).arg(maxHeight-minHeight).arg(absDis).arg(abs(maxHeight-minHeight)/absDis/1.0).arg(preDropperDis).arg(onePoleDatas.at(i).nWalkDistance);
            MeasureIdEx_t tmp=curMeaExData;
            qDebug()<<"cur pole"<<tmp.poleid;
            tmp.nWalkDistance=onePoleDatas.at(i).nWalkDistance/1.00;
            tmp.dKm=profile.startKm+onePoleDatas.at(i).nWalkDistance/1000.000;
            tmp.onePolePar.dWireHeight=onePoleDatas.at(i).wire1.y();
            tmp.onePolePar.dWireWidth=onePoleDatas.at(i).wire1.x();
            tmp.onePolePar.wireHeight1=onePoleDatas.at(i).wire2.y();
            tmp.onePolePar.wireWidth1=onePoleDatas.at(i).wire2.x();
            tmp.onePolePar.gaocha=abs(maxHeight-minHeight);
            tmp.onePolePar.slope=curSlope;
            tmp.locationRemark=QString::fromWCharArray(L"吊弦");
            m_pmDb->insertRecord_MeasureEx(tmp);

            preDropperDis=onePoleDatas.at(i).nWalkDistance;
            if(MeasureDB::overAlarm)
            {
                if(curSlope>m_alarmSlope)
                    speak1(QString::fromWCharArray(L"坡度千分之%1").arg(curSlope));
            }
            //下一次吊弦坡度计算的初始值
            maxHeight=onePoleDatas.at(i).wire1.y();
            minHeight=onePoleDatas.at(i).wire1.y();
        }
        else
        {
            maxHeight=qMax(maxHeight,onePoleDatas.at(i).wire1.y());
            minHeight=qMin(minHeight,onePoleDatas.at(i).wire1.y());
        }
    }
    double tmpAbsDist=abs(curMeaExData.nWalkDistance-isDropper.at(isDropper.count()-1));
    double tmpAbsHeight=abs(maxHeight-minHeight);
    //    curMeaExData.onePolePar.wiresWidth=curMeaData.wire1.x();
    //    curMeaExData.onePolePar.wiresHeight=curMeaData.wire1.y();
    curMeaExData.onePolePar.gaocha=tmpAbsHeight;
    curMeaExData.onePolePar.slope=int(tmpAbsHeight/tmpAbsDist);
    qDebug()<<"mid data"<<curMeaExData.onePolePar.dWireWidth<<curMeaExData.onePolePar.dWireHeight;
}
//函数功能:语音报警处理，进行超限参数的标记及报警
void MainWindow::alarmPar(int width,int height)
{
    if(MeasureDB::overAlarm)
    {
        bool f=false;
        if(width<overWidthMin)
        {
            f=true;
            curMeaData.dataType=QString::fromWCharArray(L"拉出值超限");
        }
        else if(width>overWidthMax)
        {
            f=true;
            curMeaData.dataType=QString::fromWCharArray(L"拉出值超限");
        }
        if(height<overHeightMin)
        {
            f=true;
            curMeaData.dataType=QString::fromWCharArray(L"导高超限");
        }
        else if(height>overHeightMax)
        {
            f=true;
            curMeaData.dataType=QString::fromWCharArray(L"导高超限");
        }

        if(f)
        {
            //            qDebug()<<"over par is ok "<<m_pmDb->insertRecord_overPars(curMeaData);
            //                    QString dd=QString::fromWCharArray(L"参数超限限拉出值%1导高%2").arg(tmp.wire1.x()).arg(tmp.wire1.y());
            speak1(QString::fromWCharArray(L"超限拉出值%1导高%2").arg(width).arg(height) );
            m_pmDb->updateDetailTb(curMeaData,0,curMeaData.dataType);
        }
        else
        {
            curMeaData.dataType=QString::fromWCharArray(L"普通");
        }
    }
    else
    {
        curMeaData.dataType=QString::fromWCharArray(L"普通");
    }
}
//函数功能:根据距离计算吊弦位置
void MainWindow::getDropperStep(double nWalkDistance,QVector<double> &dropperSteps,float curWalk)
{
    int tmpDist=nWalkDistance-nFirstDropperStep*2;
    QVector<double> tmp;
    //    qDebug()<<tmpDist<<maxDropperStep<<minDropperStep+1;//<<t;
    for(double i=tmpDist/maxDropperStep;i<(tmpDist/minDropperStep+1);i++)
    {
        tmp<<tmpDist/i;
    }
    double optimalStep;
    if(tmp.count())
        optimalStep=tmp.at(0);
    else
    {
        qDebug()<<"dropper is error";
    }
    double midDropperStep=(maxDropperStep-minDropperStep)/2.0+minDropperStep;
    for(int i=0;i<tmp.count()-1;i++)
    {
        if((tmp.at(i)-midDropperStep)>(tmp.at(i+1)-midDropperStep))
        {
            optimalStep=tmp.at(i);
        }
    }
    for(int i=0;i<tmpDist/optimalStep+1;i++)
    {
        dropperSteps<<((optimalStep)*i+4+curWalk);
    }
    //    qDebug()<<cc<<tmp<<"dropper count"<<polesets[m_currIdx].nWalkDistance<<dropperSteps;
}
void MainWindow::slot_updateErrorPole(bool f)
{
    bErrorPole=f;
}
void MainWindow::slot_radorPowerOff(void)
{
    m_pbtnRadorStatus->setCurStatus(DISCONNECTED);
    stopMeasure();
    MeasureDB::bRadarStatus=false;
    qDebug()<<"rador status"<<m_pRadorScanner->getRadorStatus();
    m_pRadorScanner->tcpBreak();
}
void MainWindow::slot_minUI(void)
{
    this->showMinimized();
}
