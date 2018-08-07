#ifndef SETTING_H
#define SETTING_H
#include <QVector>
#include <QString>
#include <QStringList>
#include <qnamespace.h>
class setting{
public:
    setting();
    static int strColorToInt(QString colorbuffer);
    static QString intColorToString(int colorbuffer);
};
#endif // SETTING_H
