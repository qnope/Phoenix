#pragma once
#include <ltl/overloader.h>
#include <ltl/range.h>
#include <string_view>
#include <utility>
#include <vector>

#include "Instance.h"
#include "vulkan.hpp"

namespace phx {
constexpr auto compare_const_char() {
  return [](const char *a, const char *b) {
    return std::string_view{a} < std::string_view{b};
  };
}

constexpr auto equality_const_char() {
  return [](const char *a, const char *b) {
    return std::string_view{a} == std::string_view{b};
  };
}

static constexpr auto to_name() {
  return ltl::overloader{
      [](const vk::ExtensionProperties &p) { return p.extensionName; },
      [](const vk::LayerProperties &p) { return p.layerName; }};
}

constexpr struct ExtensionTag {
  using ExceptionType = ExtentionInvalidException;
} extensionTag;

constexpr struct LayerTag {
  using ExceptionType = LayerInvalidException;
} layerTag;

inline auto getProperties(ExtensionTag) {
  return vk::enumerateInstanceExtensionProperties();
}

inline auto getProperties(LayerTag) {
  return vk::enumerateInstanceLayerProperties();
}

inline auto getProperties(ExtensionTag, vk::PhysicalDevice device) {
  return device.enumerateDeviceExtensionProperties();
}

template <typename Tag, typename... Args>
inline void checkAvailability(std::vector<const char *> &toTest, Tag tag,
                              Args &&... argsGetProperties) {
  const auto allowedProperties =
      getProperties(tag, std::forward<Args>(argsGetProperties)...);

  std::vector<const char *> allowed;
  ltl::transform(allowedProperties, std::back_inserter(allowed), to_name());
  ltl::sort(toTest, compare_const_char());
  ltl::sort(allowed, compare_const_char());

  std::vector<const char *> intersections;

  std::vector<std::string> differences;
  ltl::set_intersection(allowed, toTest, std::back_inserter(intersections),
                        compare_const_char());
  ltl::set_symmetric_difference(intersections, toTest,
                                std::back_inserter(differences),
                                compare_const_char());

  if (!differences.empty()) {
    throw typename Tag::ExceptionType{std::move(differences)};
  }
}

} // namespace phx
