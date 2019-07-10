#pragma once

#include <memory>

#include "GraphicPipeline.h"
#include "Instance.h"
#include "PipelineLayout.h"
#include "Queue.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "Surface.h"
#include "VulkanResource.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final : public VulkanResource<vk::UniqueDevice> {
public:
  Device(const Instance &instance, const Surface &surface);

  Queue &getQueue() const noexcept;

  vk::PhysicalDevice getPhysicalDevice() const noexcept;

  auto createSemaphore() const noexcept {
    return getHandle().createSemaphoreUnique(vk::SemaphoreCreateInfo());
  }

  template <typename Type>
  ShaderModule<Type> createShaderModule(const std::string &path, bool debug) const {
    return {getHandle(), path, debug};
  }

  template <typename... Uniforms>
  PipelineLayout<Uniforms...> createPipelineLayout(Uniforms... uniforms) const {
    return {getHandle(), std::move(uniforms)...};
  }

  template <typename... Uniforms, typename... RPs, typename SubpassIndex,
            typename... Args>
  GraphicPipeline<PipelineLayout<Uniforms...>, RenderPass<RPs...>, SubpassIndex, Args...>
  createGraphicPipeline(PipelineLayout<Uniforms...> pipelineLayout,
                        const RenderPass<RPs...> &renderPass, SubpassIndex subpassIndex,
                        Args... args) const {
    return {getHandle(), std::move(pipelineLayout), renderPass, subpassIndex,
            std::move(args)...};
  }

  template <typename... Attachments, typename... Subpasses, typename... Dependencies>
  RenderPass<ltl::tuple_t<Attachments...>, ltl::tuple_t<Subpasses...>,
             ltl::tuple_t<Dependencies...>>
  createRenderPass(ltl::tuple_t<Attachments...> attachments,
                   ltl::tuple_t<Subpasses...> subpasses,
                   ltl::tuple_t<Dependencies...> dependencies) const {
    return {getHandle(), attachments, subpasses, dependencies};
  }

  Fence createFence(bool signaledState) const noexcept;

private:
  vk::PhysicalDevice m_physicalDevice;
  std::unique_ptr<Queue> m_queue;
};
} // namespace phx
