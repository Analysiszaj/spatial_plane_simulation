cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILDE_TYPE Debug)

project(spatial_plane_simulation)

add_subdirectory(./3rdparty/glad)
add_subdirectory(./3rdparty/imgui)

find_package(glm REQUIRED)

add_executable(${PROJECT_NAME} main.cpp app.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui  glad glm::glm)
target_include_directories(${PROJECT_NAME} PRIVATE ./3rdparty)