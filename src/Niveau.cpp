#include "Niveau.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {

bool lireCase(char c, Couleur& couleur, bool& etoile) {
    etoile = std::islower(static_cast<unsigned char>(c));
    switch (std::toupper(static_cast<unsigned char>(c))) {
        case 'R': couleur = Couleur::Rouge; return true;
        case 'V': couleur = Couleur::Vert; return true;
        case 'B': couleur = Couleur::Bleu; return true;
        case '.': couleur = Couleur::Aucune; etoile = false; return true;
        default: return false;
    }
}

bool lireDirection(const std::string& mot, Direction& direction) {
    if (mot == "N") direction = Nord;
    else if (mot == "E") direction = Est;
    else if (mot == "S") direction = Sud;
    else if (mot == "O") direction = Ouest;
    else return false;
    return true;
}

} // namespace

bool Niveau::charger(const std::string& chemin, std::string& erreur) {
    std::ifstream fichier(chemin);
    if (!fichier) {
        erreur = "impossible d'ouvrir " + chemin;
        return false;
    }

    *this = Niveau();
    std::vector<std::string> lignesGrille;
    bool dansGrille = false;
    std::string ligne;

    while (std::getline(fichier, ligne)) {
        // Les commentaires commencent par #
        ligne = ligne.substr(0, ligne.find('#'));
        std::istringstream flux(ligne);
        std::string mot;
        if (!(flux >> mot))
            continue;

        if (dansGrille) {
            std::string rangee;
            for (char c : ligne)
                if (!std::isspace(static_cast<unsigned char>(c)))
                    rangee += c;
            lignesGrille.push_back(rangee);
            continue;
        }

        if (mot == "nom") {
            std::getline(flux >> std::ws, nom);
        } else if (mot == "taille") {
            flux >> largeur >> hauteur;
        } else if (mot == "depart") {
            std::string direction;
            flux >> departX >> departY >> direction;
            if (!lireDirection(direction, departDirection)) {
                erreur = "direction inconnue : " + direction;
                return false;
            }
        } else if (mot == "fonctions") {
            int capacite;
            while (flux >> capacite)
                capacites.push_back(capacite);
        } else if (mot == "grille") {
            dansGrille = true;
        } else {
            erreur = "mot-clé inconnu : " + mot;
            return false;
        }
    }

    if (largeur <= 0 || hauteur <= 0) {
        erreur = "taille manquante";
        return false;
    }
    if (static_cast<int>(lignesGrille.size()) != hauteur) {
        erreur = "la grille doit avoir " + std::to_string(hauteur) + " lignes";
        return false;
    }
    if (capacites.empty() || std::any_of(capacites.begin(), capacites.end(), [](int c) { return c <= 0; })) {
        erreur = "il faut au moins une fonction de taille positive";
        return false;
    }

    cases.resize(largeur * hauteur);
    etoiles.resize(largeur * hauteur);
    for (int y = 0; y < hauteur; ++y) {
        if (static_cast<int>(lignesGrille[y].size()) != largeur) {
            erreur = "la ligne " + std::to_string(y + 1) + " doit avoir " + std::to_string(largeur) + " cases";
            return false;
        }
        for (int x = 0; x < largeur; ++x) {
            Couleur couleur;
            bool etoile;
            if (!lireCase(lignesGrille[y][x], couleur, etoile)) {
                erreur = std::string("case inconnue : ") + lignesGrille[y][x];
                return false;
            }
            cases[y * largeur + x] = couleur;
            etoiles[y * largeur + x] = etoile;
        }
    }

    if (couleur(departX, departY) == Couleur::Aucune) {
        erreur = "le départ doit être sur une case";
        return false;
    }
    if (std::find(etoiles.begin(), etoiles.end(), true) == etoiles.end()) {
        erreur = "le niveau n'a aucune étoile";
        return false;
    }
    if (nom.empty())
        nom = std::filesystem::path(chemin).stem().string();
    return true;
}

bool Niveau::dansLaGrille(int x, int y) const {
    return x >= 0 && y >= 0 && x < largeur && y < hauteur;
}

Couleur Niveau::couleur(int x, int y) const {
    return dansLaGrille(x, y) ? cases[y * largeur + x] : Couleur::Aucune;
}

std::vector<Niveau> chargerNiveaux(const std::string& dossier, std::string& erreur) {
    std::vector<std::string> chemins;
    std::error_code code;
    for (const auto& entree : std::filesystem::directory_iterator(dossier, code))
        if (entree.path().extension() == ".txt")
            chemins.push_back(entree.path().string());
    std::sort(chemins.begin(), chemins.end());

    std::vector<Niveau> niveaux;
    for (const auto& chemin : chemins) {
        Niveau niveau;
        std::string raison;
        if (!niveau.charger(chemin, raison)) {
            erreur = std::filesystem::path(chemin).filename().string() + " : " + raison;
            return {};
        }
        niveaux.push_back(niveau);
    }
    if (niveaux.empty())
        erreur = "aucun niveau dans " + dossier;
    return niveaux;
}
