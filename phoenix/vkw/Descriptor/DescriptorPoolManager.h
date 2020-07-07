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
        auto *pool = getPool<Layout>();
        return pool->template allocate<Layout>(std::move(values));
    }

    template <typename Layout>
    const Layout &layout() {
        auto *pool = getPool<Layout>();
        return pool->template layout<Layout>();
    }

  private:
    template <typename Layout>
    DescriptorPool *getPool() {
        using Pool = DescriptorPoolList<Layout>;
        std::type_index type = typeid(Pool);

        if (auto pool = ltl::map_find_ptr(m_pools, type))
            return pool;

        auto it = m_pools.insert({type, m_device.createDescriptorPool<Layout>()}).first;
        return std::addressof(it->second);
    }

  private:
    Device &m_device;
    std::unordered_map<std::type_index, DescriptorPool> m_pools;
};

} // namespace phx
