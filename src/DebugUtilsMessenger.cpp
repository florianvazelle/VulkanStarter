#include <vks/DebugUtilsMessenger.hpp>
#include <vks/Instance.hpp>

#include <iostream>
#include <stdexcept>

using namespace vks;

DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance)
    : m_debugMessenger(VK_NULL_HANDLE), m_instance(instance) {
  if (instance.validationLayersEnabled()) {
    // Setup the debug messenger
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    PopulateDebugMessengerCreateInfo(createInfo);

    // Get pointer to extension function
    auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance.handle(), "vkCreateDebugUtilsMessengerEXT");

    if (createFunc == nullptr) {
      throw std::runtime_error("Debug Utils Messenger extension not available");
    }

    // Create the messenger
    VkResult result = createFunc(instance.handle(), &createInfo, nullptr, &m_debugMessenger);

    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to setup Debug Utils Messenger");
    }
  }
}

DebugUtilsMessenger::~DebugUtilsMessenger() noexcept(false) {
  if (m_instance.validationLayersEnabled()) {
    auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_instance.handle(), "vkDestroyDebugUtilsMessengerEXT");

    if (destroyFunc == nullptr) {
      throw std::runtime_error("Failed to destroy Debug Utils Messenger");
    }

    destroyFunc(m_instance.handle(), m_debugMessenger, nullptr);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugUtilsMessenger::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                   void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

void DebugUtilsMessenger::PopulateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
}