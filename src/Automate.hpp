#ifndef AUTOMATE_HPP
#define AUTOMATE_HPP

#include "Niveau.hpp"

#include <deque>
#include <string>
#include <vector>

enum class Action { Vide, Avancer, Gauche, Droite, Appel, Peindre };

struct Instruction {
    Action action = Action::Vide;
    int fonction = 0;                       // pour Appel : 0 = F1
    Couleur peinture = Couleur::Aucune;     // pour Peindre
    Couleur condition = Couleur::Aucune;    // Aucune = toujours exécutée
};

// Programme[f][i] : instruction i de la fonction F(f+1)
using Programme = std::vector<std::vector<Instruction>>;

Programme programmeVide(const Niveau& niveau);

enum class Etat { Pret, EnCours, Gagne, Perdu };

class Automate {
public:
    // Position d'une instruction dans le programme, pour l'afficher
    struct Origine {
        int fonction;
        int indice;
    };

    void demarrer(const Niveau& niveau, const Programme& programme);
    Etat pas();
    Etat executer(int limite);

    Etat etat() const { return m_etat; }
    const std::string& message() const { return m_message; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    Direction direction() const { return m_direction; }
    const std::vector<Couleur>& cases() const { return m_cases; }
    const std::vector<bool>& etoiles() const { return m_etoiles; }
    int etoilesRestantes() const { return m_restantes; }
    int nombreDePas() const { return m_pas; }
    // Instruction exécutée au dernier pas (fonction = -1 si aucune)
    Origine courante() const { return m_courante; }

private:
    void empiler(int fonction);
    void perdre(const std::string& raison);

    const Programme* m_programme = nullptr;
    int m_largeur = 0;
    int m_hauteur = 0;
    std::vector<Couleur> m_cases;
    std::vector<bool> m_etoiles;
    int m_restantes = 0;
    int m_x = 0;
    int m_y = 0;
    Direction m_direction = Est;
    std::deque<Origine> m_file;
    Origine m_courante{-1, -1};
    int m_pas = 0;
    Etat m_etat = Etat::Pret;
    std::string m_message;
};

#endif
