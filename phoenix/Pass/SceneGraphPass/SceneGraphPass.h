#pragma once

#include <SceneGraph/DrawInformations/DrawInformations.h>
#include <SceneGraph/Materials/Material.h>
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>
#include <memory>
#include <vkw/Descriptor/DescriptorSetLayout.h>

namespace phx {

class Device;
class SceneGraph;

using MatrixBufferBinding =
    DescriptorBinding<VK_SHADER_STAGE_VERTEX_BIT, vk::DescriptorType::eStorageBuffer, 1, glm::mat4>;
using MatrixBufferLayout = DescriptorSetLayout<MatrixBufferBinding>;

using MatrixPushConstant = PushConstantRange<0, sizeof(uint32_t), VK_SHADER_STAGE_VERTEX_BIT>;

class SceneGraphPass {
    class Impl;

  public:
    SceneGraphPass(Device &device) noexcept;

    void setLookAtMatrix(glm::mat4 matrix) noexcept;
    void setProjectionMatrix(glm::mat4 matrix) noexcept;

    ltl::tuple_t<DescriptorSet, std::vector<ltl::tuple_t<DrawBatche, uint32_t>>>
    generate(SceneGraph &sceneGraph) noexcept;

    const MatrixBufferLayout &matrixBufferLayout() const noexcept;

    ~SceneGraphPass();

  private:
    std::unique_ptr<Impl> m_impl;
};

} // namespace phx
