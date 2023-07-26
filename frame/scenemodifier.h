#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/qmesh.h>

#include <QCoreApplication>
#include <QDir>

class SceneModifier : public QObject
{
    Q_OBJECT

public:
    explicit SceneModifier(Qt3DCore::QEntity *rootEntity);
    ~SceneModifier();

public slots:
    void OnSetRotation(QString, QString, QString);

private:
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DCore::QEntity *auv;
    Qt3DRender::QMesh *mesh;
    Qt3DExtras::QPhongMaterial *material;
    Qt3DCore::QTransform *objTransformMesh;

};

#endif // SCENEMODIFIER_H

