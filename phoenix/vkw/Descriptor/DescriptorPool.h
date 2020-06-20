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
        auto &pool = m_pool.get<List>();
        return values([&pool](auto... xs) { return pool.allocate(std::move(xs)...); });
    }

    template <typename Layout>
    const Layout &layout() const noexcept {
        using List = DescriptorPoolList<Layout>;
        auto &pool = m_pool.get<List>();
        return pool.layout();
    }

  private:
    ltl::movable_any m_pool;
};
} // namespace phx
