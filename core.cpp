#include "core.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>
#include <cassert>

Core::Core()
{
}

Core::~Core()
{
  if (grid_VAO_ != 0)
  {
    glDeleteVertexArrays(1, &grid_VAO_);
    grid_VAO_ = 0;
  }

  if (shader_program_ != 0)
  {
    glDeleteProgram(shader_program_);
    shader_program_ = 0;
  }
}

std::pair<std::string, std::string> Core::read_shader_file(const char *vertex_path, const char *fragment_path)
{
  std::string vertex_code;
  std::string fragment_code;
  std::ifstream v_shader_file;
  std::ifstream f_shader_file;

  v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    v_shader_file.open(vertex_path);
    f_shader_file.open(fragment_path);

    std::stringstream v_shader_stream, f_shader_stream;
    v_shader_stream << v_shader_file.rdbuf();
    f_shader_stream << f_shader_file.rdbuf();

    v_shader_file.close();
    f_shader_file.close();

    vertex_code = v_shader_stream.str();
    fragment_code = f_shader_stream.str();
  }
  catch (std::ifstream::failure e)
  {
    std::cout << "ERROR::SHADER::FILE_NO_SUCESSFULLY_READ:\n"
              << std::endl;
  }
  assert(!vertex_code.empty() && "Vertex shader content cannot be empty");
  assert(!fragment_code.empty() && "Fragment shader content cannot be empty");

  return {vertex_code, fragment_code};
}

void Core::init_program()
{
  auto [vertex_code, fragment_code] = read_shader_file("/Users/mds/my/spatial_plane_simulation/glsl/vertex.glsl",
                                                       "/Users/mds/my/spatial_plane_simulation/glsl/fragment.glsl");
  const char *v_shader_code = vertex_code.c_str();
  const char *f_shader_code = fragment_code.c_str();

  GLuint vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &v_shader_code, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog
              << std::endl;
    assert(false && "Vertex Shader Compile Error");
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &f_shader_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog
              << std::endl;
    assert(false && "Fragment Shader Compile Error");
  }

  shader_program_ = glCreateProgram();
  glAttachShader(shader_program_, vertex);
  glAttachShader(shader_program_, fragment);
  glLinkProgram(shader_program_);

  glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shader_program_, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog
              << std::endl;
    assert(false && "Link Shader Program Error");
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Core::init_core()
{
  init_program();
  init_grid_VAO();
  init_cube_VAO();
}

unsigned int Core::build_grid_vertices(std::vector<float> &vertices, int grid_num)
{
  int area = grid_num / 2;
  for (int i = -area; i <= area; i++)
  {
    vertices.push_back(-area);
    vertices.push_back(0.0f);
    vertices.push_back(i);
    vertices.push_back(area);
    vertices.push_back(0.0f);
    vertices.push_back(i);
  }
  for (int i = -area; i <= area; ++i)
  {
    vertices.push_back(i);
    vertices.push_back(0.0f);
    vertices.push_back(-area);
    vertices.push_back(i);
    vertices.push_back(0.0f);
    vertices.push_back(area);
  }
  return vertices.size();
}

void Core::init_grid_VAO()
{
  // float vertices[] = {
  //     0.5f, 0.5f, 0.0f,   // top right
  //     0.5f, -0.5f, 0.0f,  // bottom right
  //     -0.5f, -0.5f, 0.0f, // bottom left
  //     -0.5f, 0.5f, 0.0f   // top left
  // };

  // unsigned int indices[] = {
  //     0, 1, 2,
  //     0, 2, 3};

  std::vector<float> vertices;
  grid_vertex_num_ = build_grid_vertices(vertices, 30);
  grid_vertex_num_ = grid_vertex_num_ / 3;

  GLuint VBO;
  glGenVertexArrays(1, &grid_VAO_);
  glGenBuffers(1, &VBO);

  glBindVertexArray(grid_VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &VBO);
}

void Core::init_cube_VAO()
{
  float vertices[] = {
      // 前面 (z = 0.25)
      -1.0f, -0.5f, 0.25f,
      1.0f, -0.5f, 0.25f,
      1.0f, 0.5f, 0.25f,
      1.0f, 0.5f, 0.25f,
      -1.0f, 0.5f, 0.25f,
      -1.0f, -0.5f, 0.25f,

      // 后面 (z = -0.25)
      -1.0f, -0.5f, -0.25f,
      1.0f, -0.5f, -0.25f,
      1.0f, 0.5f, -0.25f,
      1.0f, 0.5f, -0.25f,
      -1.0f, 0.5f, -0.25f,
      -1.0f, -0.5f, -0.25f,

      // 左面 (x = -1.0)
      -1.0f, 0.5f, 0.25f,
      -1.0f, 0.5f, -0.25f,
      -1.0f, -0.5f, -0.25f,
      -1.0f, -0.5f, -0.25f,
      -1.0f, -0.5f, 0.25f,
      -1.0f, 0.5f, 0.25f,

      // 右面 (x = 1.0)
      1.0f, 0.5f, 0.25f,
      1.0f, 0.5f, -0.25f,
      1.0f, -0.5f, -0.25f,
      1.0f, -0.5f, -0.25f,
      1.0f, -0.5f, 0.25f,
      1.0f, 0.5f, 0.25f,

      // 底面 (y = -0.5)
      -1.0f, -0.5f, -0.25f,
      1.0f, -0.5f, -0.25f,
      1.0f, -0.5f, 0.25f,
      1.0f, -0.5f, 0.25f,
      -1.0f, -0.5f, 0.25f,
      -1.0f, -0.5f, -0.25f,

      // 顶面 (y = 0.5)
      -1.0f, 0.5f, -0.25f,
      1.0f, 0.5f, -0.25f,
      1.0f, 0.5f, 0.25f,
      1.0f, 0.5f, 0.25f,
      -1.0f, 0.5f, 0.25f,
      -1.0f, 0.5f, -0.25f};

  cub_vertex_num_ = 36;

  GLuint VBO;
  glGenVertexArrays(1, &cube_VAO_);
  glGenBuffers(1, &VBO);

  glBindVertexArray(cube_VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &VBO);
}

void Core::render_cube()
{
  glUseProgram(shader_program_);
  glBindVertexArray(cube_VAO_);

  GLuint grid_color = glGetUniformLocation(shader_program_, "ObjectColor");
  glUniform3f(grid_color, 0.0f, 1.0f, 0.0f);

  glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(model, model_translate);

  model = glm::rotate(model, glm::radians(model_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(model_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(model_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

  GLuint transformLocModel = glGetUniformLocation(shader_program_, "model");
  glUniformMatrix4fv(transformLocModel, 1, GL_FALSE, glm::value_ptr(model));

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Core::render_grid()
{
  glUseProgram(shader_program_);
  glBindVertexArray(grid_VAO_);

  GLuint grid_color = glGetUniformLocation(shader_program_, "ObjectColor");
  glUniform3f(grid_color, 0.0f, 0.0f, 0.0f);

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 view = glm::lookAt(
      camera_position_,
      glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
      glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向
  );

  glm::mat4 projection;
  projection = glm::perspective(glm::radians(55.0f), 1280.0f / 800.0f, 0.1f, 100.0f);

  GLuint transformLocModel = glGetUniformLocation(shader_program_, "model");
  glUniformMatrix4fv(transformLocModel, 1, GL_FALSE, glm::value_ptr(model));

  GLuint transformLocView = glGetUniformLocation(shader_program_, "view");
  glUniformMatrix4fv(transformLocView, 1, GL_FALSE, glm::value_ptr(view));

  GLuint transformLocProjection = glGetUniformLocation(shader_program_, "projection");
  glUniformMatrix4fv(transformLocProjection, 1, GL_FALSE, glm::value_ptr(projection));

  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_LINES, 0, grid_vertex_num_);
  glBindVertexArray(0);
}

void Core::render_tool_panel()
{
  ImGui::Begin("调试");
  ImGui::SeparatorText("空间设置");
  ImGui::SliderFloat3("相机矩阵", glm::value_ptr(camera_position_), -20.0f, 20.0f);

  ImGui::SeparatorText("模型设置");
  ImGui::DragFloat3("模型旋转", glm::value_ptr(model_rotation), 1.0f, -180.0f, 180.0f);
  ImGui::DragFloat3("模型移动", glm::value_ptr(model_translate), 0.01f, -15.0f, 15.0f);

  ImGui::End();
}
