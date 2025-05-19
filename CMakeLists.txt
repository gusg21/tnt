cmake_minimum_required(VERSION 3.21)
project(TNT)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
message(STATUS "Runtime Version: ${CMAKE_MSVC_RUNTIME_LIBRARY}")

set(TNTSources)
set(TNTIncludes)

set(GladSources
	glad/src/glad.c
)
set(GladIncludePath
	glad/include/
)

set(ImGuiColorTextEditSources
	ImGuiColorTextEdit/TextEditor.cpp
	ImGuiColorTextEdit/TextEditor.h
)
set(ImGuiColorTextEditIncludePath ImGuiColorTextEdit/)

set(ImNodesSources
	imnodes/imnodes.cpp
	imnodes/imnodes.h
	imnodes/imnodes_internal.h
)
set(ImNodesIncludePath imnodes/)

set(ImGuiSources 
	imgui/imconfig.h
	imgui/imgui.cpp
	imgui/imgui.h
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/misc/cpp/imgui_stdlib.cpp
	imgui/misc/cpp/imgui_stdlib.h
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h

	imgui/backends/imgui_impl_sdl3.cpp
	imgui/backends/imgui_impl_sdl3.h
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_opengl3.h
	imgui/backends/imgui_impl_opengl3_loader.h
)
set(ImGuiIncludePath imgui/)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(SDL)

add_subdirectory(json-c)

add_subdirectory(src)
add_subdirectory(include)

if (WIN32)
add_executable(TNTExe WIN32 ${TNTSources} ${TNTIncludes} ${ImGuiSources} ${GladSources} ${ImNodesSources} ${ImGuiColorTextEditSources})
endif()
set_property(TARGET TNTExe PROPERTY CXX_STANDARD 20)
set_target_properties(TNTExe PROPERTIES OUTPUT_NAME "TNT")

target_include_directories(TNTExe PUBLIC ${TNTIncludePath} ${ImGuiIncludePath} ${GladIncludePath} ${ImNodesIncludePath} ${ImGuiColorTextEditIncludePath} ims/)
target_include_directories(TNTExe PUBLIC nativefiledialog/src/include/)
target_link_directories(TNTExe PUBLIC nativefiledialog/build/lib/Debug/x64)
target_link_directories(TNTExe PUBLIC nativefiledialog/build/lib/Release/x64)
target_link_libraries(TNTExe PUBLIC SDL3-static)
if (CMAKE_BUILD_TYPE MATCHES "Release")
	target_link_libraries(TNTExe PUBLIC nfd)
else()
	target_link_libraries(TNTExe PUBLIC nfd_d)
endif()
target_link_libraries(TNTExe PUBLIC json-c-static)


add_custom_command(TARGET TNTExe POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_directory 
     "${CMAKE_CURRENT_SOURCE_DIR}/res"
     "res" 
  COMMENT "Copying to output directory")