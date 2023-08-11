#include "serialreaddata.h"

SerialReadData::SerialReadData(QSerialPort *serial,QObject *parent)
    : QObject{parent}
{
    Q_UNUSED(parent);
    qDebug() << "构造serial" <<serial;
    m_pserial = serial;
    qDebug() << "串口复制" << m_pserial;
}

//读取数据线程任务函数
void SerialReadData::SRDworking()
{
    serialBuf = m_pserial->readLine().toStdString();

    qDebug() << QString::fromStdString(serialBuf);
    //LOG_INFO((char *)"串口收到数据%s",QString(serialBuf).toStdString().c_str());

    //发射信号给datadisplay窗口
    //emit sigLogDataDisplay(serialBuf);        //log显示数据太占资源了，导致显示落后于实际
    //发射信号给数据分拣线程
    emit sigDataSort(serialBuf);
}
