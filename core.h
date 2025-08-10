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
  GLuint path_VAO_ = 0;
  unsigned int path_vertex_num_ = 0;
  GLuint left_track_VAO_ = 0; // 左侧赛道边界
  unsigned int left_track_vertex_num_ = 0;
  GLuint right_track_VAO_ = 0; // 右侧赛道边界
  unsigned int right_track_vertex_num_ = 0;

  GLuint shader_program_ = 0;

  glm::vec3 camera_position_ = glm::vec3(0.0f, 1.0f, -6.0f);
  glm::vec3 model_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 model_translate = glm::vec3(0.0f, 0.0f, 0.0f);

  // 摄像机跟随相关
  bool follow_model_ = true;     // 是否启用模型跟随
  float camera_distance_ = 5.0f; // 摄像机距离模型的距离
  float camera_height_ = 2.0f;   // 摄像机相对模型的高度
  float yaw_angle_ = 0.0f;       // 偏航角（左右转动）

  // 路径播放相关
  struct PathPoint
  {
    glm::vec3 position;
    float yaw;
    float timestamp; // 时间戳（秒）
  };
  std::vector<PathPoint> predefined_path_; // 预定义路径
  bool is_playing_ = false;                // 是否正在播放
  float play_start_time_ = 0.0f;           // 播放开始时间
  float play_speed_ = 1.0f;                // 播放速度倍率
  int current_path_index_ = 0;             // 当前路径点索引
  bool loop_play_ = true;                  // 是否循环播放

  // 路径轨迹绘制相关
  std::vector<glm::vec3> traveled_path_;      // 车子走过的轨迹
  std::vector<glm::vec3> left_track_points_;  // 左侧赛道边界点
  std::vector<glm::vec3> right_track_points_; // 右侧赛道边界点
  bool show_path_ = true;                     // 是否显示路径
  bool show_track_boundaries_ = true;         // 是否显示赛道边界
  float track_lane_width_ = 1.5f;             // 赛道车道宽度

public:
  Core();
  ~Core();

  std::pair<std::string, std::string> read_shader_file(const char *vertex_path, const char *fragment_path);
  void init_program();
  void init_core();

  unsigned int build_grid_vertices(std::vector<float> &vertices, int grid_num);
  void init_grid_VAO();
  void init_cube_VAO();
  void init_path_VAO();   // 初始化路径VAO
  void init_track_VAOs(); // 初始化赛道边界VAO

  void render_cube();
  void render_grid();
  void render_path();             // 渲染路径
  void render_track_boundaries(); // 渲染赛道边界
  void render_tool_panel();

  void update_camera_follow(); // 更新摄像机跟随

  // 路径播放相关方法
  void init_predefined_path();                                                       // 初始化预定义路径
  void update_path_playback();                                                       // 更新路径播放
  void start_path_playback();                                                        // 开始播放
  void stop_path_playback();                                                         // 停止播放
  void reset_path_playback();                                                        // 重置播放
  glm::vec3 interpolate_position(const PathPoint &p1, const PathPoint &p2, float t); // 位置插值
  float interpolate_yaw(float yaw1, float yaw2, float t);                            // 角度插值

  // 路径轨迹相关方法
  void update_traveled_path();        // 更新走过的轨迹
  void clear_traveled_path();         // 清空轨迹
  void update_path_VAO();             // 更新路径VAO
  void calculate_path_orientations(); // 计算路径朝向
  void generate_track_boundaries();   // 生成赛道边界
  void update_track_VAOs();           // 更新赛道边界VAO
};

#endif