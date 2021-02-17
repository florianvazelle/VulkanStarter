#ifndef SYNCOBJECTS_HPP
#define SYNCOBJECTS_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vector>

namespace vks {
  class Device;

  class SyncObjects : public NonCopyable {
  public:
    SyncObjects(const Device& device, uint32_t numImages, uint32_t maxFramesInFlight);
    ~SyncObjects();

    inline VkSemaphore& imageAvailable(uint32_t index) { return m_imageAvailable[index]; }
    inline VkSemaphore& renderFinished(uint32_t index) { return m_renderFinished[index]; }
    inline VkFence& inFlightFence(uint32_t index) { return m_inFlightFences[index]; }
    inline VkFence& imageInFlight(uint32_t index) { return m_imagesInFlight[index]; }

  private:
    const Device& m_device;

    uint32_t m_numImages, m_maxFramesInFlight;

    std::vector<VkSemaphore> m_imageAvailable;
    std::vector<VkSemaphore> m_renderFinished;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
  };

}  // namespace vks

#endif  // SYNCOBJECTS_HPP