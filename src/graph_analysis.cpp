#include <iostream>
#include <fstream>
#include <chrono>
#include "graph_analysis.h"

using namespace std;
using namespace boost;


// Lecture d'un fichier CSV et stockage dans un vecteur
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
    
    // Suppression de la première ligne si elle contient des en-têtes
    if (!data.empty()) {
        data.erase(data.begin());
    }

    file.close();
}

// Calcul de la distance entre deux points en 3D
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
    
    size_t n = num_vertices(g); // Nombre de sommets
    start--;  // Ajustement car les IDs commencent à 1 mais les indices à 0
    goal--;   // Ajustement car les IDs commencent à 1 mais les indices à 0
    
    // Vecteurs pour stocker les distances et les prédécesseurs
    vector<int> distances(n, numeric_limits<int>::max());
    vector<Vertex> predecessors(n);
    
    // Exécution de l'algorithme de Dijkstra avec prise en compte des poids
    dijkstra_shortest_paths(g, start,
        predecessor_map(make_iterator_property_map(predecessors.begin(), get(vertex_index, g))).
        distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, g))).
        weight_map(get(edge_weight, g)) // Poids des arêtes
    );
    
    
    
    
    cout << "Shortest path from " << start+1 << " to " << goal+1 << " is: ";
    
    vector<Vertex> path;
    for (Vertex v = goal; v != start; v = predecessors[v]) {
        path.push_back(v);
    }
    path.push_back(start);  

    int cpt = 0;
    cout << path.back()+1;  
    
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        if (it != path.rbegin()) {  
            cout << " -> " << *it+1;
        }
        cpt++;
    }
    
    cout << " Path Length: " << cpt << endl;
    
    
}

void CSVOuptutFunction(Graph &g, const vector<pair<double, double>> &list_Node, const vector<std::tuple<double, double, double>> &coord_list, const string &filename) {
    // Création du fichier CSV
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Impossible d'ouvrir le fichier de sortie" << endl;
        return;
    }

    // En-tête du fichier CSV
    file << "Start;End;Path Length;Path" << endl;

    // Parcours de la liste des paires de sommets
    for (const auto &[start, goal] : list_Node) {
        size_t n = num_vertices(g);

        // Vérification des indices
        if (start < 1 || goal < 1 || start > n || goal > n) {
            cerr << "Erreur : Indices de sommet invalides (" << start << ", " << goal << ")" << endl;
            continue;
        }
        Vertex start_idx = start - 1;
        Vertex goal_idx = goal - 1;

        // Initialisation des structures pour Dijkstra
        vector<double> distances(n, numeric_limits<double>::max());
        vector<Vertex> predecessors(n, start_idx);

        // Exécution de Dijkstra
        dijkstra_shortest_paths(g, start_idx,
            predecessor_map(make_iterator_property_map(predecessors.begin(), get(vertex_index, g))).
            distance_map(make_iterator_property_map(distances.begin(), get(vertex_index, g))).
            weight_map(get(edge_weight, g))
        );

        // Vérification si le but est atteignable
        if (distances[goal_idx] == numeric_limits<double>::max()) {
            file << start << ";" << goal << ";Inf;No Path" << endl;
            continue;
        }

        // Reconstruction du chemin
        vector<Vertex> path;
        for (Vertex v = goal_idx; v != start_idx; v = predecessors[v]) {
            if (v == predecessors[v]) {  // Vérification d'une boucle infinie
                cerr << "Erreur dans le chemin, arrêt prématuré" << endl;
                file << start << ";" << goal << ";Inf;No Path" << endl;
                break;
            }
            path.push_back(v);
        }
        path.push_back(start_idx);

        // Construction de la chaîne de caractères pour le chemin formaté
        ostringstream path_stream;
        path_stream << "[";
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            if (it != path.rbegin()) path_stream << ", ";
            path_stream << (*it + 1);  // Conversion de l'index en sommet 1-based
        }
        path_stream << "]";

        // Écriture du chemin dans le fichier CSV
        file << start << ";" << goal << ";" << distances[goal_idx] << ";" << path_stream.str() << endl;
    }

    file.close();
    cout << "Fichier CSV '" << filename << "' généré avec succès." << endl;
}


void calculate_and_write_paths(Graph &g, const vector<pair<double, double>> &list_Node, const vector<std::tuple<double, double, double>> &coord_list, const string &filename) {
    // Démarrer le chrono
    auto start_time = std::chrono::high_resolution_clock::now();

    // Appel de la fonction pour écrire les chemins dans le fichier CSV
    CSVOuptutFunction(g, list_Node, coord_list, filename);

    // Arrêter le chrono
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // Calcul du temps écoulé en secondes
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    
    cout<< "\n9. Time calculation: " << endl;
    // Affichage du temps écoulé
    cout << "Time taken: " << elapsed_time.count() << " seconds" << endl;
}



void compute_graph(Graph &g, const vector<std::tuple<double, double, double>> &coord_list)
{
    cout<< "\n1. Node degree calculation :" << endl;
    // Calcul du degré de chaque sommet
    cout << "Node degree: {";
    for (size_t i = 0; i < num_vertices(g); ++i)
    {
        cout << i+1 << ":" << out_degree(i, g);  // +1 pour afficher l'ID à partir de 1
        if (i < num_vertices(g) - 1)
        {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout<<"\n2. Graph Degree Calculation: " << endl;
    // Calcul du degré du graphe
    cout << "Graph degree: " << num_edges(g) << endl;

    cout<<"\n3. Graph Connectivity: " << endl;
    // Vérification de la connectivité du graphe
    std::vector<int> component(num_vertices(g));
    int num = connected_components(g, &component[0]);
    cout << "Graph is connected: " << (num == 1) << endl;

    cout<<"\n4. Cycle Detection: " << endl;
    // Vérification de la présence de cycles
    vector<graph_traits<Graph>::vertex_descriptor> topo_order;
    cout <<"Graph has cycle: ";
    try {
        topological_sort(g, back_inserter(topo_order));  // Si un cycle existe, une exception sera levée
        cout << "False" << endl;
    }
    catch (const not_a_dag&) {
        cout << "True" << endl;
    }

    cout<<"\n5. Shortest Path Calculation: " << endl;
    // Calcul du plus court chemin entre les sommets 1 et 6
    Vertex start = 1; // sommet de départ
    Vertex goal = 6;  // sommet d'arrivée
    find_shortest_path(g, start, goal);

    cout<<"\n6. Connected Components: " << endl;
    cout << "Number of connected components: " << num << endl;
    
    // Afficher les composantes connexes
    cout << "Components: {";
    for (size_t i = 0; i < num_vertices(g); ++i)
    {
        cout << i+1 << ":" << component[i];
        if (i < num_vertices(g) - 1)
        {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout<<"\n8. CSV output function: " << endl;

    vector<std::pair<double, double>> list_Node;

    // Générer toutes les combinaisons uniques sans doublons
    for (size_t i = 0; i < coord_list.size(); ++i) {
        for (size_t j = i + 1; j < coord_list.size(); ++j) {  // Assurer i < j
            list_Node.push_back({i + 1, j + 1});
        }
    }

    // Appel de la fonction
    calculate_and_write_paths(g,  list_Node, coord_list, "output.csv");
   

}