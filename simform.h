#ifndef SIMFORM_H
#define SIMFORM_H

#include <QWidget>
#include "qcustomplot.h"
#include <qtextcodec.h>
#include <qtimer.h>
#include <QTime>
#include "radorscannermodule.h"
#include "database.h"
#include "preoldrailform.h"
#include "setform.h"
#define Side_Start_Angle 125
#define Side_End_Angle 150

#define SIM_TIMER_INTERVAL 1000
#define SIM_TIMER_INTERVAL_FAST 10
namespace Ui {
class simForm;
}

class simForm : public QWidget
{
    Q_OBJECT

public:
    explicit simForm(QWidget *parent = 0);
    ~simForm();
private:
    Ui::simForm *ui;

    QCustomPlot *m_drawSingle;
    QCustomPlot *m_drawSide;

    void setAxisRange(void);
    QTimer *timer;

    QStringList strPackages;//包含多帧完整的数据，包括包头和包尾。
    void drawSinglePackage(QString ,float baseAngle);
    int packageIndex;
    QVector<QPair<int,int> > zeroCounts;//存放元素，first=帧序号,second=零值数目
    QVector<QPair<int,QPair<int,int> > > onePackage;//存放一帧完整的数据,<i,<x,y> >,i数据对应的序号,x数据cos，y数据sin
    unsigned int startIndex;
    unsigned int endIndex;
    QVector<QPair<int ,int > > wireMarkIndex;
    int widthCurveIndex;
    QVector<double > startX;
    QVector<double > startY;
    QVector<double > endX;
    QVector<double > endY;
    int wireRangeMarkIndex;
    void drawSideDataRange(int startAngle,int endAngle);
    void drawWireDataRange(void);

    void updateIndex(void);
    void analyze_sideConst( QVector<QPair<int,QPair<int,int> > > side,double &a,double &b);
    void readTxtOriginalData(QString filePaths,QStringList &data);
    bool bIndexIncrease;//false，模拟数据测量状态；true,标定状态
    int verifyIndex;
    int m_nLachuzhiC ;
    int m_nDaogaoC ;
    void readConfigure(void);
    void drawWireMark(int x,int y,int n);
    int m_wireStartAngle;
    int m_wireEndAngle;
    bool indexDirection;
    double m_virtualWalk;
    QString curDrawData;
    void changeBaseAngle(float angle);
    virtual void resizeEvent(QResizeEvent *event);
    void drawRailCurve(int x,int y);
public slots:
    void slot_timerOnePackage(void);
    //绘制雷达当前一帧的数据
    void radorData(QString);
    //绘制当前当前获取的支柱数据
    void slot_updateSideCuve(QVector<QPair<int,QPair<int,int> > > tmp);
    void slot_updateInitialPar(sensorInitialValues);
    void slot_debug(QString );
    //标示当前获取的导线位置
    void slot_allwire(QVector<singleWireInfo>,bool);
    void slot_startinitialed (void);
    void slot_getVerify(wireStandardValues);
    void slot_updateRailAngle(float & f);
    void simStop(bool);
    void simStart(bool);
private slots:
    void on_pbtnStart_clicked();
    void on_pbtnStop_clicked();
    void AxisChanging1(QCPRange);
    void defaultSize1(QMouseEvent*);
    void on_pbtnAnalyze_clicked();
    //    void on_pbtnCurPackage_clicked();
    void on_pbtnOpen_clicked();
    void on_pbtnVerify_clicked();
    void on_pbtnSpeed_clicked();
    void on_pbtnDirection_clicked();
    void on_pbtnChangeWireAngle_clicked();
    void on_pbtnChangeSideAngle_clicked();
    void slot_railAngle(void);
    void on_pbtnUpdateInitialWidth_clicked();

signals:
    void oneStrPackage(QStringList,double);
    void simulationVerify(void);
    void controlTimer(bool,int n=1000);
    void updateWireRange(int,int);
    void startMeausre(void);
    void updateBaseAngle(int ,int ,bool);
    void updateRailAngle(QPair<bool,bool>);
};

#endif // SIMFORM_H

