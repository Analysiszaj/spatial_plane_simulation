#ifndef __CORE_H
#define __CORE_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <vector>

class Core
{
private:
  GLuint grid_VAO_ = 0;
  unsigned int grid_vertex_num_ = 0;
  GLuint cube_VAO_ = 0;
  unsigned int cub_vertex_num_ = 0;

  GLuint shader_program_ = 0;

  glm::vec3 camera_position_ = glm::vec3(0.0f, 1.0f, -6.0f);
  glm::vec3 model_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 model_translate = glm::vec3(0.0f, 0.0f, 0.0f);

public:
  Core();
  ~Core();

  std::pair<std::string, std::string> read_shader_file(const char *vertex_path, const char *fragment_path);
  void init_program();
  void init_core();

  unsigned int build_grid_vertices(std::vector<float> &vertices, int grid_num);
  void init_grid_VAO();
  void init_cube_VAO();

  void render_cube();
  void render_grid();
  void render_tool_panel();
};

#endif