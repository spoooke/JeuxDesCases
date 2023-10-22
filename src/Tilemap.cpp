#include "Tilemap.hpp"

TileMap::TileMap(std::string* filepath, const sf::Vector2u tileSize, const sf::Color* level, unsigned int width, unsigned int height) {
    m_filepath = filepath;
    m_height = height;
    m_width = width;
    m_level = level;
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

sf::Color TileMap:: element_at (int i, int j) {

    return m_level[i + j * m_width];
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_vertices, states);
}
