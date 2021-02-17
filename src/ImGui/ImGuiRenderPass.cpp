
#include <iostream>
#include <vks/Device.hpp>
#include <vks/ImGui/ImGuiRenderPass.hpp>
#include <vks/SwapChain.hpp>

using namespace vks;

ImGuiRenderPass::ImGuiRenderPass(const Device& device, const SwapChain& swapChain)
    : RenderPass(device, swapChain) {
  createRenderPass();
  createFrameBuffers();
}

void ImGuiRenderPass::createRenderPass() {
  // Create an attachment description for the render pass
  VkAttachmentDescription attachmentDescription = {};
  attachmentDescription.format = m_swapChain.imageFormat();
  attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  attachmentDescription.loadOp
      = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Need UI to be drawn on top of main
  attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachmentDescription.finalLayout
      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Last pass so we want to present after
  attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  // Create a color attachment reference
  VkAttachmentReference attachmentReference = {};
  attachmentReference.attachment = 0;
  attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Create a subpass
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &attachmentReference;

  // Create a subpass dependency to synchronize our main and UI render passes
  // We want to render the UI after the geometry has been written to the framebuffer
  // so we need to configure a subpass dependency as such
  VkSubpassDependency subpassDependency = {};
  subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;  // Create external dependency
  subpassDependency.dstSubpass = 0;                    // The geometry subpass comes first
  subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // Wait on writes
  subpassDependency.dstStageMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // Finally create the UI render pass
  VkRenderPassCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.attachmentCount = 1;
  createInfo.pAttachments = &attachmentDescription;
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpass;
  createInfo.dependencyCount = 1;
  createInfo.pDependencies = &subpassDependency;

  if (vkCreateRenderPass(m_device.logical(), &createInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
    throw std::runtime_error("Render pass creation failed");
  }
}
