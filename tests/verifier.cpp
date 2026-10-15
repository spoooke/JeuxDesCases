// Vérifie que chaque niveau se charge et qu'une solution connue le termine
#include "Automate.hpp"

#include <iostream>

namespace {

Instruction av(Couleur si = Couleur::Aucune) { return {Action::Avancer, 0, Couleur::Aucune, si}; }
Instruction ga(Couleur si = Couleur::Aucune) { return {Action::Gauche, 0, Couleur::Aucune, si}; }
Instruction dr(Couleur si = Couleur::Aucune) { return {Action::Droite, 0, Couleur::Aucune, si}; }
Instruction f(int numero) { return {Action::Appel, numero - 1, Couleur::Aucune, Couleur::Aucune}; }
Instruction peindre(Couleur c) { return {Action::Peindre, 0, c, Couleur::Aucune}; }

const Couleur R = Couleur::Rouge;
const Couleur V = Couleur::Vert;

} // namespace

int main() {
    std::string erreur;
    std::vector<Niveau> niveaux = chargerNiveaux(DOSSIER_NIVEAUX, erreur);
    if (niveaux.empty()) {
        std::cerr << erreur << "\n";
        return 1;
    }

    std::vector<Programme> solutions = {
        {{av(), f(1)}},
        {{av(), ga(R), f(1)}},
        {{av(), dr(), f(2)}, {av(), ga(), f(1)}},
        {{av(), ga(R), ga(R), f(1)}},
        {{peindre(V), f(2)}, {av(), ga(R), ga(R), ga(V), f(2)}},
    };

    int echecs = 0;
    if (niveaux.size() != solutions.size()) {
        std::cerr << niveaux.size() << " niveaux pour " << solutions.size() << " solutions\n";
        ++echecs;
    }

    for (std::size_t i = 0; i < niveaux.size() && i < solutions.size(); ++i) {
        const Niveau& niveau = niveaux[i];
        Automate automate;

        Programme vide = programmeVide(niveau);
        automate.demarrer(niveau, vide);
        if (automate.executer(10000) != Etat::Perdu) {
            std::cerr << niveau.nom << " : un programme vide ne doit pas gagner\n";
            ++echecs;
        }

        Programme solution = solutions[i];
        bool tailleOk = solution.size() == niveau.capacites.size();
        for (std::size_t f = 0; tailleOk && f < solution.size(); ++f) {
            tailleOk = static_cast<int>(solution[f].size()) <= niveau.capacites[f];
            solution[f].resize(niveau.capacites[f]);
        }
        if (!tailleOk) {
            std::cerr << niveau.nom << " : la solution ne tient pas dans les fonctions\n";
            ++echecs;
            continue;
        }

        automate.demarrer(niveau, solution);
        Etat etat = automate.executer(10000);
        std::cout << niveau.nom << " : " << (etat == Etat::Gagne ? "ok" : "ÉCHEC " + automate.message())
                  << " (" << automate.nombreDePas() << " pas)\n";
        if (etat != Etat::Gagne)
            ++echecs;
    }
    return echecs == 0 ? 0 : 1;
}
