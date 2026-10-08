# GAMES101 作业 5 · 光线与三角形求交（Windows / MSVC 版）
# 原文件已备份为 CMakeLists.txt.linux-original（只含 GCC 专属 flag）
cmake_minimum_required(VERSION 3.15)
project(RayTracing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 本作业不用 OpenCV、不用 Eigen —— 结果直接写成 binary.ppm（无外部依赖）
add_executable(RayTracing
    main.cpp
    Renderer.cpp
    Scene.cpp
)

# 让 exe 生成在 build/ 而不是 build/Release/
# 原因：Renderer.cpp 里是 fopen("binary.ppm", "wb") 这种相对路径，工作目录 = exe 所在目录
set_target_properties(RayTracing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

if(MSVC)
    # /utf-8：MSVC 默认按 GBK(ACP=936) 读源码，中文注释的末字节可能吞掉下一行代码
    target_compile_options(RayTracing PRIVATE /utf-8 /EHsc)
else()
    # 官方原始 flag（Linux / GCC）
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
    target_link_libraries(RayTracing PRIVATE -fsanitize=undefined)
endif()
