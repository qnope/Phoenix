#pragma once

#include <SceneGraph/DrawInformations/DrawInformations.h>
#include <SceneGraph/Materials/Material.h>
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>
#include <memory>

namespace phx {

class Device;
class SceneGraph;

class SceneGraphPass {
  class Impl;

public:
  SceneGraphPass(Device &device) noexcept;

  ltl::tuple_t<DescriptorSet, std::vector<DrawBatche>>
  generate(SceneGraph &sceneGraph) noexcept;

  ~SceneGraphPass();

private:
  std::unique_ptr<Impl> m_impl;
};

} // namespace phx
