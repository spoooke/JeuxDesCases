#include "config.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

using namespace std;



/*int main() {
    cout << "Version " << myproject_VERSION_MAJOR << "." << myproject_VERSION_MINOR << endl;

    // Create a window with SFML
    sf::RenderWindow App(sf::VideoMode(800, 600), "SFML_Project");

    // Create a render texture
    sf::RenderTexture renderTexture;
    
    if (!renderTexture.create(500, 500)) {
        // Handle error...
    }

    // Create a texture to store your white square
    sf::Texture whiteSquareTexture;
    whiteSquareTexture.create(50, 50); // Change size as needed

    // Create a sprite for the white square
    sf::Sprite whiteSquare(whiteSquareTexture);

    // Fill the white square with white color
    sf::Uint8* pixels = new sf::Uint8[50 * 50 * 4]; // * 4 because pixels have 4 components (RGBA)
    for (int i = 0; i < 50 * 50; ++i) {
        pixels[i * 4] = 255;   // Red
        pixels[i * 4 + 1] = 255; // Green
        pixels[i * 4 + 2] = 255; // Blue
        pixels[i * 4 + 3] = 255; // Alpha (fully opaque)
    }
    whiteSquareTexture.update(pixels);

    whiteSquare.setPosition(10.f, 50.f); // Set the position of the white square

    while (App.isOpen()) {
        sf::Event Event;
        while (App.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed)
                App.close();
        }

        // Clear the render texture and draw the white square on it
        renderTexture.clear();
        renderTexture.draw(whiteSquare);
        renderTexture.display();
        
        // Clear the main window and draw the white square from the render texture
        App.clear();
        App.draw(whiteSquare);
        App.display();
    }

    return 0;
}
*/
int main()
{
    // on crée la fenêtre
    sf::RenderWindow window(sf::VideoMode(512, 256), "Tilemap");

    // on définit le niveau à l'aide de numéro de tuiles
    const sf::Color level[] =
    {
        sf::Color::Blue, sf::Color::Red, 
        sf::Color::Yellow, sf::Color::Black
    };

    // on crée la tilemap avec le niveau précédemment défini
    TileMap map(sf::Vector2u(32, 32), level, 2, 2);
    

    // on fait tourner la boucle principale
    while (window.isOpen())
    {
        // on gère les évènements
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        // on dessine le niveau
        window.clear();
        window.draw(map);
        window.display();
    }

    return 0;
}