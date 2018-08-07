#ifndef FIRSTFORM_H
#define FIRSTFORM_H

//#include <QWidget>
#include <QtWidgets>
//#include <qtextcodec.h>
#include <qdebug.h>
namespace Ui {
    class firstForm;
}

class firstForm : public QWidget
{
    Q_OBJECT

public:
    explicit firstForm(QWidget *parent = 0);
    ~firstForm();
private slots:
    void on_pbtnMin_clicked();

private:
    Ui::firstForm *ui;
    void paintEvent(QPaintEvent *e);
    void readConfigerFile(void);

    virtual void resizeEvent(QResizeEvent *event);
signals:
    void signal_minUi(void);
};

#endif // FIRSTFORM_H
