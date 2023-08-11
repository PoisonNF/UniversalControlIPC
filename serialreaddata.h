#ifndef SERIALREADDATA_H
#define SERIALREADDATA_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "./log/log.h"

class SerialReadData : public QObject
{
    Q_OBJECT
public:
    explicit SerialReadData(QSerialPort *serial = nullptr,QObject *parent = nullptr);

    QSerialPort *m_pserial;
    std::string serialBuf;  //储存接收到的数据
    void SRDworking();    //线程任务函数
private:

signals:
    void sigLogDataDisplay(std::string serialBuf);
    void sigDataSort(std::string serialBuf);
};

#endif // SERIALREADDATA_H
