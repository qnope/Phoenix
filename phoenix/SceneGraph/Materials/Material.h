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
        Concept(std::type_index type, vk::DescriptorSet set) :
            m_type{type}, //
            m_descriptorSet{set} {}

        virtual bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept = 0;

        virtual void bindTo(CommandBufferWrapper &cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept = 0;

        virtual ~Concept() {}

        auto type() const noexcept { return m_type; }
        auto set() const noexcept { return m_descriptorSet; }

      private:
        std::type_index m_type;
        vk::DescriptorSet m_descriptorSet;
    };

    template <typename T>
    class Model final : public Concept {
      public:
        Model(T material) noexcept :
            Concept{typeid(T), material.descriptorSet().getHandle()}, //
            m_material{std::move(material)} {}

        bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept override {
            return m_material.isCompatibleWith(pipelineLayout);
        }

        void bindTo(CommandBufferWrapper &cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept override {
            assert(m_material.isCompatibleWith(pipelineLayout));
            m_material.bindTo(cmdBuffer, pipelineLayout);
        }

      private:
        T m_material;
    };

  public:
    template <typename T>
    Material(T material) noexcept : m_material{std::make_shared<Model<T>>(std::move(material))} {}

    void bindTo(CommandBufferWrapper &cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept {
        assert(m_material);
        m_material->bindTo(cmdBuffer, pipelineLayout);
    }

    bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept {
        assert(m_material);
        return m_material->isCompatibleWith(pipelineLayout);
    }

    friend bool operator<(const Material &m1, const Material &m2) noexcept {
        if (m1.type() == m2.type())
            return m1.set() < m2.set();
        return m1.type() < m2.type();
    }

    friend bool operator==(const Material &m1, const Material &m2) noexcept {
        return m1.type() == m2.type() && m1.set() == m2.set();
    }

  private:
    std::type_index type() const noexcept { return m_material->type(); }
    vk::DescriptorSet set() const noexcept { return m_material->set(); }

  private:
    std::shared_ptr<Concept> m_material;
};
} // namespace phx
