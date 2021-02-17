#ifndef IMGUIRENDERPASS_HPP
#define IMGUIRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vks/RenderPass.hpp>

namespace vks {
  class ImGuiRenderPass : public RenderPass {
  public:
    ImGuiRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass();
  };
}  // namespace vks

#endif  // IMGUIRENDERPASS_HPP