#pragma once

#include "DescriptorPool.h"
#include <ltl/algos.h>
#include <unordered_map>
#include <vector>

namespace phx {

class DescriptorPoolManager {
public:
  void addDescriptorPool(DescriptorPool pool) noexcept;

  template <typename Layout>
  auto allocate(decltype_t(Layout::type_list) values) {
    using List = DescriptorPoolList<Layout>;
    std::type_index type = typeid(List);
    assert(ltl::contains_map(m_pools, type));
    return m_pools.at(type).allocate<Layout>(std::move(values));
  }

private:
  std::unordered_map<std::type_index, DescriptorPool> m_pools;
};

} // namespace phx
