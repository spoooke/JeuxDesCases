#include "config.h.in"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

using namespace std;


int main()
{
    // on crée la fenêtre
    sf::RenderWindow window(sf::VideoMode(512, 256), "TileMap");

    // on définit le niveau à l'aide de numéro de tuiles
    const sf::Color level[] =
    {
        sf::Color::Blue, sf::Color::Red, 
        sf::Color::Yellow, sf::Color::Black
    };

    // rajout de TileMap dans le main.cpp
    TileMap map(sf::Vector2u(32, 32), level, 2, 2);
    

    
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