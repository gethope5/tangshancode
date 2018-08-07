#ifndef SOFTBOARDDLG_H
#define SOFTBOARDDLG_H

#include <QDialog>
#include <QSignalMapper>

namespace Ui {
    class softBoardDlg;
}

class softBoardDlg : public QDialog
{
    Q_OBJECT

public:
    explicit softBoardDlg(QWidget *parent = 0);
    ~softBoardDlg();
    QSignalMapper *m;// = QSignalMapper(this);
    void setCustomLabel(QString labelBuffer);
    QString customLabel(void);
public slots:
    void keyPress(int i);
signals:

    void keyNum(int i);
private:
    Ui::softBoardDlg *ui;
};

#endif // SOFTBOARDDLG_H
