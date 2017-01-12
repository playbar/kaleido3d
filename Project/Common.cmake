set(GLSLANG_LIBS glslang OGLCompiler OSDependent SPIRV)
set(UNITTEST_DEPENDS Core Render ShaderCompiler)
if(ANDROID)
    set(UNITTEST_DEPENDS ${UNITTEST_DEPENDS} RHI_Vulkan)
elseif(WIN32)
    set(UNITTEST_DEPENDS ${UNITTEST_DEPENDS} Engine RHI_Vulkan
         winmm comctl32 ${GLSLANG_LIBS})
    if(BUILD_WITH_D3D12)
        set(UNITTEST_DEPENDS ${UNITTEST_DEPENDS} RHI_D3D12 ${DXSDK_LIBRARIES})
    endif()
elseif(MACOS)
    set(UNITTEST_DEPENDS ${UNITTEST_DEPENDS} "-framework AppKit")
endif()

function(add_plugin PLUGIN_NAME PLUGIN_FOLDER)
    if(BUILD_SHARED)
        add_definitions(-DBUILD_SHARED_LIB -DBUILD_WITH_PLUGIN)
    endif()
	set(LINK_BEGINS FALSE)
    foreach(ITEM IN LISTS ARGN)
		if("${ITEM}" STREQUAL "LINKLIBS")
			set(LINK_BEGINS TRUE)
			continue()
		endif()
		if(NOT LINK_BEGINS)
			list(APPEND SRCS ${ITEM})
		else()
			list(APPEND LIBS ${ITEM})
		endif()
	endforeach()
	
    add_library(${PLUGIN_NAME} ${LIB_TYPE} ${SRCS})
    set_target_properties(${PLUGIN_NAME} PROPERTIES FOLDER "${PLUGIN_FOLDER}")
    target_link_libraries(${PLUGIN_NAME} Core ${LIBS})
endfunction()

if(MACOS)
    function(target_pack_depend_libraries EXE_NAME)
        set(${EXE_NAME}_DIR $<TARGET_FILE_DIR:${EXE_NAME}>)
        foreach(DEPEND_LIB IN LISTS ARGN)
            message("${EXE_NAME}: Coping lib${DEPEND_LIB}")
            set(${DEPEND_LIB}_PACK_PATH "${${EXE_NAME}_DIR}/../../Contents/Frameworks/lib${DEPEND_LIB}.dylib")
            # Copy Dependency Libraries To XX.app/Contents/Frameworks
            add_custom_command(TARGET ${EXE_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
                             copy "$<TARGET_FILE:${DEPEND_LIB}>" "${${DEPEND_LIB}_PACK_PATH}")
        endforeach()
    endfunction()

    function(target_pack_plugins EXE_NAME)
        set(${EXE_NAME}_PLUGIN_DIR $<TARGET_FILE_DIR:${EXE_NAME}>/../PlugIns)
        foreach(PLUGIN IN LISTS ARGN)
            set(${PLUGIN}_INSTALL_DIR "${${EXE_NAME}_PLUGIN_DIR}/lib${PLUGIN}.dylib")
            add_custom_command(TARGET ${EXE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                         copy "$<TARGET_FILE:${PLUGIN}>" "${${PLUGIN}_INSTALL_DIR}")
        endforeach()
    endfunction()
endif()

macro(add_unittest EXAMPLE_NAME)
    if(ANDROID)
        add_library(${EXAMPLE_NAME} SHARED ${ARGN} ../Platform/Android/jni/RendererView.cpp ../Platform/Android/jni/RendererView_JNI.cpp)
    elseif(WIN32)
        add_executable(${EXAMPLE_NAME} ${ARGN} ../Platform/Windows/win32icon.rc)
    elseif(MACOS)
        add_executable(${EXAMPLE_NAME} MACOSX_BUNDLE ${ARGN} ../Platform/Windows/win32icon.rc)
        target_pack_depend_libraries(${EXAMPLE_NAME} Core Render ShaderCompiler)
        target_pack_plugins(${EXAMPLE_NAME} RHI_Metal KawaLog)
    else()
        add_executable(${EXAMPLE_NAME} ${ARGN})
    endif()
    target_link_libraries(${EXAMPLE_NAME} ${UNITTEST_DEPENDS})
    set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "Unit Test")
endmacro()