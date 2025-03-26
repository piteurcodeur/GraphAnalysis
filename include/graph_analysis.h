#ifndef GRAPH_ANALYSIS_H
#define GRAPH_ANALYSIS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <cmath>
#include <limits>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>

using Graph = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::bidirectionalS,
    boost::no_property,
    boost::property<boost::edge_weight_t, float>
>;

using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using Edge = boost::graph_traits<Graph>::edge_descriptor;

/**
 * @brief lecture du fichier CSV et stockage des données dans un vecteur
 * 
 * @param filename 
 * @param data 
 */
void readCSV(const std::string &filename, std::vector<std::string> &data);

/**
 * @brief calcul de la distance entre deux noeuds à partir des coordonnées 3D
 * 
 * @param first 
 * @param second 
 * @param coord_list 
 * @return double 
 */
double calcul_distance(int first, int second, const std::vector<std::tuple<double, double, double>> &coord_list);

/**
 * @brief recherche du plus court chemin entre deux noeuds d'un graph non-orienté avec l'algorithme de Dijkstra
 * 
 * @param g 
 * @param start 
 * @param goal 
 */
void find_shortest_path(Graph &g, Vertex start, Vertex goal);

/**
 * @brief calcul du degré des noeuds du graph, degré du graph, connectivité du graph, détection de cycle et affichage du plus court chemin
 * 
 * @param g 
 * @param coord_list 
 */
void compute_graph(Graph &g, const std::vector<std::tuple<double, double, double>> &coord_list);

/**
 * @brief vérification de la présence de cycle dans un graph orienté
 * 
 * @param v 
 * @param visited 
 * @param recStack 
 * @param g 
 * @return true 
 * @return false 
 */
bool isCyclicUtil(int v, bool visited[], bool *recStack, Graph &g);

/**
 * @brief Calcul de tous les chemins les plus courts entre tous les noeuds du graph et écriture dans un fichier CSV
 * 
 * @param g 
 * @param list_Node 
 * @param coord_list 
 * @param filename 
 */
void CSVOuptutFunction(Graph &g, const std::vector<std::pair<double, double>> &list_Node, const std::vector<std::tuple<double, double, double>> &coord_list, const std::string &filename);


/**
 * @brief Calcul de tous les chemins les plus courts entre tous les noeuds du graph et écriture dans un fichier CSV, avec mesure du temps d'exécution
 * 
 * @param g 
 * @param list_Node 
 * @param coord_list 
 * @param filename 
 */
void calculate_and_write_paths(Graph &g, const std::vector<std::pair<double, double>> &list_Node, const std::vector<std::tuple<double, double, double>> &coord_list, const std::string &filename);


#endif // GRAPH_ANALYSIS_H