#include "Jeu.hpp"

#include <filesystem>
#include <iostream>

namespace {

std::string dossierNiveaux(int argc, char* argv[]) {
    // Le dossier des niveaux peut être passé en argument
    if (argc > 1)
        return argv[1];
    // Version distribuée : dossier niveaux à côté de l'exécutable
    std::filesystem::path aCote = std::filesystem::path(argv[0]).parent_path() / "niveaux";
    if (std::filesystem::is_directory(aCote))
        return aCote.string();
    return DOSSIER_NIVEAUX;
}

} // namespace

int main(int argc, char* argv[]) {
    std::string erreur;
    std::vector<Niveau> niveaux = chargerNiveaux(dossierNiveaux(argc, argv), erreur);
    if (niveaux.empty()) {
        std::cerr << "Niveaux : " << erreur << "\n";
        return 1;
    }

    Jeu jeu(std::move(niveaux));
    jeu.lancer();
    return 0;
}
