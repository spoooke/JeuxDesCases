#include "TileMapLoader.hpp"
#include "Tilemap.hpp"
#include <fstream>

void TileMapLoader::Save(TileMap& tilemap){
    std::ofstream fichier(tilemap.m_filepath->c_str());
    for (int i = 0; i < tilemap.m_height ; i++)
    {
        for (int j = 0; j < tilemap.m_width; j++)
        {
            sf::Color c = tilemap.element_at(i, j);
            fichier << c.toInteger () << " ";
            
        }
        fichier << std::endl;
    }
    
    fichier << std::endl;
    
}   