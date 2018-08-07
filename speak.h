#ifndef SPEAK_H
#define SPEAK_H

#include <QObject>
#include <QThread>
#include "speechlib.h"
#include <QDebug>
class Speak : public QThread
{
    Q_OBJECT
public:
    explicit Speak(QObject *parent = 0);
    QString strSpeak;
signals:

public slots:
private:
    virtual void run(void);
};

#endif // SPEAK_H
