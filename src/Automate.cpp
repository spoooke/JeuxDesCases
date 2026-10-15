#include "Automate.hpp"

namespace {

const int dx[] = {0, 1, 0, -1};
const int dy[] = {-1, 0, 1, 0};

// Au-delà, on considère que le programme tourne en rond
const int pasMaximum = 2000;
const std::size_t fileMaximum = 10000;

} // namespace

Programme programmeVide(const Niveau& niveau) {
    Programme programme;
    for (int capacite : niveau.capacites)
        programme.emplace_back(capacite);
    return programme;
}

void Automate::demarrer(const Niveau& niveau, const Programme& programme) {
    m_programme = &programme;
    m_largeur = niveau.largeur;
    m_hauteur = niveau.hauteur;
    m_cases = niveau.cases;
    m_etoiles = niveau.etoiles;
    m_x = niveau.departX;
    m_y = niveau.departY;
    m_direction = niveau.departDirection;
    m_file.clear();
    m_courante = {-1, -1};
    m_pas = 0;
    m_etat = Etat::EnCours;
    m_message.clear();

    // Une étoile sur la case de départ est ramassée tout de suite
    m_etoiles[m_y * m_largeur + m_x] = false;
    m_restantes = 0;
    for (bool etoile : m_etoiles)
        m_restantes += etoile;

    if (m_restantes == 0) {
        m_etat = Etat::Gagne;
        return;
    }
    // L'automate commence toujours par F1
    empiler(0);
}

void Automate::empiler(int fonction) {
    const auto& instructions = (*m_programme)[fonction];
    for (int i = static_cast<int>(instructions.size()) - 1; i >= 0; --i)
        if (instructions[i].action != Action::Vide)
            m_file.push_front({fonction, i});
}

void Automate::perdre(const std::string& raison) {
    m_etat = Etat::Perdu;
    m_message = raison;
}

Etat Automate::pas() {
    if (m_etat != Etat::EnCours)
        return m_etat;

    if (m_file.empty()) {
        m_courante = {-1, -1};
        perdre("Plus d'instructions, il reste " + std::to_string(m_restantes) + " étoile(s).");
        return m_etat;
    }

    m_courante = m_file.front();
    m_file.pop_front();
    const Instruction& instruction = (*m_programme)[m_courante.fonction][m_courante.indice];
    ++m_pas;

    Couleur& sousAutomate = m_cases[m_y * m_largeur + m_x];
    if (instruction.condition != Couleur::Aucune && instruction.condition != sousAutomate)
        return m_etat;

    switch (instruction.action) {
        case Action::Avancer: {
            int nx = m_x + dx[m_direction];
            int ny = m_y + dy[m_direction];
            if (nx < 0 || ny < 0 || nx >= m_largeur || ny >= m_hauteur || m_cases[ny * m_largeur + nx] == Couleur::Aucune) {
                perdre("L'automate est sorti du quadrillage.");
                return m_etat;
            }
            m_x = nx;
            m_y = ny;
            if (m_etoiles[ny * m_largeur + nx]) {
                m_etoiles[ny * m_largeur + nx] = false;
                if (--m_restantes == 0) {
                    m_etat = Etat::Gagne;
                    m_message = "Toutes les étoiles sont ramassées !";
                    return m_etat;
                }
            }
            break;
        }
        case Action::Gauche:
            m_direction = static_cast<Direction>((m_direction + 3) % 4);
            break;
        case Action::Droite:
            m_direction = static_cast<Direction>((m_direction + 1) % 4);
            break;
        case Action::Appel:
            empiler(instruction.fonction);
            if (m_file.size() > fileMaximum)
                perdre("Trop d'appels imbriqués.");
            break;
        case Action::Peindre:
            sousAutomate = instruction.peinture;
            break;
        case Action::Vide:
            break;
    }

    if (m_etat == Etat::EnCours && m_pas >= pasMaximum)
        perdre("Trop de pas, le programme tourne en rond.");
    return m_etat;
}

Etat Automate::executer(int limite) {
    for (int i = 0; i < limite && m_etat == Etat::EnCours; ++i)
        pas();
    return m_etat;
}
