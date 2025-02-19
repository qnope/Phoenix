cmake_minimum_required(VERSION 3.10)

project(Phoenix)

if(MSVC)
    add_compile_options(/Zc:preprocessor /permissive-)
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Little-Type-Library)

include_directories(phoenix/)
include_directories(${ASSIMP_PATH}/include)
include_directories(${ASSIMP_PATH}/build/include)
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

    phoenix/Pass/SkyPass/SkyPass.h
    phoenix/Pass/SkyPass/SkyPass.cpp
    phoenix/Pass/SkyPass/SkyInformation.h

    phoenix/Pass/DepthPass/DepthSubpass.h
    phoenix/Pass/DepthPass/DepthSubpass.cpp

    phoenix/Pass/SceneGraphPass/SceneGraphPass.h
    phoenix/Pass/SceneGraphPass/SceneGraphPass.cpp

    phoenix/Pass/Presentation/PresentationSubpass.h
    phoenix/Pass/Presentation/PresentationRenderPass.h

    phoenix/Pass/Presentation/PresentationSubpass.cpp
    phoenix/Pass/Presentation/PresentationRenderPass.cpp

    )

set(PHOENIX_SCENEGRAPH
    phoenix/SceneGraph/SceneGraph.h
    phoenix/SceneGraph/ModelImporter.h

    phoenix/SceneGraph/SceneGraph.cpp
    phoenix/SceneGraph/ModelImporter.cpp

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
    phoenix/SceneGraph/Materials/TexturedMaterial.h
    phoenix/SceneGraph/Materials/BufferedMaterial.h
    phoenix/SceneGraph/Materials/BufferedMaterialManager.h
    phoenix/SceneGraph/Materials/TexturedMaterialManager.h
    phoenix/SceneGraph/Materials/ColoredLambertianMaterial.h
    phoenix/SceneGraph/Materials/TexturedLambertianMaterial.h

    phoenix/SceneGraph/Materials/MaterialFactory.cpp
    phoenix/SceneGraph/Materials/AbstractMaterial.cpp

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

set(PHOENIX_VKW_RENDERPASS
    phoenix/vkw/RenderPass/Subpass.h
    phoenix/vkw/RenderPass/RenderPass.h
    phoenix/vkw/RenderPass/SubpassBuilder.h
    phoenix/vkw/RenderPass/RenderPassWrapper.h
    )

set(PHOENIX_VKW
    phoenix/vkw/Fence.h
    phoenix/vkw/Queue.h
    phoenix/vkw/Device.h
    phoenix/vkw/Vertex.h
    phoenix/vkw/vulkan.h
    phoenix/vkw/utility.h
    phoenix/vkw/Surface.h
    phoenix/vkw/Instance.h
    phoenix/vkw/Barriers.h
    phoenix/vkw/Swapchain.h
    phoenix/vkw/CommandPool.h
    phoenix/vkw/Framebuffer.h
    phoenix/vkw/ShaderModule.h
    phoenix/vkw/PipelineLayout.h
    phoenix/vkw/ShaderCompiler.h
    phoenix/vkw/MemoryTransfer.h
    phoenix/vkw/VulkanResource.h
    phoenix/vkw/GraphicPipeline.h
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
    ${PHOENIX_VKW_RENDERPASS}
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
    phoenix/shaders/GBufferPass/GBufferLambertianTexture.frag
    phoenix/shaders/GBufferPass/GBufferLambertianColored.frag

    phoenix/shaders/DepthPass/DepthPass.vert
    phoenix/shaders/DepthPass/DepthPass.frag

    phoenix/shaders/SkyPass/SkyPass.vert
    phoenix/shaders/SkyPass/SkyPass.frag

    phoenix/shaders/PresentationPass/PresentationPass.vert
    phoenix/shaders/PresentationPass/PresentationPass.frag)

add_executable(Phoenix
    ${MAIN_SRC}
    ${PHOENIX}
    ${SHADER_SRC}
    )

target_link_libraries(Phoenix
    LTL
    ${SDL_PATH}/lib/x64/SDL2main.lib
    ${SDL_PATH}/lib/x64/SDL2.lib
    ${SDL_PATH}/lib/x64/SDL2_image.lib
    ${VULKAN_SDK_PATH}/Lib/vulkan-1.lib
    ${ASSIMP_PATH}/build/lib/Release/assimp-vc142-mt.lib
    )

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_shared.lib)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_shared.lib )
endif(CMAKE_BUILD_TYPE MATCHES Debug)
