#include "joysticks.h"

Joysticks::Joysticks(QObject *parent)
    :QObject{parent}
{
    Q_UNUSED(parent);

    JoyData.LeftX = 0.0;
    JoyData.LeftY = 0.0;

    timer = new QTimer(this);
    //到达超时时间
    connect(timer, &QTimer::timeout,this,[=](){
        if(JoyData.LeftX != 0 || JoyData.LeftY != 0)
        {
            qDebug() << "X" << std::floor(JoyData.LeftX) << "Y" << std::floor(JoyData.LeftY);
            //根据正切值算角度[0,360]
            if(JoyData.LeftX > 0 && JoyData.LeftY < 0)  JoyData.Angle = angleFromTangent((-JoyData.LeftY)/JoyData.LeftX);
            else if(JoyData.LeftX < 0) JoyData.Angle = angleFromTangent((-JoyData.LeftY)/JoyData.LeftX) + 180;
            else if(JoyData.LeftX > 0 && JoyData.LeftY > 0)JoyData.Angle = angleFromTangent((-JoyData.LeftY)/JoyData.LeftX) + 270;
            //计算长度
            JoyData.Length = pow((pow(JoyData.LeftX,2)+pow(JoyData.LeftY,2)),0.5);
            if(JoyData.Length > 100) JoyData.Length = 100;
            qDebug() << "Angle" << std::floor(JoyData.Angle) << "Length" << std::floor(JoyData.Length);
            emit sigJoysticksValueGet(JoyData);
        }
    });
    // 启动定时器和事件循环
    timer->start(500);
}

Joysticks::~Joysticks()
{
    timer->deleteLater();
}

//摇杆数据处理任务函数
void Joysticks::Joysticksworking(int js_index, int axis_index, qreal value)
{
    Q_UNUSED(js_index);
    //左右x
    if (axis_index == 0){
        JoyData.LeftX = value*100;
    }
    //上下y
    if (axis_index == 1){
        JoyData.LeftY = value*100;
    }
}

//按键数据处理任务函数
void Joysticks::Joysticksworking(int js_index, int button_index, bool pressed)
{
    Q_UNUSED(js_index);
    qDebug() << button_index << pressed;
    JoyData.Button = button_index;
    JoyData.Status = pressed;

    emit sigJoysticksButtonGet(JoyData);
}


