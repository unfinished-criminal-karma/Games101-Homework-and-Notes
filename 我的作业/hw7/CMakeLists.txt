# GAMES101 作业 7 · 路径追踪（Windows / MSVC 版）
# 官方原文件已备份为 CMakeLists.txt.linux-original
# 与作业 6 的差异：本作业要求多线程渲染 → 加了 OpenMP
cmake_minimum_required(VERSION 3.15)
project(RayTracing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 多线程渲染：OpenMP（作业要求之一）。找不到也不致命，只是没有并行。
find_package(OpenMP)

add_executable(RayTracing
    main.cpp
    Vector.cpp
    Scene.cpp
    BVH.cpp
    Renderer.cpp
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(RayTracing PRIVATE OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP enabled: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found -- multi-thread rendering will not work")
endif()

# exe 输出到 build/ 根下 —— main.cpp 里是 "../models/cornellbox/*.obj"，
# 工作目录必须是 build/ 才找得到模型
set_target_properties(RayTracing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

if(MSVC)
    # /utf-8：MSVC 默认按 GBK(ACP=936) 读源码，中文注释的末字节可能吞掉下一行代码
    target_compile_options(RayTracing PRIVATE /utf-8 /EHsc)
elseif(MINGW)
    # MinGW-w64 不带 libubsan：少了这条会链接失败（ld: cannot find -lubsan）
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type)
else()
    # 官方原始 flag（Linux / GCC）
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
    target_link_libraries(RayTracing PRIVATE -fsanitize=undefined)
endif()
