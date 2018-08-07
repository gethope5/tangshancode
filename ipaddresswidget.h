#ifndef IPADDRESSWIDGET_H
#define IPADDRESSWIDGET_H
#include <QtGui>
#include <QWidget>
#include <QSpinBox>

class IpAddressWidget : public QWidget
{
    Q_OBJECT
private:
    QSpinBox *m_SpinBox[4];
    QLabel *m_Label[3];
    QString m_Separator;        //四个数字之间的分隔符
    int nowBoxIndex;
protected:
    bool eventFilter(QObject *object, QEvent *event);
    void keyReleaseEvent (QKeyEvent *e);
    void keyPressEvent(QKeyEvent *e);
public:
    IpAddressWidget(QWidget *parent = 0);
    void setDefaultIP(QString defaultIP);
    void setReadOlny(bool flag);
    ~IpAddressWidget();
    QString GetIpAddressString();
    void SetSeparator(const QString &separator);

};

#endif // IPADDRESSWIDGET_H
