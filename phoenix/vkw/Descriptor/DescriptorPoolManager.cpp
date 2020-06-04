#include "DescriptorPoolManager.h"

namespace phx {
void DescriptorPoolManager::addDescriptorPool(DescriptorPool pool) noexcept {
  auto type = pool.type();
  assert(!ltl::contains_map(m_pools, type));
  m_pools.emplace(type, std::move(pool));
}
} // namespace phx
