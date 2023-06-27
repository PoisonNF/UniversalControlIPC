#include "MotionControlWidget.h"

double LeftX;
double LastLeftX;
double LeftY;
double LastLeftY;

MotionControlWidget::MotionControlWidget(int radius, QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout -> setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
    splitter_1 = new QSplitter(this);
    splitter_1 -> setContentsMargins(0, 0, 0, 0);
    mainLayout -> addWidget(splitter_1);

    this->setFocusPolicy(Qt::ClickFocus);

    //初始化模式选择界面
    ModeSelectPage(radius);

    //定时器延时10ms后，初始化运动控制界面
    QTimer *delay = new QTimer(this);
    connect(delay, &QTimer::timeout, this, [=](){
        Init();
        JoysticksInit();
        delay->deleteLater();   //超时后释放内存
    });
    delay->setSingleShot(true);
    delay->start(10);

    //定时器，用于监听手柄数据
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,this,[](){
        if(LeftX != 0)
        {
            //如果上次也是500，不发送
            if(LastLeftX != 500)
            {
                qDebug() << "X" << LeftX;
            }
            LastLeftX = LeftX;  //寄存数据
        }

        if(LeftY != 0)
        {
            //如果上次也是500，不发送
            if(LastLeftY != 500)
            {
                qDebug() << "Y" << LeftY;
            }
            LastLeftY = LeftY;  //寄存数据
        }
    });

    // 启动定时器和事件循环
    timer->start(500);
}

//在数据显示界面的设置栏this->parentWidget());
void MotionControlWidget::ModeSelectPage(int radius){
    /* create settings page */
    modeName = "测试模式";
    ctrDescrip = "运动控制";
    settings = new SlidePage(radius, "默认设置", this->parentWidget());
    QWidget *whiteSpace = new QWidget(this);
    whiteSpace->setFixedHeight(30);
    textInputItem *rename = new textInputItem("当前模式", this);
    rename->setValue(modeName);
    textInputItem *redesc = new textInputItem("当前控制", this);
    redesc->setValue(ctrDescrip);
    QWidget *whiteSpace_2 = new QWidget(this);
    whiteSpace_2->setFixedHeight(30);
    textButton *saveBtn = new textButton("Save to file", this);
//    connect(saveBtn, &textButton::clicked, this, [=](){
//        QString savePath = QFileDialog::getSaveFileName(this, tr("Save file"), " ", tr("TXT file(*.txt)"));
//        if(!savePath.isEmpty())
//            SaveToFile(savePath);
//    });
    settings->AddContent(saveBtn);
    settings->AddContent(whiteSpace_2);
    settings->AddContent(whiteSpace);
    settings->AddContent(redesc);
    settings->AddContent(rename);
    settings->show();
}

void MotionControlWidget::JoysticksInit()
{
    m_joystick = QJoysticks::getInstance();

    connect(m_joystick, SIGNAL(axisChanged(int, int, qreal)), this, SLOT(joysitck_axis(int, int, qreal)));
    //connect(m_joystick, &QJoysticks::axisEvent, this, &MainWindow::event_axis);

    connect(m_joystick, SIGNAL(buttonChanged(int, int, bool)), this, SLOT(joysitck_button(int, int, bool)));
    //connect(m_joystick, &QJoysticks::buttonEvent, this, &MainWindow::event_button);

    m_joystick->setVirtualJoystickEnabled (true);    //启用虚拟手柄
    m_joystick->setVirtualJoystickRange (1);    //设置虚拟手柄摇杆范围[-1,1]
    QStringList js_names = m_joystick->deviceNames();    //添加手柄
    qDebug() << js_names;
}

void MotionControlWidget::Init(){
//layout

    //1为最大的布局器，2，3，4分别为垂直布局器，对应左中右
    splitter_1 = new QSplitter(this);
    splitter_1->setOrientation(Qt::Horizontal);
    splitter_2 = new QSplitter(splitter_1);
    splitter_2->setOrientation(Qt::Vertical);
    splitter_3 = new QSplitter(splitter_1);
    splitter_3->setOrientation(Qt::Vertical);
    splitter_4 = new QSplitter(splitter_1);
    splitter_4->setOrientation(Qt::Vertical);

    mainLayout->addWidget(splitter_1);
    splitter_1->addWidget(splitter_2);
    splitter_1->addWidget(splitter_3);
    splitter_1->addWidget(splitter_4);

    //设置布局策略
    QSizePolicy sizepolicy = QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);

    splitter_1->setSizePolicy(sizepolicy);
    splitter_2->setSizePolicy(sizepolicy);
    splitter_3->setSizePolicy(sizepolicy);
    splitter_4->setSizePolicy(sizepolicy);

//PID参数设置与发送
    QFont titleFont = QFont("Corbel", 20);
    PIDTitle = new QLabel(this);
    PIDTitle->setText("PID");
    PIDTitle->setFont(titleFont);
    PIDTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    PIDTitle->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *PIDSplitter = new QWidget(this);
    PIDSplitter->setFixedSize(30, 6);
    PIDSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //设置字体和大小
    QFont PIDDataFont = QFont("Corbel", 15);

    //当前PID值标签
    QLabel *CurrentPID = new QLabel("Current PID",this);
    CurrentPID->setMinimumHeight(20);
    CurrentPID->setFont(PIDDataFont);

    CurrPID_P->setMinimumHeight(20);
    CurrPID_P->setFont(PIDDataFont);
    CurrPID_I->setMinimumHeight(20);
    CurrPID_I->setFont(PIDDataFont);
    CurrPID_D->setMinimumHeight(20);
    CurrPID_D->setFont(PIDDataFont);

    //设置PID值标签
    QLabel *SetPID = new QLabel("Set PID",this);
    SetPID->setMinimumHeight(20);
    SetPID->setFont(PIDDataFont);
    //PID值的输入框
    PID_P_TII = new textInputItem("Set P:",this);
    PID_I_TII = new textInputItem("Set I:",this);
    PID_D_TII = new textInputItem("Set D:",this);

    PID_P_TII->setMaximumWidth(420);
    PID_I_TII->setMaximumWidth(420);
    PID_D_TII->setMaximumWidth(420);

    //使用验证器来避免输入字符
    /* `[0-9]*` 表示匹配 0 到多个数字，其中方括号 `[]` 中的字符为可以匹配的字符集合，`0-9` 表示匹配 0 到 9 的任意数字，
    * `*` 表示匹配 0 到多个该字符集合中的字符。
    `(\\.[0-9]*)?` 表示一个可选的小数部分，其中的第一个括号 `()` 表示这是一个组，
    并且 `\\.` 表示匹配小数点，因为小数点是一个特殊的字符，需要用反斜线转义来表示；`[0-9]*` 同上，表示匹配 0 到多个数字，`?` 表示匹配 0 到 1 个该字符。
    整个正则表达式最外层的方括号 `[]` 表示匹配一个字符集合，其中的 `*` 表示匹配 0 到多个字符。
    因此整个正则表达式表示匹配 0 到多个数字，加上一个可选的小数部分。 */

    QRegularExpression regExp("[0-9]*(\\.[0-9]*)?"); // 创建正则表达式，表示允许带有小数点的数，放止输入字符
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this); // 创建验证器
    PID_P_TII->setValidator(validator); // 设置验证器
    PID_I_TII->setValidator(validator); // 设置验证器
    PID_D_TII->setValidator(validator); // 设置验证器

    //设置PID值的按钮
    textButton *SetPIDBTN = new textButton("SetPID",this);
    SetPIDBTN->setMaximumWidth(420);

    //连接函数，按下按钮时，发送信号给主窗口，将PID值从串口发出。同时设置当前PID值的标签
    connect(SetPIDBTN,&textButton::clicked,this,[=](){

        emit SetPIDSignal();    //发射信号

        //保存PID值为现PID值
        CurrPID_P->setText(QString("P:     %1").arg(PID_P_TII->value()));
        CurrPID_I->setText(QString("I:     %1").arg(PID_I_TII->value()));
        CurrPID_D->setText(QString("D:     %1").arg(PID_D_TII->value()));
    });

    QWidget *PIDDataWidget = new QWidget(this);
    PIDDataWidget->setSizePolicy(sizepolicy);
    //设置最小大小
    PIDDataWidget->setMinimumSize(450,300);
    QVBoxLayout *PIDDataLayout = new QVBoxLayout(this);
    PIDDataWidget->setLayout(PIDDataLayout);
    PIDDataLayout->setContentsMargins(0, 0, 0, 0);
    PIDDataLayout->setAlignment(Qt::AlignTop);
    PIDDataLayout->addWidget(CurrentPID);
    PIDDataLayout->addWidget(CurrPID_P);
    PIDDataLayout->addWidget(CurrPID_I);
    PIDDataLayout->addWidget(CurrPID_D);
    PIDDataLayout->addWidget(SetPID);
    PIDDataLayout->addWidget(PID_P_TII);
    PIDDataLayout->addWidget(PID_I_TII);
    PIDDataLayout->addWidget(PID_D_TII);
    PIDDataLayout->addWidget(SetPIDBTN);

    //PIDWidget将上面所有控件收入
    PIDWidget = new QWidget(this);
    PIDWidget->setSizePolicy(sizepolicy);
    QVBoxLayout *PIDLayout = new QVBoxLayout(this);
    PIDWidget->setLayout(PIDLayout);
    PIDLayout->setContentsMargins(0, 0, 0, 0);
    PIDLayout->setAlignment(Qt::AlignTop);
    PIDLayout->addWidget(PIDTitle);
    PIDLayout->addWidget(PIDSplitter);
    PIDLayout->addWidget(PIDDataWidget);

    splitter_2->addWidget(PIDWidget);

//按键显示和读取，或者使用鼠标点击，发送动作命令
    ControlTitle = new QLabel(this);
    ControlTitle->setText("Control");
    ControlTitle->setFont(titleFont);
    ControlTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ControlTitle->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *ControlSplitter = new QWidget(this);
    ControlSplitter->setFixedSize(30, 6);
    ControlSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    /* customIcon构造函数有问题，不能初始化radius和iconHint */
    /* 六个键盘按键的初始化 */
    WIcon = new customIcon(":/icons/icons/W.svg", "W", 0, this);
    WIcon->setMinimumSize(QSize(70,70));
    AIcon = new customIcon(":/icons/icons/A.svg", "A", 0, this);
    AIcon->setMinimumSize(QSize(70,70));
    SIcon = new customIcon(":/icons/icons/S.svg", "S", 0, this);
    SIcon->setMinimumSize(QSize(70,70));
    DIcon = new customIcon(":/icons/icons/D.svg", "D", 0, this);
    DIcon->setMinimumSize(QSize(70,70));
    QIcon = new customIcon(":/icons/icons/Q.svg", "Q", 0, this);
    QIcon->setMinimumSize(QSize(70,70));
    EIcon = new customIcon(":/icons/icons/E.svg", "E", 0, this);
    EIcon->setMinimumSize(QSize(70,70));

    //鼠标点击图标也可以发送信号
    connect(WIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key W Press");
        ControlState->setText("Forward");
        emit SendControlSignal("W");
    });
    connect(AIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key A Press");
        ControlState->setText("Left");
        emit SendControlSignal("A");
    });
    connect(SIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key S Press");
        ControlState->setText("Draw back");
        emit SendControlSignal("S");
    });
    connect(DIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key D Press");
        ControlState->setText("Right");
        emit SendControlSignal("D");
    });
    connect(QIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key Q Press");
        ControlState->setText("Float");
        emit SendControlSignal("Q");
    });
    connect(EIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key E Press");
        ControlState->setText("Sink");
        emit SendControlSignal("E");
    });

    //第一排按键水平布局
    QWidget *Key1Widget = new QWidget(this);
    Key1Widget->setSizePolicy(sizepolicy);
    QHBoxLayout *Key1Layout = new QHBoxLayout(this);
    Key1Widget->setMaximumSize(250,70);
    Key1Widget->setLayout(Key1Layout);
    Key1Layout->setContentsMargins(0, 0, 0, 0);
    Key1Layout->setAlignment(Qt::AlignCenter);
    Key1Layout->addWidget(QIcon);
    Key1Layout->addWidget(WIcon);
    Key1Layout->addWidget(EIcon);

    QWidget *Key2Widget = new QWidget(this);
    Key2Widget->setSizePolicy(sizepolicy);
    QHBoxLayout *Key2Layout = new QHBoxLayout(this);
    Key2Widget->setMaximumSize(250,70);
    Key2Widget->setLayout(Key2Layout);
    Key2Layout->setContentsMargins(0, 0, 0, 0);
    Key2Layout->setAlignment(Qt::AlignCenter);
    Key2Layout->addWidget(AIcon);
    Key2Layout->addWidget(SIcon);
    Key2Layout->addWidget(DIcon);

    //显示按键数据的标签
    ControlData->setMaximumHeight(25);
    ControlData->setFont(QFont("Corbel", 15));

    //显示产生状态的标签
    ControlState->setMaximumHeight(25);
    ControlState->setFont(QFont("Corbel", 15));

    QWidget *ControlDataWidget = new QWidget(this);
    ControlDataWidget->setSizePolicy(sizepolicy);
    ControlDataWidget->setMinimumSize(450,300);
    QVBoxLayout *ControlDataLayout = new QVBoxLayout(this);
    ControlDataWidget->setLayout(ControlDataLayout);
    ControlDataLayout->setContentsMargins(0, 0, 0, 0);
    ControlDataLayout->setAlignment(Qt::AlignCenter);
    ControlDataLayout->addWidget(Key1Widget);
    ControlDataLayout->addWidget(Key2Widget);
    ControlDataLayout->addWidget(ControlData);
    ControlDataLayout->addWidget(ControlState);


    ControlWidget = new QWidget(this);
    ControlWidget->setSizePolicy(sizepolicy);
    QVBoxLayout *ControlLayout = new QVBoxLayout(this);
    ControlWidget->setLayout(ControlLayout);
    ControlLayout->setContentsMargins(0, 0, 0, 0);
    ControlLayout->setAlignment(Qt::AlignTop);
    ControlLayout->addWidget(ControlTitle);
    ControlLayout->addWidget(ControlSplitter);
    ControlLayout->addWidget(ControlDataWidget);

    splitter_2->addWidget(ControlWidget);

//动力系统
    QLabel *PropulsionSysLabel = new QLabel(this);
    PropulsionSysLabel->setText("Propulsion System");
    PropulsionSysLabel->setFont(titleFont);
    PropulsionSysLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    PropulsionSysLabel->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *PropulsionSysSplitter = new QWidget(this);
    PropulsionSysSplitter->setFixedSize(30, 6);
    PropulsionSysSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //推进器名称标签和数据标签设置
    QLabel *Thruster1 = new QLabel("Thruster1",this);
    QLabel *Thruster2 = new QLabel("Thruster2",this);
    QLabel *Thruster3 = new QLabel("Thruster3",this);
    QLabel *Thruster4 = new QLabel("Thruster4",this);

    //设置字体和大小
    QFont ThrusterDataFont = QFont("Corbel", 15);

    Thruster1->setMinimumHeight(25);
    Thruster1->setFont(ThrusterDataFont);
    Thruster2->setMinimumHeight(25);
    Thruster2->setFont(ThrusterDataFont);
    Thruster3->setMinimumHeight(25);
    Thruster3->setFont(ThrusterDataFont);
    Thruster4->setMinimumHeight(25);
    Thruster4->setFont(ThrusterDataFont);

    ThrusterData1->setMinimumSize(100,20);
    ThrusterData1->setFont(ThrusterDataFont);
    ThrusterData2->setMinimumSize(100,20);
    ThrusterData2->setFont(ThrusterDataFont);
    ThrusterData3->setMinimumSize(100,20);
    ThrusterData3->setFont(ThrusterDataFont);
    ThrusterData4->setMinimumSize(100,20);
    ThrusterData4->setFont(ThrusterDataFont);

    //舵机名称标签和数据标签设置
//    QLabel *Servo1 = new QLabel("Servo1",this);
//    QLabel *Servo2 = new QLabel("Servo2",this);
//    QLabel *Servo3 = new QLabel("Servo3",this);
//    QLabel *Servo4 = new QLabel("Servo4",this);

//    //设置字体和大小
//    QFont ServoDataFont = QFont("Corbel", 15);

//    Servo1->setMinimumHeight(25);
//    Servo1->setFont(ServoDataFont);
//    Servo2->setMinimumHeight(25);
//    Servo2->setFont(ServoDataFont);
//    Servo3->setMinimumHeight(25);
//    Servo3->setFont(ServoDataFont);
//    Servo4->setMinimumHeight(25);
//    Servo4->setFont(ServoDataFont);

//    ServoData1->setMinimumSize(100,20);
//    ServoData1->setFont(ServoDataFont);
//    ServoData2->setMinimumSize(100,20);
//    ServoData2->setFont(ServoDataFont);
//    ServoData3->setMinimumSize(100,20);
//    ServoData3->setFont(ServoDataFont);
//    ServoData4->setMinimumSize(100,20);
//    ServoData4->setFont(ServoDataFont);

    //将推进器相关的控件垂直布局
    QWidget *ThrusterDataWidget = new QWidget(this);
    QVBoxLayout *ThrusterDataLayout = new QVBoxLayout(this);
    ThrusterDataWidget->setLayout(ThrusterDataLayout);
    ThrusterDataLayout->setContentsMargins(0, 0, 0, 0);
    ThrusterDataLayout->setAlignment(Qt::AlignTop);
    ThrusterDataLayout->addWidget(Thruster1);
    ThrusterDataLayout->addWidget(ThrusterData1);
    ThrusterDataLayout->addWidget(Thruster2);
    ThrusterDataLayout->addWidget(ThrusterData2);
    ThrusterDataLayout->addWidget(Thruster3);
    ThrusterDataLayout->addWidget(ThrusterData3);
    ThrusterDataLayout->addWidget(Thruster4);
    ThrusterDataLayout->addWidget(ThrusterData4);

    //将舵机相关的控件垂直布局
//    QWidget *ServoDataWidget = new QWidget(this);
//    QVBoxLayout *ServoDataLayout = new QVBoxLayout(this);
//    ServoDataWidget->setLayout(ServoDataLayout);
//    ServoDataLayout->setContentsMargins(0, 0, 0, 0);
//    ServoDataLayout->setAlignment(Qt::AlignTop);
//    ServoDataLayout->addWidget(Servo1);
//    ServoDataLayout->addWidget(ServoData1);
//    ServoDataLayout->addWidget(Servo2);
//    ServoDataLayout->addWidget(ServoData2);
//    ServoDataLayout->addWidget(Servo3);
//    ServoDataLayout->addWidget(ServoData3);
//    ServoDataLayout->addWidget(Servo4);
//    ServoDataLayout->addWidget(ServoData4);

    //将推进器和舵机的布局再进行水平布局
    QWidget *PropulsionSysDataWidget = new QWidget(this);
    QHBoxLayout *PropulsionSysDataLayout = new QHBoxLayout(this);
    PropulsionSysDataWidget->setLayout(PropulsionSysDataLayout);
    PropulsionSysDataLayout->setContentsMargins(0, 0, 0, 0);
    PropulsionSysDataLayout->setAlignment(Qt::AlignTop);
    PropulsionSysDataLayout->addWidget(ThrusterDataWidget);
    //PropulsionSysDataLayout->addWidget(ServoDataWidget);

    QWidget *PropulsionSysWidget = new QWidget(this);
    QVBoxLayout *PropulsionSysLayout = new QVBoxLayout(PropulsionSysWidget);
    PropulsionSysWidget->setLayout(PropulsionSysLayout);
    PropulsionSysLayout->setContentsMargins(0, 0, 0, 0);
    PropulsionSysLayout->setAlignment(Qt::AlignTop);
    PropulsionSysLayout->addWidget(PropulsionSysLabel);
    PropulsionSysLayout->addWidget(PropulsionSysSplitter);
    PropulsionSysLayout->addWidget(PropulsionSysDataWidget);

    splitter_3->addWidget(PropulsionSysWidget);

//串口LOG
    //log标签设置
    QLabel *logLabel = new QLabel(this);
    logLabel->setText("Log From Uart");
    logLabel->setFont(titleFont);
    logLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    logLabel->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *logSplitter = new QWidget(this);
    logSplitter->setFixedSize(30, 6);
    logSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //log框设置，包含PTE和LE，使用垂直布局
    //用于显示接收串口传过来的数据
    logPTE = new QPlainTextEdit;
    logPTE->setReadOnly(true);
    logPTE->setMinimumSize(300,185);
    logPTE->setStyleSheet("background-color: black; color: black;border-radius:3px; background-color: #00000000;font: 20px 'Corbel Light'; border: 1px solid darkgray;");

    //串口发送行
    logTII = new textInputItem("Send Line:",this);

    QWidget *logWidget = new QWidget(this);
    QVBoxLayout *logLayout = new QVBoxLayout(this); //垂直布局
    logWidget->setLayout(logLayout);
    logWidget->setContentsMargins(0, 0, 0, 0);
    logLayout->setAlignment(Qt::AlignTop);
    logLayout->addWidget(logLabel);
    logLayout->addWidget(logSplitter);
    logLayout->addWidget(logPTE);
    logLayout->addWidget(logTII);

    //底下两个按钮的设置，使用水平布局
    textButton *SendBTN = new textButton("Send",this);
    textButton *ClearBTN = new textButton("Clear",this);

    //按下发送键，发送发送数据信号给主窗口
    connect(SendBTN,&textButton::clicked,this,[=](){
        emit SendDataSignal();
    });

    //按下清屏键，清除接收框和发送框所有的数据
    connect(ClearBTN,&textButton::clicked,this,[=]()
    {
        logPTE->clear();
        logTII->setValue("");
        qDebug() << "Clear complete";
    });

    QWidget *BTNWidget = new QWidget(this);
    QHBoxLayout *BTNLayout = new QHBoxLayout(this); //水平布局
    BTNWidget->setLayout(BTNLayout);
    BTNLayout->addWidget(SendBTN);
    BTNLayout->addWidget(ClearBTN);

    //垂直布局，将logWidget、BTNWidget摆放
    splitter_3->addWidget(logWidget);
    splitter_3->addWidget(BTNWidget);

//3d模型
//    QLabel *model3DTitle = new QLabel(this);
//    model3DTitle = new QLabel(this);
//    model3DTitle->setText("3Dmodel");
//    model3DTitle->setFont(titleFont);
//    model3DTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//    model3DTitle->setStyleSheet("color:#2c2c2c");

//    //info底下的小横条
//    QWidget *model3DSplitter = new QWidget(this);
//    //info底下的小横条长度30，高度6
//    model3DSplitter->setFixedSize(30, 6);
//    model3DSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

//    view = new Qt3DExtras::Qt3DWindow();
//    //设置背景颜色
//    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
//    //静态创建了一个 QWidget容器以便将其嵌入到其他 QWidget 界面中。
//    container = QWidget::createWindowContainer(view);
//    //将容器设置为当前窗口的子窗口
//    container->setParent(this);

//    //最小容器
//    container->setMinimumSize(300,300);
//    //最大容器为全屏
//    QSize screenSize = view->screen()->size();
//    container->setMaximumSize(screenSize);

//    //创建实体
//    mRootEntity = new Qt3DCore::QEntity;

//    Qt3DRender::QCamera* camEntity = view->camera();
//    //设置了相机的投影方式为透视投影,该函数的第一个参数是视野角度（单位为度），第二个参数是宽高比，第三个和第四个参数分别是近平面和远平面的距离。
//    camEntity->lens()->setPerspectiveProjection(10.0f, 9.0f / 9.0f, 0.1f, 1000.0f);
//    //设置了相机的位置,为xyz
//    camEntity->setPosition(QVector3D(0, -10.0f, 0.0f));
//    //设置了相机的上方向
//    camEntity->setUpVector(QVector3D(0, 1, 0));
//    //设置了相机的视点中心,视点中心用于确定相机的注视点
//    camEntity->setViewCenter(QVector3D(0, 0, 0));
//    //相机实体与轨道摄像机控制器关联起来,可以使用鼠标或手指控制相机的旋转和缩放
//    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(mRootEntity);
//    camController->setCamera(camEntity);

//    //灯光配置
//    lightEntity = new Qt3DCore::QEntity(mRootEntity);
//    light = new Qt3DRender::QPointLight(lightEntity);
//    light->setColor("white");
//    light->setIntensity(1.2f);
//    lightEntity->addComponent(light);

//    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
//    lightTransform->setTranslation(camEntity->position());
//    lightEntity->addComponent(lightTransform);

//    view->setRootEntity(mRootEntity);
//    modifier = new SceneModifier(mRootEntity);

//    QWidget *qt3dWidget = new QWidget(this);
//    QVBoxLayout *qt3dLayout = new QVBoxLayout(this);
//    qt3dWidget->setLayout(qt3dLayout);
//    qt3dLayout->setContentsMargins(10, 0, 0, 0);
//    qt3dLayout->setAlignment(Qt::AlignTop);
//    qt3dLayout->addWidget(model3DTitle);
//    qt3dLayout->addWidget(model3DSplitter);
//    qt3dLayout->addWidget(container);

    //获取了 Qt3D 中的一个 QWidget 的尺寸策略
//    sizepolicy = qt3dWidget->sizePolicy();
//    //获取了 Qt3D 中的一个 QWidget 的尺寸策略
//    sizepolicy.setRetainSizeWhenHidden(true);
//    //配置好的尺寸策略设置回QWidget ,下一次将其显示时，能够保持之前的尺寸大小。
//    qt3dWidget->setSizePolicy(sizepolicy);

//    splitter_4->addWidget(qt3dWidget);

//info 显示深度、GPS、电量百分比
    infoTitle = new QLabel(this);
    infoTitle->setText("INFO");
    infoTitle->setFont(titleFont);
    infoTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoTitle->setStyleSheet("color:#2c2c2c");

    //info底下的小横条
    QWidget *infoSplitter = new QWidget(this);
    //info底下的小横条长度30，高度6
    infoSplitter->setFixedSize(30, 6);
    infoSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    QFont InfoDataFont = QFont("Corbel", 15);

    //姿态信息标题
    QLabel *AttitudeInfo = new QLabel(this);
    AttitudeInfo->setText("Angle:");
    AttitudeInfo->setMinimumHeight(25);
    AttitudeInfo->setFont(InfoDataFont);

    AttitudeDataInfo->setMinimumHeight(25);
    AttitudeDataInfo->setFont(InfoDataFont);

    //深度信息标题
    QLabel *DepthInfo = new QLabel(this);
    DepthInfo->setText("Depth:");
    DepthInfo->setMinimumHeight(25);
    DepthInfo->setFont(InfoDataFont);

    DepthDataInfo->setMinimumHeight(25);
    DepthDataInfo->setFont(InfoDataFont);

    infoWidget = new QWidget(this);
    infoWidget->setSizePolicy(sizepolicy);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoWidget->setMinimumSize(300,350);
    infoWidget->setLayout(infoLayout);
    infoLayout->setContentsMargins(10, 0, 0, 0);
    infoLayout->setAlignment(Qt::AlignTop);
    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoSplitter);
    infoLayout->addWidget(AttitudeInfo);
    infoLayout->addWidget(AttitudeDataInfo);
    infoLayout->addWidget(DepthInfo);
    infoLayout->addWidget(DepthDataInfo);

    splitter_4->addWidget(infoWidget);

    splitter_1->setStretchFactor(0,2);
    splitter_1->setStretchFactor(1,7);
    splitter_1->setStretchFactor(2,2);

    splitter_2->setStretchFactor(0,1);
    splitter_2->setStretchFactor(1,1);

    splitter_3->setStretchFactor(0,5);
    splitter_3->setStretchFactor(1,1);

    splitter_4->setStretchFactor(0,1);
    splitter_4->setStretchFactor(1,2);

    //内部数据分拣逻辑连接
    DataSortConnect();
}

//数据分类链接函数
void MotionControlWidget::DataSortConnect()
{
    //姿态分拣
    connect(this,&MotionControlWidget::StartDataSort,this,&MotionControlWidget::AttitudeDataSort);
    //推进力分拣
    connect(this,&MotionControlWidget::StartDataSort,this,&MotionControlWidget::PropulsionSysDataSort);
    //深度分拣
    connect(this,&MotionControlWidget::StartDataSort,this,&MotionControlWidget::DepthDataSort);

    //根据姿态，3D模型转向
    //connect(this,&MotionControlWidget::AttitudeChange,modifier,&SceneModifier::OnSetRotation);
}

void MotionControlWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W: //W键被按下
        CurrentKey = Qt::Key_W;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key W Press");
        ControlState->setText("Forward");
        emit SendControlSignal("W");
        break;
    case Qt::Key_A: //A键被按下
        CurrentKey = Qt::Key_A;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key A Press");
        ControlState->setText("Left");
        emit SendControlSignal("A");
        break;
    case Qt::Key_S: //S键被按下
        CurrentKey = Qt::Key_S;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key S Press");
        ControlState->setText("Draw back");
        emit SendControlSignal("S");
        break;
    case Qt::Key_D: //D键被按下        
        CurrentKey = Qt::Key_D;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key D Press");
        ControlState->setText("Right");
        emit SendControlSignal("D");
        break;
    case Qt::Key_Q: //Q键被按下
        CurrentKey = Qt::Key_Q;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key Q Press");
        ControlState->setText("Float");
        emit SendControlSignal("Q");
        break;
    case Qt::Key_E: //E键被按下
        CurrentKey = Qt::Key_E;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key E Press");
        ControlState->setText("Sink");
        emit SendControlSignal("E");
        break;
    default:
        QWidget::keyPressEvent(event);   //必须调用父类函数
        break;
    }
}

//数据显示到PlainTextEdit中，发起数据分拣信号
void MotionControlWidget::DataDisplayPTE(QString serialBuf)
{
    if(!this->isHidden())
    {
        logPTE->ensureCursorVisible();
        logPTE->insertPlainText(serialBuf);
        //qDebug()<<serialBuf;

        //开始数据分拣
        //以空格进行分割数据，用于判断数据来源
        QStringList ProcessedData = serialBuf.split(u' ');
        //qDebug() << ProcessedData;

        emit StartDataSort(ProcessedData);
    }
}

//推进系统数据分拣
void MotionControlWidget::PropulsionSysDataSort(QStringList ProcessedData)
{
    //是推进器的数据
    if(ProcessedData.at(0) == "T")
    {
        if(ProcessedData.at(1) == "1")
        {
            //qDebug() << "NO1的数据";
            ThrusterData1->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "2")
        {
            //qDebug() << "NO2的数据";
            ThrusterData2->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "3")
        {
            //qDebug() << "NO3的数据";
            ThrusterData3->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "4")
        {
            //qDebug() << "NO4的数据";
            ThrusterData4->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
    }
    //是舵机的数据
//    else if(ProcessedData.at(0) == "S")
//    {
//        if(ProcessedData.at(1) == "1")
//        {
//            //qDebug() << "NO1的数据";
//            ServoData1->setText(QString("%1")
//                                   .arg(ProcessedData.at(2)));
//        }
//        else if(ProcessedData.at(1) == "2")
//        {
//            //qDebug() << "NO2的数据";
//            ServoData2->setText(QString("%1")
//                                   .arg(ProcessedData.at(2)));
//        }
//        else if(ProcessedData.at(1) == "3")
//        {
//            //qDebug() << "NO3的数据";
//            ServoData3->setText(QString("%1")
//                                   .arg(ProcessedData.at(2)));
//        }
//        else if(ProcessedData.at(1) == "4")
//        {
//            //qDebug() << "NO4的数据";
//            ServoData4->setText(QString("%1")
//                                   .arg(ProcessedData.at(2)));
//        }
//    }
}

//姿态数据分拣
void MotionControlWidget::AttitudeDataSort(QStringList ProcessedData)
{
    //如果是JY901S的数据
    if(ProcessedData.count() > 0 && ProcessedData.at(0) == "J")
    {
        //如果是姿态角数据
        if(ProcessedData.at(1) == "Angle")
        {
            //qDebug() << "姿态角";
            AttitudeDataInfo->setText(QString("Roll%1    Pitch%2    Yaw%3")
                                     .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));    //Roll Pitch Yaw
            //emit AttitudeChange(ProcessedData.at(3),ProcessedData.at(4),ProcessedData.at(2));   //发射信号，改变3D模型朝向
        }
    }
}

void MotionControlWidget::DepthDataSort(QStringList ProcessedData)
{

}

void MotionControlWidget::joysitck_axis(int js_index, int axis_index, qreal value)
{
    if (m_joystick->joystickExists(js_index)){
        //左右
        if (axis_index == 0){
            // axis range [-1, 1] -> range [0, 1000];
            //qDebug() << "x" << value*500+500;
            LeftX = value*500+500;
        }
        //上下
        else if (axis_index == 1){
            // axis range [-1, 1] -> range [0, 1000];
            //qDebug() << "y" << value*500+500;
            LeftY = value*500+500;
        }
    }
}

void MotionControlWidget::joysitck_button(int js_index, int button_index, bool pressed)
{
    if (m_joystick->joystickExists(js_index)){
        qDebug() << button_index << pressed;
    }
}

//暂不使用
void MotionControlWidget::SaveToFile(const QString &path){
    QFile output(path);
    output.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&output);
    ts << "VFdGeWFXUnZaekl3TURJd05ESTE=\n";

    output.close();
}
