#ifndef PRENEWRAILFRM_H
#define PRENEWRAILFRM_H

//#include <QWidget>.
#include <QtWidgets>
#include <qtextcodec.h>
#include "preoldrailform.h"
//#include <prerunsettingdlg.h>
namespace Ui {
    class preNewRailFrm;
}

class preNewRailFrm : public QWidget
{
    Q_OBJECT

public:
    explicit preNewRailFrm(QWidget *parent = 0);
    ~preNewRailFrm();

    QString railName(void);
    QString railDirection(void);
    void newRailInfo(RunProfile &profile);
signals:
    void railInfoChanged(bool);
private slots:
    void on_spbRailName_valueChanged(const QString &arg1);
    void on_cmbDirection_activated(int index);

private:
    Ui::preNewRailFrm *ui;
};

#endif // PRENEWRAILFRM_H
