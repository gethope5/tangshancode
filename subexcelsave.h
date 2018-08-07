#ifndef SUBEXCELSAVE_H
#define SUBEXCELSAVE_H

#include <QThread>
#include <QtGui>
#include <QtSql>
#include <QFile>
typedef struct
{
    bool hiddenFlag;
    int columnIndex;
    QString columnTitle;
    QString tableName;
}tableInfo;
class SubExcelSave : public QThread
{
    Q_OBJECT
public:
    explicit SubExcelSave(QObject *parent = 0);
    QString sSavePath;
    QString tableName;
    QString tableTitle;
    QSqlDatabase dataBase;
    void run(void);
    void railData(void);
signals:
    void saveOK(bool);
public slots:
private:
    bool SaveCSVFile(void);
};

#endif // SUBEXCELSAVE_H
