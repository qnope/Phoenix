#pragma once

#include <memory>
#include <typeindex>
#include <utility>
#include <vkw/vulkan.h>

#include <vkw/Descriptor/DescriptorSet.h>
#include <vkw/PipelineLayout.h>

namespace phx {
class Material {
  struct Concept {
    virtual bool isCompatibleWith(const PipelineLayout &pipelineLayout) const
        noexcept = 0;

    virtual void bindTo(vk::CommandBuffer cmdBuffer,
                        const PipelineLayout &pipelineLayout) const
        noexcept = 0;

    virtual ~Concept() {}
  };

  template <typename T> class Model final : public Concept {
  public:
    Model(T material) noexcept : m_material{std::move(material)} {}

    bool isCompatibleWith(const PipelineLayout &pipelineLayout) const
        noexcept override {
      return m_material.isCompatibleWith(pipelineLayout);
    }

    void bindTo(vk::CommandBuffer cmdBuffer,
                const PipelineLayout &pipelineLayout) const noexcept override {
      assert(m_material.isCompatibleWith(pipelineLayout));
      m_material.bindTo(cmdBuffer, pipelineLayout);
    }

  private:
    T m_material;
  };

public:
  template <typename T>
  Material(T material) noexcept
      : m_material{std::make_shared<Model<T>>(std::move(material))} {}

  void bindTo(vk::CommandBuffer cmdBuffer,
              const PipelineLayout &pipelineLayout) const noexcept {
    assert(m_material);
    m_material->bindTo(cmdBuffer, pipelineLayout);
  }

  bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept {
    assert(m_material);
    return m_material->isCompatibleWith(pipelineLayout);
  }

private:
  std::shared_ptr<Concept> m_material;
};
} // namespace phx
