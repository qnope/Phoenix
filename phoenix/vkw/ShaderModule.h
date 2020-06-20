#pragma once

#include "ShaderCompiler.h"
#include "VulkanResource.h"

namespace phx {
struct VertexShaderType {
    static constexpr vk::ShaderStageFlagBits stage = vk::ShaderStageFlagBits::eVertex;
};

struct GeometryShaderType {
    static constexpr vk::ShaderStageFlagBits stage = vk::ShaderStageFlagBits::eGeometry;
};

struct FragmentShaderType {
    static constexpr vk::ShaderStageFlagBits stage = vk::ShaderStageFlagBits::eFragment;
};

struct ComputeShaderType {
    static constexpr vk::ShaderStageFlagBits stage = vk::ShaderStageFlagBits::eCompute;
};

namespace detail {
constexpr shaderc_shader_kind getShaderKind(vk::ShaderStageFlagBits stage) {
    if (stage == vk::ShaderStageFlagBits::eVertex)
        return shaderc_vertex_shader;

    else if (stage == vk::ShaderStageFlagBits::eGeometry)
        return shaderc_geometry_shader;

    else if (stage == vk::ShaderStageFlagBits::eFragment)
        return shaderc_fragment_shader;

    else if (stage == vk::ShaderStageFlagBits::eCompute)
        return shaderc_compute_shader;

    throw "Not a good kind of shader";
}
} // namespace detail

template <typename ShaderType>
class ShaderModule final : public VulkanResource<vk::UniqueShaderModule> {
    static constexpr auto stage = ShaderType::stage;
    static constexpr auto kind = detail::getShaderKind(stage);

  public:
    ShaderModule(vk::Device device, const std::string &path, bool debug) {
        ShaderCompiler compiler{debug};
        auto spirv = compiler.compile(path, kind);

        vk::ShaderModuleCreateInfo info;
        info.codeSize = spirv.size() * sizeof(decltype(spirv)::value_type);
        info.pCode = spirv.data();
        m_handle = device.createShaderModuleUnique(info);
    }

    auto getStageInfo() {
        vk::PipelineShaderStageCreateInfo info;
        info.stage = stage;
        info.module = getHandle();
        info.pName = "main";
        return info;
    }

  private:
};

LTL_MAKE_IS_KIND(ShaderModule, is_shader_module, IsShaderModule, typename);

} // namespace phx
