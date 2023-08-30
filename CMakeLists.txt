cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

project(runme VERSION 1.0.0)

add_executable(runme proj.c)

target_link_libraries(
  runme PRIVATE
  ../SDL_lib/SDL2.framework/Versions/A/SDL2
  ../SDL_lib/Frameworks/SDL2_image.framework/Versions/A/SDL2_image
  ../SDL_lib/Frameworks/SDL2_ttf.framework/Versions/A/SDL2_ttf
)

target_include_directories(
  runme PRIVATE
  ../SDL_lib/SDL2.framework/Versions/A/Headers
  ../SDL_lib/SDL2_image.framework/Versions/A/Headers
  ../SDL_lib/SDL2_ttf.framework/Versions/A/Headers
)