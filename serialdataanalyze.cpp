#include "serialdataanalyze.h"

SerialDataAnalyze::SerialDataAnalyze(QObject *parent)
    : QObject{parent}
{
    Q_UNUSED(parent);
}

//数据分析线程任务函数
void SerialDataAnalyze::SDAworking(std::string serialBuf)
{
    std::istringstream iss(serialBuf);
    std::string block;

    //清空容器
    m_ProcessedData.clear();

    //字符串分割
    while (std::getline(iss, block, ' ')) {
        m_ProcessedData.push_back(block);
    }

    //JY901S的数据
    if(m_ProcessedData[0] == "J")
    {
        if(m_ProcessedData.at(1) == "Angle")
        {
            emit sigAngleDataAnalyze(m_ProcessedData);
            //qDebug() << "AngleData";
            //LOG_INFO((char*)"结果：姿态数据");
        }
    }
    //推进器的数据
    else if(m_ProcessedData[0] == "T")
    {
        emit sigThrusterDataAnalyze(m_ProcessedData);
    }
    //深度数据
    else if(m_ProcessedData.at(0) == "M")
    {
        emit sigDepthDataAnalyze(m_ProcessedData);
    }
    //如果所有的都不是，就是下位机发来的反馈信息
    else
    {
        emit sigOtherDataDisplay(serialBuf);
    }
}


