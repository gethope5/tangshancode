#ifndef PREOLDRAILFORM_H
#define PREOLDRAILFORM_H

#include <qtgui>
#include <qtsql>
#include <qtextcodec.h>
#include <QStackedLayout>
#include <math.h>
#include <QtWidgets>
//#include <QListWidget>
#include "database.h"
#include "syshead.h"
//#include "prerunsettingdlg.h"
namespace Ui {
    class preOldRailForm;
}

class preOldRailForm : public QWidget
{
    Q_OBJECT

public:
    explicit preOldRailForm(QWidget *parent = 0);
    ~preOldRailForm();
    void setPreRunSetting(void);
    void initialUI(void);
    QString railName(void);
    QString railDirection(void);
    void protectEnv(void);
    void restoreEnv(void);
    void getProfile(RunProfile& toprofile);
    void currentKM(int curStationIdx,int curPoleIdx);

signals:
    void railInfoChanged(bool);
    void startKm(QString,QString);
private:
    Ui::preOldRailForm *ui;
    QVector <stationSubPole> railPoleClassify;
    RunProfile profile;
    LineInfoSelect  m_lineInfo;
    void readSettingParameters( LineInfoSelect &mLineData );
    void saveSettingParameters( LineInfoSelect  data );
    void setListWidgetCheckable(QListWidget *widget,int curIdx);
    void setPole(int sidx);
    void setDirection(int id);
    void setStation(QString sTableName);
    void addItems(QListWidget* plw, QString text);
    QString sxTableName;
    QString xxTableName;
    QString      m_strLineName;
    QStringList m_strLineList;
    int         m_lineIndex;
    QString     m_strLinename;
    int         m_directionIndex;
    QString     m_strStationname;
    float       m_fPosition;
    QString     m_strPolename;
private slots:
    void slot_railNameChanged(int idx);
    void adjustPos();
    void slot_railDirectionChanged(int);
    void slot_railStationChanged(QListWidgetItem*);
    void slot_railPoleChanged(QListWidgetItem*);
};

#endif // PREOLDRAILFORM_H
