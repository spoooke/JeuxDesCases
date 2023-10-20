#include "config.h.in"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

using namespace std;


int main()
{
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(800, 600), "TileMap");

    
    const int numTiles = 100;
    sf::Color level[numTiles];

    for (int i = 0; i < numTiles; ++i) {
        level[i] = sf::Color::White;
    }

    // rajout de TileMap dans le main.cpp
    TileMap map(sf::Vector2u(32, 64), level, 10, 10);
    
    // Calcul ce la position pour centrer la carte
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f mapPosition((windowSize.x - 10 * 32) / 2, (windowSize.y - 10 * 32) / 2);
    map.setPosition(mapPosition);
    
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        // Dessin de la map
        window.clear();
        window.draw(map);
        window.display();
    }

    return 0;
}