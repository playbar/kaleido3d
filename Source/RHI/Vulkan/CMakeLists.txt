file(GLOB VKRHI_SRC_LIST "*.cpp" "*.h")
include_directories(
../.. ../../../Include
.
)

if(WIN32)
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
elseif(ANDROID)
	add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
endif()

set(RHI_HEADERS
	VkCommon.h
	Public/IVkRHI.h
)

set(RHI_IMPL_SRCS
	Private/VkRHI.h
	Private/VkObjects.h
	Private/VkObjects.cpp
	Private/DynVulkanLoader.h
	Private/VkDevice.cpp
	Private/VkEnums.h
	Private/VkUtils.h
	Private/VkUtils.cpp
	Private/VkRenderViewport.cpp
	Private/VkCommandContext.cpp
	Private/VkResource.cpp
	Private/VkPipelineState.cpp
	Private/VkDescriptor.cpp
	Private/VkSwapChain.cpp
	Private/VkRenderPass.cpp
	Private/VkDebug.cpp
	Private/VkImpl.cpp
	Private/VkRHI.cpp
	Private/DynVulkanLoader.cpp
)

source_group(
	Public FILES
	${RHI_HEADERS}
)

source_group(
	Private FILES
	${RHI_IMPL_SRCS}
)

add_plugin(RHI_Vulkan 
	FOLDER "Runtime/RHI" 
	SRCS ${VKRHI_SRC_LIST} ${RHI_HEADERS} ${RHI_IMPL_SRCS} 
	LIBS ShaderCompiler ${VULKAN_LIB})

if(NOT ANDROID)
	add_precompiled_header(RHI_Vulkan "VkCommon.h" "${Kaleido3D_ROOT_DIR}/Source/RHI/Vulkan" "${Kaleido3D_ROOT_DIR}/Source/RHI/Vulkan/Private/VkRHI.cpp")
endif()

install(TARGETS RHI_Vulkan 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)