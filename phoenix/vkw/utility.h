#pragma once
#include <cassert>
#include <iterator>
#include <ltl/overloader.h>
#include <ltl/range.h>
#include <string_view>
#include <utility>
#include <vector>

#include "vulkan.hpp"

#define FWD(x) ::std::forward<decltype(x)>(x)

namespace phx {

inline std::vector<const char *>
to_const_char_vector(std::vector<std::string> &strings) noexcept {
  std::vector<const char *> ptrs;
  auto to_const_char = [](std::string &s) { return s.c_str(); };
  ltl::transform(strings, std::back_inserter(ptrs), to_const_char);
  return ptrs;
}

inline std::vector<std::string>
to_string_vector(std::vector<const char *> &ptrs) noexcept {
  std::vector<std::string> strings;
  auto to_string = [](const char *p) -> std::string { return p; };
  ltl::transform(ptrs, std::back_inserter(strings), to_string);
  return strings;
}

constexpr struct ExtensionTag {
} extensionTag;

constexpr struct LayerTag {
} layerTag;

inline auto getProperties(ExtensionTag) {
  return vk::enumerateInstanceExtensionProperties();
}

inline auto getProperties(LayerTag) {
  return vk::enumerateInstanceLayerProperties();
}

inline auto getProperties(vk::PhysicalDevice device) {
  return device.enumerateDeviceExtensionProperties();
}

template <typename Tag> auto getPropertiesNames(Tag &&tag) {
  const auto properties = getProperties(FWD(tag));
  std::vector<std::string> propertiesNames;
  ltl::overloader to_name{
      [](vk::ExtensionProperties p) -> std::string { return p.extensionName; },
      [](vk::LayerProperties p) -> std::string { return p.layerName; }};

  ltl::transform(properties, std::back_inserter(propertiesNames), to_name);
  ltl::sort(propertiesNames);
  return propertiesNames;
};

template <typename Tag>
bool areAvailable(std::vector<std::string> &values, Tag &&tag) noexcept {
  const auto allowed = getPropertiesNames(FWD(tag));
  ltl::sort(values);
  return ltl::includes(allowed, values);
};

template <typename Tag>
std::vector<std::string> getUnavailables(std::vector<std::string> &values,
                                         Tag &&tag) noexcept {
  const auto allowed = getPropertiesNames(FWD(tag));
  ltl::sort(values);
  std::vector<std::string> differences;
  ltl::set_difference(values, allowed, std::back_inserter(differences));
  return differences;
}

} // namespace phx
