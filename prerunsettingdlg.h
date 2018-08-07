#ifndef PRERUNSETTINGDLG_H
#define PRERUNSETTINGDLG_H
#include <QtWidgets>
#include <qtextcodec.h>
#include <qtsql>
#include "database.h"
#include <QSettings>
#include "syshead.h"
namespace Ui {
class PreRunSettingDlg;
}


class PreRunSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PreRunSettingDlg(QString title,QWidget *parent = 0);
    ~PreRunSettingDlg();


    void getProfile(RunProfile& toprofile);
    QString updatePole(void);
    bool isVerify(void);
    wireStandardValues getStandardValue(void);
    QString fileName(void);
    bool bClearDist;


    void setPreRunSetting(void);
    void initialUI(void);
    QString railName(void);
    QString railDirection(void);
    void protectEnv(void);
    void restoreEnv(void);
    void currentKM(int curStationIdx,int curPoleIdx);
private:
    Ui::PreRunSettingDlg *ui;
    int  m_nDirect;
    QString  m_dbName;
    QString  m_datapath;
    void readConfigure();
    void writeSetting();
    RunProfile profile;
    void uiVerify(void);
    void uiMeasure(void);
    void    getPar(void);
    void setGroupEnable(bool);

    //old pre set
    QVector <stationSubPole> railPoleClassify;

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
    void setTableName(void);
public slots:
    void saveInitialPar(sensorInitialValues sensorIntialValue);

protected slots:
    void setSavePath();
    void setBackData();
    void slot_tabWidget(bool e);
    void slot_startKmChanged(QString,QString);
    void on_spbStationName_valueChanged(const QString &arg1);

    //old pre set
    void slot_railNameChanged(int idx);
    void adjustPos();
    void slot_railDirectionChanged(int);
    void slot_railStationChanged(QListWidgetItem*);
    void slot_railPoleChanged(QListWidgetItem*);
    void on_chbDirect_pressed();

    void on_cmbDirection_activated(int index);
    void slot_verifyType(bool);

public:
    void saveDirectFlag(void);
signals:
    void setMeasureType(bool,QString);
    void clearDistanc(COM_COMMAND_TYPE);
    void directFlag(int);
//    void baseParChanged(void);

    //old pre set
    void railInfoChanged(bool);
    void startKm(QString,QString);
};

#endif // PRERUNSETTINGDLG_H
