#pragma once

#include "../VulkanResource.h"
#include "../vulkan.h"

#include "DescriptorSetLayout.h"

#include <ltl/ltl.h>

namespace phx {

constexpr uint32_t MAX_SET_BY_POOL = 10;

template <typename DescriptorLayout> class DescriptorPool;

template <typename... Bindings>
class DescriptorPool<DescriptorSetLayout<Bindings...>>
    : public VulkanResource<vk::UniqueDescriptorPool> {

public:
  DescriptorPool(vk::Device device, DescriptorSetLayout<Bindings...> layout)
      : m_device{device}, m_layout{std::move(layout)} {
    vk::DescriptorPoolCreateInfo info;

    info.maxSets = MAX_SET_BY_POOL * sizeof...(Bindings);
    std::array poolSizes = {vk::DescriptorPoolSize(
        Bindings::type, MAX_SET_BY_POOL * Bindings::count.value)...};

    info.poolSizeCount = uint32_t(poolSizes.size());
    info.pPoolSizes = poolSizes.data();
    m_handle = device.createDescriptorPoolUnique(info);
  }

  auto allocate() noexcept {
    ++m_numberOfAllocation;

    vk::DescriptorSetAllocateInfo info;
    info.pSetLayouts = m_layout.getHandlePtr();
    info.descriptorPool = getHandle();
    info.descriptorSetCount = 1;

    return m_device.allocateDescriptorSets(info)[0];
  }

  const auto &layout() const noexcept { return m_layout; }

private:
  vk::Device m_device;
  DescriptorSetLayout<Bindings...> m_layout;
  std::size_t m_numberOfAllocation = 0;
};
} // namespace phx
