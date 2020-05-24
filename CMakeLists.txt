cmake_minimum_required(VERSION 3.10)

project(Phoenix)

if(MSVC)
    add_compile_options(/experimental:preprocessor /permissive-)
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

include_directories(${LTL_PATH})
include_directories(${SDL_PATH}/include)
include_directories(${GLM_PATH})
include_directories(${VULKAN_SDK_PATH}/Include)
include_directories(${VULKAN_MEMORY_ALLOCATOR_PATH}/src)

set(MAIN_SRC main.cpp)

set(PHOENIX_VKW_DESCRIPTOR
    phoenix/vkw/Descriptor/DescriptorSet.h
    phoenix/vkw/Descriptor/DescriptorBinding.h
    phoenix/vkw/Descriptor/DescriptorSetLayout.h
    phoenix/vkw/Descriptor/DescriptorPool.h
    phoenix/vkw/Descriptor/DescriptorPoolList.h
    )

set(PHOENIX_VKW_ALLOCATOR
    phoenix/vkw/Allocator/Allocator.h
    phoenix/vkw/Allocator/Allocator.cpp
    phoenix/vkw/Allocator/AllocatorBlock.h
    )

set(PHOENIX_VKW_BUFFER
    phoenix/vkw/Buffer/Buffer.h
    phoenix/vkw/Buffer/BufferRef.h
    phoenix/vkw/Buffer/BufferList.h
    phoenix/vkw/Buffer/BufferInfo.h
    )

set(PHOENIX_VKW_IMAGE
    phoenix/vkw/Image/Image.h
    phoenix/vkw/Image/ImageView.h
    phoenix/vkw/Image/SampledImage.h
    phoenix/vkw/Image/Sampler.h
    phoenix/vkw/Image/Sampler.cpp
    )

set(PHOENIX_VKW_PIPELINE
    phoenix/vkw/With_Pipeline/WithDynamicStates.h
    phoenix/vkw/With_Pipeline/WithShaders.h
    phoenix/vkw/With_Pipeline/WithScissors.h
    phoenix/vkw/With_Pipeline/WithViewports.h
    phoenix/vkw/With_Pipeline/WithOutputs.h
    phoenix/vkw/With_Pipeline/WithBuffer.h
    )

set(PHOENIX_VKW
    phoenix/vkw/Instance.h
    phoenix/vkw/Device.h
    phoenix/vkw/Queue.h
    phoenix/vkw/Surface.h
    phoenix/vkw/VulkanResource.h
    phoenix/vkw/utility.h
    phoenix/vkw/Swapchain.h
    phoenix/vkw/ShaderCompiler.h
    phoenix/vkw/ShaderModule.h
    phoenix/vkw/GraphicPipeline.h
    phoenix/vkw/PipelineLayout.h
    phoenix/vkw/RenderPass.h
    phoenix/vkw/RenderPassWrapper.h
    phoenix/vkw/Subpass.h
    phoenix/vkw/SubpassBuilder.h
    phoenix/vkw/Framebuffer.h
    phoenix/vkw/CommandPool.h
    phoenix/vkw/Vertex.h
    phoenix/vkw/Fence.h
    phoenix/vkw/vulkan.h
    phoenix/vkw/CommandBufferWrapper.h
    phoenix/vkw/MemoryTransfer.h
    phoenix/vkw/Barriers.h

    phoenix/vkw/Instance.cpp
    phoenix/vkw/Device.cpp
    phoenix/vkw/Queue.cpp
    phoenix/vkw/Surface.cpp
    phoenix/vkw/Swapchain.cpp
    phoenix/vkw/ShaderCompiler.cpp
    phoenix/vkw/CommandPool.cpp
    phoenix/vkw/Fence.cpp
    phoenix/vkw/Barriers.cpp
    phoenix/vkw/utility.cpp

    ${PHOENIX_VKW_IMAGE}
    ${PHOENIX_VKW_BUFFER}
    ${PHOENIX_VKW_PIPELINE}
    ${PHOENIX_VKW_ALLOCATOR}
    ${PHOENIX_VKW_DESCRIPTOR}
    )

set(PHOENIX_EXAMPLE
    ex/triangle.h)

set(PHOENIX
    phoenix/constant.h
    phoenix/PhoenixWindow.h
    phoenix/SDL_Initializer.h

    phoenix/PhoenixWindow.cpp
    phoenix/SDL_Initializer.cpp
    ${PHOENIX_VKW}
    ${PHOENIX_EXAMPLE})

set(SHADER_SRC
    phoenix/shaders/TriangleTest/triangle.vert
    phoenix/shaders/TriangleTest/triangle.frag)

add_executable(Phoenix
    ${MAIN_SRC}
    ${PHOENIX}
    ${SHADER_SRC}
    )

target_link_libraries(Phoenix
    ${SDL_PATH}/lib/x64/SDL2main.lib
    ${SDL_PATH}/lib/x64/SDL2.lib
    ${SDL_PATH}/lib/x64/SDL2_image.lib
    ${VULKAN_SDK_PATH}/Lib/vulkan-1.lib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_shared.lib)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_shared.lib )
endif(CMAKE_BUILD_TYPE MATCHES Debug)
