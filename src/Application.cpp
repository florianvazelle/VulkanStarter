#include <vks/Application.hpp>

using namespace vks;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

Application::Application()
    : instance("Hello Triangle", "No Engine", true),
      debugMessenger(instance),
      window({WIDTH, HEIGHT}, "Vulkan", instance),
      device(instance, window, Instance::DeviceExtensions),
      swapChain(device, window),
      renderPass(device, swapChain),
      graphicsPipeline(device, swapChain, renderPass),
      commandPool(device, 0),
      commandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool),
      syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT),
      interface(instance, window, device, swapChain, graphicsPipeline) {}

void Application::mainLoop() {
  window.setDrawFrameFunc([this](bool& framebufferResized) {
    drawImGui();
    drawFrame(framebufferResized);
  });

  window.mainLoop();
  vkDeviceWaitIdle(device.logical());
}

void Application::drawFrame(bool& framebufferResized) {
  vkWaitForFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame), VK_TRUE,
                  UINT64_MAX);

  // Get image from swap chain
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(device.logical(), swapChain.handle(), UINT64_MAX,
                                          syncObjects.imageAvailable(currentFrame), VK_NULL_HANDLE,
                                          &imageIndex);
  // Create new swap chain if needed
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain(framebufferResized);
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swapchain image");
  }

  if (syncObjects.imageInFlight(imageIndex) != VK_NULL_HANDLE) {
    vkWaitForFences(device.logical(), 1, &syncObjects.imageInFlight(imageIndex), VK_TRUE,
                    UINT64_MAX);
  }
  syncObjects.imageInFlight(imageIndex) = syncObjects.inFlightFence(currentFrame);

  // Record UI draw data
  interface.recordCommandBuffers(imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {syncObjects.imageAvailable(currentFrame)};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  VkCommandBuffer cmdBuffers[]
      = {commandBuffers.command(imageIndex), interface.command(imageIndex)};
  submitInfo.commandBufferCount = 2;
  submitInfo.pCommandBuffers = cmdBuffers;

  VkSemaphore signalSemaphores[] = {syncObjects.renderFinished(currentFrame)};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame));

  if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, syncObjects.inFlightFence(currentFrame))
      != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain.handle()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    recreateSwapChain(framebufferResized);
    framebufferResized = false;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::drawImGui() {
  // Start the Dear ImGui frame
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static float f = 0.0f;
  static int counter = 0;

  ImGui::Begin("Renderer Options");
  ImGui::Text("This is some useful text.");
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  if (ImGui::Button("Button")) {
    counter++;
  }
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::Render();
}

// for resize window
void Application::recreateSwapChain(bool& framebufferResized) {
  framebufferResized = true;

  glm::ivec2 size;
  window.framebufferSize(size);
  while (size[0] == 0 || size[1] == 0) {
    window.framebufferSize(size);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device.logical());

  swapChain.recreate();
  renderPass.recreate();
  graphicsPipeline.recreate();
  commandBuffers.recreate();

  interface.recreate();

  renderPass.cleanupOld();
  swapChain.cleanupOld();
}