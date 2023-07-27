#include "serialdataanalyze.h"

SerialDataAnalyze::SerialDataAnalyze(QObject *parent)
    : QObject{parent}
{
    Q_UNUSED(parent);
}

//数据分析线程任务函数
void SerialDataAnalyze::SDAworking(QStringList ProcessedData)
{
    m_ProcessedData = ProcessedData;    //暂存数据

    //JY901S的数据
    if(m_ProcessedData.count() > 0 && m_ProcessedData.at(0) == "J")
    {
        //姿态角数据
        if(m_ProcessedData.at(1) == "Angle")
        {
            emit sigAngleDataAnalyze(m_ProcessedData);
            qDebug() << "AngleData";
        }
    }

    //推进器的数据
    if(m_ProcessedData.at(0) == "T")
    {
        if(m_ProcessedData.at(1) == "1")
        {
            emit sigThrusterDataAnalyze(m_ProcessedData,1);
        }
        else if(m_ProcessedData.at(1) == "2")
        {
            emit sigThrusterDataAnalyze(m_ProcessedData,2);
        }
        else if(m_ProcessedData.at(1) == "3")
        {
            emit sigThrusterDataAnalyze(m_ProcessedData,3);
        }
        else if(m_ProcessedData.at(1) == "4")
        {
            emit sigThrusterDataAnalyze(m_ProcessedData,4);
        }
    }
    //深度数据
}


