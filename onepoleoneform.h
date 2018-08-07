#ifndef ONEPOLEONEFORM_H
#define ONEPOLEONEFORM_H

#include <QWidget>
#include <QtSql>
#include <QtWidgets>
#include <QDebug>
#include "database.h"
#include <qtextcodec.h>
namespace Ui {
    class onePoleOneForm;
}

class onePoleOneForm : public QWidget
{
    Q_OBJECT
public:
    explicit onePoleOneForm(QString labelName,QWidget *parent = 0);
    ~onePoleOneForm();
    void update_display(MeasureIdEx_t,bool );
    bool isMeasure;

private slots:
    void on_pbtnOpen_clicked();
    void slot_indexChanged(void);
private:
    Ui::onePoleOneForm *ui;
    MeasureIdEx_t curMeaExData;
    QString measDataFilepath;
    bool Opendatabase(const QString  &filepath);
    bool bOpenFile;
    QSqlDatabase m_db;
    QList<MeasureIdEx_t> onePoleInfo;
    void getDBValue(void);
    void getRailInfo(void);
    QString sRailName;
    QString sDirection;
    int curIndex;
};

#endif // ONEPOLEONEFORM_H
