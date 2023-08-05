#include "serialdataanalyze.h"

SerialDataAnalyze::SerialDataAnalyze(QObject *parent)
    : QObject{parent}
{
    Q_UNUSED(parent);
}

//数据分析线程任务函数
void SerialDataAnalyze::SDAworking(QString serialBuf)
{
    QStringList m_ProcessedData = serialBuf.split(u' ');
    //qDebug() << m_ProcessedData;
    //LOG_INFO((char*)"开始分析数据%s",QString(serialBuf).toStdString().c_str());

    //JY901S的数据
    if(m_ProcessedData.count() > 0 && m_ProcessedData.at(0) == "J")
    {
        //姿态角数据
        if(m_ProcessedData.at(1) == "Angle")
        {
            emit sigAngleDataAnalyze(m_ProcessedData);
            //qDebug() << "AngleData";
            //LOG_INFO((char*)"结果：姿态数据");
        }
    }
    //推进器的数据
    else if(m_ProcessedData.at(0) == "T")
    {
        //LOG_INFO((char*)"结果：推进器数据");
        emit sigThrusterDataAnalyze(m_ProcessedData);

    }
    //深度数据
    else if(m_ProcessedData.at(0) == "M")
    {
        //LOG_INFO((char*)"结果：深度数据");
        emit sigDepthDataAnalyze(m_ProcessedData);
    }
    //如果所有的都不是，就是下位机发来的反馈信息
    else
    {
        emit sigOtherDataDisplay(serialBuf);
    }
}


