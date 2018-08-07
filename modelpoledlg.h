#ifndef MODELPOLEDLG_H
#define MODELPOLEDLG_H

#include <QDialog>

namespace Ui {
class modelPoleDlg;
}

class modelPoleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit modelPoleDlg(QWidget *parent = 0);
    ~modelPoleDlg();
    void modifyPoleInfo(int &startId,int &endId,int &pole);
    void initialUI(bool);
    int getDeleteID(void);
private slots:
    void on_pbtnOk_clicked();

    void on_pbtnCancel_clicked();

private:
    Ui::modelPoleDlg *ui;
};

#endif // MODELPOLEDLG_H
