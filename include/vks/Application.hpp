#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vks/Basic/BasicCommandBuffers.hpp>
#include <vks/Basic/BasicRenderPass.hpp>
#include <vks/DebugUtilsMessenger.hpp>
#include <vks/Device.hpp>
#include <vks/GraphicsPipeline.hpp>
#include <vks/ImGui/ImGuiApp.hpp>
#include <vks/Instance.hpp>
#include <vks/SwapChain.hpp>
#include <vks/SyncObjects.hpp>
#include <vks/Window.hpp>

namespace vks {

  class Application {
  public:
    Application();

    void run() { mainLoop(); }

  private:
    Instance instance;
    DebugUtilsMessenger debugMessenger;
    Window window;
    Device device;
    SwapChain swapChain;
    BasicRenderPass renderPass;
    GraphicsPipeline graphicsPipeline;
    CommandPool commandPool;
    BasicCommandBuffers commandBuffers;
    SyncObjects syncObjects;

    ImGuiApp interface;

    size_t currentFrame = 0;

    void mainLoop();

    void drawFrame(bool& framebufferResized);
    void drawImGui();

    void recreateSwapChain(bool& framebufferResized);
  };

}  // namespace vks