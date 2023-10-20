#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include "SFML/Graphics.hpp"

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    TileMap(const sf::Vector2u tileSize, const sf::Color* level, unsigned int width, unsigned int height);

private:
    sf::VertexArray m_vertices;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif
