# Graph Analysis and Visualization

## Description
Ce projet permet de représenter et d'analyser des graphes en utilisant C++ et Qt pour la visualisation. Il utilise également la bibliothèque Boost pour les algorithmes de graphe.

## Fonctionnalités
- Chargement des données de graphe à partir de fichiers CSV.
- Visualisation des graphes en 2D.
- Calcul et mise en évidence du plus court chemin entre deux sommets.

## Prérequis
- Qt 5 ou supérieur
- Boost Graph Library
- Compilateur C++ compatible avec C++11 ou supérieur

## Installation
1. Clonez le dépôt :
   ```sh
   git clone https://github.com/piteurcodeur/GraphAnalysis.git
   cd GraphAnalysis
   ```

2. Assurez-vous que Qt et Boost sont installés sur votre système.

3. Compilez le projet :
   ```sh
   qmake
   make
   ```

## Utilisation
1. Exécutez l'application :
   ```sh
   ./GraphAnalysis
   ```

2. Entrez les sommets de départ et d'arrivée pour trouver le plus court chemin.

## Structure des fichiers
- `src/main.cpp` : Contient le code principal de l'application.
- `data/edges.csv` : Contient les arêtes du graphe.
- `data/nodes.csv` : Contient les sommets du graphe avec leurs coordonnées.

## Licence
Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.

