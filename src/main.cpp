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
    TileMap map(sf::Vector2u(32, 32), level, 10, 10);
    

    
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