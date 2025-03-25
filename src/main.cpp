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
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
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

        setRenderHint(QPainter::Antialiasing);
        setRenderHint(QPainter::TextAntialiasing);
        setRenderHint(QPainter::SmoothPixmapTransform);
        setRenderHint(QPainter::HighQualityAntialiasing);
    }

    void drawEdges(const Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
    {
        // Draw edges
        for (auto [ei, ei_end] = edges(g); ei != ei_end; ++ei)
        {
            auto e = *ei;
            auto u = source(e, g);
            auto v = target(e, g);
            double x1 = get<0>(coord_list[u]);
            double y1 = get<1>(coord_list[u]);
            double x2 = get<0>(coord_list[v]);
            double y2 = get<1>(coord_list[v]);

            scene->addLine(x1, y1, x2, y2, QPen(Qt::black, 6));
        }
    }

    void drawVertices(const Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
    {
        // Draw vertices
        for (size_t i = 0; i < coord_list.size(); ++i)
        {
            double x = get<0>(coord_list[i]);
            double y = get<1>(coord_list[i]);
            double z = get<2>(coord_list[i]);

            int r = 80 + ((z + 1000) / 2000) * (255 - 80);
            QColor color(r, 0, 0);

            scene->addEllipse(x - 50, y - 50, 100, 100, QPen(Qt::black), QBrush(color));

            auto *text = scene->addText(QString::number(i + 1));
            text->setPos(x - (i < 10 ? 25 : 45), y - 45);
            QFont font = text->font();
            font.setPointSize(40);
            text->setFont(font);
        }

    }

    void drawGraph(const Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
    {
        scene->clear();

        // Draw edges
        drawEdges(g, coord_list);

        // Draw vertices
        drawVertices(g, coord_list);

        fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }

    void highlightShortestPath(const Graph &g, const vector<std::tuple<double, double, double>> &coord_list, int start, int goal)
    {

        vector<int> distances(num_vertices(g), numeric_limits<int>::max());
        vector<Vertex> predecessors(num_vertices(g));

        dijkstra_shortest_paths(g, start,
            predecessor_map(make_iterator_property_map(predecessors.begin(), get(vertex_index, g))).
            distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, g))).
            weight_map(get(edge_weight, g))
        );

        if (distances[goal] == numeric_limits<int>::max()) {
            QMessageBox::warning(this, "No Path", "There is no path between the selected vertices.");
            return;
        }

        vector<Vertex> path;
        for (Vertex v = goal; v != start; v = predecessors[v]) {
            path.push_back(v);
        }
        path.push_back(start);

        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            double x1 = get<0>(coord_list[path[i]]);
            double y1 = get<1>(coord_list[path[i]]);
            double x2 = get<0>(coord_list[path[i + 1]]);
            double y2 = get<1>(coord_list[path[i + 1]]);

            scene->addLine(x1, y1, x2, y2, QPen(Qt::green, 6));
        }

        //draw again so there are on first plan
        drawVertices(g, coord_list);
    }

protected:
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
        setMinimumSize(100, 300);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        QLinearGradient gradient(20, 10, 20, height() - 10);
        gradient.setColorAt(0, QColor(255, 0, 0));
        gradient.setColorAt(1, QColor(80, 0, 0));

        QRectF rect(20, 40, 20, height() - 60);
        painter.fillRect(rect, gradient);

        painter.setPen(Qt::black);
        int numLabels = 5;
        for (int i = 0; i < numLabels; i++) {
            int y = 40 + i * (height() - 70) / (numLabels - 1);
            int value = 1000 - i * 2000 / (numLabels - 1);
            painter.drawText(50, y + 10, QString::number(value));
        }

        painter.setFont(QFont("Arial", 10));
        painter.drawText(10, 20, "Distance : ");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
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

    Graph g;

    const string filename_edges = "data/edges.csv";
    const string filename_nodes = "data/nodes.csv";
    vector<string> edges;
    vector<string> nodes;

    readCSV(filename_edges, edges);
    readCSV(filename_nodes, nodes);

    vector<pair<int, int>> edge_list;
    for (const auto &line : edges)
    {
        int node1, node2;
        sscanf(line.c_str(), "%d;%d", &node1, &node2);
        edge_list.emplace_back(node1 - 1, node2 - 1);
    }

    vector<std::tuple<double, double, double>> coord_list;
    for (const auto &line : nodes)
    {
        int node_id;
        double x, y, z;
        sscanf(line.c_str(), "%d;%lf;%lf;%lf", &node_id, &x, &y, &z);
        coord_list.emplace_back(x, y, z);
    }

    for (const auto &edge : edge_list)
    {
        double distance = calcul_distance(edge.first, edge.second, coord_list);
        add_edge(edge.first, edge.second, property<edge_weight_t, float>(distance), g);
        add_edge(edge.second, edge.first, property<edge_weight_t, float>(distance), g); // Add reverse edge for undirected graph
    }

    compute_graph(g, coord_list);

    QWidget mainWidget;
    mainWidget.setWindowTitle("Graph Visualization");
    mainWidget.resize(WIDTH + 150, HEIGHT + 150);

    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWidget);
    QHBoxLayout *inputLayout = new QHBoxLayout();

    QLineEdit *startVertexInput = new QLineEdit();
    startVertexInput->setPlaceholderText("Start Vertex");
    startVertexInput->setFixedHeight(40);
    startVertexInput->setFont(QFont("Arial", 12));

    QLineEdit *goalVertexInput = new QLineEdit();
    goalVertexInput->setPlaceholderText("Goal Vertex");
    goalVertexInput->setFixedHeight(40);
    goalVertexInput->setFont(QFont("Arial", 12));

    QPushButton *findPathButton = new QPushButton("Find Path");
    findPathButton->setFixedHeight(40);
    findPathButton->setFont(QFont("Arial", 12));
    findPathButton->setStyleSheet("background-color:rgb(139, 139, 139); color: white;");

    inputLayout->addWidget(startVertexInput);
    inputLayout->addWidget(goalVertexInput);
    inputLayout->addWidget(findPathButton);

    mainLayout->addLayout(inputLayout);

    QHBoxLayout *layout = new QHBoxLayout();
    GraphView *graphView = new GraphView();
    graphView->resize(WIDTH, HEIGHT);
    graphView->drawGraph(g, coord_list);
    layout->addWidget(graphView);

    ColorLegend *colorLegend = new ColorLegend();
    layout->addWidget(colorLegend);

    mainLayout->addLayout(layout);

    QObject::connect(findPathButton, &QPushButton::clicked, [&]() {
        bool ok1, ok2;
        int start = startVertexInput->text().toInt(&ok1) - 1;
        int goal = goalVertexInput->text().toInt(&ok2) - 1;
        if (ok1 && ok2 && start >= 0 && goal >= 0 && start < num_vertices(g) && goal < num_vertices(g)) {
            graphView->drawGraph(g, coord_list);
            graphView->highlightShortestPath(g, coord_list, start, goal);
        } else {
            QMessageBox::warning(&mainWidget, "Invalid Input", "Please enter valid vertex numbers.");
        }
    });

    mainWidget.show();

    return app.exec();
}


