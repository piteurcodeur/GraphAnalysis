#include "Graph3D.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DRender/QCameraLens>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector3D>
#include <QVector>
#include <QMatrix4x4>
#include <QTimer>
#include <cmath>  // Pour acos et autres fonctions mathématiques

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Graph3D::Graph3D(QWidget *parent) : QWidget(parent) {
    // 🌟 Création de la fenêtre Qt3D
    view = new Qt3DExtras::Qt3DWindow();
    container = QWidget::createWindowContainer(view);
    container->setMinimumSize(640, 480);
    layout = new QVBoxLayout(this);
    layout->addWidget(container);
    setLayout(layout);

    // 🌟 Création de la scène 3D
    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    // Configuration de la caméra
    view->camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    view->camera()->setPosition(QVector3D(0, 0, 10));
    view->camera()->setViewCenter(QVector3D(0, 0, 0));
    
    // Définir la couleur de fond - utiliser une méthode compatible avec votre version Qt
    // Pour les versions plus anciennes, n'utilisez pas setClearColor
    // Remarque: Nous supprimons cette ligne car elle n'est pas disponible dans votre version de Qt
    // Si vous avez besoin de définir la couleur de fond, vous devrez créer un framegraph personnalisé
    
    // 🌟 Création du graphe 3D
    createGraph();
    
    // Timer pour mettre à jour les positions des labels
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Graph3D::updateLabels);
    timer->start(16); // ~60 FPS
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
    
    // 🔹 Liste des arêtes (paires d'indices de sommets)
    QVector<QPair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}
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
        label->raise(); // S'assurer que le label est au-dessus
        label->hide(); // Masquer initialement
        labels.append(label);
    }
    
    // 🔹 Création des arêtes (cylindres)
    for (const auto &edge : edges) {
        createEdge(edge.first, edge.second);
    }

    // 🔹 Mise à jour des étiquettes
    updateLabels();
}

void Graph3D::createEdge(int fromNode, int toNode) {
    QVector3D fromPos = nodePositions[fromNode];
    QVector3D toPos = nodePositions[toNode];
    
    // Calculer le vecteur direction
    QVector3D direction = toPos - fromPos;
    float length = direction.length();
    direction.normalize();
    
    // Calculer la rotation pour aligner le cylindre
    QVector3D startVector(0, 1, 0); // Vecteur initial du cylindre
    QVector3D rotationAxis = QVector3D::crossProduct(startVector, direction);
    float dotProduct = QVector3D::dotProduct(startVector, direction);
    float rotationAngle = std::acos(dotProduct);  // Utilisez std::acos au lieu de acos
    
    // Création du cylindre
    auto *cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius(0.05f);
    cylinderMesh->setLength(length);
    
    auto *transform = new Qt3DCore::QTransform();
    transform->setTranslation(fromPos + direction * length / 2);
    
    if (rotationAxis.length() > 0.001f) { // Éviter la division par zéro
        // Convertir les radians en degrés manuellement
        transform->setRotation(QQuaternion::fromAxisAndAngle(rotationAxis.normalized(), 
                                                          rotationAngle * 180.0f / M_PI));
    }
    
    auto *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(200, 200, 200));
    
    auto *edgeEntity = new Qt3DCore::QEntity(rootEntity);
    edgeEntity->addComponent(cylinderMesh);
    edgeEntity->addComponent(transform);
    edgeEntity->addComponent(material);
}

void Graph3D::updateLabels() {
    if (!isVisible()) {
        return;
    }
    
    // Obtenir la position de la caméra
    Qt3DRender::QCamera *camera = view->camera();
    
    for (int i = 0; i < labels.size(); ++i) {
        QVector3D worldPos = nodePositions[i];
        
        // Convertir les coordonnées 3D en coordonnées 2D de l'écran
        QVector3D viewPos = worldPos;
        
        // Appliquer les transformations de vue et de projection
        QMatrix4x4 viewMatrix = camera->viewMatrix();
        QMatrix4x4 projectionMatrix = camera->projectionMatrix();
        QVector4D clipPos = projectionMatrix * viewMatrix * QVector4D(viewPos, 1.0f);
        
        // Si le point est derrière la caméra, cacher le label
        if (clipPos.z() > clipPos.w()) {
            labels[i]->hide();
            continue;
        }
        
        // Convertir en coordonnées normalisées (-1 à 1)
        QVector3D ndcPos = QVector3D(clipPos.x() / clipPos.w(), 
                                   clipPos.y() / clipPos.w(), 
                                   clipPos.z() / clipPos.w());
        
        // Vérifier si le point est dans le frustum de la caméra
        if (ndcPos.x() < -1.0f || ndcPos.x() > 1.0f || 
            ndcPos.y() < -1.0f || ndcPos.y() > 1.0f) {
            labels[i]->hide();
            continue;
        }
        
        // Convertir en coordonnées d'écran
        float screenX = ((ndcPos.x() + 1.0f) / 2.0f) * container->width();
        float screenY = ((1.0f - ndcPos.y()) / 2.0f) * container->height();
        
        // Positionner le label
        labels[i]->move(screenX - labels[i]->width() / 2, screenY - labels[i]->height() / 2);
        labels[i]->show();
    }
}

void Graph3D::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateLabels();
}

void Graph3D::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    updateLabels();
}