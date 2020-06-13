#pragma once

#include <memory>

#include "Allocator/Allocator.h"
#include "Framebuffer.h"
#include "Image/Sampler.h"
#include "Instance.h"
#include "PipelineLayout.h"
#include "Queue.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "Surface.h"
#include "TemplatedGraphicPipeline.h"
#include "VulkanResource.h"

#include "Descriptor/DescriptorPoolList.h"
#include "Descriptor/DescriptorSetLayout.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final : public VulkanResource<vk::UniqueDevice> {
public:
  Device(const Instance &instance, const Surface &surface);

  Queue &getQueue() const noexcept;
  Allocator &getAllocator() const noexcept;

  vk::PhysicalDevice getPhysicalDevice() const noexcept;

  auto createSemaphore() const noexcept {
    return getHandle().createSemaphoreUnique(vk::SemaphoreCreateInfo());
  }

  template <typename Type>
  ShaderModule<Type> createShaderModule(const std::string &path,
                                        bool debug) const {
    return {getHandle(), path, debug};
  }

  template <typename... Args>
  PipelineLayout createPipelineLayout(const Args &... args) const {
    return {getHandle(), args...};
  }

  template <typename... RPs, typename SubpassIndex, typename... Args>
  TemplatedGraphicPipeline<RenderPass<RPs...>, SubpassIndex, Args...>
  createGraphicPipeline(PipelineLayout pipelineLayout,
                        const RenderPass<RPs...> &renderPass,
                        SubpassIndex subpassIndex, Args... args) const {
    return {getHandle(), std::move(pipelineLayout), renderPass, subpassIndex,
            std::move(args)...};
  }

  template <typename... Attachments, typename... Subpasses,
            typename... Dependencies>
  RenderPass<ltl::tuple_t<Attachments...>, ltl::tuple_t<Subpasses...>,
             ltl::tuple_t<Dependencies...>>
  createRenderPass(ltl::tuple_t<Attachments...> attachments,
                   ltl::tuple_t<Subpasses...> subpasses,
                   ltl::tuple_t<Dependencies...> dependencies) const {
    return {getHandle(), attachments, subpasses, dependencies};
  }

  template <typename Buffer> Buffer createBuffer(vk::DeviceSize size) const {
    return {*m_allocator, size};
  }

  template <typename Layout>
  DescriptorPoolList<Layout> createDescriptorPool() const {
    return {getHandle(), Layout{getHandle()}};
  }

  template <typename Image, typename... Ts> Image createImage(Ts... ts) const {
    return {*m_allocator, ts...};
  }

  Sampler createSampler(vk::Filter filterMinMag,
                        vk::SamplerMipmapMode mipmapMode) const noexcept {
    return {getHandle(), filterMinMag, mipmapMode};
  }

  template <typename... Ts>
  Framebuffer<Ts...> createFramebuffer(vk::RenderPass renderPass,
                                       uint32_t width, uint32_t height,
                                       Ts... attachments) const noexcept {
    return {getHandle(), renderPass, width, height, {attachments...}};
  }

  Fence createFence(bool signaledState) const;

private:
  vk::PhysicalDevice m_physicalDevice;
  std::unique_ptr<Queue> m_queue;
  std::unique_ptr<Allocator> m_allocator;
};
} // namespace phx
