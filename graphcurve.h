#ifndef GRAPHCURVE_H
#define GRAPHCURVE_H

#include <QWidget>
#include <QtGui>
#include <QVector>
#include "setting.h"

using namespace std;
#define MAX_X_CORD_MILE     1000
typedef struct __DisplayColor{
    QColor  scanWireColor;
    QColor  backGroundColor;
    QColor axisColor;
} DisplayColor;
typedef struct __DispalyRect{
    int width;
    int height;
} DispalyRect;
class PoleData
{
public:
    PoleData(){dist = -1.0;}
    QString name;
    double  dist;
};

class graphcurve : public QPushButton
{
    Q_OBJECT
public:
    graphcurve( QWidget *parent = 0);
    bool dotFlag;
    void refreshPixmap();
    QString startAngle,endAngle,angleResolution,frequency;
    void enqueue(QStringList * drawDatabuffer,float AngleResolution,float starAngle);
protected:
    void penInitial(void);
    void readParamter(void);
    void paintEvent(QPaintEvent *);
    void drawAxies(QPainter* painter);
    void drawCurves(QPainter *painter);
    void resizeEvent(QResizeEvent * /* event */);

    int             m_startPoleIdx;
    QPen            gridPenAxis;
    QPen            gridPenCurves;
    double          m_startDist;
    QPixmap         pixmap;
    DisplayColor    uiColor;
    QVector<QPointF> drawData;
    static bool     fristDraw;
    unsigned char   constZoom;
    unsigned int    dataCount;
    DispalyRect     displayRect;

public slots:
    void slot_setDisplayColor(int type,QColor color);
    //QVector<int>  m_poleList;
};
#endif // GRAPHCURVE_H
