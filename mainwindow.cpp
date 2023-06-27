#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainterPath>
#include <QRegion>
#include <QTimer>

#if (QT_VERSION > QT_VERSION_CHECK(6,3,0))
#include <QFileDialog>
#endif


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setMouseTracking(true);
#ifdef Q_OS_LINUX
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    cornerRadius = 0;
#endif

    //定时器延时10ms后，初始化所有界面
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=](){Init();});
    t->setSingleShot(true);
    t->start(10);

    connect(ui->adjSizeBtn, &QPushButton::clicked, this, [=](){controlWindowScale();});
}

/* 初始化 */
void MainWindow::Init(){

    InitFrame();    //框架初始化
    InitDefaultPage();  //默认主界面初始化
    InitDefaultSettingsPage(); //默认设置界面初始化
    InitLayersPage();   //层页界面和按钮初始化
    InitDataDisplayWidget();    //数据显示界面初始化
    InitMotionControlWidget();  //运动控制界面初始化
    InitSerialPage();   //串口设置界面初始化
}

/* 框架初始化 */
void MainWindow::InitFrame()
{
    /* 创建主窗口小部件并设置掩码、样式表和阴影 */
#ifdef Q_OS_LINUX
    QPainterPath path;
    path.addRect(ui->mainWidget->rect());
#else
    QPainterPath path;
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    QString mainStyle;
    ui->mainWidget->setObjectName("mainWidget");
    mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
    ui->mainWidget->setStyleSheet(mainStyle);

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#ifdef Q_OS_WINDOWS
    //增加窗口阴影
    windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainWidget->setGraphicsEffect(windowShadow);
#endif
#endif

    /* 创建边界以覆盖区域的锯齿形边缘 */
#ifdef Q_OS_WINDOWS
    border = new QWidget(this);
    border->move(ui->mainWidget->pos() - QPoint(1, 1));
    border->resize(ui->mainWidget->size() + QSize(2, 2));
    QString borderStyle;
    borderStyle = "background-color:#00FFFFFF;border:1.5px solid #686868; border-radius:" + QString::asprintf("%d",cornerRadius) + "px";
    border->setStyleSheet(borderStyle);
    border->setAttribute(Qt::WA_TransparentForMouseEvents);
    border->show();
#endif
}

/* 默认设置界面初始化 */
void MainWindow::InitDefaultSettingsPage()
{
    /* 创建设置关于页 */
    defaultSettingsPage = new SlidePage(cornerRadius, "ABOUT", ui->mainWidget);
    textInputItem *version = new textInputItem("version", defaultSettingsPage);
    version->setValue("1.3-beta");
    version->setEnabled(false);
    textInputItem *updateDate = new textInputItem("last-upd", defaultSettingsPage);
    updateDate->setValue("2021/12/6 10:14");
    updateDate->setEnabled(false);
    textInputItem *Author = new textInputItem("author", defaultSettingsPage);
    Author->setValue("Linloir | PoisonNF");
    Author->setEnabled(false);
    textInputItem *lic = new textInputItem("lic", defaultSettingsPage);
    lic->setValue("MIT License");
    lic->setEnabled(false);
    textInputItem *GitHub = new textInputItem("git", defaultSettingsPage);
    GitHub->setValue("github.com/PoisonNF");
    GitHub->setEnabled(false);
    defaultSettingsPage->AddContent(GitHub);
    defaultSettingsPage->AddContent(lic);
    defaultSettingsPage->AddContent(Author);
    defaultSettingsPage->AddContent(updateDate);
    defaultSettingsPage->AddContent(version);
    curSettingsPage = defaultSettingsPage;
    defaultSettingsPage->show();
    pageList.push_back(defaultSettingsPage);
}

/* 默认主界面初始化 */
void MainWindow::InitDefaultPage()
{
    /* 初始化主界面上部标题栏 */
    //主标题设置
    QFont titleFont = QFont("Corbel Light", 24);
    QFontMetrics titleFm(titleFont);
    Heading = new QLineEdit(this);
    Heading->setFont(titleFont);
    Heading->setText("UniversalControlIPC");
    Heading->setMaxLength(20);
    Heading->setReadOnly(true);
    Heading->setMinimumHeight(titleFm.height());
    Heading->setMaximumWidth(titleFm.size(Qt::TextSingleLine, "UniversalControlIPC").width() + 10);
    Heading->setStyleSheet("background-color:#00000000;border-style:none;border-width:0px;margin-left:1px;");
    connect(Heading, &QLineEdit::textEdited, Heading, [=](QString text){Heading->setMaximumWidth(titleFm.size(Qt::TextSingleLine, text).width());});

    //副标题设置
    QFont descFont = QFont("Corbel Light", 12);
    QFontMetrics descFm(descFont);
    Subtitle = new QLineEdit(this);
    Subtitle->setFont(descFont);
    Subtitle->setText("Controller and Observer for AUV");
    Subtitle->setMaxLength(128);
    Subtitle->setReadOnly(true);
    Subtitle->setMinimumHeight(descFm.lineSpacing());
    Subtitle->setStyleSheet("background-color:#00000000;border-style:none;border-width:0px;");

    //配置设置图标的大小
    settingsIcon = new customIcon(":/icons/icons/settings.svg", "settings", 5, this);
    settingsIcon->setMinimumHeight(Heading->height() * 0.7);
    settingsIcon->setMaximumWidth(Heading->height() * 0.7);

    //点击设置图标调出副窗口
    connect(settingsIcon, &customIcon::clicked, this, [=](){
        //设置图标转动，0 逐渐变为 90度，持续时间为 750 毫秒，并使用插值曲线使动画效果更加自然
        QPropertyAnimation *rotate = new QPropertyAnimation(settingsIcon, "rotationAngle", this);
        rotate->setDuration(750);
        rotate->setStartValue(0);
        rotate->setEndValue(90);
        rotate->setEasingCurve(QEasingCurve::InOutExpo);
        rotate->start();

        //当前设置界面滑入
        curSettingsPage->slideIn();
    });

    //配置层图标的大小
    layersIcon = new customIcon(":/icons/icons/layers.svg", "layers", 5, this);
    layersIcon->setMinimumHeight(Heading->height() * 0.7);
    layersIcon->setMaximumWidth(Heading->height() * 0.7);

    /* 布局配置 */
    QWidget *titleInnerWidget = new QWidget(this);
    titleInnerWidget->setFixedHeight(Heading->height());
    QHBoxLayout *innerLayout = new QHBoxLayout(titleInnerWidget);   //水平布局
    titleInnerWidget->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(10);
    innerLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    innerLayout->addWidget(Heading);
    innerLayout->addWidget(settingsIcon);
    innerLayout->addWidget(layersIcon);

    QWidget *titleWidget = new QWidget(this);
    titleWidget->setMaximumHeight(Heading->height() + Subtitle->height());
    QVBoxLayout *outerLayout = new QVBoxLayout(titleWidget);        //垂直布局
    titleWidget->setLayout(outerLayout);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(titleInnerWidget);
    outerLayout->addWidget(Subtitle);
    /* 创建主界面 */
    defaultPage = new QWidget(ui->mainWidget);
    defaultPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //放置两个大图标
    bigIconButton *Data_display = new bigIconButton(":/icons/icons/DataDisplay.svg", "数据显示", 10, this);
    Data_display->setScale(0.9);
    bigIconButton *Motion_control = new bigIconButton(":/icons/icons/GamePad.svg", "运动控制", 10, this);

    //数据显示按钮槽函数连接
    connect(Data_display, &bigIconButton::clicked, this, [=](){
        //左图标事件槽函数
//        qDebug() << "Data_displayBtn";
//        ChangeDataDisplayWidget();
    });

    //运动控制按钮槽函数连接
    connect(Motion_control, &bigIconButton::clicked, this, [=](){
        //右图标事件槽函数
        qDebug() << "Motion_controlBtn";
        ChangeMotionControlWidget();
    });

    //两个大按钮进行水平布局
    QHBoxLayout *defaultPageLayout = new QHBoxLayout(defaultPage);
    defaultPage->setLayout(defaultPageLayout);
    defaultPageLayout->setContentsMargins(50, 30, 50, 80);
    defaultPageLayout->setSpacing(20);
    defaultPageLayout->addWidget(Data_display);
    defaultPageLayout->addWidget(Motion_control);

    ui->displayLayout->addWidget(titleWidget);
    ui->displayLayout->addWidget(defaultPage);
    ui->displayLayout->setAlignment(Qt::AlignTop);

}

/* 层页初始化函数 */
void MainWindow::InitLayersPage()
{
    layersPage = new SlidePage(cornerRadius, "功能选择", ui->mainWidget);

    //标题到可选项之间的留白
    QWidget *whiteSpace = new QWidget(layersPage);
    whiteSpace->setFixedHeight(30);

    modeSelGroup = new singleSelectGroup("模式", layersPage);
    selectionItem *DDModeItem = new selectionItem("数据显示", "展示所有传感器的数据", layersPage);
    selectionItem *MotionModeItem = new selectionItem("运动控制", "进行PID和手柄控制", layersPage);
    modeSelGroup->AddItem(DDModeItem);
    modeSelGroup->AddItem(MotionModeItem);

    textButton *ensureBtn = new textButton("确认", layersPage);

    //按照从下到上的顺序排布
    layersPage->AddContent(ensureBtn);          //确认按钮
    layersPage->AddContent(modeSelGroup);       //主模式组
    layersPage->AddContent(whiteSpace);         //留白
    layersPage->show(); //界面显示

    //将层页放入页表容器中
    pageList.push_back(layersPage);

    //按下层图标的时候，触发层页滑入
    connect(layersIcon, &customIcon::clicked, layersPage, &SlidePage::slideIn);

    connect(DDModeItem, &selectionItem::selected, this, [=](){
        //模式标志置数据显示
        Switch_Mode = DATADISPLAY;

    });
    connect(MotionModeItem, &selectionItem::selected, this, [=](){
        //模式标志置运动控制
        Switch_Mode = MOTIONCONTROl;

    });

    //按下确定键会根据模式标志切换到不同的界面
    connect(ensureBtn, &textButton::clicked, this, [=](){

        if(Switch_Mode == DATADISPLAY)
        {
            qDebug()<<"进入数据显示界面";
            //ChangeDataDisplayWidget();
        }
        else if(Switch_Mode == MOTIONCONTROl)
        {
            qDebug()<<"进入运动控制界面";
            ChangeMotionControlWidget();
        }

        layersPage->slideOut();
    });
}

/* 串口界面初始化 */
void MainWindow::InitSerialPage()
{
    //串口配置按钮 还有个 空出的按钮
    textButton *serialBtn = new textButton("串口配置", ui->titleBar,1.2);
    serialBtn->setMinimumWidth(50);
    textButton *internetBtn = new textButton("", ui->titleBar,1.2);
    internetBtn->setMinimumWidth(50);

    ui->horizontalLayout->insertWidget(1,serialBtn);
    ui->horizontalLayout->insertWidget(2,internetBtn);

    serial = new QSerialPort;
    serialDialog = new SlideDialog(cornerRadius,"串口配置",ui->mainWidget);

    QWidget *serialSetWidget = new QWidget(serialDialog);
    serialSetWidget->setMinimumHeight(200);
    //创建布局器
    QGridLayout *serialSetWidgetLayout = new QGridLayout(serialSetWidget);
    //布局器与QWidget之间的边际四周都为20
    serialSetWidgetLayout->setContentsMargins(20, 20, 20, 20);
    //设置指定列的伸缩因子 第一个参数为列，第二个参数为伸缩比例
    serialSetWidgetLayout->setColumnStretch(0,1);
    serialSetWidgetLayout->setColumnStretch(1,2);
    serialSetWidgetLayout->setColumnStretch(2,2);
    serialSetWidget->setLayout(serialSetWidgetLayout);

    QLabel *comPortLabel = new QLabel("串口号", serialSetWidget);
    QLabel *baudrateLabel = new QLabel("波特率", serialSetWidget);
    QLabel *dataBitsLabel = new QLabel("数据位", serialSetWidget);
    QLabel *checkBitsLabel = new QLabel("校验位", serialSetWidget);
    QLabel *stopBitsLabel = new QLabel("停止位", serialSetWidget);
    QLabel *flowCtlLabel = new QLabel("流控制", serialSetWidget);

    comPortLabel->setContentsMargins(0, 0, 18, 0);
    baudrateLabel->setContentsMargins(0, 0, 18, 0);
    dataBitsLabel->setContentsMargins(0, 0, 18, 0);
    checkBitsLabel->setContentsMargins(0, 0, 18, 0);
    stopBitsLabel->setContentsMargins(0, 0, 18, 0);
    flowCtlLabel->setContentsMargins(0, 0, 18, 0);

    comPortCBox = new QComboBox(serialSetWidget);
    baudrateCBox = new QComboBox(serialSetWidget);
    dataBitsCBox = new QComboBox(serialSetWidget);
    checkBitsCBox = new QComboBox(serialSetWidget);
    stopBitsCBox = new QComboBox(serialSetWidget);
    flowCtlCBox = new QComboBox(serialSetWidget);

    openSerialBtn = new textButton("打开串口", serialSetWidget);
    closeSerialBtn = new textButton("关闭串口", serialSetWidget);

    foreach(qint32 baud, QSerialPortInfo::standardBaudRates())
        baudrateCBox->addItem(QString::number(baud));
    baudrateCBox->setCurrentText("115200");
    serial->setBaudRate(baudrateCBox->currentText().toInt());

    dataBitsCBox->addItem(QString::number(5));
    dataBitsCBox->addItem(QString::number(6));
    dataBitsCBox->addItem(QString::number(7));
    dataBitsCBox->addItem(QString::number(8));
    dataBitsCBox->setCurrentText("8");
    serial->setDataBits(QSerialPort::DataBits(dataBitsCBox->currentText().toInt()));

    checkBitsCBox->addItem("NoParity");
    checkBitsCBox->addItem("EvenParity");
    checkBitsCBox->addItem("OddParity");
    checkBitsCBox->setCurrentText("NoParity");
    serial->setParity(QSerialPort::NoParity);

    stopBitsCBox->addItem(QString::number(1));
    stopBitsCBox->addItem(QString::number(1.5));
    stopBitsCBox->addItem(QString::number(2));
    stopBitsCBox->setCurrentText("1");
    serial->setStopBits(QSerialPort::OneStop);

    flowCtlCBox->addItem("NoFlowControl");
    flowCtlCBox->addItem("FlowControl");
    flowCtlCBox->setCurrentText("NoFlowControl");
    serial->setFlowControl(QSerialPort::NoFlowControl);

    serialSetWidgetLayout->addWidget(comPortLabel, 0, 0, Qt::AlignRight);
    serialSetWidgetLayout->addWidget(baudrateLabel, 1, 0, Qt::AlignRight);
    serialSetWidgetLayout->addWidget(dataBitsLabel, 2, 0, Qt::AlignRight);
    serialSetWidgetLayout->addWidget(checkBitsLabel, 3, 0, Qt::AlignRight);
    serialSetWidgetLayout->addWidget(stopBitsLabel, 4, 0, Qt::AlignRight);
    serialSetWidgetLayout->addWidget(flowCtlLabel, 5, 0, Qt::AlignRight);

    serialSetWidgetLayout->addWidget(comPortCBox, 0, 1);
    serialSetWidgetLayout->addWidget(baudrateCBox, 1, 1);
    serialSetWidgetLayout->addWidget(dataBitsCBox, 2, 1);
    serialSetWidgetLayout->addWidget(checkBitsCBox, 3, 1);
    serialSetWidgetLayout->addWidget(stopBitsCBox, 4, 1);
    serialSetWidgetLayout->addWidget(flowCtlCBox, 5, 1);

    serialSetWidgetLayout->addWidget(openSerialBtn, 0, 2, 3, 1);
    serialSetWidgetLayout->addWidget(closeSerialBtn, 3, 2, 3, 1);

    serialDialog->AddContent(serialSetWidget);
    serialDialog->show();

    pageList.push_back(serialDialog);

    //按下串口设置按钮时，设置串口串口滑入
    connect(serialBtn, &textButton::clicked, serialDialog, &SlidePage::slideIn);
    //按下串口设置按钮时，设置界面中comPortCBox的值
    connect(serialBtn, &textButton::clicked, this, [=](){
        comPortCBox->clear();
        foreach(QSerialPortInfo portInfo, QSerialPortInfo::availablePorts())
            comPortCBox->addItem(portInfo.portName()+":"+portInfo.description());
        openSerialBtn->setEnabled(comPortCBox->count()>0);
        serial->setPortName(comPortCBox->currentText());
    });

    //按下串口设置按钮时，设置串口串口滑入
    connect(Data_display, &bigIconButton::clicked, serialDialog, &SlidePage::slideIn);
    //按下串口设置按钮时，设置界面中comPortCBox的值
    connect(Data_display, &bigIconButton::clicked, this, [=](){
        comPortCBox->clear();
        foreach(QSerialPortInfo portInfo, QSerialPortInfo::availablePorts())
            comPortCBox->addItem(portInfo.portName()+":"+portInfo.description());
        openSerialBtn->setEnabled(comPortCBox->count()>0);
        serial->setPortName(comPortCBox->currentText());
    });

    //按下打开串口按钮时，打开串口
    connect(openSerialBtn, &textButton::clicked, this, &MainWindow::OpenSerialPort);
    //按下关闭串口按钮时，关闭串口
    connect(closeSerialBtn, &textButton::clicked, this, &MainWindow::CloseSerialPort);
    //当串口数据准备可读时，读取其中的数据
    connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadData);

    //接收到读取完成信号，根据界面隐藏情况，显示在不同界面的接收框中
    //connect(this,&MainWindow::DataReadCplt,dataDisplayWidget,&DataDisplayWidget::DataDisplayPTE);
    connect(this,&MainWindow::DataReadCplt,motionControlWidget,&MotionControlWidget::DataDisplayPTE);

    //接收到数据显示界面发送信号，往串口中写入发送框的数据
//    connect(dataDisplayWidget,&DataDisplayWidget::SendDataSignal,this,[=]()
//    {
//        if(!dataDisplayWidget->isHidden())  //如果数据显示界面没有被隐藏
//        {
//            qDebug() << "从数据显示界面获取，串口开始发送";
//            serial->write(dataDisplayWidget->logTII->value().toLocal8Bit().data());
//        }
//    });

    //接收到运动控制发送信号，往串口中写入发送框的数据
    connect(motionControlWidget,&MotionControlWidget::SendDataSignal,this,[=]()
    {
        if(!motionControlWidget->isHidden())    //如果运动控制界面没有被隐藏
        {
            qDebug() << "从运动控制界面获取，串口开始发送";
            serial->write(motionControlWidget->logTII->value().toLocal8Bit().data());
            //qDebug() << dataDisplayWidget->logTII->value().toLocal8Bit().data();
        }
    });

    //接收到按键或者鼠标点击发送信号，往串口中写入信号携带的数据
    connect(motionControlWidget,&MotionControlWidget::SendControlSignal,this,[=](QString str)
    {
        qDebug() << "获取控制信号，串口开始发送" << str;
        serial->write(QString("C " + str + "\r\n").toLocal8Bit().data());
    });

    //接收到PID设置信号，往串口中写入PID值
    connect(motionControlWidget,&MotionControlWidget::SetPIDSignal,this,[=](){
        qDebug() << "设置PID值";
        //根据协议传输PID值 形如PID X X X
        QString PIDValue;
        PIDValue = "PID "
                 + QString(motionControlWidget->PID_P_TII->value()) + " "
                 + QString(motionControlWidget->PID_I_TII->value()) + " "
                 + QString(motionControlWidget->PID_D_TII->value()) + "\r\n";
        qDebug() << PIDValue;
        serial->write(PIDValue.toLocal8Bit().data());
    });
}

/* 初始化数据显示窗口 */
void MainWindow::InitDataDisplayWidget()
{
    dataDisplayWidget = new DataDisplayWidget(cornerRadius, ui->mainWidget);
    dataDisplayWidget->hide();
    dataDisplayWidget->settingPage()->setParent(ui->mainWidget);
    pageList.push_back(dataDisplayWidget->settingPage());

}

/* 初始化运动控制窗口 */
void MainWindow::InitMotionControlWidget()
{
    motionControlWidget = new MotionControlWidget(cornerRadius, ui->mainWidget);
    motionControlWidget->hide();
    motionControlWidget->settingPage()->setParent(ui->mainWidget);
    pageList.push_back(motionControlWidget->settingPage());
}

/* 切换到数据显示窗口 */
void MainWindow::ChangeDataDisplayWidget()
{

    //旧界面隐藏
    ui->displayLayout->removeWidget(defaultPage);
    defaultPage->hide();
    motionControlWidget->hide();

    //切换数据显示界面
    ui->displayLayout->addWidget(dataDisplayWidget);
    curSettingsPage = dataDisplayWidget->settingPage();
    Subtitle->setText("DataDisplay");
    dataDisplayWidget->show();

}

void MainWindow::ChangeMotionControlWidget()
{
    //旧界面隐藏
    ui->displayLayout->removeWidget(defaultPage);
    defaultPage->hide();
    dataDisplayWidget->hide();

    //切换到运动控制界面
    ui->displayLayout->addWidget(motionControlWidget);
    curSettingsPage = motionControlWidget->settingPage();
    Subtitle->setText("MotionControl");
    motionControlWidget->show();
}

/* 打开串口槽函数 */
void MainWindow::OpenSerialPort()
{
    if (serial->isOpen())
    {
        QMessageBox::warning(this,"错误","串口已打开");
        return;
    }

    //设置串口名称
    QString textPortName;
    textPortName = comPortCBox->currentText();
    //根据“：”分割字符串，保留COMx部分
    QStringList PortName = textPortName.split(":");
    //qDebug() << PortName.at(0) << PortName.at(1);
    serial->setPortName(PortName[0]);

    //设置串口通信参数
    serial->setBaudRate(baudrateCBox->currentText().toInt());//设置波特率

    serial->setDataBits(QSerialPort::DataBits(dataBitsCBox->currentText().toInt()));//数据位，默认8位

    //获取校验位控件上面的数据
    QSerialPort::Parity checkBits  = QSerialPort::NoParity;
    if(checkBitsCBox->currentText() == "NoParity") checkBits = QSerialPort::NoParity;
    else if(checkBitsCBox->currentText() == "OddParity") checkBits = QSerialPort::OddParity;
    else if(checkBitsCBox->currentText() == "EvenParity") checkBits = QSerialPort::EvenParity;

    serial->setParity(checkBits);

    if (serial->open(QIODeviceBase::ReadWrite))
    {
        comPortCBox->setEnabled(false);  //串口设置面板
        openSerialBtn->setEnabled(false);
        closeSerialBtn->setEnabled(true);

        QMessageBox::information(this,"提示信息","串口已经被成功打开");
    }
    else
        QMessageBox::warning(this,"错误","打开串口失败");
}

/* 关闭串口槽函数 */
void MainWindow::CloseSerialPort()
{
    if (serial->isOpen())
    {
        serial->close();
        openSerialBtn->setEnabled(true);
        closeSerialBtn->setEnabled(false);
        QMessageBox::information(this,"提示信息","串口关闭成功");
        return;
    }
}

/* 数据读取发射信号槽函数 */
void MainWindow::ReadData()
{
    QString serialBuf;    //储存接收到的数据
    serialBuf = QString(serial->readLine());
    //qDebug() << buf;

    //发射信号给datadisplay窗口
    emit DataReadCplt(serialBuf);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() == Qt::NoButton)
        mousePressed = false;
    if(!mousePressed){
        mouseState = 0;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x()) < 5)
            mouseState |= AT_LEFT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y()) < 5)
            mouseState |= AT_TOP;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x() - ui->mainWidget->width()) < 5)
            mouseState |= AT_RIGHT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y() - ui->mainWidget->height()) < 5)
            mouseState |= AT_BOTTOM;
        if(mouseState == AT_TOP_LEFT  || mouseState == AT_BOTTOM_RIGHT)
            setCursor(Qt::SizeFDiagCursor);
        else if(mouseState == AT_TOP_RIGHT || mouseState == AT_BOTTOM_LEFT)
            setCursor(Qt::SizeBDiagCursor);
        else if(mouseState & (AT_LEFT | AT_RIGHT))
            setCursor(Qt::SizeHorCursor);
        else if(mouseState & (AT_TOP | AT_BOTTOM))
            setCursor(Qt::SizeVerCursor);
        else
            unsetCursor();
    }
    else{
        if(mouseState == 0){
            if(maximized){
                qreal wRatio = (double)event->pos().x() / (double)ui->mainWidget->width();
                controlWindowScale();
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(QPoint(event->globalPosition().x() - ui->mainWidget->width() * wRatio, -30));
#else
                this->move(QPoint(event->globalPos().x() - ui->mainWidget->width() * wRatio, -30));
#endif
                lastPos = QPoint(ui->mainWidget->width() * wRatio, event->pos().y());
            }
            else
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(event->globalPosition().toPoint() - lastPos);
#else
                this->move(event->globalPos() - lastPos);
#endif
        }
        else{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
            QPoint d = event->globalPosition().toPoint() - frameGeometry().topLeft() - lastPos;
#else
            QPoint d = event->globalPos() - frameGeometry().topLeft() - lastPos;
#endif
            if(mouseState & AT_LEFT){
                this->move(this->frameGeometry().x() + d.x(), this->frameGeometry().y());
                this->resize(this->width() - d.x(), this->height());
            }
            if(mouseState & AT_RIGHT){
                this->resize(this->width() + d.x(), this->height());
            }
            if(mouseState & AT_TOP){
                this->move(this->frameGeometry().x(), this->frameGeometry().y() + d.y());
                this->resize(this->width(), this->height() - d.y());
            }
            if(mouseState & AT_BOTTOM){
                this->resize(this->width(), this->height() + d.y());
            }
        }
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){
    //Resize border
    if(border)
        border->resize(ui->mainWidget->size() + QSize(2, 2));

    //Resize mask
    QPainterPath path;
#ifdef Q_OS_WINDOWS
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
#else
    path.addRect(ui->mainWidget->rect());
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    //Resize all pages
    for(int i = 0; i < pageList.size(); i++){
        pageList[i]->resize(ui->mainWidget->width() * 0.4 < pageList[i]->preferWidth ? pageList[i]->preferWidth - 1 : ui->mainWidget->width() * 0.4 - 1, ui->mainWidget->height());
        pageList[i]->resize(pageList[i]->width() + 1, pageList[i]->height());
    }
}

void MainWindow::controlWindowScale(){
#ifdef Q_OS_WINDOWS
    if(!maximized){
        lastGeometry = this->frameGeometry();
        windowShadow->setEnabled(false);
        ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
        border->hide();
        QString mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + ";border-radius:0px;}";
        ui->mainWidget->setStyleSheet(mainStyle);
        this->showMaximized();
        maximized = true;
        QPainterPath path;
        path.addRect(ui->mainWidget->rect());
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
    }
    else{
        ui->verticalLayout->setContentsMargins(30, 30, 30, 30);
        this->showNormal();
        QString mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
        ui->mainWidget->setStyleSheet(mainStyle);
        QPainterPath path;
        path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
        border->show();
        windowShadow->setEnabled(true);
        this->resize(lastGeometry.width(), lastGeometry.height());
        this->move(lastGeometry.x(), lastGeometry.y());
        maximized = false;
    }
#endif
}

