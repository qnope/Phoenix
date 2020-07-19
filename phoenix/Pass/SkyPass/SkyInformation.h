#pragma once

#include <vkw/vulkan.h>
#include <vkw/PipelineLayout.h>

namespace phx {
struct SkyInformations {
    glm::mat4 inversedMatrix;
};
static_assert(sizeof(SkyInformations) < 128);

using SkyPushConstantRange = PushConstantRange<0, sizeof(SkyInformations), VK_SHADER_STAGE_FRAGMENT_BIT>;

} // namespace phx
