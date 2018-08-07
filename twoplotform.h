#ifndef TWOPLOTFORM_H
#define TWOPLOTFORM_H

#include <QWidget>
#include <QtGui>
#include <QStackedLayout>
#include "qcustomplot.h"
#include "dlgDrawStly.h"
#include "database.h"
#include "saveui.h"
#include "setdrawpardlg.h"
#include "newrailpolefrm.h"
#include "oldrailpolefrm.h"
#include "hoverbitmapbtn.h"
#include <qtextcodec.h>
#include "speak.h"

#define std_Height 5300
#define heightOver2Up std_Height+100
#define heightOver2Down std_Height-100

#define widthOver2 450

#define Default_X_Value 100
#define DEBUG_51UI 0
#define Another_Curve_STD 5
struct curveInfo
{
    int curveIndex;
    QVector<double> x;
    QVector<double> y;
    QColor color;
    curveInfo():curveIndex(0)
    {
        x.clear();
        y.clear();
        curveIndex=0;
        color=Qt::red;
    }
    void clear(void)
    {
        x.clear();
        y.clear();
    }
};

namespace Ui {
class twoPlotForm;
}
#if DEBUG_51UI
class twoPlotForm : public QWidget,public QThread
        #else
class twoPlotForm : public QWidget
        #endif
{
    Q_OBJECT

public:
    explicit twoPlotForm(QWidget *parent = 0);
    ~twoPlotForm();
    QMenu* selectSubMenu;
    bool bMeasureType;
    void setMeasureStatus(bool flag);
    void updateMeasureData(MeasureDebugData &databuffer,int nSide,bool flag=true);
    void setPole(QString poleNumber,double sDistance);
    void setDisplayPar(void);
    void setRailType(bool);
    unsigned long curDistance;
    newRailPoleFrm *newRailPole;
    oldRailPoleFrm *oldRailPole;
    int oldRail_idx;
    void forgeCurve(void);
    void clearCurves();
    void updatePoleNumber(QString);
private:
    Ui::twoPlotForm *ui;
    Speak alarmSpeak;
    void addOneData( MeasureDebugData &databuffer);
    void uiInitial(void);
    QPoint preWire1;
    QCustomPlot *m_drawWireWidth;
    QCustomPlot *m_drawWireHeight;
    QPoint m_XRange;
    QPoint m_YRange;
    QColor defaultAxisColor;
    QColor defaultCurveColor;
    QColor defaultBackgroundColor;
    bool    defaultDrawStyle;


    curveInfo m_heightDropperCurve;     //2 吊弦曲线序号、颜色、二维数据等信息
    curveInfo m_heightOver1Down;    //4
    curveInfo m_heightOver1Up;      //5
    curveInfo m_heightOver2Down;    //6
    curveInfo m_heightOver2Up;      //7
    curveInfo m_HeightToolTip;

    curveInfo m_widthOver2Up;       //2
    curveInfo m_widthOver1Up;       //3
    curveInfo m_widthOver1Down;     //4
    curveInfo m_widthOver2Down;     //5

    curveInfo m_widthToolTip;     //5

    //    curveInfo m_WidthCurve2;     //5

    QVector<curveInfo> heightCarriers;                  //承力索曲线组
    QVector<QPair<curveInfo,curveInfo> > wire1Curves;   //拉出值及导高曲线组
    QVector<QPair<curveInfo,curveInfo> > wire2Curves;   //锚段及线岔处的导线2曲线组
    QVector<QPair<curveInfo,curveInfo> > poleCurves;    //拉出值及导高的提示杆号曲线组
    int curWire2Index;
    int curWire1Index;
    int curCarrierIndex;


    QPair<curveInfo,curveInfo > addWireCurve(QColor color);
    void addWire2Point(MeasureDebugData &databuffer );
    QVector<double> rBars;

    QVector <QPair<int,QVector<QPoint> > > carrirCurveData;

    double preDropperWalk;
    QCPErrorBars *errorBars;
    QColor axisColor;
    QColor curveColor;
    QColor backgroundColor;
    bool   drawStyle;

    QStackedLayout *stackRailType;
    void setDrawStyle(bool bLine);
    void readConfigDisplay(void);
    void getDefaultDisplay();
    QString uiSavePath;
    setDrawParDlg   *setDrawPar;
    bool bNewRailType;
    void addPoleText(QCustomPlot *textParent,QString poleNumber,unsigned long sDistance,bool flag=true);
    bool m_bUIVisible;
    void setExpand(bool flag);
    void strToColor(QString str,QColor &color);
    QString colorTostr(QColor &color);
    void setAxisColor(QColor);
    QVector <QCPItemText *> widthPole;
    QVector <QCPItemText *> heightPole;
    void axisChange(const int);
    void drawAlarmCurve(const int x,bool flag);
    bool bAlarm1Display;
    bool bAlarm2Display;
    bool bDropperDisplay;
    bool bPoleDisplay;
    bool bCarrierDisplay;
    void setAlarm1Visible(bool flag);
    void setAlarm2Visible(bool flag);
    void setPoleVisible(bool flag);
    void setCarrierVisible(bool flag);
    void setDropperVisible(bool flag);
    void displayTypeInitial(void);
    void updateOverPar(void);
    //超限参数
    int overWidthMax;
    int overWidthMin;
    int overHeightMax;
    int overHeightMin;

    int updateRangY(QCustomPlot *textParent);
    void drawCarrier(int carrier,double walk);

    QPair<bool, QList<int> > wire2Start;               //
    QColor  curWire1Color;
    void drawPoleCurve(double dist,bool flag,int curveIndex);
    void updateALlPoleCurve(bool flag);
    curveInfo add1Curve(QCustomPlot *drawWire,QColor color);
    void updateTipCurve(QCustomPlot *p,curveInfo &c,QMouseEvent *e);
    void clearTipCurve(QCustomPlot *plot,curveInfo &curve);
    void updatePolePosition(void);
    void addWire2Curve(void);
    void processWire2Replay(QVector<QPoint> &wire2,QVector<double> &dDistance);
    void processCarrierReplay(QVector<double> &dCarrierHeight,QVector<double>&dDistance);
    void setWire2Visible(bool flag);
    void updateDrawUI(void);
public slots:
    void AxisChangingWH(QCPRange);
    void AxisChangingWX(QCPRange);

    void AxisChangingHX(QCPRange);
    void AxisChangingHY(QCPRange);
    void defaultSize(QMouseEvent *event);

    void defaultSize1(QMouseEvent *event);
    void slot_initialPole(bool,QString);
    void slot_expandUI(bool b);
    void on_pbtnMeasures_clicked();
    //用于数据回放
    void slot_dataReplay(QVector<double> dDistance,QVector<QPoint> wire1,QVector<QPoint> wire2,QVector<double> dCarrier,QVector<QPair<double,int> > &poles);
    void on_pbtnUIExpand_clicked();
    void on_pbtnUISet_clicked();
    void slot_overParChange(void);
    void slot_dropperData(QPair<int,QVector<double> >);
    void slot_mouseMove(QMouseEvent *);
    void slot_mouseRelease(QEvent *e);
    void slot_endWire2(void);
    void slot_createWire1Curve(void);
    void slot_createCarrier(void);
    void slot_clearCurve(void);
signals:
    void MeasuresSatus(void);
    void stopMeasure(void);
    void newWireCross(bool);
    void endWire2(void);
    void errorPole(bool);
private slots:
    void on_pbtnStop_clicked();
    void on_pbtnSpeech_clicked();
    void slot_curveVisiable(curveType,bool);
    void slot_setDisplayStyle(curveType type,QColor color);
    void slot_setAxisRange(void);
    void on_chkErrorPole_clicked();
protected:
    bool  eventFilter(QObject *o, QEvent *e );
};

#endif // TWOPLOTFORM_H
