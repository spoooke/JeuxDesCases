#ifndef TILEMAPLOADER_HPP
#define TILEMAPLOADER_HPP

#include "SFML/Graphics.hpp"
#include "Tilemap.hpp"

class TileMapLoader {
public:
        
    static TileMap Load(const std::string& filepath);  
    static void Save(TileMap& tilemap);

};
#endif