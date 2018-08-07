#ifndef HOVERBITMAPBTN_H
#define HOVERBITMAPBTN_H
#include <QtWidgets>
#include <QPushButton>
#include <QPixmap>


class HoverBitmapBtn : public QPushButton
{
    Q_OBJECT
public:
    HoverBitmapBtn(QWidget *parent = 0);
    explicit HoverBitmapBtn(const QPixmap& normal, const QPixmap& highmap, QWidget *parent = 0);
    void setBitmap(const QPixmap& normal, const QPixmap& highmap);
    void setAlignment(int mode) { alignFlag = mode;}
    void setHold(bool hold) { bHold = hold;}
    virtual ~HoverBitmapBtn() {}
    bool clickflag;
    void SetClickFlag(HoverBitmapBtn *pBtn,int pBtnLength,int CurrentTrueClick);
    void noHovedDisplay();
    static HoverBitmapBtn *prePbtn;
    static HoverBitmapBtn *curPbtn;
    bool    bHover;
    void initialPbtnUI(QString path,QString hoverPath);
public slots:
    void slot_changeStatic(void);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    QString beforeCSS;
    QString afterCSS;
    QString backColor;
    QString hoverFontColor;
protected:
    QPixmap nmap, hmap;
    int alignFlag;
    bool bHold;
    void initialButton(void);
    void getConfigInfo( void );

signals:
    void getHover();
};/*
static HoverBitmapBtn *prePbtn=NULL;*/
#endif // HOVERBITMAPBTN_H
