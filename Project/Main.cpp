#include <iostream>
#include "scene_lua.hpp"

int main(int argc, char** argv)
{
  std::string filename = "project.lua";
  int t = 1;
  if (argc != 2) {
    std::cerr << "Usage: Project [number of images]" << std::endl;
    return 2;
  } else {
    t = std::stoi(argv[1]);
    if (t < 1) {
      std::cerr << "Usage: Project [number of images > 0]" << std::endl;
      return 3;
    }
  }

	// Before beginning, remove previously created project_* files
	// This is important to do so that later when we create .gif
	// It's not somehow corrupted
	system("rm project_* 2> /dev/null");
  system("rm project.gif 2> /dev/null");

  if (!run_lua(filename, t)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }

  // Now make a gif for my animation if there are more than one images generated
  if (t >= 2) {
    system("convert -delay 20 -loop 0 project_* project.gif");
  }
}
