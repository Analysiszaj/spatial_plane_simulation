#include "app.h"
int main()
{
  App app("spatial_plane_simulation", 1280, 800);
  app.app_run();
  app.app_exit();
  return 0;
}