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
#include <vector>


using Graph = boost::adjacency_list<
    boost::vecS, boost::vecS, boost::bidirectionalS,
    boost::no_property,
    boost::property<boost::edge_weight_t, float>
>;

using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using Edge = boost::graph_traits<Graph>::edge_descriptor;



// Fonctions
void readCSV(const std::string &filename, std::vector<std::string> &data);
double calcul_distance(int first, int second, const std::vector<std::tuple<double, double, double>> &coord_list);
void find_shortest_path(Graph &g, Vertex start, Vertex goal);
void compute_graph(Graph &g, const std::vector<std::tuple<double, double, double>> &coord_list);
void CSVOutputFunction(Graph& g, const std::vector<std::pair<double, double>>& list_Node, const std::vector<std::tuple<double, double, double>>& coord_list, const std::string& filename);

#endif // GRAPH_ANALYSIS_H