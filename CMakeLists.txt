cmake_minimum_required(VERSION 3.10)

project(Endeavour)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cmake_modules/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(Endeavour src/main.cpp src/Game.cpp src/Entity.cpp src/Player.cpp src/pathfinding.cpp src/TextureManager.cpp src/AI.cpp src/Level.cpp src/Line.cpp src/common.cpp vendor/pugixml/pugixml.cpp)
add_executable(test src/tests.cpp src/Game.cpp src/Entity.cpp src/Player.cpp src/pathfinding.cpp src/TextureManager.cpp src/AI.cpp src/Level.cpp src/Line.cpp src/common.cpp vendor/pugixml/pugixml.cpp)

set_target_properties(Endeavour PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib"  
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)

target_include_directories(Endeavour PUBLIC
                           "include/"
                           "vendor/include"
                           "vendor/pugixml/"
                           ${SDL2_INCLUDE_DIRS}
                           ${SDL2_IMAGE_INCLUDE_DIRS}
                           ${SDL2_TTF_INCLUDE_DIRS}
                           )
target_include_directories(test PUBLIC
                           "include/"
                           "vendor/include"
                           "vendor/pugixml/"
                           ${SDL2_INCLUDE_DIRS}
                           ${SDL2_IMAGE_INCLUDE_DIRS}
                           ${SDL2_TTF_INCLUDE_DIRS}
                           )


target_link_libraries(Endeavour ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
target_link_libraries(test ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
