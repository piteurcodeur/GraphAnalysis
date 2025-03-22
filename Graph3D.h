#ifndef GRAPH3D_H
#define GRAPH3D_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QCamera>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector3D>
#include <QVector>

class Graph3D : public QWidget
{
    Q_OBJECT

public:
    Graph3D(QWidget *parent = nullptr);
    ~Graph3D();

private:
    void createGraph();
    void updateLabels();

    Qt3DExtras::Qt3DWindow *view;
    QWidget *container;
    QVBoxLayout *layout;
    Qt3DCore::QEntity *rootEntity;
    QVector<QLabel*> labels;
    QVector<QVector3D> nodePositions;
};

#endif // GRAPH3D_H
