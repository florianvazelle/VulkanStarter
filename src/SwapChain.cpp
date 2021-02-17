#include <vks/SwapChain.hpp>

#include <vks/Device.hpp>
#include <vks/Window.hpp>

#include <iostream>

using namespace vks;

SwapChain::SwapChain(const Device& device, const Window& window)
    : m_swapChain(VK_NULL_HANDLE),
      m_oldSwapChain(VK_NULL_HANDLE),
      m_extent(),
      m_imageFormat(),
      m_device(device),
      m_window(window) {
  createSwapChain();
  createImageViews();
}

void SwapChain::recreate() {
  destroyImageViews();
  m_oldSwapChain = m_swapChain;
  createSwapChain();
  createImageViews();
}

void SwapChain::cleanupOld() {
  // Destroy old swap chain if it exists
  if (m_oldSwapChain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(m_device.logical(), m_oldSwapChain, nullptr);
    m_oldSwapChain = VK_NULL_HANDLE;
  }
}

void SwapChain::createSwapChain() {
  m_supportDetails = QuerySwapChainSupport(m_device.physical(), m_window.surface());
  m_extent = ChooseSwapExtent(m_supportDetails.capabilities, m_window);

  VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(m_supportDetails.formats);
  VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_supportDetails.presentModes);

  m_imageFormat = surfaceFormat.format;

  // How many images should be in the swap chain
  // One more than the minimum helps with wait times before another image is
  // available from driver
  uint32_t imageCount = m_supportDetails.capabilities.minImageCount + 1;

  // Make sure image count doesn't exceed maximum
  // A max image count of 0 indicates that there is no maximum
  if (m_supportDetails.capabilities.maxImageCount > 0
      && imageCount > m_supportDetails.capabilities.maxImageCount) {
    imageCount = m_supportDetails.capabilities.maxImageCount;
  }

  // Setup the swapchain
  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_window.surface();

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = m_extent;
  // Amount of layers in each image
  // This is always 1 unless doing something like stereoscopic 3D
  createInfo.imageArrayLayers = 1;
  // Image is being directly rendered to, so make i a color attachment
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // How to handle swap chain images across multiple queue families
  const QueueFamilyIndices& indices = m_device.queueFamilyIndices();
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  // Determine if there are multiple queue families
  if (indices.graphicsFamily != indices.presentFamily) {
    // Use concurrent mode if there are. Worse performance but no ownership transfers
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    // Use exclusive mode if single queue family. Best performance
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  // Don't do any extra transformations to images
  createInfo.preTransform = m_supportDetails.capabilities.currentTransform;
  // Blend with other windows in window system
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  // Clip obscured pixels
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = m_oldSwapChain;

  if (vkCreateSwapchainKHR(m_device.logical(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
    throw std::runtime_error("Swap chain creation failed");
  }

  // Get new swap chain images
  vkGetSwapchainImagesKHR(m_device.logical(), m_swapChain, &imageCount, nullptr);
  m_images.resize(imageCount);
  vkGetSwapchainImagesKHR(m_device.logical(), m_swapChain, &imageCount, m_images.data());
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  // Try to find support for 8-bit SRGB instead
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
        && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  // Default to first available format
  return availableFormats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
  // Try to find support for Mailbox
  // MAILBOX (triple buffering) uses a queue to present images,
  // and if the queue is full already queued images are overwritten with newer images
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  // Otherwise default to VK_PRESENT_MODE_FIFO_KHR
  // FIFO is guaranteed to be present and is essentially traditional V-Sync
  return VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::createImageViews() {
  m_imageViews.resize(m_images.size());

  for (size_t i = 0; i < m_images.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_images[i];

    // How image should be interpreted
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_imageFormat;

    // Leave swizzling as default
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Purpose of image and which parts should be accessed
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device.logical(), &createInfo, nullptr, &m_imageViews[i])
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

void SwapChain::destroyImageViews() {
  for (VkImageView& view : m_imageViews) {
    vkDestroyImageView(m_device.logical(), view, nullptr);
  }
}

SwapChain::~SwapChain() {
  destroyImageViews();
  vkDestroySwapchainKHR(m_device.logical(), m_swapChain, nullptr);
}

SwapChainSupportDetails SwapChain::QuerySwapChainSupport(const VkPhysicalDevice& device,
                                                         const VkSurfaceKHR& surface) {
  SwapChainSupportDetails details;

  // Get capabilities of both device and surface
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  // Get number of supported surface formats
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  // Get supported surface formats
  if (formatCount > 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  // Get number of supported presentation modes
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount > 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                       const Window& window) {
  // Vulkan uses uint32 max value to signify window resolution should be used
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }
  // Otherwise, the window manager allows a custom resolution
  else {
    VkExtent2D actualExtent = {static_cast<uint32_t>(window.dimensions().x),
                               static_cast<uint32_t>(window.dimensions().y)};

    // Determine if resolution given by vulkan or custom window resolution is the better fit
    actualExtent.width = std::max(capabilities.minImageExtent.width,
                                  std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height
        = std::max(capabilities.minImageExtent.height,
                   std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}