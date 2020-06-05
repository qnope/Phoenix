#pragma once

#include "../Device.h"
#include "DescriptorPool.h"
#include "DescriptorPoolList.h"
#include <ltl/algos.h>
#include <unordered_map>
#include <vector>

namespace phx {

class DescriptorPoolManager {
public:
  DescriptorPoolManager(Device &device) noexcept : m_device{device} {}

  template <typename Layout>
  auto allocate(decltype_t(Layout::type_list) values) {
    using Pool = DescriptorPoolList<Layout>;
    std::type_index type = typeid(Pool);

    DescriptorPool *pool = ltl::find_map_ptr(m_pools, type);

    if (!pool) {
      auto it =
          m_pools.insert({type, m_device.createDescriptorPool<Layout>()}).first;
      pool = std::addressof(it->second);
    }
    return pool->template allocate<Layout>(std::move(values));
  }

private:
  Device &m_device;
  std::unordered_map<std::type_index, DescriptorPool> m_pools;
};

} // namespace phx
