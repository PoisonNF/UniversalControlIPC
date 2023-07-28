#include "datadisplaywidget.h"

DataDisplayWidget::DataDisplayWidget(int radius, QWidget *parent) :
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

    //定时器延时10ms后，初始化数据显示界面
    QTimer *delay = new QTimer(this);
    connect(delay, &QTimer::timeout, this, [=](){
        Init();
        delay->deleteLater();   //超时后释放内存
    });
    delay->setSingleShot(true);
    delay->start(10);
}

//该构造函数不使用
DataDisplayWidget::DataDisplayWidget(QTextStream &ts, int radius, QWidget *parent) :
    QWidget(parent)
{
    Q_UNUSED(ts);
    Q_UNUSED(radius);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);

    this->setFocusPolicy(Qt::ClickFocus);

}

//在数据显示界面的设置栏
void DataDisplayWidget::ModeSelectPage(int radius){
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
//        QString savePath = QFileDialog::getSaveFileName(this, tr("Save map"), " ", tr("Map file(*.map)"));
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

void DataDisplayWidget::Init(){
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

    QSizePolicy sizepolicy = QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    sizepolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizepolicy.setHorizontalPolicy(QSizePolicy::Expanding);

    splitter_1->setSizePolicy(sizepolicy);
    splitter_2->setSizePolicy(sizepolicy);
    splitter_3->setSizePolicy(sizepolicy);
    splitter_4->setSizePolicy(sizepolicy);

//JY901S
    QFont titleFont = QFont("Corbel", 20);
    JY901STitle = new QLabel(this);
    JY901STitle->setText("JY901S");
    JY901STitle->setFont(titleFont);
    JY901STitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    JY901STitle->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *JY901SSplitter = new QWidget(this);
    JY901SSplitter->setFixedSize(30, 6);
    JY901SSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    QLabel *JY901SAcc = new QLabel("Acc:",this);    //JY901S加速度数据标签
    QLabel *JY901SGyro = new QLabel("Gyro:",this);   //JY901S中角速度标签
    QLabel *JY901SAngle = new QLabel("Angle:",this);  //JY901S中欧拉角标签
    QLabel *JY901SMag = new QLabel("Mag:",this);    //JY901S中磁场标签

    //设置字体和大小
    QFont JY901SDataFont = QFont("Corbel", 15);

    JY901SAcc->setMinimumHeight(25);
    JY901SAcc->setFont(JY901SDataFont);
    JY901SGyro->setMinimumHeight(25);
    JY901SGyro->setFont(JY901SDataFont);
    JY901SAngle->setMinimumHeight(25);
    JY901SAngle->setFont(JY901SDataFont);
    JY901SMag->setMinimumHeight(25);
    JY901SMag->setFont(JY901SDataFont);

    JY901SDataAcc->setMinimumSize(300,25);
    JY901SDataAcc->setFont(JY901SDataFont);
    JY901SDataGyro->setMinimumSize(300,25);
    JY901SDataGyro->setFont(JY901SDataFont);
    JY901SDataAngle->setMinimumSize(300,25);
    JY901SDataAngle->setFont(JY901SDataFont);
    JY901SDataMag->setMinimumSize(300,25);
    JY901SDataMag->setFont(JY901SDataFont);

    QWidget *JY901SDataWidget = new QWidget(this);
    JY901SDataWidget->setSizePolicy(sizepolicy);
    //设置最小大小
    JY901SDataWidget->setMinimumSize(450,300);
    QVBoxLayout *JY901SDataLayout = new QVBoxLayout(this);
    JY901SDataWidget->setLayout(JY901SDataLayout);
    JY901SDataLayout->setContentsMargins(0, 0, 0, 0);
    JY901SDataLayout->setAlignment(Qt::AlignTop);
    JY901SDataLayout->addWidget(JY901SAcc);
    JY901SDataLayout->addWidget(JY901SDataAcc);
    JY901SDataLayout->addWidget(JY901SGyro);
    JY901SDataLayout->addWidget(JY901SDataGyro);
    JY901SDataLayout->addWidget(JY901SAngle);
    JY901SDataLayout->addWidget(JY901SDataAngle);
    JY901SDataLayout->addWidget(JY901SMag);
    JY901SDataLayout->addWidget(JY901SDataMag);

    JY901SWidget = new QWidget(this);
    JY901SWidget->setSizePolicy(sizepolicy);
    QVBoxLayout *JY901SLayout = new QVBoxLayout(this);
    JY901SWidget->setLayout(JY901SLayout);
    JY901SLayout->setContentsMargins(0, 0, 0, 0);
    JY901SLayout->setAlignment(Qt::AlignTop);
    JY901SLayout->addWidget(JY901STitle);
    JY901SLayout->addWidget(JY901SSplitter);
    JY901SLayout->addWidget(JY901SDataWidget);

    splitter_2->addWidget(JY901SWidget);
    //splitter_2->addWidget(splitter_3);

//RM3100
    RM3100Title = new QLabel(this);
    RM3100Title->setText("RM3100");
    RM3100Title->setFont(titleFont);
    RM3100Title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    RM3100Title->setStyleSheet("color:#2c2c2c");

    //小横条设置
    QWidget *RM3100Splitter = new QWidget(this);
    RM3100Splitter->setFixedSize(30, 6);
    RM3100Splitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");

    QLabel *RM3100NO1 = new QLabel("NO1:",this);    //RM3100 1号标签
    QLabel *RM3100NO2 = new QLabel("NO2:",this);    //RM3100 2号标签
    QLabel *RM3100NO3 = new QLabel("NO3:",this);    //RM3100 3号标签
    QLabel *RM3100NO4 = new QLabel("NO4:",this);    //RM3100 4号标签

    //设置字体和大小
    QFont RM3100DataFont = QFont("Corbel", 15);

    RM3100NO1->setMinimumHeight(25);
    RM3100NO1->setFont(RM3100DataFont);
    RM3100NO2->setMinimumHeight(25);
    RM3100NO2->setFont(RM3100DataFont);
    RM3100NO3->setMinimumHeight(25);
    RM3100NO3->setFont(RM3100DataFont);
    RM3100NO4->setMinimumHeight(25);
    RM3100NO4->setFont(RM3100DataFont);

    RM3100DataNO1->setMinimumSize(300,25);
    RM3100DataNO1->setFont(RM3100DataFont);
    RM3100DataNO2->setMinimumSize(300,25);
    RM3100DataNO2->setFont(RM3100DataFont);
    RM3100DataNO3->setMinimumSize(300,25);
    RM3100DataNO3->setFont(RM3100DataFont);
    RM3100DataNO4->setMinimumSize(300,25);
    RM3100DataNO4->setFont(RM3100DataFont);

    QWidget *RM3100DataWidget = new QWidget(this);
    RM3100DataWidget->setSizePolicy(sizepolicy);
    RM3100DataWidget->setMinimumSize(450,300);
    QVBoxLayout *RM3100DataLayout = new QVBoxLayout(this);
    RM3100DataWidget->setLayout(RM3100DataLayout);
    RM3100DataLayout->setContentsMargins(0, 0, 0, 0);
    RM3100DataLayout->setAlignment(Qt::AlignTop);
    RM3100DataLayout->addWidget(RM3100NO1);
    RM3100DataLayout->addWidget(RM3100DataNO1);
    RM3100DataLayout->addWidget(RM3100NO2);
    RM3100DataLayout->addWidget(RM3100DataNO2);
    RM3100DataLayout->addWidget(RM3100NO3);
    RM3100DataLayout->addWidget(RM3100DataNO3);
    RM3100DataLayout->addWidget(RM3100NO4);
    RM3100DataLayout->addWidget(RM3100DataNO4);


    RM3100Widget = new QWidget(this);
    RM3100Widget->setSizePolicy(sizepolicy);
    QVBoxLayout *RM3100Layout = new QVBoxLayout(this);
    RM3100Widget->setLayout(RM3100Layout);
    RM3100Layout->setContentsMargins(0, 0, 0, 0);
    RM3100Layout->setAlignment(Qt::AlignTop);
    RM3100Layout->addWidget(RM3100Title);
    RM3100Layout->addWidget(RM3100Splitter);
    RM3100Layout->addWidget(RM3100DataWidget);

    splitter_2->addWidget(RM3100Widget);

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
    QLabel *Servo1 = new QLabel("Servo1",this);
    QLabel *Servo2 = new QLabel("Servo2",this);
    QLabel *Servo3 = new QLabel("Servo3",this);
    QLabel *Servo4 = new QLabel("Servo4",this);

    //设置字体和大小
    QFont ServoDataFont = QFont("Corbel", 15);

    Servo1->setMinimumHeight(25);
    Servo1->setFont(ServoDataFont);
    Servo2->setMinimumHeight(25);
    Servo2->setFont(ServoDataFont);
    Servo3->setMinimumHeight(25);
    Servo3->setFont(ServoDataFont);
    Servo4->setMinimumHeight(25);
    Servo4->setFont(ServoDataFont);

    ServoData1->setMinimumSize(100,20);
    ServoData1->setFont(ServoDataFont);
    ServoData2->setMinimumSize(100,20);
    ServoData2->setFont(ServoDataFont);
    ServoData3->setMinimumSize(100,20);
    ServoData3->setFont(ServoDataFont);
    ServoData4->setMinimumSize(100,20);
    ServoData4->setFont(ServoDataFont);

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
    QWidget *ServoDataWidget = new QWidget(this);
    QVBoxLayout *ServoDataLayout = new QVBoxLayout(this);
    ServoDataWidget->setLayout(ServoDataLayout);
    ServoDataLayout->setContentsMargins(0, 0, 0, 0);
    ServoDataLayout->setAlignment(Qt::AlignTop);
    ServoDataLayout->addWidget(Servo1);
    ServoDataLayout->addWidget(ServoData1);
    ServoDataLayout->addWidget(Servo2);
    ServoDataLayout->addWidget(ServoData2);
    ServoDataLayout->addWidget(Servo3);
    ServoDataLayout->addWidget(ServoData3);
    ServoDataLayout->addWidget(Servo4);
    ServoDataLayout->addWidget(ServoData4);

    //将推进器和舵机的布局再进行水平布局
    QWidget *PropulsionSysDataWidget = new QWidget(this);
    QHBoxLayout *PropulsionSysDataLayout = new QHBoxLayout(this);
    PropulsionSysDataWidget->setLayout(PropulsionSysDataLayout);
    PropulsionSysDataLayout->setContentsMargins(0, 0, 0, 0);
    PropulsionSysDataLayout->setAlignment(Qt::AlignTop);
    PropulsionSysDataLayout->addWidget(ThrusterDataWidget);
    PropulsionSysDataLayout->addWidget(ServoDataWidget);

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

    infoWidget = new QWidget(this);
    infoWidget->setSizePolicy(sizepolicy);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoWidget->setMinimumSize(300,350);
    infoWidget->setLayout(infoLayout);
    infoLayout->setContentsMargins(10, 0, 0, 0);
    infoLayout->setAlignment(Qt::AlignTop);
    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoSplitter);

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
void DataDisplayWidget::DataSortConnect()
{
    connect(this,&DataDisplayWidget::StartDataSort,this,&DataDisplayWidget::JY901SDataSort);
    connect(this,&DataDisplayWidget::StartDataSort,this,&DataDisplayWidget::RM3100DataSort);
    connect(this,&DataDisplayWidget::StartDataSort,this,&DataDisplayWidget::PropulsionSysDataSort);

    //connect(this,&DataDisplayWidget::AttitudeChange,modifier,&SceneModifier::OnSetRotation);
}

//数据显示到PlainTextEdit中，发起数据分拣信号
void DataDisplayWidget::DataDisplayPTE(QString serialBuf)
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

//JY901S数据分拣
void DataDisplayWidget::JY901SDataSort(QStringList ProcessedData)
{
    //是JY901S的数据
    if(ProcessedData.count() > 0 && ProcessedData.at(0) == "J")
    {
        //qDebug() << "JY901S的数据";
        //是加速度
        if(ProcessedData.at(1) == "Acc")
        {
            //qDebug() << "Acc的数据";
            JY901SDataAcc->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
        //是角速度
        else if(ProcessedData.at(1) == "Gyro")
        {
            //qDebug() << "Gyro的数据";
            JY901SDataGyro->setText(QString("%1    %2    %3")
                                    .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
        //是欧拉角
        else if(ProcessedData.at(1) == "Angle")
        {
            //qDebug() << "Angle的数据";
            JY901SDataAngle->setText(QString("%1    %2    %3")
                                     .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));    //Roll Pitch Yaw
            emit AttitudeChange(ProcessedData.at(3),ProcessedData.at(4),ProcessedData.at(2));  //姿态改变，对应的3D模型进行旋转  Pitch Yaw Roll
        }
        //是磁场
        else if(ProcessedData.at(1) == "Mag")
        {
            //qDebug() << "Mag的数据";
            JY901SDataMag->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
    }
}

//RM3100数据分拣
void DataDisplayWidget::RM3100DataSort(QStringList ProcessedData)
{
    //是RM3100的数据
    if(ProcessedData.count() > 0 && ProcessedData.at(0) == "R")
    {
        if(ProcessedData.at(1) == "1")
        {
            //qDebug() << "NO1的数据";
            RM3100DataNO1->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
        else if(ProcessedData.at(1) == "2")
        {
            //qDebug() << "NO2的数据";
            RM3100DataNO2->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
        else if(ProcessedData.at(1) == "3")
        {
            //qDebug() << "NO3的数据";
            RM3100DataNO3->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
        else if(ProcessedData.at(1) == "4")
        {
            //qDebug() << "NO4的数据";
            RM3100DataNO4->setText(QString("%1    %2    %3")
                                   .arg(ProcessedData.at(2),ProcessedData.at(3),ProcessedData.at(4)));
        }
    }
}

//推进系统数据分拣
void DataDisplayWidget::PropulsionSysDataSort(QStringList ProcessedData)
{
    //是推进器的数据
    if(ProcessedData.count() > 0 && ProcessedData.at(0) == "T")
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
    else if(ProcessedData.count() > 0 && ProcessedData.at(0) == "S")
    {
        if(ProcessedData.at(1) == "1")
        {
            //qDebug() << "NO1的数据";
            ServoData1->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "2")
        {
            //qDebug() << "NO2的数据";
            ServoData2->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "3")
        {
            //qDebug() << "NO3的数据";
            ServoData3->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
        else if(ProcessedData.at(1) == "4")
        {
            //qDebug() << "NO4的数据";
            ServoData4->setText(QString("%1")
                                   .arg(ProcessedData.at(2)));
        }
    }
}

//暂不使用
void DataDisplayWidget::SaveToFile(const QString &path){
    QFile output(path);
    output.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&output);
    ts << "VFdGeWFXUnZaekl3TURJd05ESTE=\n";

    output.close();
}
