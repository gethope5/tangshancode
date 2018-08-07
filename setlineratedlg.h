#ifndef SETLINERATEDLG_H
#define SETLINERATEDLG_H

#include <QDialog>
#include "syshead.h"
namespace Ui {
class setLineRateDlg;
}

class setLineRateDlg : public QDialog
{
    Q_OBJECT

public:
    explicit setLineRateDlg(QWidget *parent = 0);
    ~setLineRateDlg();

private:
    Ui::setLineRateDlg *ui;
private slots:
    void slot_setRate(void);
    void slot_lineRate(eDeviceType,QString);
    void slot_Expousre(eDeviceType,QString);
signals:
    void setRate(eDeviceType,QString);
    void setExpousre(eDeviceType,QString);
};

#endif // SETLINERATEDLG_H
