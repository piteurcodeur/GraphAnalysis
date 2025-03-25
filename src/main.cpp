#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <cmath>
#include <limits>


#ifdef _WIN32
    #include <windows.h>
    #undef max
#endif

#include "graph_analysis.h"
#include <QApplication>
#include <QWindow>
#include <QSurfaceFormat>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QHBoxLayout>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>


using namespace std;
using namespace boost;

#define WIDTH 800
#define HEIGHT 600

class GraphView : public QGraphicsView
{
public:
    GraphView(QWidget *parent = nullptr) : QGraphicsView(parent)
    {
        scene = new QGraphicsScene(this);
        setScene(scene);
    }

    void drawGraph(const Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
    {
        scene->clear();

        // Dessiner les arêtes
        graph_traits<Graph>::edge_iterator ei, ei_end;
        for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        {
            Edge e = *ei;
            Vertex u = source(e, g);
            Vertex v = target(e, g);
            //float weight = get(boost::edge_weight, g, e);
            double x1 = get<0>(coord_list[u]);
            double y1 = get<1>(coord_list[u]);
            double x2 = get<0>(coord_list[v]);
            double y2 = get<1>(coord_list[v]);

            scene->addLine(x1, y1, x2, y2, QPen(Qt::black));
        }

        // Dessiner les sommets
        for (size_t i = 0; i < coord_list.size(); ++i)
        {
            double x = get<0>(coord_list[i]);
            double y = get<1>(coord_list[i]);
            
            double z = get<2>(coord_list[i]);
            //plus z est grand plus le sommet est proche de l'utilisateur
            //plus z est petit plus le sommet est loin de l'utilisateur
            //plus z est petit, plus la couleur du rond est foncée
            //plus z est grand, plus la couleur du rond est claire
            int r = 80 + ((z+1000)/2000)*(255-80); //variation de rouge en fonction de z
            //int r = 255 - ((z+1000)/2000)*(255-80); //variation de rouge en fonction de z
            QColor color(r, 0, 0); 
            //cout << z << "-> r="<<r << endl;

            //QGraphicsEllipseItem *ellipse = 
            scene->addEllipse(x-50, y-50 , 100, 100, QPen(Qt::black), QBrush(color));
            //ellipse->setToolTip(QString("Node %1").arg(i));


            //écrire dans l'élipce le numéro du sommet
            QGraphicsTextItem *text = scene->addText(QString::number(i+1));
            if(i<10)
                text->setPos(x-25, y-45);
            else{
                text->setPos(x-45, y-45);
            }
            //changer la police et la taille de la police
            QFont font = text->font();
            font.setPointSize(40);
            text->setFont(font);
            //ellipse->setFlag(QGraphicsItem::ItemIsMovable);
            //ellipse->setFlag(QGraphicsItem::ItemIsSelectable);

        }
        // Ajuster la vue pour s'adapter à la scène
        fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }   

    protected:
        // Redéfinir l'événement de redimensionnement pour ajuster automatiquement la vue
        void resizeEvent(QResizeEvent *event) override
        {
            QGraphicsView::resizeEvent(event);
            fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        }

    private:
        QGraphicsScene *scene;
};



class ColorLegend : public QWidget {
    public:
        ColorLegend(QWidget *parent = nullptr) : QWidget(parent) {
            setMinimumSize(100, 300); // Taille minimale pour l'affichage
        }
    
    protected:
        void paintEvent(QPaintEvent *event) override {
            QPainter painter(this);
            
            // Création du dégradé vertical
            QLinearGradient gradient(20, 10, 20, height() - 10);
            gradient.setColorAt(0, QColor(255, 0, 0)); // Rouge clair en haut
            gradient.setColorAt(1, QColor(80, 0, 0));     // Rouge foncé en bas
    
            // Dessiner la barre avec le dégradé
            QRectF rect(20, 40, 20, height() - 60);
            painter.fillRect(rect, gradient);
    
            // Dessiner les valeurs sur le côté droit
            painter.setPen(Qt::black);
            int numLabels = 5;
            for (int i = 0; i < numLabels; i++) {
                int y = 40 + i * (height() - 70) / (numLabels - 1);
                //100 - i * 25
                //valeur de -1000 à 1000+
                int valeur = 1000 - i * 2000 / (numLabels - 1);
                QString value = QString::number(valeur);
                //QString value = QString::number(1000 + ((i+4)/4) * 2000) + "%"; // Ex: 100%, 75%, 50%...
                painter.drawText(50, y + 10, value);
            }
            // Afficher le titre
            //taille du texte = 20
            painter.setFont(QFont("Arial", 10));
            painter.drawText(10, 20, "Distance : ");
        }
    };


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //app.setWindowIcon(QIcon("icons/app_icon.png"));
   
    

    #ifdef _WIN32
    // Configurer la console pour l'UTF-8
    SetConsoleOutputCP(CP_UTF8);
    
    // Activer les modes de police de la console qui supportent les caractères Unicode
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif

    cout << "\n+-----------------------------------------------------------+" << endl;
    cout << "|                                                           |" << endl;
    cout << "|                ███╗   ███╗██╗███╗   ██╗██╗                |" << endl;
    cout << "|                ████╗ ████║██║████╗  ██║██║                |" << endl;
    cout << "|                ██╔████╔██║██║██╔██╗ ██║██║                |" << endl;
    cout << "|                ██║╚██╔╝██║██║██║╚██╗██║██║                |" << endl;
    cout << "|                ██║ ╚═╝ ██║██║██║ ╚████║██║                |" << endl;
    cout << "|                ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝                |" << endl;
    cout << "|                                                           |" << endl;
    cout << "|    ██████╗ ██████╗  ██████╗      ██╗███████╗████████╗     |" << endl;
    cout << "|    ██╔══██╗██╔══██╗██╔═══██╗     ██║██╔════╝╚══██╔══╝     |" << endl;
    cout << "|    ██████╔╝██████╔╝██║   ██║     ██║█████╗     ██║        |" << endl;
    cout << "|    ██╔═══╝ ██╔══██╗██║   ██║██   ██║██╔══╝     ██║        |" << endl;
    cout << "|    ██║     ██║  ██║╚██████╔╝╚█████╔╝███████╗   ██║        |" << endl;
    cout << "|    ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚════╝ ╚══════╝   ╚═╝        |" << endl;
    cout << "|                                                           |" << endl;
    cout << "|             GRAPH REPRESENTATION AND ANALYSIS             |" << endl;
    cout << "|                                                           |" << endl;
    cout << "+-----------------------------------------------------------+\n" << endl;
    
    // Création du Graphe
    Graph g;

    const string filename_edges = "data/edges.csv"; /* contient ID1;ID2 */
    const string filename_nodes = "data/nodes.csv"; /* contient ID;x;y;z */
    vector<string> edges; /* liste des arrêtes */
    vector<string> nodes; /* liste des sommets */

    // Lire les arêtes et les sommets depuis les fichiers CSV
    readCSV(filename_edges, edges);
    readCSV(filename_nodes, nodes);

    /*
    cout << "Nombre d'arêtes lues: " << edges.size() << endl;
    cout << "Nombre de noeuds lus: " << nodes.size() << endl;
    */

    /* Création de la liste des arêtes */
    vector<pair<int, int>> edge_list;
    for (const auto &line : edges)
    {
        int node1, node2;
        sscanf(line.c_str(), "%d;%d", &node1, &node2);
        edge_list.emplace_back(node1 - 1, node2 - 1);  // -1 car les IDs commencent à 1 mais les indices à 0
    }

    /* Création de la liste des coordonnées 3D */
    vector<std::tuple<double, double, double>> coord_list;
    for (const auto &line : nodes)
    {
        int node_id;
        double x, y, z;
        sscanf(line.c_str(), "%d;%lf;%lf;%lf", &node_id, &x, &y, &z);
        coord_list.emplace_back(x, y, z);
    }

    // Ajout des arêtes avec distances
    for (const auto &edge : edge_list)
    {
        double distance = calcul_distance(edge.first, edge.second, coord_list);
        add_edge(edge.first, edge.second, property<edge_weight_t, float>(distance), g);
    }

    /*
    cout << "Nombre d'arêtes obtenus : " << num_edges(g) << endl;
    cout << "Nombre de sommets obtenus: " << num_vertices(g) << endl;
    */
    

    // Analyse du graphe
    compute_graph(g, coord_list);

    // Création du widget principal qui contiendra le graphe et la légende
    QWidget mainWidget;
    mainWidget.setWindowTitle("Graph Visualization");
    mainWidget.resize(WIDTH + 150, HEIGHT + 150); // Augmenter la largeur pour la légende

    // Layout principal en horizontal (gauche = graphe, droite = légende)
    QHBoxLayout *layout = new QHBoxLayout(&mainWidget);

    // Widget du graphe
    GraphView *graphView = new GraphView();
    graphView->resize(WIDTH, HEIGHT);
    graphView->drawGraph(g, coord_list);
    layout->addWidget(graphView);

    // Widget de la légende
    ColorLegend *colorLegend = new ColorLegend();
    layout->addWidget(colorLegend);
    

    // Affichage de la fenêtre principale
    mainWidget.show();


    return app.exec();
}


