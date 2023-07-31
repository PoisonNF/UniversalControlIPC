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
    serialBuf = QString(m_pserial->readLine());
    if(!serialBuf.isEmpty())
    {
        qDebug() << serialBuf;
        LOG_INFO((char *)"YOLO串口收到数据%s",QString(serialBuf).toStdString().c_str());

        //往motioncontrolwidget发送显示信号
        emit sigYOLODataDisplay(serialBuf);
    }
}
