#pragma once
#include <ltl/concept.h>
#include <utility>

namespace phx {
template <typename T>
struct VulkanResource {
  public:
    auto getHandle() const noexcept { return *m_handle; }
    auto getHandlePtr() const noexcept { return std::addressof(*m_handle); }

  protected:
    T m_handle;
};

inline auto getHandle = [](const auto &value) { return value.getHandle(); };

#define MAKE_IS_VULKAN_RESOURCE(vkType, name, conceptName)                                                             \
    template <typename T>                                                                                              \
    constexpr auto name(T &&t) noexcept {                                                                              \
        auto hasHandle = IS_VALID((x), x.getHandle());                                                                 \
        if_constexpr(hasHandle(FWD(t))) { return ltl::type_v<vkType> == type_from(t.getHandle()); }                    \
        else {                                                                                                         \
            return false_v;                                                                                            \
        }                                                                                                              \
    }                                                                                                                  \
    template <typename T>                                                                                              \
    constexpr auto conceptName = decltype(name(std::declval<T>()))::value

} // namespace phx
