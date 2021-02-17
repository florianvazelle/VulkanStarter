#include <vks/Device.hpp>

#include <vks/Instance.hpp>
#include <vks/Window.hpp>

#include <map>
#include <set>
#include <vector>

#include <vks/QueueFamily.hpp>
#include <vks/SwapChain.hpp>

using namespace vks;

Device::Device(const Instance& instance,
               const Window& window,
               const std::vector<const char*>& extensions)
    : m_physical(VK_NULL_HANDLE),
      m_logical(VK_NULL_HANDLE),
      m_window(window),
      m_instance(instance),
      m_graphicsQueue(VK_NULL_HANDLE),
      m_presentQueue(VK_NULL_HANDLE) {
  m_physical = PickPhysicalDevice(m_instance.handle(), m_window.surface(), extensions);
  m_indices = QueueFamily::FindQueueFamilies(m_physical, m_window.surface());

  // Setup queue families for device
  std::set<uint32_t> uniqueQueueFamilies
      = {m_indices.graphicsFamily.value(), m_indices.presentFamily.value()};
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  float priority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.queueFamilyIndex = queueFamily;
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = &priority;
    queueCreateInfos.push_back(createInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};

  // Setup logical device
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (m_instance.validationLayersEnabled()) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(Instance::ValidationLayers.size());
    createInfo.ppEnabledLayerNames = Instance::ValidationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_physical, &createInfo, nullptr, &m_logical) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  // Get handles for graphics and presentation queues
  vkGetDeviceQueue(m_logical, m_indices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logical, m_indices.presentFamily.value(), 0, &m_presentQueue);
}

Device::~Device() { vkDestroyDevice(m_logical, nullptr); }

bool Device::CheckDeviceExtensionSupport(const VkPhysicalDevice& device,
                                         const std::vector<const char*>& extensions) {
  // Get number of extension supported
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  if (extensionCount < 1) return false;

  // Get supported extensions
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  // Iterate through available extensions and make sure that all are present
  std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

  for (const auto& extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

VkPhysicalDevice Device::PickPhysicalDevice(const VkInstance& instance,
                                            const VkSurfaceKHR& surface,
                                            const std::vector<const char*>& requiredExtensions) {
  // Check for devices with vulkan support
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  // Get available devices
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  for (const auto& device : devices) {
    if (IsDeviceSuitable(device, surface)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  return physicalDevice;
}

bool Device::IsDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
  QueueFamilyIndices indices = QueueFamily::FindQueueFamilies(device, surface);

  bool extensionsSupported = CheckDeviceExtensionSupport(device, Instance::DeviceExtensions);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = SwapChain::QuerySwapChainSupport(device, surface);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}
