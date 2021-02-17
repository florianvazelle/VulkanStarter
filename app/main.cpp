#include <vks/Application.hpp>

int main() {
  // need to init glfw first, to get the suitable glfw extension for the vkinstance
  glfwInit();

  // Disable OpenGL
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  vks::Application app;

  try {
    app.run();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}