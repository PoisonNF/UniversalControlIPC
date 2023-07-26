#ifndef DATADISPLAYWIDGET_H
#define DATADISPLAYWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QSplitter>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QDebug>
#include <QPainter>
#include <QLineEdit>
#include <QPaintEvent>
#include <QTimer>
#include <QFont>
#include <QtGui/QScreen>
#include <QGuiApplication>
#include <QPlainTextEdit>

#include "./frame/slidepage.h"
#include "./frame/customWidgets.h"

#if (QT_VERSION > QT_VERSION_CHECK(6,3,0))
#include <QFileDialog>
#endif

class DataDisplayWidget : public QWidget
{
    Q_OBJECT
private:
    enum MODE{TEST_MV_CTR, TEST_BAL_CTR, TEST_ACT_CTR, TRAVEL_MV_CTR, TRAVEL_BAL_CTR, TRAVEL_ACT_CTR};

    SlidePage *settings;
    Qt3DCore::QEntity *mRootEntity;
    Qt3DCore::QEntity *lightEntity;
    Qt3DRender::QPointLight *light;

    QVBoxLayout *mainLayout;
    QSplitter *splitter_1;
    QSplitter *splitter_2;
    QSplitter *splitter_3;
    QSplitter *splitter_4;

    QLabel *JY901STitle;
    QLabel *RM3100Title;
    QLabel *logTitle;
    QLabel *infoTitle;

    QLabel *JY901SDataAcc = new QLabel(this);    //显示JY901S中加速度数据
    QLabel *JY901SDataGyro = new QLabel(this);   //显示JY901S中角速度数据
    QLabel *JY901SDataAngle = new QLabel(this);  //显示JY901S中欧拉角数据
    QLabel *JY901SDataMag = new QLabel(this);    //显示JY901S中磁场数据

    QLabel *RM3100DataNO1 = new QLabel(this);   //RM3100 1号数据
    QLabel *RM3100DataNO2 = new QLabel(this);   //RM3100 2号数据
    QLabel *RM3100DataNO3 = new QLabel(this);   //RM3100 3号数据
    QLabel *RM3100DataNO4 = new QLabel(this);   //RM3100 4号数据

    QLabel *ThrusterData1 = new QLabel(this);   //1号推进器的数据
    QLabel *ThrusterData2 = new QLabel(this);   //2号推进器的数据
    QLabel *ThrusterData3 = new QLabel(this);   //3号推进器的数据
    QLabel *ThrusterData4 = new QLabel(this);   //4号推进器的数据

    QLabel *ServoData1 = new QLabel(this);  //1号舵机的数据
    QLabel *ServoData2 = new QLabel(this);  //2号舵机的数据
    QLabel *ServoData3 = new QLabel(this);  //3号舵机的数据
    QLabel *ServoData4 = new QLabel(this);  //4号舵机的数据

    QString modeName;
    QString ctrDescrip;

    QWidget *JY901SWidget = nullptr;
    QWidget *RM3100Widget = nullptr;
    QWidget *PropulsionSysWidget = nullptr;
    QWidget *logWidget = nullptr;
    QWidget *infoWidget = nullptr;

    QPlainTextEdit *logPTE;

    void Init();
    void SaveToFile(const QString &path);
    void ModeSelectPage(int r);  //模式选择页
    void DataSortConnect(); //数据分类链接函数

public:
    explicit DataDisplayWidget(int radius, QWidget *parent = nullptr);
    DataDisplayWidget(QTextStream &ts, int radius, QWidget *parent = nullptr);

    SlidePage *settingPage(){return settings;}
    Qt3DRender::QPointLight *Light(){return light;}

    Qt3DExtras::Qt3DWindow *view;
    QWidget *container = nullptr;

    textButton *SendBTN;    //串口发送按钮
    textButton *ClearBTN;   //串口清空按钮

    textInputItem *logTII;  //串口发送栏

signals:
    void SendDataSignal();
    void StartDataSort(QStringList ProcessedData);
    void AttitudeChange(QString pitch, QString yaw, QString roll);

public slots:
    void DataDisplayPTE(QString serialBuf);

private slots:
    void JY901SDataSort(QStringList ProcessedData);  //JY901S数据分拣槽函数
    void RM3100DataSort(QStringList ProcessedData);  //RM3100数据分拣槽函数
    void PropulsionSysDataSort(QStringList ProcessedData);  //动力系统数据分拣槽函数

};

#endif // DATADISPLAYWIDGET_H
