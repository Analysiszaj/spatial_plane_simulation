#ifndef __APP_H
#define __APP_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <functional>

#include "core.h"

class App
{
private:
  float main_scale_ = 1.0f;
  bool show_demo_window_ = true;
  bool show_another_window_ = false;
  ImVec4 clear_color_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

  GLFWwindow *window_;
  Core *core_;

public:
  App(const char *title, int width, int height);
  ~App() = default;

  void app_run();
  void app_exit();

private:
  void init_imgui();
  void render_tool_gui();
  void render_gl_program();
};

#endif