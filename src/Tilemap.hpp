#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include "SFML/Graphics.hpp"

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    TileMap(std::string* filepath, const sf::Vector2u tileSize, const sf::Color* level, unsigned int width, unsigned int height);
    sf::Color element_at (int i, int j);
    std::string* m_filepath;
    unsigned int m_width; 
    unsigned int m_height;
    const sf::Color* m_level;
private:
    sf::VertexArray m_vertices;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif
