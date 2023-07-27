#ifndef SERIALDATAANALYZE_H
#define SERIALDATAANALYZE_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>

class SerialDataAnalyze : public QObject
{
    Q_OBJECT
public:
    explicit SerialDataAnalyze(QObject *parent = nullptr);

    void SDAworking(QStringList ProcessedData);
private:
    QStringList m_ProcessedData;    //临时存放数据

signals:
    void sigAngleDataAnalyze(QStringList ProcessedData);
    void sigDepthDataAnalyze(QStringList ProcessedData);
    void sigThrusterDataAnalyze(QStringList ProcessedData,int ThrusterNum);
};

#endif // SERIALDATAANALYZE_H
