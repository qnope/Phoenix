cmake_minimum_required(VERSION 3.10)

project(Phoenix)

if(MSVC)
    add_compile_options(/Zc:preprocessor /permissive-)
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

include_directories(phoenix/)

include_directories(${LTL_PATH})
include_directories(${SDL_PATH}/include)
include_directories(${GLM_PATH})
include_directories(${VULKAN_SDK_PATH}/Include)
include_directories(${VULKAN_MEMORY_ALLOCATOR_PATH}/src)

set(MAIN_SRC main.cpp)

set(PHOENIX_PASS
    phoenix/Pass/GBuffer/GBufferRenderPass.h
    phoenix/Pass/GBuffer/GBufferOutputSubpass.h

    phoenix/Pass/GBuffer/GBufferRenderPass.cpp
    phoenix/Pass/GBuffer/GBufferOutputSubpass.cpp
    )

set(PHOENIX_SCENEGRAPH
    phoenix/SceneGraph/SceneGraph.h

    phoenix/SceneGraph/SceneGraph.cpp

    phoenix/SceneGraph/Nodes/Node.h
    phoenix/SceneGraph/Nodes/GroupNode.h
    phoenix/SceneGraph/Nodes/AbstractNode.h
    phoenix/SceneGraph/Nodes/GeometryNode.h
    phoenix/SceneGraph/Nodes/ActivableNode.h
    phoenix/SceneGraph/Nodes/TransformationNode.h

    phoenix/SceneGraph/Nodes/GroupNode.cpp
    phoenix/SceneGraph/Nodes/GeometryNode.cpp
    phoenix/SceneGraph/Nodes/ActivableNode.cpp
    phoenix/SceneGraph/Nodes/TransformationNode.cpp

    phoenix/SceneGraph/Materials/Material.h
    phoenix/SceneGraph/Materials/MaterialFactory.h
    phoenix/SceneGraph/Materials/AbstractMaterial.h
    phoenix/SceneGraph/Materials/TexturedLambertianMaterial.h

    phoenix/SceneGraph/Materials/MaterialFactory.cpp
    phoenix/SceneGraph/Materials/AbstractMaterial.cpp
    phoenix/SceneGraph/Materials/TexturedLambertianMaterial.cpp

    phoenix/SceneGraph/Visitors/NodeVisitor.h
    phoenix/SceneGraph/Visitors/TypedVisitor.h
    phoenix/SceneGraph/Visitors/OneResultVisitor.h
    phoenix/SceneGraph/Visitors/ListResultVisitor.h
    phoenix/SceneGraph/Visitors/WithResultVisitor.h
    phoenix/SceneGraph/Visitors/GetDrawBatchesVisitor.h

    phoenix/SceneGraph/Visitors/GetDrawBatchesVisitor.cpp

    phoenix/SceneGraph/DrawInformations/DrawInformations.h
    phoenix/SceneGraph/DrawInformations/DrawInformationsAllocator.h

    phoenix/SceneGraph/DrawInformations/DrawInformationsAllocator.cpp
    )

set(PHOENIX_VKW_DESCRIPTOR
    phoenix/vkw/Descriptor/DescriptorSet.h
    phoenix/vkw/Descriptor/DescriptorPool.h
    phoenix/vkw/Descriptor/DescriptorBinding.h
    phoenix/vkw/Descriptor/DescriptorPoolList.h
    phoenix/vkw/Descriptor/DescriptorSetLayout.h
    phoenix/vkw/Descriptor/DescriptorPoolManager.h
    phoenix/vkw/Descriptor/TemplatedDescriptorPool.h
    )

set(PHOENIX_VKW_ALLOCATOR
    phoenix/vkw/Allocator/Allocator.h
    phoenix/vkw/Allocator/AllocatorBlock.h

    phoenix/vkw/Allocator/Allocator.cpp
    )

set(PHOENIX_VKW_BUFFER
    phoenix/vkw/Buffer/Buffer.h
    phoenix/vkw/Buffer/BufferRef.h
    phoenix/vkw/Buffer/BufferList.h
    phoenix/vkw/Buffer/BufferInfo.h
    )

set(PHOENIX_VKW_IMAGE
    phoenix/vkw/Image/Image.h
    phoenix/vkw/Image/Sampler.h
    phoenix/vkw/Image/ImageView.h
    phoenix/vkw/Image/ImageLoader.h
    phoenix/vkw/Image/SampledImage.h

    phoenix/vkw/Image/Sampler.cpp
    )

set(PHOENIX_VKW_PIPELINE
    phoenix/vkw/With_Pipeline/WithBuffer.h
    phoenix/vkw/With_Pipeline/WithShaders.h
    phoenix/vkw/With_Pipeline/WithOutputs.h
    phoenix/vkw/With_Pipeline/WithScissors.h
    phoenix/vkw/With_Pipeline/WithViewports.h
    phoenix/vkw/With_Pipeline/WithDynamicStates.h
    )

set(PHOENIX_VKW
    phoenix/vkw/Fence.h
    phoenix/vkw/Queue.h
    phoenix/vkw/Device.h
    phoenix/vkw/Vertex.h
    phoenix/vkw/vulkan.h
    phoenix/vkw/utility.h
    phoenix/vkw/Subpass.h
    phoenix/vkw/Surface.h
    phoenix/vkw/Instance.h
    phoenix/vkw/Barriers.h
    phoenix/vkw/Swapchain.h
    phoenix/vkw/RenderPass.h
    phoenix/vkw/CommandPool.h
    phoenix/vkw/Framebuffer.h
    phoenix/vkw/ShaderModule.h
    phoenix/vkw/SubpassBuilder.h
    phoenix/vkw/PipelineLayout.h
    phoenix/vkw/ShaderCompiler.h
    phoenix/vkw/MemoryTransfer.h
    phoenix/vkw/VulkanResource.h
    phoenix/vkw/GraphicPipeline.h
    phoenix/vkw/RenderPassWrapper.h
    phoenix/vkw/CommandBufferWrapper.h
    phoenix/vkw/TemplatedGraphicPipeline.h

    phoenix/vkw/Fence.cpp
    phoenix/vkw/Queue.cpp
    phoenix/vkw/Device.cpp
    phoenix/vkw/utility.cpp
    phoenix/vkw/Surface.cpp
    phoenix/vkw/Instance.cpp
    phoenix/vkw/Barriers.cpp
    phoenix/vkw/Swapchain.cpp
    phoenix/vkw/CommandPool.cpp
    phoenix/vkw/ShaderCompiler.cpp

    ${PHOENIX_VKW_IMAGE}
    ${PHOENIX_VKW_BUFFER}
    ${PHOENIX_VKW_PIPELINE}
    ${PHOENIX_VKW_ALLOCATOR}
    ${PHOENIX_VKW_DESCRIPTOR}
    )

set(PHOENIX
    phoenix/constant.h
    phoenix/PhoenixWindow.h
    phoenix/SDL_Initializer.h

    phoenix/PhoenixWindow.cpp
    phoenix/SDL_Initializer.cpp
    ${PHOENIX_VKW}

    ${PHOENIX_PASS}
    ${PHOENIX_SCENEGRAPH})

set(SHADER_SRC
    phoenix/shaders/GBufferPass/GBufferOutput.vert
    phoenix/shaders/GBufferPass/GBufferOutput.frag
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
