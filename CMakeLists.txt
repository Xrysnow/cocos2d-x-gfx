cmake_minimum_required(VERSION 3.10)

set(lib_name gfx)
set(target_name ext_${lib_name})

project(${lib_name})

if(WINDOWS)
    set(CC_USE_VULKAN ON)
    set(CC_USE_GLES3 ON)
    set(CC_USE_GLES2 ON)
    set(CC_USE_METAL OFF)
elseif(ANDROID)
    set(CC_USE_VULKAN ON)
    set(CC_USE_GLES3 ON)
    set(CC_USE_GLES2 ON)
    set(CC_USE_METAL OFF)
elseif(IOS)
    set(CC_USE_VULKAN ON)
    set(CC_USE_GLES3 OFF)
    set(CC_USE_GLES2 OFF)
    set(CC_USE_METAL ON)
elseif(MACOSX)
    set(CC_USE_VULKAN ON)
    set(CC_USE_GLES3 OFF)
    set(CC_USE_GLES2 OFF)
    set(CC_USE_METAL ON)
else()
    set(CC_USE_VULKAN ON)
    set(CC_USE_GLES3 ON)
    set(CC_USE_GLES2 ON)
    set(CC_USE_METAL OFF)
endif()

set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(gfx_src
  base/Agent.h
  base/CachedArray.h
  base/CoreStd.h
  base/IndexHandle.h
  base/Log.cpp
  base/Log.h
  base/Macros.h
  base/Object.h
  base/Random.h
  base/StringHandle.cpp
  base/StringHandle.h
  base/StringPool.h
  base/StringUtil.cpp
  base/StringUtil.h
  base/ThreadPool.cpp
  base/ThreadPool.h
  base/TypeDef.h
  base/Utils.cpp
  base/Utils.h

  base/memory/AllocatedObj.cpp
  base/memory/AllocatedObj.h
  base/memory/CallStack.cpp
  base/memory/CallStack.h
  base/memory/JeAlloc.cpp
  base/memory/JeAlloc.h
  base/memory/MemDef.h
  base/memory/Memory.cpp
  base/memory/Memory.h
  base/memory/MemoryHook.cpp
  base/memory/MemoryHook.h
  base/memory/MemTracker.cpp
  base/memory/MemTracker.h
  base/memory/NedPooling.cpp
  base/memory/NedPooling.h
  base/memory/StdAlloc.h
  base/memory/StlAlloc.h

  base/threading/ConditionVariable.h
  base/threading/ConditionVariable.cpp
  base/threading/Event.h
  base/threading/MessageQueue.h
  base/threading/MessageQueue.cpp
  base/threading/Semaphore.h
  base/threading/Semaphore.cpp
  base/threading/ThreadPool.h
  base/threading/ThreadPool.cpp
  base/threading/ThreadSafeCounter.h
  base/threading/ThreadSafeLinearAllocator.h
  base/threading/ThreadSafeLinearAllocator.cpp

  base/job-system/JobSystem.h

  math/Math.h

  backend/GFXDeviceManager.h
  backend/gfx-gles-common/GLESCommandPool.h
)

macro(extend_src _dir)
    file(GLOB_RECURSE _src
        "${_dir}/*.cpp"
        "${_dir}/*.c"
        "${_dir}/*.hpp"
        "${_dir}/*.h"
    )
    foreach(_var ${_src})
        list(APPEND gfx_src ${_var})
    endforeach()
endmacro()

extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-base")
extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-agent")
extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-validator")
extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-empty")
extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/frame-graph")

if(CC_USE_VULKAN)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-vulkan")
endif()

if(CC_USE_METAL)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-metal")
    file(GLOB metal_src
        "${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-metal/*.mm"
    )
    foreach(_var ${metal_src})
        list(APPEND ${gfx_src} ${_var})
    endforeach()
endif()

if(CC_USE_GLES3)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-gles3")
endif()

if(CC_USE_GLES2)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/backend/gfx-gles2")
endif()

if(USE_JOB_SYSTEM_TASKFLOW)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/base/job-system/job-system-taskflow")
elseif(USE_JOB_SYSTEM_TBB)
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/base/job-system/job-system-tbb")
else()
    extend_src("${CMAKE_CURRENT_SOURCE_DIR}/base/job-system/job-system-dummy")
endif()

add_library(${target_name} STATIC
    ${gfx_src}
)

if(CC_USE_VULKAN)
    target_compile_definitions(${target_name} PUBLIC VK_NO_PROTOTYPES)
    target_compile_definitions(${target_name} PUBLIC CC_USE_VULKAN)

    if(WINDOWS)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_WIN32_KHR)
    elseif(ANDROID)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
    elseif(IOS)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_IOS_MVK)
    elseif(MACOSX)
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_MACOS_MVK)
    else()
        target_compile_definitions(${target_name} PUBLIC VK_USE_PLATFORM_XCB_KHR)
    endif()
endif()

if(CC_USE_METAL)
    target_compile_definitions(${target_name} PUBLIC CC_USE_METAL)
endif()

if(CC_USE_GLES3)
    target_compile_definitions(${target_name} PUBLIC CC_USE_GLES3)
endif()

if(CC_USE_GLES2)
    target_compile_definitions(${target_name} PUBLIC CC_USE_GLES2)
endif()

if(USE_JOB_SYSTEM_TASKFLOW)
    target_include_directories(${target_name} PUBLIC
        ${COCOS2DX_ROOT_PATH}/external/taskflow
    )
elseif(USE_JOB_SYSTEM_TBB)
    target_include_directories(${target_name} PUBLIC
        ${COCOS2DX_ROOT_PATH}/external/tbb/include
    )
    target_link_libraries(${target_name} PUBLIC
        tbb_static tbbmalloc_static tbbmalloc_static
    )
endif()

target_compile_definitions(${target_name} PUBLIC
    $<IF:$<BOOL:${USE_JOB_SYSTEM_TBB}>,USE_JOB_SYSTEM_TBB=1,USE_JOB_SYSTEM_TBB=0>
    $<IF:$<BOOL:${USE_JOB_SYSTEM_TASKFLOW}>,USE_JOB_SYSTEM_TASKFLOW=1,USE_JOB_SYSTEM_TASKFLOW=0>
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)

target_include_directories(${target_name} PUBLIC
  ${src_dir}
  ${src_dir}/backend
  ${src_dir}/external
  ${src_dir}/external/khronos
  ${COCOS2DX_ROOT_PATH}/external
  ${COCOS2DX_ROOT_PATH}/external/glslang
)

target_link_libraries(${target_name} PUBLIC
  spirv-cross-c spirv-cross-util spirv-cross-hlsl spirv-cross-msl spirv-cross-reflect spirv-cross-cpp spirv-cross-glsl spirv-cross-core SPVRemapper SPIRV HLSL glslang MachineIndependent OGLCompiler OSDependent GenericCodeGen
)

if(WINDOWS)
    target_link_libraries(${target_name} PUBLIC
        ws2_32 userenv psapi winmm Iphlpapi
    )
endif()

target_compile_definitions(${target_name} PUBLIC
    CC_STATIC
    WIN32
    _WIN32
    _WINDOWS
    NOMINMAX
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
    _USRLIBSIMSTATIC
    SE_ENABLE_INSPECTOR
)

if(XCODE OR VS)
    cocos_mark_code_files(${target_name})
endif()

if(MSVC)
    target_compile_options(${target_name} PUBLIC /MP)
endif()
