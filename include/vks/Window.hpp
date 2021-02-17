#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <NonCopyable.hpp>

namespace vks {

  class Instance;

  class Window : public NonCopyable {
  public:
    // using DrawFrameFunc = void(*)(bool& framebufferResized);

    Window(const glm::ivec2& dimensions, const std::string& title, const vks::Instance& instance);
    Window() = delete;
    ~Window();
    void mainLoop();

    inline const glm::ivec2& dimensions() const { return m_dimensions; }

    inline const GLFWwindow* window() const { return m_window; }
    inline GLFWwindow* window() { return m_window; }

    inline const VkSurfaceKHR& surface() const { return m_surface; }

    inline void framebufferSize(glm::ivec2& size) const {
      glfwGetFramebufferSize(m_window, &size[0], &size[1]);
    }

    inline void setDrawFrameFunc(const std::function<void(bool&)>& func) { m_drawFrameFunc = func; }

    static void GetRequiredExtensions(std::vector<const char*>& out);

  private:
    GLFWwindow* m_window;

    glm::ivec2 m_dimensions;
    std::string m_title;

    const vks::Instance& m_instance;
    VkSurfaceKHR m_surface;

    bool m_framebufferResized;
    std::function<void(bool&)> m_drawFrameFunc;

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  };

}  // namespace vks

#endif  // WINDOW_HPP