#pragma once
#include <cassert>

#include <filesystem>
#include <fstream>

#include <iterator>
#include <ltl/algos.h>
#include <string_view>
#include <utility>
#include <vector>

#include <ltl/Tuple.h>

#include "../constant.h"
#include "vulkan.h"

namespace phx {

struct FileNotFoundException {
    std::string path;
};

constexpr struct ExtensionTag {
} extensionTag;

constexpr struct LayerTag {
} layerTag;

std::vector<const char *> to_const_char_vector(const std::vector<std::string> &strings) noexcept;

std::vector<std::string> to_string_vector(std::vector<const char *> &ptrs) noexcept;

inline auto getProperties(ExtensionTag) noexcept { return vk::enumerateInstanceExtensionProperties(); }

inline auto getProperties(LayerTag) { return vk::enumerateInstanceLayerProperties(); }

inline auto getProperties(vk::PhysicalDevice device) { return device.enumerateDeviceExtensionProperties(); }

template <typename Tag>
auto getPropertiesNames(Tag &&tag) {
    const auto properties = getProperties(FWD(tag));
    std::vector<std::string> propertiesNames;
    ltl::overloader to_name{[](vk::ExtensionProperties p) -> std::string { return p.extensionName.data(); },
                            [](vk::LayerProperties p) -> std::string { return p.layerName.data(); }};

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
std::vector<std::string> getUnavailables(std::vector<std::string> &values, Tag &&tag) noexcept {
    const auto allowed = getPropertiesNames(FWD(tag));
    ltl::sort(values);
    std::vector<std::string> differences;
    ltl::set_difference(values, allowed, std::back_inserter(differences));
    return differences;
}

std::string readFile(const std::string &path);

std::string getBaseDirectory(std::string_view path) noexcept;

ltl::tuple_t<Width, Height, std::vector<std::byte>> loadImage(std::string path);

} // namespace phx
