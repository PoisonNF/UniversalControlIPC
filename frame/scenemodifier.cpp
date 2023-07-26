#include "scenemodifier.h"

#include <QtCore/QDebug>

SceneModifier::SceneModifier(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{

    auv = new Qt3DCore::QEntity(m_rootEntity);

//    QString relativePath = "Crucis3D.obj";
//    QString absolutePath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()
//                                                    + QDir::separator()
//                                                    + relativePath);
//    qDebug() << absolutePath;
    mesh = new Qt3DRender::QMesh();
    mesh->setSource(QUrl("file:///E:/Desktop/CrucisIPC/Crucis3D.obj")); //路径要随着存储位置进行更改，目前只能使用绝对路径

    material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0xbeb32b)));						//漫反射颜色
    material->setShininess(0.0f);

    objTransformMesh = new Qt3DCore::QTransform();
    objTransformMesh->setScale(0.0018f);
    //objTransformMesh->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
    objTransformMesh->setRotation(QQuaternion::fromEulerAngles(270, 0, 180));
    objTransformMesh->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

    auv->addComponent(mesh);
    auv->addComponent(material);
    auv->addComponent(objTransformMesh);


}

SceneModifier::~SceneModifier()
{
}

void SceneModifier::OnSetRotation(QString pitch, QString yaw, QString roll)
{
    float pitchF = pitch.toFloat();
    float yawF = yaw.toFloat();
    float rollF = roll.toFloat();

    objTransformMesh->setRotation(QQuaternion::fromEulerAngles(pitchF, yawF, rollF));
//    objTransformMesh->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), pitchF));
//    objTransformMesh->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), yawF));
//    objTransformMesh->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), rollF));
}
