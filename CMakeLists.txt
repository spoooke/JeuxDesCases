cmake_minimum_required(VERSION 3.16)

project(JeuxDesCases CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Version Windows autonome : SFML compilé depuis ses sources, tout en statique
option(SFML_DEPUIS_SOURCES "Compiler SFML depuis ses sources" OFF)

if(MSVC)
  add_compile_options(/utf-8)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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

# Le jeu, sans console sous Windows
add_executable(JeuxDesCases WIN32 src/main.cpp src/Jeu.cpp)
target_link_libraries(JeuxDesCases moteur)
target_compile_definitions(JeuxDesCases PRIVATE DOSSIER_NIVEAUX="${DOSSIER_NIVEAUX}")

if(SFML_DEPUIS_SOURCES)
  include(FetchContent)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
  set(SFML_USE_STATIC_STD_LIBS ON CACHE BOOL "" FORCE)
  FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW ON)
  FetchContent_MakeAvailable(SFML)
  target_link_libraries(JeuxDesCases sfml-graphics)
  if(WIN32)
    target_link_libraries(JeuxDesCases sfml-main)
  endif()
else()
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
  find_package(SFML 2 REQUIRED graphics window system)
  target_include_directories(JeuxDesCases PRIVATE ${SFML_INCLUDE_DIR})
  target_link_libraries(JeuxDesCases ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()
