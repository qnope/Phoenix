#pragma once

#include "ShaderCompiler.h"
#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
struct VertexShaderType {
  static constexpr vk::ShaderStageFlags type = vk::ShaderStageFlagBits::eVertex;
};

struct GeometryShaderType {
  static constexpr vk::ShaderStageFlags type =
      vk::ShaderStageFlagBits::eGeometry;
};

struct FragmentShaderType {
  static constexpr vk::ShaderStageFlags type =
      vk::ShaderStageFlagBits::eFragment;
};

struct ComputeShaderType {
  static constexpr vk::ShaderStageFlags type =
      vk::ShaderStageFlagBits::eCompute;
};

namespace detail {
constexpr shaderc_shader_kind getShaderKind(vk::ShaderStageFlags flag) {
  if (flag == vk::ShaderStageFlagBits::eVertex)
    return shaderc_vertex_shader;

  else if (flag == vk::ShaderStageFlagBits::eGeometry)
    return shaderc_geometry_shader;

  else if (flag == vk::ShaderStageFlagBits::eFragment)
    return shaderc_fragment_shader;

  else if (flag == vk::ShaderStageFlagBits::eCompute)
    return shaderc_compute_shader;

  throw "Not a good kind of shader";
}
} // namespace detail

template <typename ShaderType>
class ShaderModule final : public VulkanResource<vk::UniqueShaderModule> {
public:
  static constexpr auto type = ShaderType::type;
  static constexpr auto kind = detail::getShaderKind(type);

  ShaderModule(vk::Device device, const std::string &path, bool debug) {
    ShaderCompiler compiler{debug};
    auto spirv = compiler.compile(path, kind);

    vk::ShaderModuleCreateInfo info;
    info.codeSize = spirv.size() * sizeof(decltype(spirv)::value_type);
    info.pCode = spirv.data();
    m_handle = device.createShaderModuleUnique(info);
  }

private:
};

} // namespace phx
