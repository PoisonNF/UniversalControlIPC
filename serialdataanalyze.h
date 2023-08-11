#ifndef SERIALDATAANALYZE_H
#define SERIALDATAANALYZE_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "./Log/log.h"

class SerialDataAnalyze : public QObject
{
    Q_OBJECT
public:
    explicit SerialDataAnalyze(QObject *parent = nullptr);

    void SDAworking(std::string serialBuf);
private:
    std::vector<std::string> m_ProcessedData;    //临时存放数据

signals:
    void sigAngleDataAnalyze(std::vector<std::string> ProcessedData);
    void sigDepthDataAnalyze(std::vector<std::string> ProcessedData);
    void sigThrusterDataAnalyze(std::vector<std::string> ProcessedData);

    void sigOtherDataDisplay(std::string serialBuf);
};

#endif // SERIALDATAANALYZE_H
