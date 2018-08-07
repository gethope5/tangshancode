#ifndef DLGDRAWSTLY_H
#define DLGDRAWSTLY_H

#include <QDialog>
#include <qtextcodec.h>
namespace Ui {
    class dlgDrawStly;
}

class dlgDrawStly : public QDialog
{
    Q_OBJECT

public:
    explicit dlgDrawStly(QWidget *parent = 0);
    ~dlgDrawStly();
    bool bDrawStyle;
private slots:
    void on_pbtnOK_clicked();

private:
    Ui::dlgDrawStly *ui;
};

#endif // DLGDRAWSTLY_H
