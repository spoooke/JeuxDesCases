// TileMap.cpp
#include "TileMap.hpp"  

TileMap::TileMap(const sf::Vector2u tileSize, unsigned int width, unsigned int height) {
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j) {
            // Obtenir un pointeur vers le quad à definir dans le tableau de vertex
            sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

            
            for (int k = 0; k < 4; ++k) {
                quad[k].color = sf::Color::White;
            }

            // Positions des coins du quad
            quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
        }
    }
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_vertices, states);
}
