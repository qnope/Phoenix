#pragma once

#include "../VulkanResource.h"
#include "../vulkan.h"

#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

#include <ltl/ltl.h>

namespace phx {

constexpr uint32_t MAX_SET_BY_POOL = 10;

template <typename DescriptorLayout> class TemplatedDescriptorPool;

template <typename... Bindings>
class TemplatedDescriptorPool<DescriptorSetLayout<Bindings...>>
    : public VulkanResource<vk::UniqueDescriptorPool> {

  using SetLayout = DescriptorSetLayout<Bindings...>;

public:
  TemplatedDescriptorPool(vk::Device device,
                          const DescriptorSetLayout<Bindings...> &layout)
      : m_device{device}, m_layout{layout.getHandle()} {
    vk::DescriptorPoolCreateInfo info;

    info.maxSets = MAX_SET_BY_POOL * sizeof...(Bindings);
    std::array poolSizes = {vk::DescriptorPoolSize(
        Bindings::descriptorType, MAX_SET_BY_POOL * Bindings::count.value)...};

    info.poolSizeCount = uint32_t(poolSizes.size());
    info.pPoolSizes = poolSizes.data();
    m_handle = device.createDescriptorPoolUnique(info);
  }

  DescriptorSet<SetLayout>
  allocate(DescriptorBindingTypes<Bindings>... values) noexcept {
    assert(m_numberOfAllocation < MAX_SET_BY_POOL);
    ++m_numberOfAllocation;

    vk::DescriptorSetAllocateInfo info;
    info.pSetLayouts = &m_layout;
    info.descriptorPool = getHandle();
    info.descriptorSetCount = 1;

    return {m_device, m_device.allocateDescriptorSets(info)[0], {values...}};
  }

  bool isFull() const noexcept {
    return m_numberOfAllocation == MAX_SET_BY_POOL;
  }

private:
  vk::Device m_device;
  vk::DescriptorSetLayout m_layout;
  std::size_t m_numberOfAllocation = 0;
};
} // namespace phx
