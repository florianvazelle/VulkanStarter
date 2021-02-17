#include <vks/Device.hpp>
#include <vks/SyncObjects.hpp>

#include <stdexcept>

using namespace vks;

SyncObjects::SyncObjects(const Device& device, uint32_t numImages, uint32_t maxFramesInFlight)
    : m_device(device),
      m_numImages(numImages),
      m_maxFramesInFlight(maxFramesInFlight),
      m_imageAvailable(maxFramesInFlight),
      m_renderFinished(maxFramesInFlight),
      m_inFlightFences(maxFramesInFlight) {
  m_imagesInFlight.resize(m_numImages);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  // Initialize fence to already signaled so it doesn't
  // hang on first frame
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < m_maxFramesInFlight; ++i) {
    if (vkCreateSemaphore(m_device.logical(), &semaphoreInfo, nullptr, &m_imageAvailable[i])
            != VK_SUCCESS
        || vkCreateSemaphore(m_device.logical(), &semaphoreInfo, nullptr, &m_renderFinished[i])
               != VK_SUCCESS
        || vkCreateFence(m_device.logical(), &fenceInfo, nullptr, &m_inFlightFences[i])
               != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

SyncObjects::~SyncObjects() {
  for (size_t i = 0; i < m_maxFramesInFlight; ++i) {
    vkDestroySemaphore(m_device.logical(), m_renderFinished[i], nullptr);
    vkDestroySemaphore(m_device.logical(), m_imageAvailable[i], nullptr);
    vkDestroyFence(m_device.logical(), m_inFlightFences[i], nullptr);
  }
}