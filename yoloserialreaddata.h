#ifndef YOLOSERIALREADDATA_H
#define YOLOSERIALREADDATA_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include "./log/log.h"

class YOLOSerialReadData : public QObject
{
    Q_OBJECT
public:
    explicit YOLOSerialReadData(QSerialPort *serial = nullptr,QObject *parent = nullptr);

    QSerialPort *m_pserial;
    QString serialBuf;    //储存接收到的数据
    void YSRDworking();    //YOLO线程任务函数
private:

signals:
    void sigYOLODataDisplay(QString serialBuf);
};


#endif // YOLOSERIALREADDATA_H
