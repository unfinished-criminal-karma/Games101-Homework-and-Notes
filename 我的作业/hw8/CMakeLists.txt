# GAMES101 作业 8 · 绳子（质点弹簧）模拟 —— Windows / MSVC 版
# 官方原文件已备份为 CMakeLists.txt.linux-original
#
# 与官方版的差异：
#   1) CMake 4.x 拒绝 <3.5 的 cmake_minimum_required（CGL 内嵌的 GLFW 写的是 2.8.9）
#      → 用 CMAKE_POLICY_VERSION_MINIMUM 兜底，本工程自己的版本提到 3.15
#   2) MSVC 不认官方的 -std=gnu++11，改用 CMAKE_CXX_STANDARD
#   3) FreeType 用 UE 5.7 自带的那份（本机没单独装）
#   4) 去掉 Linux 专用的 link_directories(/usr/lib/x86_64-linux-gnu/mesa/)
#   5) 加 /utf-8（MSVC 默认按 GBK 读源码，非 ASCII 注释的末字节会吞掉下一行）
#
# OpenGL / GLEW / GLFW：GLEW 与 GLFW 的源码都在 CGL/deps 下随工程一起编译，
# 不需要另外安装；Windows 的 opengl32 由 find_package(OpenGL) 提供。

cmake_minimum_required(VERSION 3.15)
project(RopeSim C CXX)

# CGL 里的 GLFW 声明的是 2.8.9，CMake 4.x 不再接受 —— 让它按 3.5 的策略处理
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_LIBCGL "Build with libCGL" ON)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# FreeType —— 用 UE 5.7 自带的（和 Eigen 一个思路：不为一次作业单独装库）
set(FREETYPE_ROOT "F:/epic/UE_5.7/Engine/Source/ThirdParty/FreeType2/FreeType2-2.10.0")
if(EXISTS "${FREETYPE_ROOT}/include/ft2build.h")
  set(FREETYPE_INCLUDE_DIRS "${FREETYPE_ROOT}/include")
  set(FREETYPE_LIBRARIES    "${FREETYPE_ROOT}/lib/Win64/x64/Release/freetype.lib")
  message(STATUS "FreeType (UE bundled): ${FREETYPE_LIBRARIES}")
else()
  message(WARNING "UE FreeType2 not found, falling back to find_package(Freetype)")
  find_package(Freetype REQUIRED)
endif()

#-------------------------------------------------------------------------------
# Subdirectories
#-------------------------------------------------------------------------------
if(BUILD_LIBCGL)
  add_subdirectory(CGL)
  include_directories(CGL/include)
else()
  find_package(CGL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(GLFW REQUIRED)
endif()

add_subdirectory(src)

# Install settings
set(CMAKE_INSTALL_PREFIX "${RopeSim_SOURCE_DIR}/")
