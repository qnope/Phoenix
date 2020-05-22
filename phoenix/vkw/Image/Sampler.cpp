#include "Sampler.h"

namespace phx {
Sampler::Sampler(vk::Device device, vk::Filter minMag,
                 vk::SamplerMipmapMode mipmapMode) noexcept {
  vk::SamplerCreateInfo info{};
  info.magFilter = info.minFilter = minMag;
  info.mipmapMode = mipmapMode;
  info.addressModeU = info.addressModeV = info.addressModeW =
      vk::SamplerAddressMode::eRepeat;
  info.mipLodBias = 0.0f;
  info.anisotropyEnable = false;
  info.compareEnable = false;
  info.minLod = 0.0;
  info.maxLod = 1000.0;
  info.unnormalizedCoordinates = false;
  m_handle = device.createSamplerUnique(info);
}
} // namespace phx
