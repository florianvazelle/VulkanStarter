#include <vks/GraphicsPipeline.hpp>

#include <base_frag.h>
#include <base_vert.h>

#include <iostream>
#include <vks/Device.hpp>
#include <vks/RenderPass.hpp>
#include <vks/SwapChain.hpp>

using namespace vks;

GraphicsPipeline::GraphicsPipeline(const Device& device, const SwapChain& swapChain, const RenderPass& renderPass)
    : m_pipeline(VK_NULL_HANDLE),
      m_layout(VK_NULL_HANDLE),
      m_oldLayout(VK_NULL_HANDLE),

      m_device(device),
      m_swapChain(swapChain),
      m_renderPass(renderPass) {
  createPipeline();
}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
}

void GraphicsPipeline::recreate() {
  vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
  createPipeline();
}

void GraphicsPipeline::createPipeline() {
  VkShaderModule vertShaderModule = createShaderModule(BASE_VERT);
  VkShaderModule fragShaderModule = createShaderModule(BASE_FRAG);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName  = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName  = "main";

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount   = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  // Create information struct about input assembly
  // We'll be organizing our vertices in triangles and the GPU should treat the data accordingly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable                 = VK_FALSE;

  // Pipeline viewport
  VkViewport viewport = {};
  viewport.x          = 0.0f;
  viewport.y          = 0.0f;
  viewport.width      = static_cast<float>(m_swapChain.extent().width);
  viewport.height     = static_cast<float>(m_swapChain.extent().height);
  // Depth buffer range
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Pixel boundary cutoff
  VkRect2D scissor = {};
  scissor.offset   = {0, 0};
  scissor.extent   = m_swapChain.extent();

  // Combine viewport(s) and scissor(s) (some graphics cards allow multiple of each)
  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount                     = 1;
  viewportState.pViewports                        = &viewport;
  viewportState.scissorCount                      = 1;
  viewportState.pScissors                         = &scissor;

  // Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  // Clip fragments instead of clipping them to near and far planes
  rasterizer.depthClampEnable = VK_FALSE;
  // Don't allow the rasterizer to discard geometry
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  // Fill fragments
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth   = 1.0f;
  rasterizer.cullMode    = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace   = VK_FRONT_FACE_CLOCKWISE;
  // Bias depth values
  // This is good for shadow mapping, but we're not doing that currently
  // so we'll disable for now
  rasterizer.depthBiasEnable = VK_FALSE;

  // Multisampling
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable                  = VK_FALSE;
  multisampling.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading                     = 1.0f;

  // Color Blending
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask
      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  // Disable blending
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  // Disable bitwise combination blending
  colorBlending.logicOpEnable   = VK_FALSE;
  colorBlending.logicOp         = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments    = &colorBlendAttachment;
  // Which color channels in framebuffer will be affected
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(m_device.logical(), &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
    throw std::runtime_error("Pipeline Layout creation failed");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount                   = 2;
  pipelineInfo.pStages                      = shaderStages;
  pipelineInfo.pVertexInputState            = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState          = &inputAssembly;
  pipelineInfo.pViewportState               = &viewportState;
  pipelineInfo.pRasterizationState          = &rasterizer;
  pipelineInfo.pColorBlendState             = &colorBlending;
  pipelineInfo.pMultisampleState            = &multisampling;
  pipelineInfo.pDepthStencilState           = nullptr;
  pipelineInfo.pDynamicState                = nullptr;
  pipelineInfo.layout                       = m_layout;
  pipelineInfo.renderPass                   = m_renderPass.handle();
  pipelineInfo.subpass                      = 0;
  pipelineInfo.basePipelineHandle           = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex            = -1;

  if (vkCreateGraphicsPipelines(m_device.logical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline)
      != VK_SUCCESS) {
    throw std::runtime_error("Graphics Pipeline creation failed");
  }

  for (auto& shader : shaderStages) {
    vkDestroyShaderModule(m_device.logical(), shader.module, nullptr);
  }
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<unsigned char>& code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize                 = code.size();

  // Vector class already ensures that the data is correctly aligned,
  // so no need to manually do it
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule module;
  if (vkCreateShaderModule(m_device.logical(), &createInfo, nullptr, &module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return module;
}