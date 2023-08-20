#ifndef MONTIONCONTROLWIDGET_H
#define MONTIONCONTROLWIDGET_H

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
#include <QMessageBox>
#include <QComboBox>
#include <QListView>
#include <QTextCursor>

#include "./frame/slidepage.h"
#include "./frame/customWidgets.h"
#include "QJoysticks.h"
#include "joysticks.h"
#include "./Log/log.h"

#if (QT_VERSION > QT_VERSION_CHECK(6,3,0))
#include <QFileDialog>
#endif

class MotionControlWidget : public QWidget
{
    Q_OBJECT
private:
    enum MODE{TEST_MV_CTR, TEST_BAL_CTR, TEST_ACT_CTR, TRAVEL_MV_CTR, TRAVEL_BAL_CTR, TRAVEL_ACT_CTR};

    SlidePage *settings;
    Qt3DCore::QEntity *mRootEntity;
    Qt3DCore::QEntity *lightEntity;
    Qt3DRender::QPointLight *light;

    QSizePolicy sizepolicy;
    QFont TitleFont = QFont("Corbel", 20);

    QVBoxLayout *mainLayout;
    QSplitter *splitter_1;
    QSplitter *splitter_2;
    QSplitter *splitter_3;
    QSplitter *splitter_4;

    //PID
    QLabel *PIDTitle;
    QLabel *ControlTitle;
    QLabel *logTitle;
    QLabel *infoTitle;

    QComboBox *PIDComboBox = new QComboBox(this);

public:
    typedef struct
    {
        double P;
        double I;
        double D;
    }CurrPIDstore;  //存储不同PID环中的PID数值

    typedef enum
    {
        DepthPID,
        YawPID,
        AngleLoopPID,
        PositionLoopPID,
        LinePatrolPID
    }PIDtype;

private:
    CurrPIDstore DepthPIDstore; //储存深度环PID
    CurrPIDstore YawPIDstore;   //储存艏向环PID
    CurrPIDstore AngleLoopPIDstore;   //储存角度环PID
    CurrPIDstore PositionLoopPIDstore;   //储存位置环PID
    CurrPIDstore LinePatrolPIDstore; //储存巡线环PID

    QLabel *CurrPID_P = new QLabel("P:              0",this);   //当前P值显示标签
    QLabel *CurrPID_I = new QLabel("I:              0",this);   //当前I值显示标签
    QLabel *CurrPID_D = new QLabel("D:              0",this);   //当前D值显示标签

    //control
    QLabel *ControlData = new QLabel(this);    //当前控制数据标签
    QLabel *ControlState = new QLabel(this);    //当前控制状态标签

    //PropulsionSys
    QLabel *ThrusterData1 = new QLabel(this);   //1号推进器的数据
    QLabel *ThrusterData2 = new QLabel(this);   //2号推进器的数据
    QLabel *ThrusterData3 = new QLabel(this);   //3号推进器的数据
    QLabel *ThrusterData4 = new QLabel(this);   //4号推进器的数据

    QLabel *ServoData1 = new QLabel(this);  //1号舵机的数据
    QLabel *ServoData2 = new QLabel(this);  //2号舵机的数据
    QLabel *ServoData3 = new QLabel(this);  //3号舵机的数据
    QLabel *ServoData4 = new QLabel(this);  //4号舵机的数据

    //log
    QTextCursor TextCursor;
    QPlainTextEdit *logPTE; //串口数据显示框

    //YOLO
    QLabel *YOLOlogLabel; //YOLO串口数据显示标签

    //Info
    QLabel *AttitudeDataInfo = new QLabel(this);    //Info姿态数据
    QLabel *DepthDataInfo = new QLabel(this);       //Info深度数据
    QLabel *JoystickAxisDataInfo = new QLabel(this);        //Info手柄坐标数据
    QLabel *JoystickButtonDataInfo = new QLabel(this);        //Info手柄按键数据

    QString modeName;
    QString ctrDescrip;

    QWidget *PIDWidget = nullptr;
    QWidget *ControlWidget = nullptr;
    QWidget *PropulsionSysWidget = nullptr;
    QWidget *logWidget = nullptr;
    QWidget *YOLOlogWidget = nullptr;
    QWidget *infoWidget = nullptr;

    Qt::Key CurrentKey; //当前按下按键的键值

    QJoysticks* m_joystick;
    Joysticks *JSwork;

    void Init();
    void InitJoysticks();           //初始化手柄
    void InitLayout();              //初始化总体布局
    void InitPIDWidget();           //初始化PID窗口
    void InitControlWidget();       //初始化控制窗口
    void InitPropulsionSysWidget(); //初始化动力系统窗口
    void InitLogWidget();           //初始化串口log窗口
    void InitYOLOLogWidget();       //初始化YOLO串口log窗口
    void InitInfoWidget();          //初始化信息窗口


    void SaveToFile(const QString &path);
    void ModeSelectPage(int r); //模式选择页

    void keyPressEvent(QKeyEvent *event);   //重写键盘按下事件

public:
    explicit MotionControlWidget(int radius, QWidget *parent = nullptr);

    SlidePage *settingPage(){return settings;}

    textInputItem *PID_P_TII;   //PID P值输入框
    textInputItem *PID_I_TII;   //PID I值输入框
    textInputItem *PID_D_TII;   //PID D值输入框

    textButton *SetPIDBTN;  //设置PID值按钮

    textButton *SendBTN;        //串口发送按钮
    textButton *ClearBTN;       //串口清空按钮
    textInputItem *logTII;      //串口发送栏

signals:
    void sigLogDataSend();  //发送数据信号往主窗口
    //void AttitudeChange(QString pitch, QString yaw, QString roll);
    void sigSendPIDSignal(MotionControlWidget::CurrPIDstore PIDstore,MotionControlWidget::PIDtype PIDtype);   //设置PID信号往主窗口
    void sigSendControlSignal(QString str);  //发送控制信号往主窗口
    void sigJoyAxisSend(QString str);     //发送手柄坐标数据往主窗口
    void sigJoyButtonSend(QString str);   //发送手柄按键数据往主窗口

public slots:
    void slotLogDataDisplay(std::string serialBuf);
    void slotYOLOLogDataDisplay(QString serialBuf);
    void slotAngleDataDisplay(std::vector<std::string> ProcessedData);
    void slotDepthDataDisplay(std::vector<std::string> ProcessedData);
    void slotThrusterDataDisplay(std::vector<std::string> ProcessedData);

private slots:
};

#endif // MONTIONCONTROLWIDGET_H
