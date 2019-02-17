cmake_minimum_required(VERSION 3.10)

project(Phoenix)

if(MSVC)
    add_compile_options(/experimental:preprocessor /permissive-)
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

set(LIBRARIES_INCLUDE ./lib/include)
set(LIBRARIES_LIB ${CMAKE_SOURCE_DIR}/lib/lib)

include_directories(${LIBRARIES_INCLUDE})
include_directories(${VULKAN_SDK_PATH}/Include)

set(MAIN_SRC main.cpp)

set(HEADERS_LPL
    lib/include/lpl/lpl.h)

set(HEADERS_LTL
    lib/include/ltl/ltl.h)

set(HEADERS_LOL
    lib/include/lol/lol.h)

set(HEADERS_PHOENIX_VKW
    phoenix/vkw/Instance.h
    phoenix/vkw/Device.h
    phoenix/vkw/Queue.h
    phoenix/vkw/Surface.h
    phoenix/vkw/VulkanResource.h
    phoenix/vkw/utility.h
    phoenix/vkw/Swapchain.h
    phoenix/vkw/Image.h
    phoenix/vkw/ImageView.h
    phoenix/vkw/ShaderCompiler.h
    phoenix/vkw/ShaderModule.h
    phoenix/vkw/GraphicPipeline.h
    phoenix/vkw/With_Pipeline/WithDynamicStates.h
    phoenix/vkw/With_Pipeline/WithShaders.h
    phoenix/vkw/With_Pipeline/WithScissors.h
    phoenix/vkw/With_Pipeline/WithViewports.h
    phoenix/vkw/With_Pipeline/WithOutputs.h
    )

set(SRC_PHOENIX_VKW
    phoenix/vkw/Instance.cpp
    phoenix/vkw/Device.cpp
    phoenix/vkw/Queue.cpp
    phoenix/vkw/Surface.cpp
    phoenix/vkw/Swapchain.cpp
    phoenix/vkw/ShaderCompiler.cpp
    )

set(HEADERS_PHOENIX
    phoenix/constant.h
    phoenix/PhoenixWindow.h
    phoenix/SDL_Initializer.h
    ${HEADERS_PHOENIX_VKW})


set(SRC_PHOENIX
    phoenix/PhoenixWindow.cpp
    phoenix/SDL_Initializer.cpp
    ${SRC_PHOENIX_VKW}
    )

add_executable(Phoenix
    ${MAIN_SRC}
    ${SRC_PHOENIX}
    ${HEADERS_LPL} ${HEADERS_LTL} ${HEADERS_LOL}
    ${HEADERS_PHOENIX}
    )

target_link_libraries(Phoenix
    ${LIBRARIES_LIB}/SDL2main.lib ${LIBRARIES_LIB}/SDL2.lib
    ${VULKAN_SDK_PATH}/Lib/vulkan-1.lib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_sharedd.lib)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(Phoenix ${VULKAN_SDK_PATH}/Lib/shaderc_shared.lib )
endif(CMAKE_BUILD_TYPE MATCHES Debug)
