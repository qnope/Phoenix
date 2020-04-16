#pragma once
#include <cassert>

#include <filesystem>
#include <fstream>

#include <iterator>
#include <ltl/algos.h>
#include <string_view>
#include <utility>
#include <vector>

#include "vulkan.h"

namespace phx {

inline std::vector<const char *>
to_const_char_vector(const std::vector<std::string> &strings) noexcept {
  std::vector<const char *> ptrs;
  auto to_const_char = [](const std::string &s) { return s.c_str(); };
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

struct FileNotFoundException {
  std::string path;
};

inline std::string readFile(const std::string &path) {
  std::ifstream stream(path, std::ios::ate | std::ios::binary);

  if (stream) {
    std::string result;
    result.reserve(stream.tellg());
    stream.seekg(0);
    return {std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
  }

  throw FileNotFoundException{path};
}

inline std::string getBaseDirectory(std::string_view path) noexcept {
  return std::filesystem::path(path).parent_path().string();
}

} // namespace phx
