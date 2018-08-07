#ifndef NEWRAILPOLEFRM_H
#define NEWRAILPOLEFRM_H

#include <QWidget>
#include "database.h"
namespace Ui {
    class newRailPoleFrm;
}

class newRailPoleFrm : public QWidget
{
    Q_OBJECT

public:
    explicit newRailPoleFrm(QWidget *parent = 0);
    ~newRailPoleFrm();
    void updateNumber(QString sPoleNumber);
    void initialUI(bool b);
public slots:
    void slot_newPole(void);
private slots:
    void on_pbtnAddPole_clicked();
    void slot_poleDirection(bool b);
private:
    Ui::newRailPoleFrm *ui;
    void poleChanged(void);
    void readConfig(void);
signals:
    void newRailPole(QString newPole);
};

#endif // NEWRAILPOLEFRM_H
