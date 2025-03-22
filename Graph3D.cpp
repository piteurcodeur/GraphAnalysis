#include "Graph3D.h"
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
#include <QMatrix4x4>

Graph3D::Graph3D(QWidget *parent) : QWidget(parent) {
    // 🌟 Création de la fenêtre Qt3D
    view = new Qt3DExtras::Qt3DWindow();
    container = QWidget::createWindowContainer(view);
    layout = new QVBoxLayout(this);
    layout->addWidget(container);
    setLayout(layout);

    // 🌟 Création de la scène 3D
    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    // 🌟 Création du graphe 3D
    createGraph();
}

Graph3D::~Graph3D() {
    // Nettoyage des labels
    qDeleteAll(labels);
}

void Graph3D::createGraph() {
    // 🔹 Liste des positions des sommets (en 3D)
    nodePositions = {
        QVector3D(0, 0, 0),
        QVector3D(2, 1, -1),
        QVector3D(-2, -1, 2),
        QVector3D(1, -2, 3),
        QVector3D(-3, 2, -2)
    };

    // 🔹 Création des nœuds (sphères)
    for (int i = 0; i < nodePositions.size(); ++i) {
        // 🌟 Création de la géométrie de la sphère
        auto *sphereMesh = new Qt3DExtras::QSphereMesh();
        sphereMesh->setRadius(0.2f);

        auto *transform = new Qt3DCore::QTransform();
        transform->setTranslation(nodePositions[i]);

        auto *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor(100, 100, 255));

        auto *nodeEntity = new Qt3DCore::QEntity(rootEntity);
        nodeEntity->addComponent(sphereMesh);
        nodeEntity->addComponent(transform);
        nodeEntity->addComponent(material);

        // 🌟 Ajout du label pour le numéro du sommet
        QLabel *label = new QLabel(QString::number(i), this);
        label->setStyleSheet("font-size: 16px; color: white; background-color: rgba(0, 0, 0, 128); border-radius: 5px;");
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(30, 30);
        labels.append(label);
    }

    // 🔹 Mise à jour des étiquettes
    updateLabels();
}

void Graph3D::updateLabels() {
    // Obtenir la position de la caméra
    Qt3DRender::QCamera *camera = view->camera();

    for (int i = 0; i < labels.size(); ++i) {
        // Convertir la position 3D en position 2D sur l'écran
        QVector3D worldPos = nodePositions[i];
        QMatrix4x4 viewMatrix = camera->viewMatrix();
        QMatrix4x4 projectionMatrix = camera->projectionMatrix();
        QVector3D screenPos = projectionMatrix * viewMatrix * worldPos;

        // Normaliser les coordonnées de l'écran
        screenPos /= screenPos.z();

        // Convertir les coordonnées normalisées en coordonnées de l'écran
        QPoint screenPoint = view->mapToGlobal(QPoint(screenPos.x(), screenPos.y()));

        // 🔹 Positionner le label sur l'écran en fonction de la caméra
        labels[i]->move(screenPoint.x(), screenPoint.y());
    }
}
