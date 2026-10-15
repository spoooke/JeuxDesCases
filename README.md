<div align="center">

# Jeux des cases

**A tile grid drawn with SFML, first step of a small tile-based game.**

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square)
![SFML](https://img.shields.io/badge/SFML-2.x-8CC445?style=flat-square)

</div>

A 10 × 10 grid of tiles, centred in an 800 × 600 window. Each tile has its own colour, and the grid is saved to a text file (`level1.lvl`).

| | |
|---|---|
| `src/Tilemap` | the grid, drawn as a single vertex array |
| `src/TileMapLoader` | saves the grid to a file |
| `cmake_modules/` | SFML detection for CMake |

### Build

Needs SFML 2.x (not 3).

```bash
brew install sfml@2
cmake -B build -DSFML_ROOT="$(brew --prefix sfml@2)"
cmake --build build
./bin/SFML_Project
```

---

Une grille de 10 × 10 cases, centrée dans une fenêtre de 800 × 600. Chaque case a sa propre couleur, et la grille est enregistrée dans un fichier texte (`level1.lvl`).

| | |
|---|---|
| `src/Tilemap` | la grille, dessinée en un seul tableau de sommets |
| `src/TileMapLoader` | enregistre la grille dans un fichier |
| `cmake_modules/` | détection de SFML pour CMake |

### Compiler

Nécessite SFML 2.x (pas la 3).

```bash
brew install sfml@2
cmake -B build -DSFML_ROOT="$(brew --prefix sfml@2)"
cmake --build build
./bin/SFML_Project
```
