#include "Jeu.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sstream>

namespace {

const float largeurFenetre = 1000;
const float margeHaut = 60;
const float hauteurGrille = 380;
const float largeurGrille = 620;
const float xPanneau = 660;
const float yProgramme = 470;
const float tailleCase = 44;
const float pasCase = 50;
const float hauteurLigne = 56;

// Délai entre deux instructions, en secondes
const float delais[] = {1.0f, 0.5f, 0.25f, 0.12f, 0.05f, 0.015f};
const int nombreDeVitesses = 6;

const char* polices[] = {
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    "/Library/Fonts/Arial.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "C:/Windows/Fonts/arial.ttf",
};

const sf::Color fond(30, 32, 40);
const sf::Color gris(70, 72, 84);
const sf::Color jaune(250, 205, 60);

sf::Color teinte(Couleur couleur) {
    switch (couleur) {
        case Couleur::Rouge: return sf::Color(205, 70, 70);
        case Couleur::Vert: return sf::Color(70, 170, 90);
        case Couleur::Bleu: return sf::Color(70, 120, 210);
        default: return gris;
    }
}

sf::Color assombrir(sf::Color c) {
    return sf::Color(c.r * 6 / 10, c.g * 6 / 10, c.b * 6 / 10);
}

sf::ConvexShape etoile(sf::Vector2f centre, float rayon) {
    sf::ConvexShape forme(10);
    for (int i = 0; i < 10; ++i) {
        float r = i % 2 == 0 ? rayon : rayon * 0.45f;
        float angle = -3.14159265f / 2 + i * 3.14159265f / 5;
        forme.setPoint(i, centre + sf::Vector2f(r * std::cos(angle), r * std::sin(angle)));
    }
    forme.setFillColor(jaune);
    forme.setOutlineColor(sf::Color(120, 90, 20));
    forme.setOutlineThickness(1.5f);
    return forme;
}

// Flèche pointant vers le haut, centrée sur l'origine
sf::ConvexShape fleche(float rayon) {
    sf::ConvexShape forme(4);
    forme.setPoint(0, {0, -rayon});
    forme.setPoint(1, {rayon * 0.75f, rayon * 0.8f});
    forme.setPoint(2, {0, rayon * 0.35f});
    forme.setPoint(3, {-rayon * 0.75f, rayon * 0.8f});
    return forme;
}

// Flèche coudée : tourner à gauche (sens = -1) ou à droite (sens = 1)
void dessinerVirage(sf::RenderTarget& cible, sf::Vector2f centre, float taille, int sens) {
    const float l = taille * 0.5f;
    const float e = 5;
    float xBas = centre.x - sens * l * 0.3f;
    float xBout = centre.x + sens * l * 0.15f;
    float yCoude = centre.y - l * 0.2f;

    sf::RectangleShape montant({e, l * 0.65f});
    montant.setPosition(xBas - e / 2, yCoude - e / 2);
    sf::RectangleShape traverse({std::abs(xBout - xBas), e});
    traverse.setPosition(std::min(xBas, xBout), yCoude - e / 2);
    sf::ConvexShape pointe(3);
    pointe.setPoint(0, {xBout + sens * l * 0.35f, yCoude});
    pointe.setPoint(1, {xBout, yCoude - 8});
    pointe.setPoint(2, {xBout, yCoude + 8});
    for (sf::Shape* forme : std::initializer_list<sf::Shape*>{&montant, &traverse, &pointe}) {
        forme->setFillColor(sf::Color::White);
        cible.draw(*forme);
    }
}

} // namespace

Jeu::Jeu(std::vector<Niveau> niveaux) : m_niveaux(std::move(niveaux)) {
    for (const Niveau& niveau : m_niveaux)
        m_programmes.push_back(programmeVide(niveau));
    m_reussis.assign(m_niveaux.size(), false);

    for (const char* chemin : polices)
        if (std::filesystem::exists(chemin) && m_police.loadFromFile(chemin)) {
            m_policeChargee = true;
            break;
        }

    std::size_t fonctionsMax = 1;
    for (const Niveau& niveau : m_niveaux)
        fonctionsMax = std::max(fonctionsMax, niveau.capacites.size());
    unsigned hauteur = static_cast<unsigned>(yProgramme + fonctionsMax * hauteurLigne + 20);

    m_fenetre.create(sf::VideoMode(static_cast<unsigned>(largeurFenetre), std::max(hauteur, 640u)),
                     "Jeux des cases", sf::Style::Titlebar | sf::Style::Close);
    m_fenetre.setVerticalSyncEnabled(true);
    changerNiveau(0);
}

void Jeu::lancer() {
    sf::Clock horloge;
    while (m_fenetre.isOpen()) {
        sf::Event evenement;
        while (m_fenetre.pollEvent(evenement)) {
            if (evenement.type == sf::Event::Closed)
                m_fenetre.close();
            else if (evenement.type == sf::Event::MouseButtonPressed)
                clic(static_cast<float>(evenement.mouseButton.x), static_cast<float>(evenement.mouseButton.y),
                     evenement.mouseButton.button == sf::Mouse::Right);
            else if (evenement.type == sf::Event::KeyPressed)
                touche(evenement.key.code);
        }
        mettreAJour(horloge.restart().asSeconds());
        dessiner();
    }
}

// ---------------------------------------------------------------- Actions

void Jeu::changerNiveau(int indice) {
    if (indice < 0 || indice >= static_cast<int>(m_niveaux.size()))
        return;
    m_indice = indice;
    m_selection = {0, 0};
    arreter();
}

void Jeu::lancerOuPause() {
    if (m_automate.etat() == Etat::EnCours) {
        m_enMarche = !m_enMarche;
        return;
    }
    m_automate.demarrer(niveau(), programme());
    m_enMarche = true;
    m_attente = 0;
}

void Jeu::avancerDUnPas() {
    if (m_automate.etat() != Etat::EnCours)
        m_automate.demarrer(niveau(), programme());
    else
        m_automate.pas();
    m_enMarche = false;
}

void Jeu::arreter() {
    m_automate = Automate();
    m_enMarche = false;
}

void Jeu::effacer() {
    programme() = programmeVide(niveau());
    m_selection = {0, 0};
    arreter();
}

void Jeu::choisirAction(const Instruction& modele) {
    if (!m_selection.valide())
        return;
    arreter();
    Instruction& instruction = programme()[m_selection.fonction][m_selection.indice];
    Couleur condition = instruction.condition;
    instruction = modele;
    instruction.condition = condition;
    // On passe à l'emplacement suivant pour enchaîner la saisie
    if (m_selection.indice + 1 < static_cast<int>(programme()[m_selection.fonction].size()))
        ++m_selection.indice;
}

void Jeu::choisirCondition(Couleur condition) {
    if (!m_selection.valide())
        return;
    arreter();
    programme()[m_selection.fonction][m_selection.indice].condition = condition;
}

// ---------------------------------------------------------------- Événements

void Jeu::clic(float x, float y, bool droit) {
    sf::Vector2f point(x, y);

    for (int f = 0; f < static_cast<int>(programme().size()); ++f)
        for (int i = 0; i < static_cast<int>(programme()[f].size()); ++i)
            if (zoneEmplacement(f, i).contains(point)) {
                m_selection = {f, i};
                if (droit) {
                    arreter();
                    programme()[f][i] = Instruction();
                }
                return;
            }

    for (const auto& [zone, modele] : paletteActions())
        if (zone.contains(point))
            return choisirAction(modele);
    for (const auto& [zone, condition] : paletteConditions())
        if (zone.contains(point))
            return choisirCondition(condition);

    if (sf::FloatRect(xPanneau, margeHaut, 150, 40).contains(point)) lancerOuPause();
    else if (sf::FloatRect(xPanneau + 170, margeHaut, 150, 40).contains(point)) avancerDUnPas();
    else if (sf::FloatRect(xPanneau, margeHaut + 50, 150, 40).contains(point)) arreter();
    else if (sf::FloatRect(xPanneau + 170, margeHaut + 50, 150, 40).contains(point)) effacer();
    else if (sf::FloatRect(xPanneau, margeHaut + 105, 40, 34).contains(point)) m_vitesse = std::max(0, m_vitesse - 1);
    else if (sf::FloatRect(xPanneau + 280, margeHaut + 105, 40, 34).contains(point))
        m_vitesse = std::min(nombreDeVitesses - 1, m_vitesse + 1);
    else if (sf::FloatRect(largeurFenetre - 110, 12, 40, 32).contains(point)) changerNiveau(m_indice - 1);
    else if (sf::FloatRect(largeurFenetre - 60, 12, 40, 32).contains(point)) changerNiveau(m_indice + 1);
}

void Jeu::touche(sf::Keyboard::Key touche) {
    switch (touche) {
        case sf::Keyboard::Space: lancerOuPause(); break;
        case sf::Keyboard::Enter: avancerDUnPas(); break;
        case sf::Keyboard::Escape: arreter(); break;
        case sf::Keyboard::PageUp: changerNiveau(m_indice - 1); break;
        case sf::Keyboard::PageDown: changerNiveau(m_indice + 1); break;
        case sf::Keyboard::Backspace:
        case sf::Keyboard::Delete:
            if (m_selection.valide()) {
                arreter();
                programme()[m_selection.fonction][m_selection.indice] = Instruction();
            }
            break;
        default: break;
    }
}

void Jeu::mettreAJour(float secondes) {
    if (!m_enMarche || m_automate.etat() != Etat::EnCours)
        return;
    m_attente += secondes;
    while (m_attente >= delais[m_vitesse] && m_automate.etat() == Etat::EnCours) {
        m_attente -= delais[m_vitesse];
        m_automate.pas();
    }
    if (m_automate.etat() != Etat::EnCours)
        m_enMarche = false;
    if (m_automate.etat() == Etat::Gagne)
        m_reussis[m_indice] = true;
}

// ---------------------------------------------------------------- Dessin

void Jeu::dessiner() {
    m_fenetre.clear(fond);

    std::ostringstream titre;
    titre << "Niveau " << m_indice + 1 << "/" << m_niveaux.size() << " : " << niveau().nom;
    if (m_reussis[m_indice])
        titre << "  (réussi)";
    dessinerTexte(titre.str(), 20, 16, 22);
    dessinerBouton({largeurFenetre - 110, 12, 40, 32}, "<", m_indice > 0);
    dessinerBouton({largeurFenetre - 60, 12, 40, 32}, ">", m_indice + 1 < static_cast<int>(m_niveaux.size()));

    dessinerGrille();
    dessinerPanneau();
    dessinerProgramme();
    m_fenetre.display();
}

void Jeu::dessinerGrille() {
    const Niveau& n = niveau();
    bool actif = m_automate.etat() != Etat::Pret;
    const std::vector<Couleur>& cases = actif ? m_automate.cases() : n.cases;
    const std::vector<bool>& etoiles = actif ? m_automate.etoiles() : n.etoiles;

    float taille = std::min({largeurGrille / n.largeur, hauteurGrille / n.hauteur, 72.f});
    sf::Vector2f origine(20 + (largeurGrille - taille * n.largeur) / 2, margeHaut + (hauteurGrille - taille * n.hauteur) / 2);

    for (int y = 0; y < n.hauteur; ++y)
        for (int x = 0; x < n.largeur; ++x) {
            Couleur couleur = cases[y * n.largeur + x];
            if (couleur == Couleur::Aucune)
                continue;
            sf::RectangleShape tuile({taille - 3, taille - 3});
            tuile.setPosition(origine + sf::Vector2f(x * taille + 1.5f, y * taille + 1.5f));
            tuile.setFillColor(teinte(couleur));
            m_fenetre.draw(tuile);
            if (etoiles[y * n.largeur + x])
                m_fenetre.draw(etoile(origine + sf::Vector2f((x + 0.5f) * taille, (y + 0.5f) * taille), taille * 0.32f));
        }

    int ax = actif ? m_automate.x() : n.departX;
    int ay = actif ? m_automate.y() : n.departY;
    Direction direction = actif ? m_automate.direction() : n.departDirection;
    sf::ConvexShape automate = fleche(taille * 0.34f);
    automate.setPosition(origine + sf::Vector2f((ax + 0.5f) * taille, (ay + 0.5f) * taille));
    automate.setRotation(direction * 90.f);
    automate.setFillColor(sf::Color(20, 20, 24));
    automate.setOutlineColor(sf::Color::White);
    automate.setOutlineThickness(2);
    m_fenetre.draw(automate);
}

void Jeu::dessinerPanneau() {
    Etat etat = m_automate.etat();
    dessinerBouton({xPanneau, margeHaut, 150, 40}, etat == Etat::EnCours && m_enMarche ? "Pause" : "Lancer");
    dessinerBouton({xPanneau + 170, margeHaut, 150, 40}, "Pas à pas");
    dessinerBouton({xPanneau, margeHaut + 50, 150, 40}, "Stop", etat != Etat::Pret);
    dessinerBouton({xPanneau + 170, margeHaut + 50, 150, 40}, "Effacer");

    dessinerBouton({xPanneau, margeHaut + 105, 40, 34}, "-", m_vitesse > 0);
    dessinerBouton({xPanneau + 280, margeHaut + 105, 40, 34}, "+", m_vitesse + 1 < nombreDeVitesses);
    dessinerTexte("Vitesse " + std::to_string(m_vitesse + 1) + "/" + std::to_string(nombreDeVitesses),
                  xPanneau + 115, margeHaut + 111, 18);

    dessinerTexte("Instructions", xPanneau, margeHaut + 160, 16, sf::Color(180, 180, 190));
    for (const auto& [zone, modele] : paletteActions())
        dessinerInstruction(modele, zone, true);

    dessinerTexte("Condition de couleur", xPanneau, margeHaut + 290, 16, sf::Color(180, 180, 190));
    for (const auto& [zone, condition] : paletteConditions()) {
        Instruction exemple;
        exemple.condition = condition;
        dessinerInstruction(exemple, zone, true);
        if (condition == Couleur::Aucune)
            dessinerTexte("toujours", zone.left + 7, zone.top + 15, 13);
    }

    std::string message;
    sf::Color couleur = sf::Color(200, 200, 210);
    if (etat == Etat::Pret)
        message = "Clique une case de F1, puis une instruction. Clic droit pour vider.";
    else if (etat == Etat::EnCours)
        message = std::to_string(m_automate.etoilesRestantes()) + " étoile(s) restante(s), "
                  + std::to_string(m_automate.nombreDePas()) + " pas.";
    else {
        message = m_automate.message();
        couleur = etat == Etat::Gagne ? sf::Color(120, 220, 130) : sf::Color(240, 120, 120);
    }
    dessinerTexte(message, xPanneau, margeHaut + 375, 16, couleur, 320);
}

void Jeu::dessinerProgramme() {
    Automate::Origine courante = m_automate.courante();
    bool afficherCourante = m_automate.etat() != Etat::Pret;

    for (int f = 0; f < static_cast<int>(programme().size()); ++f) {
        dessinerTexte("F" + std::to_string(f + 1), 20, yProgramme + f * hauteurLigne + 10, 22);
        for (int i = 0; i < static_cast<int>(programme()[f].size()); ++i) {
            sf::FloatRect zone = zoneEmplacement(f, i);
            dessinerInstruction(programme()[f][i], zone, false);

            sf::RectangleShape cadre({zone.width, zone.height});
            cadre.setPosition(zone.left, zone.top);
            cadre.setFillColor(sf::Color::Transparent);
            cadre.setOutlineThickness(3);
            if (afficherCourante && courante.fonction == f && courante.indice == i)
                cadre.setOutlineColor(sf::Color::White);
            else if (m_selection.fonction == f && m_selection.indice == i)
                cadre.setOutlineColor(jaune);
            else
                continue;
            m_fenetre.draw(cadre);
        }
    }
}

void Jeu::dessinerInstruction(const Instruction& instruction, sf::FloatRect zone, bool palette) {
    sf::RectangleShape case_({zone.width, zone.height});
    case_.setPosition(zone.left, zone.top);
    case_.setFillColor(instruction.condition == Couleur::Aucune ? gris : assombrir(teinte(instruction.condition)));
    if (palette) {
        case_.setOutlineColor(sf::Color(110, 112, 125));
        case_.setOutlineThickness(1);
    }
    m_fenetre.draw(case_);

    sf::Vector2f centre(zone.left + zone.width / 2, zone.top + zone.height / 2);
    switch (instruction.action) {
        case Action::Avancer: {
            sf::ConvexShape forme = fleche(zone.width * 0.3f);
            forme.setPosition(centre);
            forme.setFillColor(sf::Color::White);
            m_fenetre.draw(forme);
            break;
        }
        case Action::Gauche:
            dessinerVirage(m_fenetre, centre, zone.width, -1);
            break;
        case Action::Droite:
            dessinerVirage(m_fenetre, centre, zone.width, 1);
            break;
        case Action::Appel:
            dessinerTexte("F" + std::to_string(instruction.fonction + 1), zone.left + 10, zone.top + 10, 20);
            break;
        case Action::Peindre: {
            sf::CircleShape tache(zone.width * 0.22f);
            tache.setOrigin(tache.getRadius(), tache.getRadius());
            tache.setPosition(centre);
            tache.setFillColor(teinte(instruction.peinture));
            tache.setOutlineColor(sf::Color::White);
            tache.setOutlineThickness(3);
            m_fenetre.draw(tache);
            break;
        }
        case Action::Vide:
            break;
    }
}

void Jeu::dessinerBouton(sf::FloatRect zone, const std::string& texte, bool actif) {
    sf::RectangleShape bouton({zone.width, zone.height});
    bouton.setPosition(zone.left, zone.top);
    bouton.setFillColor(actif ? sf::Color(60, 90, 140) : sf::Color(50, 52, 60));
    m_fenetre.draw(bouton);
    if (!m_policeChargee)
        return;
    sf::Text etiquette(sf::String::fromUtf8(texte.begin(), texte.end()), m_police, 18);
    sf::FloatRect bornes = etiquette.getLocalBounds();
    etiquette.setPosition(std::round(zone.left + (zone.width - bornes.width) / 2 - bornes.left),
                          std::round(zone.top + (zone.height - bornes.height) / 2 - bornes.top));
    etiquette.setFillColor(actif ? sf::Color::White : sf::Color(120, 120, 130));
    m_fenetre.draw(etiquette);
}

void Jeu::dessinerTexte(const std::string& texte, float x, float y, unsigned taille, sf::Color couleur, float largeurMax) {
    if (!m_policeChargee)
        return;
    sf::Text rendu("", m_police, taille);
    rendu.setFillColor(couleur);

    // Retour à la ligne entre les mots si le texte dépasse largeurMax
    std::istringstream mots(texte);
    std::string mot, ligne;
    float decalage = 0;
    auto ecrire = [&](const std::string& contenu) {
        rendu.setString(sf::String::fromUtf8(contenu.begin(), contenu.end()));
        rendu.setPosition(std::round(x), std::round(y + decalage));
        m_fenetre.draw(rendu);
        decalage += taille * 1.35f;
    };
    while (mots >> mot) {
        std::string essai = ligne.empty() ? mot : ligne + " " + mot;
        rendu.setString(sf::String::fromUtf8(essai.begin(), essai.end()));
        if (largeurMax > 0 && !ligne.empty() && rendu.getLocalBounds().width > largeurMax) {
            ecrire(ligne);
            ligne = mot;
        } else {
            ligne = essai;
        }
    }
    if (!ligne.empty())
        ecrire(ligne);
}

// ---------------------------------------------------------------- Positions

sf::FloatRect Jeu::zoneEmplacement(int fonction, int indice) const {
    return {70 + indice * pasCase, yProgramme + fonction * hauteurLigne, tailleCase, tailleCase};
}

std::vector<std::pair<sf::FloatRect, Instruction>> Jeu::paletteActions() const {
    std::vector<Instruction> modeles = {
        {Action::Avancer}, {Action::Gauche}, {Action::Droite},
        {Action::Peindre, 0, Couleur::Rouge}, {Action::Peindre, 0, Couleur::Vert}, {Action::Peindre, 0, Couleur::Bleu},
    };
    for (int f = 0; f < static_cast<int>(m_niveaux[m_indice].capacites.size()); ++f)
        modeles.push_back({Action::Appel, f});

    std::vector<std::pair<sf::FloatRect, Instruction>> palette;
    for (std::size_t i = 0; i < modeles.size(); ++i) {
        float x = xPanneau + (i % 6) * 54;
        float y = margeHaut + 185 + (i / 6) * 50;
        palette.push_back({{x, y, tailleCase, tailleCase}, modeles[i]});
    }
    return palette;
}

std::vector<std::pair<sf::FloatRect, Couleur>> Jeu::paletteConditions() const {
    const Couleur couleurs[] = {Couleur::Aucune, Couleur::Rouge, Couleur::Vert, Couleur::Bleu};
    std::vector<std::pair<sf::FloatRect, Couleur>> palette;
    float x = xPanneau;
    for (Couleur couleur : couleurs) {
        float largeur = couleur == Couleur::Aucune ? 70 : tailleCase;
        palette.push_back({{x, margeHaut + 315, largeur, tailleCase}, couleur});
        x += largeur + 10;
    }
    return palette;
}
