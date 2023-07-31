#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QMessageBox>
#include <QThread>
#include "./frame/slidedialog.h"
#include "datadisplaywidget.h"
#include "motioncontrolwidget.h"
#include "serialreaddata.h"
#include "serialdataanalyze.h"
#include "yoloserialreaddata.h"
#include "./log/log.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    int cornerRadius = 20;

    LOG *m_pLog;

    QWidget *border = nullptr;  //边界
    QWidget *defaultPage;       //主界面除标题之外
    QWidget *titleWidget;       //标题栏

    DataDisplayWidget *dataDisplayWidget = nullptr; //数据采集窗口对象
    MotionControlWidget *motionControlWidget = nullptr; //运动控制窗口对象

    QGraphicsDropShadowEffect *windowShadow;
    QColor mainBackGround = QColor(251, 251, 251);


    QLineEdit *Heading = nullptr;   //主界面主标题
    QLineEdit *Subtitle = nullptr;  //主界面副标题

    customIcon *settingsIcon = nullptr; //设置图标
    customIcon *layersIcon = nullptr;   //层图标

    bigIconButton *Data_display = nullptr;
    bigIconButton *Motion_control = nullptr;

    QVector<SlidePage*> pageList;   //储存页面的容器

    SlidePage *defaultSettingsPage = nullptr;
    SlidePage *curSettingsPage = nullptr;

    //换层页相关
    SlidePage *layersPage = nullptr;    //层页
    singleSelectGroup *modeSelGroup = nullptr;
    singleSelectGroup *subModeSelGroup = nullptr;

    //串口相关
    QSerialPort *serial;    //串口对象
    SlideDialog *serialDialog = nullptr; //串口配置滑入对话框
    textButton *openSerialBtn = nullptr;
    textButton *closeSerialBtn = nullptr;
    QComboBox *comPortCBox;
    QComboBox *baudrateCBox;
    QComboBox *dataBitsCBox;
    QComboBox *checkBitsCBox;
    QComboBox *stopBitsCBox;
    QComboBox *flowCtlCBox;

    SerialReadData *SRDwork = nullptr;
    SerialDataAnalyze *SDAwork = nullptr;

    //串口YOLO相关
    QSerialPort *serialYOLO;    //串口对象
    SlideDialog *serialYOLODialog = nullptr; //串口配置滑入对话框
    textButton *openSerialYOLOBtn = nullptr;
    textButton *closeSerialYOLOBtn = nullptr;
    QComboBox *YOLOcomPortCBox;
    QComboBox *YOLObaudrateCBox;
    QComboBox *YOLOdataBitsCBox;
    QComboBox *YOLOcheckBitsCBox;
    QComboBox *YOLOstopBitsCBox;
    QComboBox *YOLOflowCtlCBox;

    YOLOSerialReadData *YSRDwork = nullptr;

    //函数声明
    void Init();            //界面初始化
    void InitFrame();       //框架初始化
    void InitDefaultSettingsPage(); //默认设置界面初始化
    void InitDefaultPage(); //默认主界面初始化
    void InitLayersPage();  //换层页初始化
    void InitSerialPage();  //串口设置界面初始化
    void InitSerialYOLOPage();  //串口YOLO设置界面初始化
    void InitDataDisplayWidget();   //初始化数据显示窗口
    void InitMotionControlWidget(); //初始化运动控制窗口
    void InitLog();   //log初始化

    //模式枚举类型
    enum MODE{DATADISPLAY = 1, MOTIONCONTROl = 2};
    MODE Switch_Mode;

    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
           AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    bool mousePressed = false;
    int mouseState;
    QPoint lastPos;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event){
        mousePressed = false;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        if(event->globalPosition().y() < 2)
            controlWindowScale();
#else
        if(event->globalPos().y() < 2)
            controlWindowScale();
#endif
    }
    void resizeEvent(QResizeEvent *event);

    bool maximized = false;
    QRect lastGeometry;
    void controlWindowScale();

private slots:
    void ChangeDataDisplayWidget();  //切换到数据显示的窗口槽函数
    void ChangeMotionControlWidget();   //切换到运动控制串口槽函数
    void OpenSerialPort();  //打开串口
    void CloseSerialPort(); //关闭串口
    void OpenYOLOSerialPort();  //打开串口
    void CloseYOLOSerialPort(); //关闭串口

public slots:

signals:


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
