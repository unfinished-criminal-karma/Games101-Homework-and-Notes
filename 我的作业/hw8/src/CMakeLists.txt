# GAMES101 作业 8 · 应用侧（官方原文件备份：CMakeLists.txt.linux-original）
cmake_minimum_required(VERSION 3.15)

# Application source
set(APPLICATION_SOURCE
    rope.cpp
    application.cpp
    main.cpp
)

#-------------------------------------------------------------------------------
# Windows：补 POSIX 的 unistd.h / getopt（MSVC 没有）
#   main.cpp 里 `#include <unistd.h>` 和 getopt() 是 POSIX API。
#   compat/unistd.h 把 unistd.h 转发到 GLFW 自带的那份 getopt 实现，
#   作业原文件一个字都不用改。只在 WIN32 下加，避免遮蔽 Linux 的系统头。
#-------------------------------------------------------------------------------
if(WIN32)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compat)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../CGL/deps/glfw/deps)
  list(APPEND APPLICATION_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../CGL/deps/glfw/deps/getopt.c)
endif()

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
  ${CGL_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(ropesim ${APPLICATION_SOURCE})

# UE 的 freetype.lib 是带 PNG / GZip 支持编译的，链接时还要补上 libpng 与 zlib
set(UE_THIRDPARTY "F:/epic/UE_5.7/Engine/Source/ThirdParty")

target_link_libraries( ropesim
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    "${UE_THIRDPARTY}/libPNG/libPNG-1.6.44/lib/Win64/x64/Release/libpng.lib"
    "${UE_THIRDPARTY}/zlib/1.3/lib/Win64/Release/zlibstatic.lib"
    ${CMAKE_THREADS_INIT}
)

if(MSVC)
  target_compile_options(ropesim PRIVATE /utf-8 /EHsc)
  # M_PI 在 MSVC 上需要先定义 _USE_MATH_DEFINES；GLEW_STATIC 见 CGL 侧的说明
  target_compile_definitions(ropesim PRIVATE _USE_MATH_DEFINES GLEW_STATIC)
endif()

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)
