#pragma once

#include <vkw/Image/ImageLoader.h>
#include <vkw/Image/SampledImage.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
#include "TexturedMaterial.h"

namespace phx {

template <typename SampledImage, std::size_t ImageNumber>
class TexturedMaterialManager {
    static constexpr auto buildLayout() {
        auto index_sequence = ltl::build_index_sequence(number_v<ImageNumber>);

        return index_sequence([](auto... is) {
            return ltl::type_v<DescriptorSetLayout<decltype(
                is, DescriptorBinding<VK_SHADER_STAGE_FRAGMENT_BIT, vk::DescriptorType::eCombinedImageSampler, 1,
                                      SampledImage>{})...>>;
        });
    }

  public:
    using Layout = decltype_t(buildLayout());

    TexturedMaterialManager(ImageLoader<SampledImage> &imageLoader, DescriptorPoolManager &descriptorPool) noexcept :
        m_imageLoader{imageLoader}, m_descriptorPool{descriptorPool} {}

    TexturedMaterial<TexturedMaterialManager> allocate(std::array<std::string, ImageNumber> imagePaths, bool withMipmap,
                                                       vk::PipelineStageFlags pipelineStage) noexcept {
        if (auto descriptorSet = getDescriptorSet(imagePaths)) {
            return *descriptorSet;
        }

        auto pathToImage = [withMipmap, pipelineStage, this](const auto &... path) {
            return (ltl::tuple_t{} + ... +
                    ltl::tuple_t<ltl::tuple_t<SampledImage>>{m_imageLoader.load(path, withMipmap, pipelineStage)});
        };

        auto images = std::apply(pathToImage, imagePaths);

        auto allocateDescriptorSet = [this](auto... images) { //
            return m_descriptorPool.allocate<Layout>(images...);
        };

        return {images(allocateDescriptorSet)};
    }

  private:
    std::optional<DescriptorSet> getDescriptorSet(const std::array<std::string, ImageNumber> &imagePaths) {
        auto matchingPath = [&](auto tuple) { return ltl::equal(tuple[0_n], imagePaths); };

        if (auto x = ltl::find_if_ptr(m_setsByPath, matchingPath)) {
            return (*x)[1_n];
        }

        return {};
    }

  private:
    ImageLoader<SampledImage> &m_imageLoader;
    DescriptorPoolManager &m_descriptorPool;
    std::vector<ltl::tuple_t<std::array<std::string, ImageNumber>, DescriptorSet>> m_setsByPath;
};

} // namespace phx
