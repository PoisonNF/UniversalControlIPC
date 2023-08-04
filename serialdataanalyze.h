#ifndef SERIALDATAANALYZE_H
#define SERIALDATAANALYZE_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include "./Log/log.h"

class SerialDataAnalyze : public QObject
{
    Q_OBJECT
public:
    explicit SerialDataAnalyze(QObject *parent = nullptr);

    void SDAworking(QString serialBuf);
private:
    QStringList m_ProcessedData;    //临时存放数据

signals:
    void sigAngleDataAnalyze(QStringList ProcessedData);
    void sigDepthDataAnalyze(QStringList ProcessedData);
    void sigThrusterDataAnalyze(QStringList ProcessedData);

    void sigOtherDataDisplay(QString serialBuf);
};

#endif // SERIALDATAANALYZE_H
