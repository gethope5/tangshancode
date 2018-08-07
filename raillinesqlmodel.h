#ifndef RAILLINESQLMODEL_H
#define RAILLINESQLMODEL_H

#include <QSqlTableModel>

class RailLineSqlModel : public QSqlTableModel
{
    Q_OBJECT

public:
    RailLineSqlModel ( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() );

    void updateRowIndex(int row, bool insert);
    void setPrimKeyAsRow(int row);

    QVariant data(const QModelIndex &item, int role) const;
    bool DeleteRecord(int row);
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual void setquery(const QSqlQuery &query);
//    virtual bool deleteRowFromTable(int row);
protected:

};

#endif // RAILLINESQLMODEL_H
