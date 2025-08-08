#ifndef __APP_H
#define __APP_H
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <functional>

class App
{
private:
  float main_scale_ = 1.0f;
  bool show_demo_window_ = true;
  bool show_another_window_ = false;
  ImVec4 clear_color_ = ImVec4(0.4f, 0.55f, 0.60f, 0.0f);

  GLFWwindow *window_;

  std::function<void()> gl_render_callback;

public:
  App(const char *title, int width, int height);
  void init_imgui();
  void app_run();
  void app_exit();

  template <typename F>
  void set_gl_render(F &&callback)
  {
    gl_render_callback = std::forward<F>(callback);
  }
};

#endif