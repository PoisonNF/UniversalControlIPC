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

#include "slidepage.h"
#include "customWidgets.h"
#include "QJoysticks.h"

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

    QVBoxLayout *mainLayout;
    QSplitter *splitter_1;
    QSplitter *splitter_2;
    QSplitter *splitter_3;
    QSplitter *splitter_4;

    QLabel *PIDTitle;
    QLabel *ControlTitle;
    QLabel *logTitle;
    QLabel *infoTitle;

    QLabel *CurrPID_P = new QLabel("P:              0",this);   //当前P值显示标签
    QLabel *CurrPID_I = new QLabel("I:              0",this);   //当前I值显示标签
    QLabel *CurrPID_D = new QLabel("D:              0",this);   //当前D值显示标签

    customIcon *WIcon = nullptr;    //按键W图标
    customIcon *AIcon = nullptr;    //按键A图标
    customIcon *SIcon = nullptr;    //按键S图标
    customIcon *DIcon = nullptr;    //按键D图标
    customIcon *QIcon = nullptr;    //按键Q图标
    customIcon *EIcon = nullptr;    //按键E图标

    QLabel *ControlData = new QLabel(this);   //当前控制情况标签
    QLabel *ControlState = new QLabel(this);    //当前控制状态标签

    QLabel *ThrusterData1 = new QLabel(this);   //1号推进器的数据
    QLabel *ThrusterData2 = new QLabel(this);   //2号推进器的数据
    QLabel *ThrusterData3 = new QLabel(this);   //3号推进器的数据
    QLabel *ThrusterData4 = new QLabel(this);   //4号推进器的数据

    QLabel *ServoData1 = new QLabel(this);  //1号舵机的数据
    QLabel *ServoData2 = new QLabel(this);  //2号舵机的数据
    QLabel *ServoData3 = new QLabel(this);  //3号舵机的数据
    QLabel *ServoData4 = new QLabel(this);  //4号舵机的数据

    QLabel *AttitudeDataInfo = new QLabel(this);    //Info姿态数据
    QLabel *DepthDataInfo = new QLabel(this);       //Info深度数据
    QLabel *JoystickAxisDataInfo = new QLabel(this);        //Info手柄坐标数据

    QString modeName;
    QString ctrDescrip;

    QWidget *PIDWidget = nullptr;
    QWidget *ControlWidget = nullptr;
    QWidget *PropulsionSysWidget = nullptr;
    QWidget *logWidget = nullptr;
    QWidget *infoWidget = nullptr;

    QPlainTextEdit *logPTE; //串口数据显示框

    Qt::Key CurrentKey; //当前按下按键的键值

    QJoysticks* m_joystick;

    void Init();
    void SaveToFile(const QString &path);
    void ModeSelectPage(int r); //模式选择页
    void JoysticksInit();       //手柄初始化
    void DataSortConnect();     //数据分类链接函数

    void keyPressEvent(QKeyEvent *event);   //重写键盘按下事件

public:
    explicit MotionControlWidget(int radius, QWidget *parent = nullptr);

    SlidePage *settingPage(){return settings;}
    Qt3DRender::QPointLight *Light(){return light;}

    Qt3DExtras::Qt3DWindow *view;
    QWidget *container = nullptr;

    textButton *SendBTN;        //串口发送按钮
    textButton *ClearBTN;       //串口清空按钮
    textInputItem *logTII;      //串口发送栏

    textInputItem *PID_P_TII;   //PID P值输入框
    textInputItem *PID_I_TII;   //PID I值输入框
    textInputItem *PID_D_TII;   //PID D值输入框

    textButton *SetPIDBTN;  //设置PID值按钮

    typedef struct
    {
        double LeftX;
        double LastLeftX;
        double LeftY;
        double LastLeftY;
    }Axis;

signals:
    void SendDataSignal();  //发送数据信号往主窗口
    void StartDataSort(QStringList ProcessedData);
    void AttitudeChange(QString pitch, QString yaw, QString roll);
    void SetPIDSignal();   //设置PID信号往主窗口
    void SendControlSignal(QString str);  //发送控制信号往主窗口
    void axisChange(MotionControlWidget::Axis axis);
    void axisSend(QString str);

public slots:
    void DataDisplayPTE(QString serialBuf);

private slots:
    void PropulsionSysDataSort(QStringList ProcessedData);  //动力系统数据分拣槽函数
    void AttitudeDataSort(QStringList ProcessedData);   //分拣JY901S中的姿态数据
    void DepthDataSort(QStringList ProcessedData);   //分拣深度数据

    void joysitck_axis(int js_index, int axis_index, qreal value);  //获取手柄摇杆信息
    void joysitck_button(int js_index, int button_index, bool pressed);//获取手柄按键信息
};

#endif // MONTIONCONTROLWIDGET_H
