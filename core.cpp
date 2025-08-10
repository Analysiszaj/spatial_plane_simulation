#include "core.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>
#include <cassert>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

  if (cube_VAO_ != 0)
  {
    glDeleteVertexArrays(1, &cube_VAO_);
    cube_VAO_ = 0;
  }

  if (path_VAO_ != 0)
  {
    glDeleteVertexArrays(1, &path_VAO_);
    path_VAO_ = 0;
  }

  if (left_track_VAO_ != 0)
  {
    glDeleteVertexArrays(1, &left_track_VAO_);
    left_track_VAO_ = 0;
  }

  if (right_track_VAO_ != 0)
  {
    glDeleteVertexArrays(1, &right_track_VAO_);
    right_track_VAO_ = 0;
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
  init_path_VAO();
  init_track_VAOs();
  init_predefined_path();
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
  // 修正长方体尺寸，让它看起来更像一个车子
  // X轴（宽度）: -0.4 到 0.4 （宽度 = 0.8）
  // Y轴（高度）: -0.3 到 0.3 （高度 = 0.6）
  // Z轴（长度）: -1.0 到 1.0 （长度 = 2.0，这是车子的前后方向）
  float vertices[] = {
      // 前面 (z = 1.0, 车头)
      -0.4f, -0.3f, 1.0f,
      0.4f, -0.3f, 1.0f,
      0.4f, 0.3f, 1.0f,
      0.4f, 0.3f, 1.0f,
      -0.4f, 0.3f, 1.0f,
      -0.4f, -0.3f, 1.0f,

      // 后面 (z = -1.0, 车尾)
      -0.4f, -0.3f, -1.0f,
      0.4f, -0.3f, -1.0f,
      0.4f, 0.3f, -1.0f,
      0.4f, 0.3f, -1.0f,
      -0.4f, 0.3f, -1.0f,
      -0.4f, -0.3f, -1.0f,

      // 左面 (x = -0.4)
      -0.4f, 0.3f, 1.0f,
      -0.4f, 0.3f, -1.0f,
      -0.4f, -0.3f, -1.0f,
      -0.4f, -0.3f, -1.0f,
      -0.4f, -0.3f, 1.0f,
      -0.4f, 0.3f, 1.0f,

      // 右面 (x = 0.4)
      0.4f, 0.3f, 1.0f,
      0.4f, 0.3f, -1.0f,
      0.4f, -0.3f, -1.0f,
      0.4f, -0.3f, -1.0f,
      0.4f, -0.3f, 1.0f,
      0.4f, 0.3f, 1.0f,

      // 底面 (y = -0.3)
      -0.4f, -0.3f, -1.0f,
      0.4f, -0.3f, -1.0f,
      0.4f, -0.3f, 1.0f,
      0.4f, -0.3f, 1.0f,
      -0.4f, -0.3f, 1.0f,
      -0.4f, -0.3f, -1.0f,

      // 顶面 (y = 0.3)
      -0.4f, 0.3f, -1.0f,
      0.4f, 0.3f, -1.0f,
      0.4f, 0.3f, 1.0f,
      0.4f, 0.3f, 1.0f,
      -0.4f, 0.3f, 1.0f,
      -0.4f, 0.3f, -1.0f};

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

void Core::init_path_VAO()
{
  // 初始化为空的VAO，后续通过update_path_VAO更新
  glGenVertexArrays(1, &path_VAO_);
  glBindVertexArray(path_VAO_);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // 设置顶点属性
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &VBO);

  path_vertex_num_ = 0;
}

void Core::render_cube()
{
  glUseProgram(shader_program_);
  glBindVertexArray(cube_VAO_);

  GLuint grid_color = glGetUniformLocation(shader_program_, "ObjectColor");
  glUniform3f(grid_color, 0.0f, 1.0f, 0.0f);

  glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(model, model_translate);

  // 在跟随模式下，让模型的Y轴旋转跟随偏航角
  if (follow_model_)
  {
    model = glm::rotate(model, glm::radians(yaw_angle_), glm::vec3(0.0f, 1.0f, 0.0f)); // 先应用偏航角
  }

  model = glm::rotate(model, glm::radians(model_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(model_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(model_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 view;
  if (follow_model_)
  {
    // 摄像机看向模型的位置
    view = glm::lookAt(
        camera_position_,
        model_translate, // 看向模型位置，而不是原点
        glm::vec3(0.0f, 1.0f, 0.0f));
  }
  else
  {
    view = glm::lookAt(
        camera_position_,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::mat4 projection = glm::perspective(glm::radians(55.0f), 1280.0f / 800.0f, 0.1f, 100.0f);

  GLuint transformLocModel = glGetUniformLocation(shader_program_, "model");
  glUniformMatrix4fv(transformLocModel, 1, GL_FALSE, glm::value_ptr(model));

  GLuint transformLocView = glGetUniformLocation(shader_program_, "view");
  glUniformMatrix4fv(transformLocView, 1, GL_FALSE, glm::value_ptr(view));

  GLuint transformLocProjection = glGetUniformLocation(shader_program_, "projection");
  glUniformMatrix4fv(transformLocProjection, 1, GL_FALSE, glm::value_ptr(projection));

  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void Core::render_grid()
{
  // 更新路径播放
  if (is_playing_)
  {
    update_path_playback();
  }

  // 更新摄像机跟随
  if (follow_model_)
  {
    update_camera_follow();
  }

  glUseProgram(shader_program_);
  glBindVertexArray(grid_VAO_);

  GLuint grid_color = glGetUniformLocation(shader_program_, "ObjectColor");
  glUniform3f(grid_color, 0.0f, 0.0f, 0.0f);

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 view;
  if (follow_model_)
  {
    // 摄像机看向模型的位置
    view = glm::lookAt(
        camera_position_,
        model_translate, // 看向模型位置
        glm::vec3(0.0f, 1.0f, 0.0f));
  }
  else
  {
    view = glm::lookAt(
        camera_position_,
        glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
        glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向
    );
  }

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

  ImGui::Checkbox("摄像机跟随", &follow_model_);

  if (!follow_model_)
  {
    // 只有在非跟随模式下才显示手动摄像机控制
    ImGui::SliderFloat3("相机位置", glm::value_ptr(camera_position_), -20.0f, 20.0f);
  }
  else
  {
    // 跟随模式下显示跟随参数
    ImGui::SliderFloat("相机距离", &camera_distance_, 2.0f, 20.0f);
    ImGui::SliderFloat("相机高度", &camera_height_, 0.5f, 10.0f);
  }

  ImGui::SeparatorText("路径播放");

  // 播放控制按钮
  if (!is_playing_)
  {
    if (ImGui::Button("播放路径"))
    {
      start_path_playback();
    }
  }
  else
  {
    if (ImGui::Button("停止播放"))
    {
      stop_path_playback();
    }
  }

  ImGui::SameLine();
  if (ImGui::Button("重置路径"))
  {
    reset_path_playback();
  }

  // 播放设置
  ImGui::SliderFloat("播放速度", &play_speed_, 0.1f, 5.0f);
  ImGui::Checkbox("循环播放", &loop_play_);

  // 播放状态显示
  if (is_playing_)
  {
    float current_time = (ImGui::GetTime() - play_start_time_) * play_speed_;
    ImGui::Text("播放状态: 进行中");
    ImGui::Text("当前时间: %.2f秒", current_time);
    ImGui::Text("路径点: %d/%zu", current_path_index_, predefined_path_.size());
    ImGui::Text("当前朝向: %.1f°", yaw_angle_);
  }
  else
  {
    ImGui::Text("播放状态: 停止");
  }

  // 路径显示控制
  ImGui::Checkbox("显示中心线", &show_path_);
  ImGui::Checkbox("显示赛道边界", &show_track_boundaries_);

  if (ImGui::SliderFloat("赛道宽度", &track_lane_width_, 0.5f, 3.0f))
  {
    // 当赛道宽度改变时，重新生成边界
    generate_track_boundaries();
  }

  if (ImGui::Button("清空轨迹"))
  {
    clear_traveled_path();
  }

  ImGui::Text("预定义路径点: %zu", predefined_path_.size());
  ImGui::Text("轨迹点数: %zu", traveled_path_.size());

  ImGui::SeparatorText("模型控制");

  // 只在非播放状态下显示手动控制
  if (!is_playing_)
  {
    if (!follow_model_)
    {
      // 非跟随模式下显示完整的旋转控制
      ImGui::DragFloat3("模型旋转", glm::value_ptr(model_rotation), 1.0f, -180.0f, 180.0f);
      ImGui::DragFloat3("模型移动", glm::value_ptr(model_translate), 0.01f, -15.0f, 15.0f);
    }
    else
    {
      // 跟随模式下只显示X和Z轴旋转（Y轴由偏航角控制）
      ImGui::DragFloat("模型X轴旋转", &model_rotation.x, 1.0f, -180.0f, 180.0f);
      ImGui::DragFloat("模型Z轴旋转", &model_rotation.z, 1.0f, -180.0f, 180.0f);

      // 跟随模式下用偏航角控制移动方向和朝向
      ImGui::SliderFloat("偏航角 (车头朝向)", &yaw_angle_, -180.0f, 180.0f);
      if (ImGui::Button("前进"))
      {
        float yaw_rad = glm::radians(yaw_angle_);
        model_translate.x += 0.1f * sin(yaw_rad);
        model_translate.z += 0.1f * cos(yaw_rad);
        update_traveled_path(); // 手动移动时也记录轨迹
      }
      ImGui::SameLine();
      if (ImGui::Button("后退"))
      {
        float yaw_rad = glm::radians(yaw_angle_);
        model_translate.x -= 0.1f * sin(yaw_rad);
        model_translate.z -= 0.1f * cos(yaw_rad);
        update_traveled_path(); // 手动移动时也记录轨迹
      }
      if (ImGui::Button("左转"))
      {
        yaw_angle_ -= 5.0f;
        if (yaw_angle_ < -180.0f)
          yaw_angle_ = 180.0f;
      }
      ImGui::SameLine();
      if (ImGui::Button("右转"))
      {
        yaw_angle_ += 5.0f;
        if (yaw_angle_ > 180.0f)
          yaw_angle_ = -180.0f;
      }
    }
  }
  else
  {
    ImGui::TextDisabled("播放中，手动控制已禁用");
  }

  ImGui::End();
}

void Core::update_camera_follow()
{
  // 汽车导航式跟随：摄像机在模型后方，跟随模型的朝向
  float yaw_radians = glm::radians(yaw_angle_);

  // 计算模型的后方位置（相对于模型朝向）
  glm::vec3 backward_direction;
  backward_direction.x = -sin(yaw_radians); // 模型后方的 x 方向
  backward_direction.y = 0.0f;
  backward_direction.z = -cos(yaw_radians); // 模型后方的 z 方向

  // 摄像机位置 = 模型位置 + 后方偏移 + 高度偏移
  camera_position_ = model_translate + backward_direction * camera_distance_;
  camera_position_.y = model_translate.y + camera_height_;
}

void Core::init_predefined_path()
{
  // 创建一个简单的圆形赛道，确保在网格范围内（-15到15）
  predefined_path_.clear();

  const int total_points = 1000;
  const float total_time = 120.0f; // 总时长120秒
  const float time_step = total_time / total_points;

  // 圆形赛道参数
  const float radius = 10.0f; // 半径10，确保在网格范围内

  for (int i = 0; i < total_points; i++)
  {
    float t = (float)i / (total_points - 1); // 0.0 到 1.0
    float time = i * time_step;

    // 圆形的参数方程
    float angle = t * 2.0f * M_PI; // 完整的一圈，从0到2π

    glm::vec3 position;
    position.x = radius * cos(angle);
    position.z = radius * sin(angle);
    position.y = 0.0f;

    predefined_path_.push_back({position, 0.0f, time});
  }

  // 自动计算每个路径点的正确朝向
  calculate_path_orientations();

  // 生成赛道边界
  generate_track_boundaries();
}

void Core::start_path_playback()
{
  if (!predefined_path_.empty())
  {
    is_playing_ = true;
    play_start_time_ = ImGui::GetTime();
    current_path_index_ = 0;

    // 设置初始位置
    model_translate = predefined_path_[0].position;
    yaw_angle_ = predefined_path_[0].yaw;
  }
}

void Core::stop_path_playback()
{
  is_playing_ = false;
}

void Core::reset_path_playback()
{
  is_playing_ = false;
  current_path_index_ = 0;
  clear_traveled_path(); // 重置时清空轨迹
  if (!predefined_path_.empty())
  {
    model_translate = predefined_path_[0].position;
    yaw_angle_ = predefined_path_[0].yaw;
  }
}

void Core::update_path_playback()
{
  if (!is_playing_ || predefined_path_.empty())
  {
    return;
  }

  float current_time = (ImGui::GetTime() - play_start_time_) * play_speed_;

  // 找到当前时间对应的路径段
  while (current_path_index_ < predefined_path_.size() - 1 &&
         current_time > predefined_path_[current_path_index_ + 1].timestamp)
  {
    current_path_index_++;
  }

  // 检查是否到达路径末尾
  if (current_path_index_ >= predefined_path_.size() - 1)
  {
    if (loop_play_)
    {
      // 循环播放，重新开始
      reset_path_playback();
      start_path_playback();
      return;
    }
    else
    {
      // 停止播放
      stop_path_playback();
      return;
    }
  }

  // 在当前路径段内进行插值
  const PathPoint &current_point = predefined_path_[current_path_index_];
  const PathPoint &next_point = predefined_path_[current_path_index_ + 1];

  float segment_duration = next_point.timestamp - current_point.timestamp;
  float segment_progress = (current_time - current_point.timestamp) / segment_duration;
  segment_progress = glm::clamp(segment_progress, 0.0f, 1.0f);

  // 插值计算当前位置和朝向
  model_translate = interpolate_position(current_point, next_point, segment_progress);

  // 改进：使用更平滑的朝向计算
  glm::vec3 current_direction = next_point.position - current_point.position;

  // 计算目标朝向，考虑前瞻性转向
  float target_yaw;
  if (current_path_index_ + 2 < predefined_path_.size())
  {
    const PathPoint &next_next_point = predefined_path_[current_path_index_ + 2];
    glm::vec3 future_direction = next_next_point.position - next_point.position;

    // 使用更平滑的前瞻混合
    float lookahead_factor = glm::smoothstep(0.3f, 1.0f, segment_progress);
    glm::vec3 blended_direction = glm::mix(current_direction, future_direction, lookahead_factor * 0.3f);

    if (glm::length(blended_direction) > 0.001f)
    {
      target_yaw = glm::degrees(atan2(blended_direction.x, blended_direction.z));
    }
    else
    {
      target_yaw = yaw_angle_; // 保持当前朝向
    }
  }
  else
  {
    // 使用当前移动方向
    if (glm::length(current_direction) > 0.001f)
    {
      target_yaw = glm::degrees(atan2(current_direction.x, current_direction.z));
    }
    else
    {
      target_yaw = yaw_angle_;
    }
  }

  // 使用更平滑的插值到目标朝向，动态调整插值速度
  float turn_speed = 0.08f; // 基础转向速度
  float angle_diff = abs(interpolate_yaw(yaw_angle_, target_yaw, 1.0f) - yaw_angle_);

  // 如果角度差异很大，稍微加快转向速度
  if (angle_diff > 45.0f)
  {
    turn_speed = 0.12f;
  }
  else if (angle_diff < 10.0f)
  {
    turn_speed = 0.05f; // 小角度时更平滑
  }

  yaw_angle_ = interpolate_yaw(yaw_angle_, target_yaw, turn_speed);

  // 更新轨迹记录
  update_traveled_path();
}

glm::vec3 Core::interpolate_position(const PathPoint &p1, const PathPoint &p2, float t)
{
  // 使用平滑的Hermite插值（可以使用线性插值：mix）
  return glm::mix(p1.position, p2.position, t);
}

float Core::interpolate_yaw(float yaw1, float yaw2, float t)
{
  // 处理角度插值，考虑360度环绕
  float diff = yaw2 - yaw1;

  // 选择最短路径
  if (diff > 180.0f)
  {
    diff -= 360.0f;
  }
  else if (diff < -180.0f)
  {
    diff += 360.0f;
  }

  float result = yaw1 + diff * t;

  // 保持在[-180, 180]范围内
  while (result > 180.0f)
    result -= 360.0f;
  while (result < -180.0f)
    result += 360.0f;

  return result;
}

void Core::update_traveled_path()
{
  // 记录当前位置到轨迹中，使用更小的距离阈值来获得更详细的轨迹
  if (traveled_path_.empty() ||
      glm::distance(traveled_path_.back(), model_translate) > 0.05f)
  {
    traveled_path_.push_back(model_translate);

    // 增加轨迹点数量限制，因为现在我们有更多的路径点
    if (traveled_path_.size() > 2000)
    {
      traveled_path_.erase(traveled_path_.begin());
    }

    // 更新路径VAO
    update_path_VAO();
  }
}

void Core::clear_traveled_path()
{
  traveled_path_.clear();
  update_path_VAO();
}

void Core::update_path_VAO()
{
  if (traveled_path_.size() < 2)
  {
    path_vertex_num_ = 0;
    return;
  }

  // 准备顶点数据：为每条线段创建两个顶点
  std::vector<float> vertices;
  for (size_t i = 0; i < traveled_path_.size() - 1; i++)
  {
    // 线段起点
    vertices.push_back(traveled_path_[i].x);
    vertices.push_back(traveled_path_[i].y + 0.01f); // 稍微抬高避免与地面重叠
    vertices.push_back(traveled_path_[i].z);

    // 线段终点
    vertices.push_back(traveled_path_[i + 1].x);
    vertices.push_back(traveled_path_[i + 1].y + 0.01f);
    vertices.push_back(traveled_path_[i + 1].z);
  }

  path_vertex_num_ = vertices.size() / 3;

  // 更新VAO
  glBindVertexArray(path_VAO_);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &VBO);
}

void Core::render_path()
{
  if (!show_path_ || path_vertex_num_ == 0)
  {
    return;
  }

  glUseProgram(shader_program_);
  glBindVertexArray(path_VAO_);

  GLuint path_color = glGetUniformLocation(shader_program_, "ObjectColor");
  glUniform3f(path_color, 1.0f, 0.3f, 0.0f); // 橙色中心线（更明显）

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 view;
  if (follow_model_)
  {
    view = glm::lookAt(
        camera_position_,
        model_translate,
        glm::vec3(0.0f, 1.0f, 0.0f));
  }
  else
  {
    view = glm::lookAt(
        camera_position_,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::mat4 projection = glm::perspective(glm::radians(55.0f), 1280.0f / 800.0f, 0.1f, 100.0f);

  GLuint transformLocModel = glGetUniformLocation(shader_program_, "model");
  glUniformMatrix4fv(transformLocModel, 1, GL_FALSE, glm::value_ptr(model));

  GLuint transformLocView = glGetUniformLocation(shader_program_, "view");
  glUniformMatrix4fv(transformLocView, 1, GL_FALSE, glm::value_ptr(view));

  GLuint transformLocProjection = glGetUniformLocation(shader_program_, "projection");
  glUniformMatrix4fv(transformLocProjection, 1, GL_FALSE, glm::value_ptr(projection));

  // 设置线宽（增加中心线粗细）
  glLineWidth(4.0f);
  glDrawArrays(GL_LINES, 0, path_vertex_num_);
  glLineWidth(1.0f);

  glBindVertexArray(0);
}

void Core::calculate_path_orientations()
{
  if (predefined_path_.size() < 2)
    return;

  // 为每个路径点计算正确的朝向，使用简单而稳定的算法
  for (size_t i = 0; i < predefined_path_.size(); i++)
  {
    glm::vec3 direction;

    if (i == 0)
    {
      // 第一个点：朝向下一个点
      direction = predefined_path_[i + 1].position - predefined_path_[i].position;
    }
    else if (i == predefined_path_.size() - 1)
    {
      // 最后一个点：朝向第一个点（闭合路径）
      direction = predefined_path_[0].position - predefined_path_[i].position;
    }
    else
    {
      // 中间点：朝向下一个点
      direction = predefined_path_[i + 1].position - predefined_path_[i].position;
    }

    // 计算yaw角度
    if (glm::length(direction) > 0.001f)
    {
      direction = glm::normalize(direction);
      float yaw_radians = atan2(direction.x, direction.z);
      float yaw_degrees = glm::degrees(yaw_radians);
      predefined_path_[i].yaw = yaw_degrees;
    }
    else
    {
      // 如果方向向量长度太小，保持前一个点的朝向
      if (i > 0)
      {
        predefined_path_[i].yaw = predefined_path_[i - 1].yaw;
      }
      else
      {
        predefined_path_[i].yaw = 0.0f;
      }
    }
  }

  // 简单的角度平滑处理
  for (size_t i = 1; i < predefined_path_.size(); i++)
  {
    float prev_yaw = predefined_path_[i - 1].yaw;
    float curr_yaw = predefined_path_[i].yaw;

    // 处理角度跳跃（例如从179度到-179度）
    float diff = curr_yaw - prev_yaw;

    if (diff > 180.0f)
    {
      predefined_path_[i].yaw -= 360.0f;
    }
    else if (diff < -180.0f)
    {
      predefined_path_[i].yaw += 360.0f;
    }
  }
}

void Core::init_track_VAOs()
{
  // 初始化左侧边界VAO
  glGenVertexArrays(1, &left_track_VAO_);
  glBindVertexArray(left_track_VAO_);

  GLuint VBO_left;
  glGenBuffers(1, &VBO_left);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_left);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &VBO_left);

  // 初始化右侧边界VAO
  glGenVertexArrays(1, &right_track_VAO_);
  glBindVertexArray(right_track_VAO_);

  GLuint VBO_right;
  glGenBuffers(1, &VBO_right);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_right);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &VBO_right);

  left_track_vertex_num_ = 0;
  right_track_vertex_num_ = 0;
}

void Core::generate_track_boundaries()
{
  if (predefined_path_.size() < 2)
    return;

  left_track_points_.clear();
  right_track_points_.clear();

  for (size_t i = 0; i < predefined_path_.size(); i++)
  {
    glm::vec3 position = predefined_path_[i].position;

    // 计算垂直于路径的方向向量
    glm::vec3 forward_dir;
    if (i < predefined_path_.size() - 1)
    {
      forward_dir = predefined_path_[i + 1].position - position;
    }
    else
    {
      forward_dir = position - predefined_path_[i - 1].position;
    }

    if (glm::length(forward_dir) > 0.001f)
    {
      forward_dir = glm::normalize(forward_dir);

      // 计算右侧方向（垂直于前进方向）
      glm::vec3 right_dir = glm::cross(forward_dir, glm::vec3(0.0f, 1.0f, 0.0f));
      right_dir = glm::normalize(right_dir);

      // 生成左右边界点
      glm::vec3 left_point = position - right_dir * track_lane_width_;
      glm::vec3 right_point = position + right_dir * track_lane_width_;

      left_point.y = 0.02f; // 稍微抬高避免与地面重叠
      right_point.y = 0.02f;

      left_track_points_.push_back(left_point);
      right_track_points_.push_back(right_point);
    }
  }

  // 更新VAO
  update_track_VAOs();
}

void Core::update_track_VAOs()
{
  // 更新左侧边界VAO
  if (left_track_points_.size() >= 2)
  {
    std::vector<float> left_vertices;
    for (size_t i = 0; i < left_track_points_.size() - 1; i++)
    {
      // 线段起点
      left_vertices.push_back(left_track_points_[i].x);
      left_vertices.push_back(left_track_points_[i].y);
      left_vertices.push_back(left_track_points_[i].z);

      // 线段终点
      left_vertices.push_back(left_track_points_[i + 1].x);
      left_vertices.push_back(left_track_points_[i + 1].y);
      left_vertices.push_back(left_track_points_[i + 1].z);
    }

    left_track_vertex_num_ = left_vertices.size() / 3;

    glBindVertexArray(left_track_VAO_);
    GLuint VBO_left;
    glGenBuffers(1, &VBO_left);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_left);
    glBufferData(GL_ARRAY_BUFFER, left_vertices.size() * sizeof(float), left_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO_left);
  }

  // 更新右侧边界VAO
  if (right_track_points_.size() >= 2)
  {
    std::vector<float> right_vertices;
    for (size_t i = 0; i < right_track_points_.size() - 1; i++)
    {
      // 线段起点
      right_vertices.push_back(right_track_points_[i].x);
      right_vertices.push_back(right_track_points_[i].y);
      right_vertices.push_back(right_track_points_[i].z);

      // 线段终点
      right_vertices.push_back(right_track_points_[i + 1].x);
      right_vertices.push_back(right_track_points_[i + 1].y);
      right_vertices.push_back(right_track_points_[i + 1].z);
    }

    right_track_vertex_num_ = right_vertices.size() / 3;

    glBindVertexArray(right_track_VAO_);
    GLuint VBO_right;
    glGenBuffers(1, &VBO_right);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_right);
    glBufferData(GL_ARRAY_BUFFER, right_vertices.size() * sizeof(float), right_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO_right);
  }
}

void Core::render_track_boundaries()
{
  if (!show_track_boundaries_)
    return;

  glUseProgram(shader_program_);

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view;
  if (follow_model_)
  {
    view = glm::lookAt(
        camera_position_,
        model_translate,
        glm::vec3(0.0f, 1.0f, 0.0f));
  }
  else
  {
    view = glm::lookAt(
        camera_position_,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
  }

  glm::mat4 projection = glm::perspective(glm::radians(55.0f), 1280.0f / 800.0f, 0.1f, 100.0f);

  GLuint transformLocModel = glGetUniformLocation(shader_program_, "model");
  glUniformMatrix4fv(transformLocModel, 1, GL_FALSE, glm::value_ptr(model));

  GLuint transformLocView = glGetUniformLocation(shader_program_, "view");
  glUniformMatrix4fv(transformLocView, 1, GL_FALSE, glm::value_ptr(view));

  GLuint transformLocProjection = glGetUniformLocation(shader_program_, "projection");
  glUniformMatrix4fv(transformLocProjection, 1, GL_FALSE, glm::value_ptr(projection));

  // 绘制左侧边界（红色）
  if (left_track_vertex_num_ > 0)
  {
    glBindVertexArray(left_track_VAO_);
    GLuint color_loc = glGetUniformLocation(shader_program_, "ObjectColor");
    glUniform3f(color_loc, 1.0f, 0.0f, 0.0f); // 红色（更鲜明）

    glLineWidth(5.0f);
    glDrawArrays(GL_LINES, 0, left_track_vertex_num_);
    glBindVertexArray(0);
  }

  // 绘制右侧边界（青色）
  if (right_track_vertex_num_ > 0)
  {
    glBindVertexArray(right_track_VAO_);
    GLuint color_loc = glGetUniformLocation(shader_program_, "ObjectColor");
    glUniform3f(color_loc, 0.0f, 1.0f, 1.0f); // 青色（对比度更强）

    glLineWidth(5.0f);
    glDrawArrays(GL_LINES, 0, right_track_vertex_num_);
    glBindVertexArray(0);
  }

  glLineWidth(1.0f);
}
