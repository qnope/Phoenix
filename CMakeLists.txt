cmake_minimum_required(VERSION 3.10)

project(Phoenix)

if(MSVC)
    add_compile_options(/experimental:preprocessor /permissive- /NODEFAULTLIB)
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

set(LIBRARIES_INCLUDE ./lib/include)
set(LIBRARIES_LIB ${CMAKE_SOURCE_DIR}/lib/lib)

include_directories(${LIBRARIES_INCLUDE})
include_directories(${VULKAN_SDK_PATH}/Include)

set(MAIN_SRC main.cpp)

set(HEADERS_LPL
    lib/include/lpl/and.h
    lib/include/lpl/cat.h
    lib/include/lpl/checker.h
    lib/include/lpl/compl.h
    lib/include/lpl/dec_inc.h
    lib/include/lpl/evaluation.h
    lib/include/lpl/if.h
    lib/include/lpl/lpl.h
    lib/include/lpl/stringify.h)

set(HEADERS_LTL_TUPLE
    lib/include/ltl/tuple/algorithm.h
    lib/include/ltl/tuple/argument.h
    lib/include/ltl/tuple/template.h
    lib/include/ltl/tuple/tuple_t.h)

set(HEADERS_LTL_STRONG_TYPE
    lib/include/ltl/strong_type/strong_type_t.h
    lib/include/ltl/strong_type/equality.h
    lib/include/ltl/strong_type/arithmetic.h
    lib/include/ltl/strong_type/converter.h
    lib/include/ltl/strong_type/stream.h)

set(HEADERS_LTL
    lib/include/ltl/bool_t.h
    lib/include/ltl/crtp.h
    lib/include/ltl/decay_reference_wrapper.h
    lib/include/ltl/is_valid.h
    lib/include/ltl/number_t.h
    lib/include/ltl/overloader.h
    lib/include/ltl/tuple.h
    lib/include/ltl/type_t.h
    lib/include/ltl/type_traits.h
    lib/include/ltl/strong_type.h
    lib/include/ltl/range.h
    lib/include/ltl/smart_iterator.h
    ${HEADERS_LTL_TUPLE} ${HEADERS_LTL_STRONG_TYPE})

set(HEADERS_PHOENIX_VKW
    phoenix/vkw/Instance.h
    phoenix/vkw/Device.h
    )

set(PHOENIX_VKW_SRC
    phoenix/vkw/Instance.cpp
    phoenix/vkw/Device.cpp
    )

set(HEADERS_PHOENIX
    phoenix/constant.h
    phoenix/PhoenixWindow.h
    ${HEADERS_PHOENIX_VKW})



set(PHOENIX_SRC
    phoenix/PhoenixWindow.cpp
    ${PHOENIX_VKW_SRC}
    )

add_executable(Phoenix
    ${MAIN_SRC}
    ${PHOENIX_SRC}
    ${HEADERS_LPL} ${HEADERS_LTL}
    ${HEADERS_PHOENIX}
    )

target_link_libraries(Phoenix
    ${LIBRARIES_LIB}/SDL2main.lib ${LIBRARIES_LIB}/SDL2.lib
    ${VULKAN_SDK_PATH}/lib/vulkan-1.lib )
