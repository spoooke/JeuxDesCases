#ifndef NIVEAU_HPP
#define NIVEAU_HPP

#include <string>
#include <vector>

enum class Couleur { Aucune, Rouge, Vert, Bleu };

// Directions dans le sens des aiguilles d'une montre
enum Direction { Nord, Est, Sud, Ouest };

struct Niveau {
    std::string nom;
    int largeur = 0;
    int hauteur = 0;
    std::vector<Couleur> cases;   // Couleur::Aucune = trou
    std::vector<bool> etoiles;
    int departX = 0;
    int departY = 0;
    Direction departDirection = Est;
    std::vector<int> capacites;   // nombre d'instructions de F1, F2, ...

    bool charger(const std::string& chemin, std::string& erreur);

    bool dansLaGrille(int x, int y) const;
    Couleur couleur(int x, int y) const;
};

// Tous les fichiers .txt du dossier, triés par nom
std::vector<Niveau> chargerNiveaux(const std::string& dossier, std::string& erreur);

#endif
