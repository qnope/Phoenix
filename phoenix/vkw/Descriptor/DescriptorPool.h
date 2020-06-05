#pragma once

#include "DescriptorPoolList.h"
#include <ltl/movable_any.h>

namespace phx {
class DescriptorPool {
public:
  template <typename T>
  DescriptorPool(T poolList) noexcept : m_pool{std::move(poolList)} {}

  std::type_index type() const noexcept { return m_pool.type(); }

  template <typename Layout>
  auto allocate(decltype_t(Layout::type_list) values) {
    using List = DescriptorPoolList<Layout>;
    auto &ptr = m_pool.get<List>();
    return values(
        [&ptr](auto... xs) { return ptr.allocate(std::move(xs)...); });
  }

private:
  ltl::movable_any m_pool;
};
} // namespace phx
