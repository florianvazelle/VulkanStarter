#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <vulkan/vulkan.h>
#include <vector>

#include <NonCopyable.hpp>
#include <vks/QueueFamily.hpp>

namespace vks {

  class Instance;
  class Window;

  class Device : public NonCopyable {
  public:
    Device(const Instance& instance,
           const Window& window,
           const std::vector<const char*>& extensions);
    ~Device();

    inline const VkPhysicalDevice& physical() const { return m_physical; }
    inline const VkDevice& logical() const { return m_logical; }
    inline const QueueFamilyIndices& queueFamilyIndices() const { return m_indices; }
    inline const VkQueue& graphicsQueue() const { return m_graphicsQueue; }
    inline const VkQueue& presentQueue() const { return m_presentQueue; }

  private:
    VkPhysicalDevice m_physical;
    VkDevice m_logical;

    const Instance& m_instance;
    const Window& m_window;

    QueueFamilyIndices m_indices;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device,
                                            const std::vector<const char*>& extensions);

    static VkPhysicalDevice PickPhysicalDevice(const VkInstance& instance,
                                               const VkSurfaceKHR& surface,
                                               const std::vector<const char*>& requiredExtensions);

    static bool IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
  };
}  // namespace vks

#endif  // DEVICE_HPP