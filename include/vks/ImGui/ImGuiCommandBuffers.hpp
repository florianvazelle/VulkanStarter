#ifndef IMGUICOMMANDBUFFERS_HPP
#define IMGUICOMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <vks/CommandBuffers.hpp>

namespace vks {

  class ImGuiCommandBuffers : public CommandBuffers {
  public:
    ImGuiCommandBuffers(const Device& device,
                        const RenderPass& renderpass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool);

    void recordCommandBuffers(uint32_t bufferIdx);
    void recreate();

  private:
    void createCommandBuffers();
  };

}  // namespace vks

#endif  // IMGUICOMMANDBUFFERS_HPP