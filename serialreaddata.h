#ifndef SERIALREADDATA_H
#define SERIALREADDATA_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>

class SerialReadData : public QObject
{
    Q_OBJECT
public:
    explicit SerialReadData(QSerialPort *serial = nullptr,QObject *parent = nullptr);

    QSerialPort *m_pserial;
    QString serialBuf;    //储存接收到的数据
    void SRDworking();    //线程任务函数
private:

signals:
    void sigLogDataDisplay(QString serialBuf);
    void sigDataSort(QStringList ProcessedData);
};

#endif // SERIALREADDATA_H
