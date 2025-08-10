#include "app.h"

static void glfw_error_callback(int error, const char *descroption)
{
  std::cout << "GLFW Error " << error << ":" << descroption << "\n"
            << std::endl;
}

App::App(const char *title, int width, int height)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
  {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  main_scale_ = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
  window_ = glfwCreateWindow((int)(width * main_scale_), (int)(height * main_scale_), title, nullptr, nullptr);

  if (!window_)
  {
    std::cout << "Fail create GLFW Window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Fail initalize GLAD" << std::endl;
    exit(EXIT_FAILURE);
  }

  init_imgui();
  core_ = new Core();
  core_->init_core();
}

void App::init_imgui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  ImGui::StyleColorsDark();

  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale_);
  style.FontScaleDpi = 0.6f;

  style.Alpha = 0.95f;

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  style.FontSizeBase = 16.0f;
  // io.Fonts->AddFontDefault();
  io.Fonts->AddFontFromFileTTF("/Users/mds/my/spatial_plane_simulation/assets/AlimamaFangYuanTiVF-Thin-2.ttf");
}

void App::render_tool_gui()
{
  core_->render_tool_panel();
}

void App::render_gl_program()
{
  core_->render_grid();
  core_->render_track_boundaries(); // 先渲染赛道边界
  core_->render_path();             // 然后渲染中心线
  core_->render_cube();             // 最后渲染车子
}

void App::app_run()
{

  while (!glfwWindowShouldClose(window_))
  {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window_, GLFW_ICONIFIED) != 0)
    {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    render_tool_gui();
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color_.x * clear_color_.w, clear_color_.y * clear_color_.w, clear_color_.z * clear_color_.w, clear_color_.w);
    glClear(GL_COLOR_BUFFER_BIT);
    render_gl_program();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
  }
}

void App::app_exit()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  delete core_;
  glfwDestroyWindow(window_);
  glfwTerminate();

  exit(EXIT_SUCCESS);
}
