#include <vks/CommandBuffers.hpp>
#include <vks/CommandPool.hpp>
#include <vks/Device.hpp>
#include <vks/GraphicsPipeline.hpp>
#include <vks/RenderPass.hpp>
#include <vks/SwapChain.hpp>

#include <stdexcept>

using namespace vks;

CommandBuffers::CommandBuffers(const Device& device,
                               const RenderPass& renderPass,
                               const SwapChain& swapChain,
                               const GraphicsPipeline& graphicsPipeline,
                               const CommandPool& commandPool)
    : m_device(device),
      m_renderPass(renderPass),
      m_swapChain(swapChain),
      m_graphicsPipeline(graphicsPipeline),
      m_commandPool(commandPool) {}

CommandBuffers::~CommandBuffers() { destroyCommandBuffers(); }

void CommandBuffers::destroyCommandBuffers() {
  vkFreeCommandBuffers(m_device.logical(), m_commandPool.handle(),
                       static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
}

void CommandBuffers::SingleTimeCommands(const Device& device,
                                        const CommandPool& cmdPool,
                                        const std::function<void(const VkCommandBuffer&)>& func) {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = cmdPool.handle();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer = {};
  if (vkAllocateCommandBuffers(device.logical(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Could not create one-time command buffer!");
  }

  func(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(device.graphicsQueue());

  vkFreeCommandBuffers(device.logical(), cmdPool.handle(), 1, &commandBuffer);
}