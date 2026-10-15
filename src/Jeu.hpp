#ifndef JEU_HPP
#define JEU_HPP

#include "Automate.hpp"

#include <SFML/Graphics.hpp>

class Jeu {
public:
    explicit Jeu(std::vector<Niveau> niveaux);
    void lancer();

private:
    struct Emplacement {
        int fonction = -1;
        int indice = -1;
        bool valide() const { return fonction >= 0; }
    };

    // Actions
    void changerNiveau(int indice);
    void lancerOuPause();
    void avancerDUnPas();
    void arreter();
    void effacer();
    void choisirAction(const Instruction& modele);
    void choisirCondition(Couleur condition);

    // Événements
    void clic(float x, float y, bool droit);
    void touche(sf::Keyboard::Key touche);
    void mettreAJour(float secondes);

    // Dessin
    void dessiner();
    void dessinerGrille();
    void dessinerPanneau();
    void dessinerProgramme();
    void dessinerInstruction(const Instruction& instruction, sf::FloatRect zone, bool palette);
    void dessinerBouton(sf::FloatRect zone, const std::string& texte, bool actif = true);
    void dessinerTexte(const std::string& texte, float x, float y, unsigned taille,
                       sf::Color couleur = sf::Color::White, float largeurMax = 0);

    // Positions à l'écran
    sf::FloatRect zoneEmplacement(int fonction, int indice) const;
    std::vector<std::pair<sf::FloatRect, Instruction>> paletteActions() const;
    std::vector<std::pair<sf::FloatRect, Couleur>> paletteConditions() const;

    Niveau& niveau() { return m_niveaux[m_indice]; }
    Programme& programme() { return m_programmes[m_indice]; }

    std::vector<Niveau> m_niveaux;
    std::vector<Programme> m_programmes;
    std::vector<bool> m_reussis;
    int m_indice = 0;

    sf::RenderWindow m_fenetre;
    sf::Font m_police;
    bool m_policeChargee = false;

    Automate m_automate;
    bool m_enMarche = false;
    int m_vitesse = 2;
    float m_attente = 0;
    Emplacement m_selection;
};

#endif
