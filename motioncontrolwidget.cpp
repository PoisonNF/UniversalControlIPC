#include "motioncontrolwidget.h"

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
        delay->deleteLater();   //超时后释放内存
    });
    delay->setSingleShot(true);
    delay->start(10);
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

void MotionControlWidget::Init(){
    InitJoysticks();            //初始化手柄
    InitLayout();               //初始化总体布局
    InitPIDWidget();            //初始化PID窗口
    InitControlWidget();        //初始化控制窗口
    InitPropulsionSysWidget();  //初始化动力系统窗口
    InitLogWidget();            //初始化串口log窗口
    InitYOLOLogWidget();        //初始化YOLO串口log窗口
    InitInfoWidget();           //初始化信息窗口
}

void MotionControlWidget::InitJoysticks()
{
    m_joystick = QJoysticks::getInstance();
    QStringList js_names = m_joystick->deviceNames();    //添加手柄
    qDebug() << js_names;

    QThread *JSThread = new QThread;
    JSwork = new Joysticks;
    JSwork->moveToThread(JSThread);
    JSThread->start();

    connect(m_joystick,SIGNAL(axisChanged(int,int,qreal)),JSwork,SLOT(Joysticksworking(int,int,qreal)));
    connect(m_joystick,SIGNAL(buttonChanged(int,int,bool)),JSwork,SLOT(Joysticksworking(int,int,bool)));

    connect(JSwork,&Joysticks::sigJoysticksValueGet,this,[=](Joysticks::JoystickData JoyData)
    {
        QString str;
        QString sendstr;

        //将double截取为整数，并且转换为QString
        QString x = QString::number(std::floor(JoyData.LeftX));
        QString y = QString::number(std::floor(JoyData.LeftY));

        QString angle = QString::number(std::floor(JoyData.Angle));
        QString length = QString::number(std::floor(JoyData.Length));

        str = "x " + x + " y " + y + " angle " + angle + " length " + length;
        sendstr = "angle " + angle + " " + "length " + length + "\r\n";
        emit sigJoyAxisSend(sendstr);
        JoystickAxisDataInfo->setText(str);
    });

    connect(JSwork,&Joysticks::sigJoysticksButtonGet,this,[=](Joysticks::JoystickData JoyData)
    {
        QString str;

        //根据按键状态发送不同的字符串给下位机
        if(JoyData.Status){
            str = "Button " + QString::number(JoyData.Button) + " press" + "\r\n";
        }
        else {
            str = "Button " + QString::number(JoyData.Button) + " release" + "\r\n";
        }

        emit sigJoyButtonSend(str);
        JoystickButtonDataInfo->setText(str);
    });

    connect(JSwork,&Joysticks::destroyed,this,[=](){
        JSThread->quit();
        JSThread->wait();
        JSThread->deleteLater();
    });

    //connect(m_joystick, SIGNAL(axisChanged(int, int, qreal)), this, SLOT(joysitck_axis(int, int, qreal)));
    //connect(m_joystick, &QJoysticks::axisEvent, this, &MainWindow::event_axis);

    //connect(m_joystick, SIGNAL(buttonChanged(int, int, bool)), this, SLOT(joysitck_button(int, int, bool)));
    //connect(m_joystick, &QJoysticks::buttonEvent, this, &MainWindow::event_button);

    //m_joystick->setVirtualJoystickEnabled (true);    //启用虚拟手柄
    //m_joystick->setVirtualJoystickRange (1);    //设置虚拟手柄摇杆范围[-1,1]
}

void MotionControlWidget::InitLayout()
{
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


    splitter_1->setStretchFactor(0,2);
    splitter_1->setStretchFactor(1,7);
    splitter_1->setStretchFactor(2,2);

    splitter_2->setStretchFactor(0,1);
    splitter_2->setStretchFactor(1,1);

    splitter_3->setStretchFactor(0,5);
    splitter_3->setStretchFactor(1,1);

    splitter_4->setStretchFactor(0,1);
    splitter_4->setStretchFactor(1,2);

    //设置布局策略
    QSizePolicy sizepolicy = QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);

    splitter_1->setSizePolicy(sizepolicy);
    splitter_2->setSizePolicy(sizepolicy);
    splitter_3->setSizePolicy(sizepolicy);
    splitter_4->setSizePolicy(sizepolicy);
}

void MotionControlWidget::InitPIDWidget()
{
    //PID参数设置与发送
    PIDTitle = new QLabel(this);
    PIDTitle->setText("PID");
    PIDTitle->setFont(TitleFont);
    PIDTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    PIDTitle->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *PIDSplitter = new QWidget(this);
    PIDSplitter->setFixedSize(30, 6);
    PIDSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //选项框设置
    PIDComboBox->setMaximumWidth(100);
    PIDComboBox->insertItems(0,QStringList("深度环"));
    PIDComboBox->insertItems(1,QStringList("艏向环"));
    PIDComboBox->setFont(QFont("Corbel", 12));

    QString sheet = "\
            QComboBox {\
                border: 1px solid gray; \
                border-radius: 3px;\
                padding: 1px 18px 1px 3px; \
                color: #000;\
                font: normal normal 15px Microsoft YaHei;\
                background: transparent;\
            }\
            QComboBox QAbstractItemView {\
                font-family: Microsoft YaHei;\
                font-size: 15px;\
                border:1px solid rgba(0,0,0,10%);\
                border-radius: 3px;\
                padding:5px;\
                background-color: #FFFFFF;\
            }\
            ";

    PIDComboBox->setView(new QListView());
    PIDComboBox->setStyleSheet(sheet);

    DepthPIDstore.P = 0;
    DepthPIDstore.I = 0;
    DepthPIDstore.D = 0;

    YawPIDstore.P = 0;
    YawPIDstore.I = 0;
    YawPIDstore.D = 0;

    connect(PIDComboBox,&QComboBox::currentIndexChanged,this,[=](){
        qDebug() << PIDComboBox->currentIndex() <<PIDComboBox->currentText();
        if(PIDComboBox->currentIndex() == DepthPID)         //深度环
        {
            CurrPID_P->setText(QString("DepthPID P:              %1").arg(DepthPIDstore.P));
            CurrPID_I->setText(QString("DepthPID I:              %1").arg(DepthPIDstore.I));
            CurrPID_D->setText(QString("DepthPID D:              %1").arg(DepthPIDstore.D));
        }
        else if(PIDComboBox->currentIndex() == YawPID)     //艏向环
        {
            CurrPID_P->setText(QString("YawPID P:              %1").arg(YawPIDstore.P));
            CurrPID_I->setText(QString("YawPID I:              %1").arg(YawPIDstore.I));
            CurrPID_D->setText(QString("YawPID D:              %1").arg(YawPIDstore.D));
        }
    });

    //设置字体和大小
    QFont PIDTitleFont = QFont("Corbel", 18);
    QFont PIDDataFont = QFont("Corbel", 12);

    //当前PID值标签
    QLabel *CurrentPID = new QLabel("Current PID",this);
    CurrentPID->setMinimumHeight(20);
    CurrentPID->setFont(PIDTitleFont);

    CurrPID_P->setMinimumHeight(20);
    CurrPID_P->setFont(PIDDataFont);
    CurrPID_I->setMinimumHeight(20);
    CurrPID_I->setFont(PIDDataFont);
    CurrPID_D->setMinimumHeight(20);
    CurrPID_D->setFont(PIDDataFont);

    //设置PID值标签
    QLabel *SetPID = new QLabel("Set PID",this);
    SetPID->setMinimumHeight(20);
    SetPID->setFont(PIDTitleFont);
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

        //保存PID值为现PID值
        if(PIDComboBox->currentIndex() == DepthPID)
        {
            CurrPID_P->setText(QString("DepthPID P:              %1").arg(PID_P_TII->value()));
            CurrPID_I->setText(QString("DepthPID I:              %1").arg(PID_I_TII->value()));
            CurrPID_D->setText(QString("DepthPID D:              %1").arg(PID_D_TII->value()));

            DepthPIDstore.P = QString(PID_P_TII->value()).toDouble();
            DepthPIDstore.I = QString(PID_I_TII->value()).toDouble();
            DepthPIDstore.D = QString(PID_D_TII->value()).toDouble();

            emit sigSendPIDSignal(DepthPIDstore,DepthPID);    //发射设置深度环PID信号
        }
        else if(PIDComboBox->currentIndex() == YawPID)
        {
            CurrPID_P->setText(QString("YawPID P:              %1").arg(PID_P_TII->value()));
            CurrPID_I->setText(QString("YawPID I:              %1").arg(PID_I_TII->value()));
            CurrPID_D->setText(QString("YawPID D:              %1").arg(PID_D_TII->value()));

            YawPIDstore.P = QString(PID_P_TII->value()).toDouble();
            YawPIDstore.I = QString(PID_I_TII->value()).toDouble();
            YawPIDstore.D = QString(PID_D_TII->value()).toDouble();

            emit sigSendPIDSignal(YawPIDstore,YawPID);       //发射设置艏向环PID信号
        }
    });

    QWidget *PIDDataWidget = new QWidget(this);
    PIDDataWidget->setSizePolicy(sizepolicy);
    //设置最小大小
    PIDDataWidget->setMinimumSize(410,0);
    QVBoxLayout *PIDDataLayout = new QVBoxLayout(this);
    PIDDataWidget->setLayout(PIDDataLayout);
    PIDDataLayout->setContentsMargins(0, 0, 0, 0);
    PIDDataLayout->setAlignment(Qt::AlignTop);
    PIDDataLayout->addWidget(PIDComboBox);
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
}

void MotionControlWidget::InitControlWidget()
{
    //按键显示和读取，或者使用鼠标点击，发送动作命令
    ControlTitle = new QLabel(this);
    ControlTitle->setText("Control");
    ControlTitle->setFont(TitleFont);
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
        emit sigSendControlSignal("W");
    });
    connect(AIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key A Press");
        ControlState->setText("Left");
        emit sigSendControlSignal("A");
    });
    connect(SIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key S Press");
        ControlState->setText("Draw back");
        emit sigSendControlSignal("S");
    });
    connect(DIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key D Press");
        ControlState->setText("Right");
        emit sigSendControlSignal("D");
    });
    connect(QIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key Q Press");
        ControlState->setText("Float");
        emit sigSendControlSignal("Q");
    });
    connect(EIcon,&customIcon::clicked,this,[=]()
    {
        ControlData->setText("Key E Press");
        ControlState->setText("Sink");
        emit sigSendControlSignal("E");
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
    ControlDataWidget->setMinimumSize(410,0);
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
}

void MotionControlWidget::InitPropulsionSysWidget()
{
    //动力系统
    QLabel *PropulsionSysLabel = new QLabel(this);
    PropulsionSysLabel->setText("Propulsion System");
    PropulsionSysLabel->setFont(TitleFont);
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
    QFont ThrusterTitleFont = QFont("Corbel", 18);
    QFont ThrusterDataFont = QFont("Arial",15);

    Thruster1->setMinimumHeight(25);
    Thruster1->setFont(ThrusterTitleFont);
    Thruster2->setMinimumHeight(25);
    Thruster2->setFont(ThrusterTitleFont);
    Thruster3->setMinimumHeight(25);
    Thruster3->setFont(ThrusterTitleFont);
    Thruster4->setMinimumHeight(25);
    Thruster4->setFont(ThrusterTitleFont);

    ThrusterData1->setMinimumSize(80,20);
    ThrusterData1->setFont(ThrusterDataFont);
    ThrusterData2->setMinimumSize(80,20);
    ThrusterData2->setFont(ThrusterDataFont);
    ThrusterData3->setMinimumSize(80,20);
    ThrusterData3->setFont(ThrusterDataFont);
    ThrusterData4->setMinimumSize(80,20);
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
}

void MotionControlWidget::InitLogWidget()
{
    //串口LOG
    //log标签设置
    QLabel *logLabel = new QLabel(this);
    logLabel->setText("Log From Uart");
    logLabel->setFont(TitleFont);
    logLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    logLabel->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *logSplitter = new QWidget(this);
    logSplitter->setFixedSize(30, 6);
    logSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //log框设置，包含PTE和LE，使用垂直布局
    //用于显示接收串口传过来的数据
    QFont logPTEFont = QFont("Arial",10);
    logPTE = new QPlainTextEdit;
    logPTE->setReadOnly(true);
    logPTE->setMinimumSize(300,185);
    logPTE->setFont(logPTEFont);
    logPTE->setStyleSheet("background-color: black; color: black;border-radius:3px; background-color: #00000000; border: 1px solid darkgray;");

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
        emit sigLogDataSend();
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
}

void MotionControlWidget::InitYOLOLogWidget()
{
    //串口LOG
    //log标签设置
    QLabel *logLabel = new QLabel(this);
    logLabel->setText("YOLO");
    logLabel->setFont(TitleFont);
    logLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    logLabel->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *logSplitter = new QWidget(this);
    logSplitter->setFixedSize(30, 6);
    logSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    //log框设置，包含PTE和LE，使用垂直布局
    //用于显示接收串口传过来的数据
    QFont logPTEFont = QFont("Arial",10);
    YOLOlogPTE = new QPlainTextEdit;
    YOLOlogPTE->setReadOnly(true);
    YOLOlogPTE->setMinimumSize(300,185);
    YOLOlogPTE->setFont(logPTEFont);
    YOLOlogPTE->setStyleSheet("background-color: black; color: black;border-radius:3px; background-color: #00000000; border: 1px solid darkgray;");

    QWidget *YOLOlogWidget = new QWidget(this);
    QVBoxLayout *logLayout = new QVBoxLayout(this); //垂直布局
    YOLOlogWidget->setLayout(logLayout);
    YOLOlogWidget->setContentsMargins(0, 0, 0, 0);
    logLayout->setAlignment(Qt::AlignTop);
    logLayout->addWidget(logLabel);
    logLayout->addWidget(logSplitter);
    logLayout->addWidget(YOLOlogPTE);

    //底下按钮的设置，使用水平布局
    textButton *ClearBTN = new textButton("Clear",this);

    //按下清屏键，清除接收框和发送框所有的数据
    connect(ClearBTN,&textButton::clicked,this,[=]()
    {
        YOLOlogPTE->clear();
        qDebug() << "Clear complete";
    });

    QWidget *BTNWidget = new QWidget(this);
    QHBoxLayout *BTNLayout = new QHBoxLayout(this); //水平布局
    BTNWidget->setLayout(BTNLayout);
    BTNLayout->addWidget(ClearBTN);

    //垂直布局，将logWidget、BTNWidget摆放
    splitter_4->addWidget(YOLOlogWidget);
    splitter_4->addWidget(BTNWidget);
}

void MotionControlWidget::InitInfoWidget()
{
    //info 显示深度、GPS、电量百分比
    infoTitle = new QLabel(this);
    infoTitle->setText("INFO");
    infoTitle->setFont(TitleFont);
    infoTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoTitle->setStyleSheet("color:#2c2c2c");

    //info底下的小横条
    QWidget *infoSplitter = new QWidget(this);
    //info底下的小横条长度30，高度6
    infoSplitter->setFixedSize(30, 6);
    infoSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    QFont InfoTitleFont = QFont("Corbel",18);
    QFont InfoDataFont = QFont("Arial",15);

    //姿态信息标题
    QLabel *AttitudeInfo = new QLabel(this);
    AttitudeInfo->setText("Angle:");
    AttitudeInfo->setMinimumHeight(25);
    AttitudeInfo->setFont(InfoTitleFont);

    AttitudeDataInfo->setMinimumHeight(25);
    AttitudeDataInfo->setFont(InfoDataFont);

    //深度信息标题
    QLabel *DepthInfo = new QLabel(this);
    DepthInfo->setText("Depth:");
    DepthInfo->setMinimumHeight(25);
    DepthInfo->setFont(InfoTitleFont);

    DepthDataInfo->setMinimumHeight(25);
    DepthDataInfo->setFont(InfoDataFont);

    //手柄数据输出
    QLabel *JoystickInfo = new QLabel(this);
    JoystickInfo->setText("Joystick:");
    JoystickInfo->setMinimumHeight(25);
    JoystickInfo->setFont(InfoTitleFont);

    JoystickAxisDataInfo->setMinimumHeight(25);
    JoystickAxisDataInfo->setFont(InfoDataFont);

    JoystickButtonDataInfo->setMinimumHeight(25);
    JoystickButtonDataInfo->setFont(InfoDataFont);

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
    infoLayout->addWidget(JoystickInfo);
    infoLayout->addWidget(JoystickAxisDataInfo);
    infoLayout->addWidget(JoystickButtonDataInfo);

    splitter_4->addWidget(infoWidget);
}

void MotionControlWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W: //W键被按下
        CurrentKey = Qt::Key_W;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key W Press");
        ControlState->setText("Forward");
        emit sigSendControlSignal("W");
        break;
    case Qt::Key_A: //A键被按下
        CurrentKey = Qt::Key_A;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key A Press");
        ControlState->setText("Left");
        emit sigSendControlSignal("A");
        break;
    case Qt::Key_S: //S键被按下
        CurrentKey = Qt::Key_S;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key S Press");
        ControlState->setText("Draw back");
        emit sigSendControlSignal("S");
        break;
    case Qt::Key_D: //D键被按下        
        CurrentKey = Qt::Key_D;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key D Press");
        ControlState->setText("Right");
        emit sigSendControlSignal("D");
        break;
    case Qt::Key_Q: //Q键被按下
        CurrentKey = Qt::Key_Q;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key Q Press");
        ControlState->setText("Float");
        emit sigSendControlSignal("Q");
        break;
    case Qt::Key_E: //E键被按下
        CurrentKey = Qt::Key_E;
        qDebug() << CurrentKey << "被按下";
        ControlData->setText("Key E Press");
        ControlState->setText("Sink");
        emit sigSendControlSignal("E");
        break;
    default:
        QWidget::keyPressEvent(event);   //必须调用父类函数
        break;
    }
}

//数据显示到PlainTextEdit中，发起数据分拣信号
void MotionControlWidget::slotLogDataDisplay(QString serialBuf)
{
    if(!this->isHidden())
    {
        logPTE->ensureCursorVisible();
        logPTE->insertPlainText(serialBuf);
    }
    LOG_INFO((char*)"串口数据显示");
}

void MotionControlWidget::slotYOLOLogDataDisplay(QString serialBuf)
{
    if(!this->isHidden())
    {
        YOLOlogPTE->ensureCursorVisible();
        YOLOlogPTE->insertPlainText(serialBuf);
    }
    LOG_INFO((char*)"YOLO串口数据显示");
}

void MotionControlWidget::slotAngleDataDisplay(QStringList ProcessedData)
{
    if(!this->isHidden())
    {
        AttitudeDataInfo->setText(QString("Roll%1    Pitch%2    Yaw%3")
                                 .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));    //Roll Pitch Yaw
    }
    LOG_INFO((char*)"姿态数据显示");
}

void MotionControlWidget::slotDepthDataDisplay(QStringList ProcessedData)
{
    if(!this->isHidden())
    {

    }
    LOG_INFO((char*)"深度数据显示");
}

void MotionControlWidget::slotThrusterDataDisplay(QStringList ProcessedData, int ThrusterNum)
{
    if(!this->isHidden())
    {
        switch(ThrusterNum)
        {
        case 1:
            ThrusterData1->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
            break;
        case 2:
            ThrusterData2->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
            break;
        case 3:
            ThrusterData3->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
            break;
        case 4:
            ThrusterData4->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
            break;
        default:
            break;
        }
    }
    LOG_INFO((char*)"推进器数据显示");
}


//暂不使用
void MotionControlWidget::SaveToFile(const QString &path){
    QFile output(path);
    output.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&output);
    ts << "VFdGeWFXUnZaekl3TURJd05ESTE=\n";

    output.close();
}
