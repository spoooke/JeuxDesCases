#include "Jeu.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    // Le dossier des niveaux peut être passé en argument
    std::string dossier = argc > 1 ? argv[1] : DOSSIER_NIVEAUX;
    std::string erreur;
    std::vector<Niveau> niveaux = chargerNiveaux(dossier, erreur);
    if (niveaux.empty()) {
        std::cerr << "Niveaux : " << erreur << "\n";
        return 1;
    }

    Jeu jeu(std::move(niveaux));
    jeu.lancer();
    return 0;
}
