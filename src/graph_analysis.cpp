#include <iostream>
#include <fstream>
#include <chrono>
#include "graph_analysis.h"

using namespace std;
using namespace boost;

void readCSV(const string &filename, vector<string> &data)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Ouverture du fichier impossible : " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        data.push_back(line);
    }

    if (!data.empty()) {
        data.erase(data.begin());
    }

    file.close();
}

double calcul_distance(int first, int second, const vector<std::tuple<double, double, double>> &coord_list)
{
    double x1 = get<0>(coord_list[first]);
    double y1 = get<1>(coord_list[first]);
    double z1 = get<2>(coord_list[first]);
    double x2 = get<0>(coord_list[second]);
    double y2 = get<1>(coord_list[second]);
    double z2 = get<2>(coord_list[second]);

    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

void find_shortest_path(Graph &g, Vertex start, Vertex goal) {
    size_t n = num_vertices(g);
    start--;
    goal--;

    vector<int> distances(n, numeric_limits<int>::max());
    vector<Vertex> predecessors(n);

    dijkstra_shortest_paths(g, start,
        predecessor_map(make_iterator_property_map(predecessors.begin(), get(vertex_index, g))).
        distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, g))).
        weight_map(get(edge_weight, g))
    );

    cout << "Shortest path from " << start + 1 << " to " << goal + 1 << " is: ";

    vector<Vertex> path;
    for (Vertex v = goal; v != start; v = predecessors[v]) {
        path.push_back(v);
    }
    path.push_back(start);

    cout << path.back() + 1;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        if (it != path.rbegin()) {
            cout << " -> " << *it + 1;
        }
    }
    cout << " Path Length: " << path.size() << endl;
}

void CSVOuptutFunction(Graph &g, const vector<pair<double, double>> &list_Node, const vector<std::tuple<double, double, double>> &coord_list, const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Impossible d'ouvrir le fichier de sortie" << endl;
        return;
    }

    file << "Start;End;Path Length;Path" << endl;

    for (const auto &[start, goal] : list_Node) {
        size_t n = num_vertices(g);

        if (start < 1 || goal < 1 || start > n || goal > n) {
            cerr << "Erreur : Indices de sommet invalides (" << start << ", " << goal << ")" << endl;
            continue;
        }
        Vertex start_idx = start - 1;
        Vertex goal_idx = goal - 1;

        vector<double> distances(n, numeric_limits<double>::max());
        vector<Vertex> predecessors(n, start_idx);

        dijkstra_shortest_paths(g, start_idx,
            predecessor_map(make_iterator_property_map(predecessors.begin(), get(vertex_index, g))).
            distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, g))).
            weight_map(get(edge_weight, g))
        );

        if (distances[goal_idx] == numeric_limits<double>::max()) {
            file << start << ";" << goal << ";Inf;No Path" << endl;
            continue;
        }

        vector<Vertex> path;
        for (Vertex v = goal_idx; v != start_idx; v = predecessors[v]) {
            if (v == predecessors[v]) {
                cerr << "Erreur dans le chemin, arrêt prématuré" << endl;
                file << start << ";" << goal << ";Inf;No Path" << endl;
                break;
            }
            path.push_back(v);
        }
        path.push_back(start_idx);

        ostringstream path_stream;
        path_stream << "[";
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            if (it != path.rbegin()) path_stream << ",";
            path_stream << (*it + 1);
        }
        path_stream << "]";

        file << start << ";" << goal << ";" << distances[goal_idx] << ";" << path_stream.str() << endl;
    }

    file.close();
    cout << "Fichier CSV '" << filename << "' généré avec succès." << endl;
}

void calculate_and_write_paths(Graph &g, const vector<pair<double, double>> &list_Node, const vector<std::tuple<double, double, double>> &coord_list, const string &filename) {
    auto start_time = std::chrono::high_resolution_clock::now();

    CSVOuptutFunction(g, list_Node, coord_list, filename);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    cout << "\n9. Time calculation: " << endl;
    cout << "Time taken: " << elapsed_time.count() << " seconds" << endl;
}

bool isCyclicUtil(int v, bool visited[], bool *recStack, Graph &g) {
    if (!visited[v]) {
        visited[v] = true;
        recStack[v] = true;

        for (auto [ai, ai_end] = adjacent_vertices(v, g); ai != ai_end; ++ai) {
            if (!visited[*ai] && isCyclicUtil(*ai, visited, recStack, g)) {
                return true;
            } else if (recStack[*ai]) {
                return true;
            }
        }
    }
    recStack[v] = false;
    return false;
}

void compute_graph(Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
{
    cout << "\n1. Node degree calculation :" << endl;
    cout << "Node degree: {";
    for (size_t i = 0; i < num_vertices(g); ++i)
    {
        cout << i + 1 << ":" << out_degree(i, g);
        if (i < num_vertices(g) - 1)
        {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout << "\n2. Graph Degree Calculation: " << endl;
    cout << "Graph degree: " << num_edges(g) << endl;

    cout << "\n3. Graph Connectivity: " << endl;
    vector<int> component(num_vertices(g));
    int num = connected_components(g, &component[0]);
    cout << "Graph is connected: " << (num == 1) << endl;

    cout << "\n4. Cycle Detection: " << endl;
    bool *visited = new bool[num_vertices(g)]();
    bool *recStack = new bool[num_vertices(g)]();
    bool cycle = false;
    for (size_t i = 0; i < num_vertices(g); i++) {
        if (!visited[i] && isCyclicUtil(i, visited, recStack, g)) {
            cycle = true;
            break;
        }
    }
    cout << "Graph has cycle: " << (cycle ? "True" : "False") << endl;
    delete[] visited;
    delete[] recStack;

    cout << "\n5. Shortest Path Calculation: " << endl;
    find_shortest_path(g, 1, 6);

    cout << "\n8. CSV output function: " << endl;
    vector<pair<double, double>> list_Node;
    for (size_t i = 0; i < coord_list.size(); ++i) {
        for (size_t j = i + 1; j < coord_list.size(); ++j) {
            list_Node.push_back({i + 1, j + 1});
        }
    }
    calculate_and_write_paths(g, list_Node, coord_list, "output.csv");
}