#ifndef OLDRAILPOLEFRM_H
#define OLDRAILPOLEFRM_H
#include "global.h"
#include <QWidget>
#include "confirmpolechangedialog.h"
#include <QListWidgetItem>
namespace Ui {
    class oldRailPoleFrm;
}

class oldRailPoleFrm : public QWidget
{
    Q_OBJECT

public:
    explicit oldRailPoleFrm(QWidget *parent = 0);
    ~oldRailPoleFrm();
    void updateNumber(QString sPoleNumber);
    void fillPoleSelList(int id );
    unsigned long walkDistance;
    QStringList curDisplayPoles;
    QString curDisplayPole;
    void slots_railPoleClicked(QString toPole);
    void clearOldPoleDipslay(void);

private:
    Ui::oldRailPoleFrm *ui;
    void slot_changePoleId(QString toPole);
    int m_currIdx;
private slots:
    void slots_railPoleClicked(QListWidgetItem*e);
signals:
    void changingPole(QString);
//    void passedPoleId(QString poleid);
};

#endif // OLDRAILPOLEFRM_H
