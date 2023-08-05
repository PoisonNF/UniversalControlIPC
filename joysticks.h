#ifndef JOYSTICKS_H
#define JOYSTICKS_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <QTimer>
#include <cmath>

class Joysticks : public QObject
{
    Q_OBJECT
public:
    explicit Joysticks(QObject *parent = nullptr);
    ~Joysticks();

    typedef struct
    {
        //摇杆坐标信息
        double LeftX;
        double LeftY;

        //换算成角度和推进力[0,100]
        double Angle;
        double Pro;

        //按键编号和状态
        int Button;
        bool Status;
    }JoystickData;

    JoystickData JoyData;

public slots:
    void Joysticksworking(int js_index, int axis_index, qreal value);
    void Joysticksworking(int js_index, int button_index, bool pressed);

private:
    QTimer *timer;  //定时器

    // 将正切值转换为角度值
    double angleFromTangent(double tanValue) {
        double angleInRadians = std::atan(tanValue);  // 计算反正切值（弧度）
        double angleInDegrees = qRadiansToDegrees(angleInRadians);  // 将弧度转换为角度
        return angleInDegrees;
    }

signals:
    void sigJoysticksValueGet(Joysticks::JoystickData JoyData);
    void sigJoysticksButtonGet(Joysticks::JoystickData JoyData);
};

#endif // JOYSTICKS_H
