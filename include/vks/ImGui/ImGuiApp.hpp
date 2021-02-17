
#ifndef IMGUIAPP_HPP
#define IMGUIAPP_HPP

#include <vulkan/vulkan.h>

#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vks/CommandPool.hpp>
#include <vks/Device.hpp>
#include <vks/ImGui/ImGuiCommandBuffers.hpp>
#include <vks/ImGui/ImGuiRenderPass.hpp>
#include <vks/Instance.hpp>
#include <vks/SwapChain.hpp>
#include <vks/Window.hpp>

namespace vks {

  class ImGuiApp {
  public:
    ImGuiApp(const Instance& instance,
             Window& window,
             const Device& device,
             const SwapChain& swapChain,
             const GraphicsPipeline& graphicsPipeline);
    ~ImGuiApp();

    inline VkCommandBuffer& command(uint32_t index) { return commandBuffers.command(index); }
    inline const VkCommandBuffer& command(uint32_t index) const {
      return commandBuffers.command(index);
    }

    void recordCommandBuffers(uint32_t bufferIdx) {
      commandBuffers.recordCommandBuffers(bufferIdx);
    }

    void recreate();

  private:
    VkDescriptorPool imGuiDescriptorPool;

    ImGuiRenderPass renderPass;
    CommandPool commandPool;
    ImGuiCommandBuffers commandBuffers;

    const Instance& m_instance;
    const Device& m_device;
    const SwapChain& m_swapChain;
    const GraphicsPipeline& m_graphicsPipeline;

    void createImGuiDescriptorPool();
  };
}  // namespace vks

#endif  // IMGUIAPP_HPP