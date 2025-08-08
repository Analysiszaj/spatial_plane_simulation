
project(imgui)


find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(${PROJECT_NAME} imgui.cpp 
                  imgui_demo.cpp 
                  imgui_draw.cpp
                  imgui_tables.cpp
                  imgui_widgets.cpp
                  backend/imgui_impl_glfw.cpp
                  backend/imgui_impl_opengl3.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw OpenGL::GL)

target_include_directories(imgui PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/backend>
  $<INSTALL_INTERFACE:include>
)