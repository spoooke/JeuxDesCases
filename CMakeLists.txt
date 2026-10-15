cmake_minimum_required(VERSION 3.16)

project(JeuxDesCases CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(DOSSIER_NIVEAUX "${CMAKE_SOURCE_DIR}/niveaux")

# Règles du jeu, sans dépendance à SFML
add_library(moteur STATIC src/Niveau.cpp src/Automate.cpp)
target_include_directories(moteur PUBLIC src)

enable_testing()
add_executable(verifier tests/verifier.cpp)
target_link_libraries(verifier moteur)
target_compile_definitions(verifier PRIVATE DOSSIER_NIVEAUX="${DOSSIER_NIVEAUX}")
add_test(NAME niveaux COMMAND verifier)

# Le jeu
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SFML 2 REQUIRED graphics window system)

add_executable(JeuxDesCases src/main.cpp src/Jeu.cpp)
target_include_directories(JeuxDesCases PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(JeuxDesCases moteur ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_compile_definitions(JeuxDesCases PRIVATE DOSSIER_NIVEAUX="${DOSSIER_NIVEAUX}")
