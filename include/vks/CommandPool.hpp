#ifndef COMMANDPOOL_HPP
#define COMMANDPOOL_HPP

#include <NonCopyable.hpp>
#include <vulkan/vulkan.h>

namespace vks {
class Device;

class CommandPool : public NonCopyable {
public:
  CommandPool(const Device &device, const VkCommandPoolCreateFlags &flags);
  ~CommandPool();

  inline const VkCommandPool &handle() const { return m_pool; };

private:
  VkCommandPool m_pool;
  VkCommandPoolCreateFlags m_flags;

  const Device &m_device;
};
} // namespace vks

#endif // COMMANDPOOL_HPP
