#ifndef DEBUGUTILSMESSENGER_HPP
#define DEBUGUTILSMESSENGER_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>

namespace vks {

  class Instance;

  class DebugUtilsMessenger : public NonCopyable {
  public:
    explicit DebugUtilsMessenger(const Instance& instance);
    ~DebugUtilsMessenger() noexcept(false);

    inline const VkDebugUtilsMessengerEXT& handle() const { return m_debugMessenger; }

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  private:
    const Instance& m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
  };

}  // namespace vks

#endif  // DEBUGUTILSMESSENGER_HPP