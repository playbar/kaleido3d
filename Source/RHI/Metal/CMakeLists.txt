include_directories(.)
set(TARGET RHI_Metal)

set(SRCS 
Private/MetalDevice.mm
Private/MetalCommandContext.mm
Private/MetalBuffer.mm
Private/MetalTexture.mm
Private/MetalPipelineState.mm
Private/MetalResource.mm
Private/Shader.mm
Private/MetalRHI.mm
Private/MetalRHI.h
)

set(INCS
Public/IMetalRHI.h
Public/MetalRHIResource.h
../IRHI.h
../IRHIDefs.h
../ICrossShaderCompiler.h
)

source_group(Private FILES ${SRCS})
source_group(Public FILES ${INCS})
set(CMAKE_CXX_FLAGS "-fobjc-abi-version=2 -fobjc-arc ${CMAKE_CXX_FLAGS}")

add_plugin(${TARGET} "Runtime/RHI" ${SRCS} ${INCS})
target_link_libraries(${TARGET} "-framework Metal" "-framework MetalKit" "-framework AppKit" "-framework Foundation")