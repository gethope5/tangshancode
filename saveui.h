#ifndef SAVEUI_H
#define SAVEUI_H

#include <QtGui>
#include <QThread>
#include <QMessageBox>
#include <qtextcodec.h>
class saveUI : public QThread
{
    Q_OBJECT
public:
    explicit saveUI(QObject *parent = 0);
    QString saveFilePath;
    QPixmap UIPixmap;
    void run(void);
signals:
    void saveStatus(bool);
public slots:
    void slot_saveUIStatus(bool cc);
};

#endif // SAVEUI_H
