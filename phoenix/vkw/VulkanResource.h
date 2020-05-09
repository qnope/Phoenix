#pragma once
#include <utility>

namespace phx {
template <typename T> struct VulkanResource {
public:
  auto getHandle() const noexcept { return *m_handle; }
  auto getHandlePtr() const noexcept { return std::addressof(*m_handle); }

protected:
  T m_handle;
};

inline auto getHandle = [](const auto &value) { return value.getHandle(); };

} // namespace phx
