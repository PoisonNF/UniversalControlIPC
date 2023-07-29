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
    serialBuf = QString(m_pserial->readLine());
    if(!serialBuf.isEmpty())
    {
        qDebug() << serialBuf;
        LOG_INFO((char *)"串口收到数据%s",QString(serialBuf).toStdString().c_str());
        //发射信号给datadisplay窗口
        emit sigLogDataDisplay(serialBuf);
        //发射信号给数据分拣线程
        emit sigDataSort(serialBuf);
    }
}
