#pragma once
#include <utility>

namespace phx {
template <typename T> struct VulkanResource {
public:
  auto getHandle() const noexcept { return *m_handle; }

protected:
  T m_handle;
};
} // namespace phx
