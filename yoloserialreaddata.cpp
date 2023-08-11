#include "yoloserialreaddata.h"

YOLOSerialReadData::YOLOSerialReadData(QSerialPort *serial, QObject *parent)
{
    Q_UNUSED(parent);

    qDebug() << "构造YOLOserial" <<serial;
    m_pserial = serial;
    qDebug() << "YOLO串口复制" << m_pserial;
}

void YOLOSerialReadData::YSRDworking()
{
    serialBuf = m_pserial->readAll();

    //qDebug() << serialBuf;
    //LOG_INFO((char *)"YOLO串口收到数据%s",QString(serialBuf).toStdString().c_str());

    //YOLO数据显示到YOLOlogLabel标签信号
    //emit sigYOLODataDisplay(serialBuf);
    //YOLO数据由串口发送到下位机信号
    emit sigYOLODataSend(serialBuf);
}
