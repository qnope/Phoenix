#pragma once

#include <vector>

#include <ltl/algos.h>

#include "DescriptorPool.h"

namespace phx {
template <typename Layout> class DescriptorPoolList;

template <typename... Bindings>
class DescriptorPoolList<DescriptorSetLayout<Bindings...>> {
  using Layout = DescriptorSetLayout<Bindings...>;
  using Pool = DescriptorPool<Layout>;

public:
  DescriptorPoolList(vk::Device device, Layout layout)
      : m_device{device}, m_layout{std::move(layout)} {}

  const auto &layout() const noexcept { return m_layout; }

  DescriptorSet<Layout>
  allocate(DescriptorBindingTypes<Bindings>... values) noexcept {
    return getCandidatePool().allocate(values...);
  }

private:
  auto &getCandidatePool() {
    using namespace ltl;
    if (auto pool = find_if_ptr(m_pools, not_(&Pool::isFull))) {
      return *pool;
    }
    return m_pools.emplace_back(m_device, m_layout);
  }

private:
  vk::Device m_device;
  Layout m_layout;
  std::vector<Pool> m_pools;
};
} // namespace phx
