#pragma once

#include "../vulkan.h"
#include <ltl/tuple_algos.h>

#include "../Buffer/BufferRef.h"
#include "../Image/Image.h"

#include <ltl/condition.h>

namespace phx {

template <VkShaderStageFlags _stages, vk::DescriptorType DescriptorType,
          uint32_t Count, typename... Types>
struct DescriptorBinding {
  template <int binding>
  auto toDescriptorBinding(ltl::number_t<binding>) const noexcept {
    return vk::DescriptorSetLayoutBinding(binding, descriptorType, count.value,
                                          stages);
  }

  static constexpr auto count = number_v<Count>;
  static constexpr auto descriptorType = DescriptorType;

  static constexpr auto compute_tuple_types() {
    if constexpr (DescriptorType == vk::DescriptorType::eUniformBuffer) {
      return ltl::type_v<ltl::tuple_t<UniformBufferRef<Types>...>>;
    }

    else if constexpr (DescriptorType == vk::DescriptorType::eStorageBuffer) {
      return ltl::type_v<ltl::tuple_t<StorageBufferRef<Types>...>>;
    }

    else if constexpr (DescriptorType ==
                       vk::DescriptorType::eCombinedImageSampler) {
      return ltl::type_v<ltl::tuple_t<Types...>>;
    }
  }

  static constexpr auto types = decltype(compute_tuple_types()){};

  static_assert(sizeof...(Types), "Types must not be empty");

  static constexpr auto stages = vk::ShaderStageFlags(_stages);
};

template <typename T>
using DescriptorBindingTypes = decltype_t(std::decay_t<T>::types);

namespace details {
static constexpr auto is_buffer(vk::DescriptorType type) {
  return type == ltl::AnyOf{vk::DescriptorType::eUniformBuffer,
                            vk::DescriptorType::eStorageBuffer};
}

static constexpr auto is_image(vk::DescriptorType type) {
  return type == ltl::AnyOf{vk::DescriptorType::eCombinedImageSampler,
                            vk::DescriptorType::eSampledImage};
}

template <typename T> static constexpr auto getInfos(T values) {
  return values([](auto... refs) {
    return std::array{::phx::getDescriptorInfo(refs)...};
  });
}

} // namespace details
template <int bindingIndex, typename Binding>
void writeDescriptorSet(vk::Device device, vk::DescriptorSet set,
                        ltl::number_t<bindingIndex>, Binding binding,
                        decltype_t(Binding::types) values) {
  vk::WriteDescriptorSet write;
  write.dstSet = set;
  write.dstBinding = bindingIndex;
  write.descriptorType = binding.descriptorType;
  write.dstArrayElement = 0;

  auto infos = details::getInfos(FWD(values));
  write.descriptorCount = binding.count.value;

  if constexpr (details::is_buffer(binding.descriptorType)) {
    write.pBufferInfo = infos.data();
  }

  else if constexpr (details::is_image(binding.descriptorType)) {
    write.pImageInfo = infos.data();
  }

  device.updateDescriptorSets(write, {});
}

} // namespace phx
