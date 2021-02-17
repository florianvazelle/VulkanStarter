#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vector>

namespace vks {
  class Device;
  class SwapChain;

  class RenderPass : public NonCopyable {
  public:
    RenderPass(const Device& device, const SwapChain& swapChain);
    ~RenderPass();

    inline const VkRenderPass& handle() const { return m_renderPass; }
    inline const VkFramebuffer& frameBuffer(uint32_t index) const { return m_frameBuffers[index]; }
    inline size_t size() const { return m_frameBuffers.size(); }

    void recreate();
    void cleanupOld();

  protected:
    VkRenderPass m_renderPass;
    VkRenderPass m_oldRenderPass;

    std::vector<VkFramebuffer> m_frameBuffers;

    const Device& m_device;
    const SwapChain& m_swapChain;

    virtual void createRenderPass() = 0;
    void createFrameBuffers();

    void destroyFrameBuffers();
  };
}  // namespace vks

#endif  // RENDERPASS_HPP