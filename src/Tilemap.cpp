#include "Tilemap.hpp"

TileMap::TileMap(const sf::Vector2u tileSize, const sf::Color* level, unsigned int width, unsigned int height) {
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    for (unsigned int i = 0; i < width; ++i) {
        for (unsigned int j = 0; j < height; ++j) {
            sf::Vertex* quad = &m_vertices[(i + j * width) * 4];
            
            // Couleurs des quads : Blanc
            for (int k = 0; k < 4; ++k) {
                quad[k].color = level[i + j * width];
            }

            // position de la dalle
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
